<?php

  /**
   * Module Project
   * Class project_personemployee
   *
   * This class makes it possible to relate employees to a project in a certain role(s).
   *
   * $Id: class.project_personemployee.inc 5061 2008-06-25 10:11:47Z danny $
   * @author Yury Golovnya  <yury@acheivo.org>
   * @version $Revision$
   *
   */
atkimport("module.project.utils.resourceutils");
atkimport("module.utils.dateutil");
atkimport("module.timereg.overtime_balance");
useattrib("atkdurationattribute");
include_once("achievotools.inc");

  /**
   * Constructor.
   *
   */
  class projectplanning extends atkNode
  {
    var $m_employeeId;

    /**
     * Array with plan/fact hours
     *
     * Has the following structure:
     * ['project'|'package'|'task'|'employee'] - type - resource accumulated by type
     * ['employee_id'] - employee_id - resource accumulated by user
     * ['id'] - id  - id of type
     * array() - array with plan and fact hours
     *
     * @var array()
     */
    var $m_resourceHours = array();

    /**
     * Parent-child hierarchy
     *
     * Has the following structure:
     * ['project.package'|'project.project'] - parent type
     * ['id'] - parent id
     * ['project'|'package'|'task'] - child type
     * array() - array with name and id of child
     *
     *
     * @var array()
     */
    var $m_parentChild =  array();

    public function __construct()
    {
      $this->atkNode("projectplanning");
    }

    function action_report(&$handler)
    {
      $view = $this->getView();

      $startdate = $enddate = '';

      $this->handleDates($startdate, $enddate);

      $dummyrec = array("startdate"=>$startdate, "enddate"=>$enddate);
      $start = new atkDateAttribute("startdate");
      $end = new atkDateAttribute("enddate");
      $timerange = $this->text("from")." : ".$start->edit($dummyrec).$this->text("till")." : ".$end->edit($dummyrec);

//      list($name,$id) = explode("=",atkArrayNvl($this->m_postvars,"atkfilter"));
      $theme = &atkinstance("atk.ui.atktheme");

      $page = &atkinstance("atk.ui.atkpage");
      $page->register_style($theme->stylePath("style.css"));

      $header = array();
      $data = array();
      $min_width = 0;
      $depth = 0;

      $this->getWeekView($startdate, $enddate, '', $header, $data, $min_width);

      $vars = array('session_form'=>atkSessionManager::formState(),
                    'dispatch'=>getDispatchFile(),
                    'timerange'=>$timerange,
                    'planheader'=>$header,
                    'plan'=>$data,
                    'min_width'=>$min_width,
                    'depth'=>$depth,
                    'width'=>190-$depth*20,
                    'padding'=>$depth*20

                      );

      $ui = &atkinstance("atk.ui.atkui");

      $content = $ui->render(moduleDir('project').'templates/projectplanning.tpl',$vars);
      $page->addContent($ui->renderBox(array('title'=>$this->actionTitle("report"),"content"=>$content)));
    }

    function getRows()
    {
      $res = $this->getDb()->getrows("
        SELECT
          'employee' type, p.id employee, p.id, p.lastname,p.firstname
        FROM
          project_person pp
  	      JOIN person p ON p.id=pp.personid
        WHERE
            p.status='active'
        GROUP BY
          employee
      ");

      $node = &atkGetNode("employee.employee");
      for($i=0,$_i=count($res);$i<$_i;$i++)
      {
        $res[$i]['name'] = $node->lastname_display($res[$i], "list");
      }
      return $res;
    }

    function getWeekView($startdate, $enddate,$cookie='', &$header, &$data, &$min_width, $employee='', $package ='')
    {
      $startweek = date("W",dateutil::str2stamp($startdate));
      $endweek = date("W",dateutil::str2stamp($enddate));

      $startdate = dateutil::startOfWeek(dateutil::arr2str((dateutil::str2arr($startdate))));
      $enddate = dateutil::endOfWeek(dateutil::arr2str((dateutil::str2arr($enddate))));

      $this->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employee);

      $rows = $this->getRows();

      //Header row
      $min_width = ($endweek - $startweek +3)*50+190;

      //Header creation
      $header[] = $this->text("contract_hours");

      $weeks = dateutil::weeksBetween($startdate,$enddate);

      foreach ($weeks as $w)
      {
        $header[] = substr($w['week'],-4);
      }

      $header[] = $this->text("plan");
      $header[] = $this->text("fact");
      $header[] = $this->text("diff");

      //Data rows
      foreach ($rows as $r)
      {
        $line = array();
        //get employee contract hours
        $line[] = overtime_balance::getContractHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$r['employee']);

        foreach ($weeks as $w)
        {
          $line[] = $this->getAvailability(resourceutils::str2str($w['startdate']),resourceutils::str2str($w['enddate']),$r['employee']);
        }

        $plan = $this->getPlan($r['type'],$r['id'],$r['employee']);
        $fact = $this->getFact($r['type'],$r['id'],$r['employee']);
        $line[] = atkDurationAttribute::_minutes2string($plan);
        $line[] = atkDurationAttribute::_minutes2string($fact);
        $line[] = atkDurationAttribute::_minutes2string($plan - $fact);

        $data[] = array("data"=>$line,"id"=>$r['id'],"type"=>$r['type'],"name"=>$r['name'],"employeeid"=>$r['employee']);
      }
    }

    /**
     * Calculate employees availability at period of time
     *
     * @param string $start - start of period yyyy-mm-dd
     * @param string $end- end of period yyyy-mm-dd
     * @param int $employee - employees id
     * @return string html code - image with title
     */
    function getAvailability($start,$end,$employee)
    {
      $rows = $this->getDb()->getrows("
        SELECT
            h.id,
            h.current_planning plan,
            h.startdate,
            h.enddate
          FROM
            phase h
          WHERE
           h.assign_to='$employee' AND
          ( (h.startdate BETWEEN '$start' AND '$end') OR
          (h.enddate BETWEEN '$start' AND '$end') OR
          (h.startdate < '$start' AND h.enddate > '$end') )
      ");

      $res = 0;
      foreach ($rows as $r)
      {
        $factdays = $this->getWorkingDaysFromTask($r['startdate'],$r['enddate'],$start, $end);
        $plandays = resourceutils::getWorkingDays($r['startdate'],$r['enddate']);
        $res += $factdays*$r['plan']/$plandays;
      }
      $contractHours = overtime_balance::getContractHours(resourceutils::str2str($start), resourceutils::str2str($end),$employee)*60;
      if($contractHours)
      {
        $availability = (1-$res/$contractHours)*100;
        if(eval("return ".atkConfig::get("project","project_resource_planning_green").";"))
        {
          $image = "user_green.png";
          $alt = 'green';
        }
        elseif(eval("return ".atkConfig::get("project","project_resource_planning_yellow").";"))
        {
          $image = "user_orange.png";
          $alt = 'yellow';
        }
        elseif(eval("return ".atkConfig::get("project","project_resource_planning_red").";"))
        {
          $image = "user_red.png";
          $alt = 'red';
        }
        else
        {
          $image = "error.png";
          $alt = 'error';
        }

        return "<img src='images/$image' alt='$alt' title='".number_format($availability,2)."%' />";
      }
      else
        return "<img src='images/error.png' alt='no data available' title='".$this->text('no_data_available')."%' />";

    }

    function handleSubPackage($id, $employee)
    {
      $this->getPackageChild($id, $employee);
      foreach ($this->m_parentChild['project.package'][$id]['package'] as $child)
      {
        $this->handleSubPackage($child['id'],$employee);

        $value = $this->getPlan("package",$id,$employee) + $this->getPlan("package",$child['id'],$employee);
        $this->setPlan("package",$id,$employee,$value);

        $value = $this->getFact("package",$id,$employee) + $this->getFact("package",$child['id'],$employee);
        $this->setFact("package",$id,$employee,$value);
      }
    }

    function handleSubProject($id,$employee)
    {
      $this->getProjectChild($id,$employee);
      foreach ($this->m_parentChild['project.project'][$id]['project'] as $child)
      {
        $this->handleSubProject($child['id'],$employee);

        $value = $this->getPlan("project",$id,$employee) + $this->getPlan("project",$child['id'],$employee);
        $this->setPlan("project",$id,$employee,$value);

        $value = $this->getFact("project",$id,$employee) + $this->getFact("project",$child['id'],$employee);
        $this->setFact("project",$id,$employee,$value);
      }
    }

    function getTaskHours($start,$end, $employee='',$package='',$project='')
    {
      $rows = $this->getDb()->getrows("
        SELECT
            h.id,
            h.name,
            h.assign_to employee,
            h.current_planning plan,
            a.fact,
            h.master_id,
            h.master,
            h.startdate,
            h.enddate,
            p.name master_name,
            p.parent,
            p.parent_id,
            h.projectid,
            project.name projectname,
            project.master_project
          FROM
            phase h
            LEFT JOIN
            ( SELECT 'project.package' master, id master_id, name,
            	master parent,
            	master_id parent_id
              FROM package
            ) p USING (master,master_id)
            JOIN project ON project.id=h.projectid
            LEFT JOIN
            (
            	SELECT SUM(time) fact, phaseid
              FROM hours
              WHERE
              activitydate BETWEEN '$start' AND '$end'
              GROUP BY phaseid
            ) a ON a.phaseid=h.id
          WHERE
          ".($project ? " h.projectid='$project' AND " : "") ."
          ".($employee ? " h.assign_to='$employee' AND " : "" )."
          ".($package ? " h.master_id='$package' AND h.master='project.package' AND " : "" )."
          ( (h.startdate BETWEEN '$start' AND '$end') OR
          (h.enddate BETWEEN '$start' AND '$end') )
      ");

      foreach ($rows as $r)
      {
        //per task
        $factdays = $this->getWorkingDaysFromTask($r['startdate'],$r['enddate'],$start, $end);
        $plandays = resourceutils::getWorkingDays($r['startdate'],$r['enddate']);
        $plan = $factdays*$r['plan']/$plandays;

        $this->m_resourceHours['task'][$r['employee']][$r['id']] = array("plan"=>$plan,"fact"=>$r['fact'],"name"=>$r['name']);

        //per package
        if($r['master'] == 'project.package')
        {
          $this->m_resourceHours['package'][$r['employee']][$r['master_id']]['plan'] += $plan;
          $this->m_resourceHours['package'][$r['employee']][$r['master_id']]['fact'] += $r['fact'];
          $this->m_resourceHours['package'][$r['employee']][$r['master_id']]['name'] = $r['master_name'];
        }

        //per project
        $this->m_resourceHours['project'][$r['employee']][$r['projectid']]['plan'] += $plan;
        $this->m_resourceHours['project'][$r['employee']][$r['projectid']]['fact'] += $r['fact'];
        $this->m_resourceHours['project'][$r['employee']][$r['projectid']]['name'] = $r['projectname'];
        $this->m_resourceHours['project'][$r['employee']][$r['projectid']]['master_project'] = $r['master_project'];

        //per employee
        $this->m_resourceHours['employee'][$r['employee']][$r['employee']]['plan'] += $plan;
        $this->m_resourceHours['employee'][$r['employee']][$r['employee']]['fact'] += $r['fact'];
      }
    }

    function getWorkingDaysFromTask($starttask,$endtask,$startperiod,$endperiod)
    {
      if($starttask > $endperiod)
      {
        return 0;
      }
      elseif($starttask < $startperiod)
      {
        if(($endtask > $startperiod) && ($endtask < $endperiod))
        {
          $start = $startperiod;
          $end = $endtask;
        }
        elseif($endtask > $endperiod)
        {
          $start = $startperiod;
          $end = $endperiod;
        }
      }
      elseif(($starttask > $startperiod) && ($starttask < $endperiod))
      {
        if(($endtask > $startperiod) && ($endtask < $endperiod))
        {
          $start = $starttask;
          $end = $endtask;
        }
        elseif($endtask > $endperiod)
        {
          $start = $starttask;
          $end = $endperiod;
        }
      }

      $ret = resourceutils::getWorkingDays($start,$end);

      return $ret;
    }

    public function getPlan($type,$id,$employee)
    {
      return $this->m_resourceHours[$type][$employee][$id]['plan'];
    }

    public function getFact($type,$id,$employee)
    {
      return $this->m_resourceHours[$type][$employee][$id]['fact'];
    }

    public function setPlan($type,$id,$employee,$value)
    {
      $this->m_resourceHours[$type][$employee][$id]['plan'] = $value;
    }

    public function setFact($type,$id,$employee,$value)
    {
      $this->m_resourceHours[$type][$employee][$id]['fact'] = $value;
    }

    function getView()
    {
      // We use the session manager to retrieve the view.
      $sessionManager = &atkGetSessionManager();
      $view = $sessionManager->pageVar("view");
      if (!in_array($view, array("day", "week", "workweek", "quartal")))
        $view = atkconfig::get("project","projectplanning_view");
      return $view;
    }

    function getDate($name)
    {
      $ret = "";
      $sm = &atkGetSessionManager();

      $date = $sm->pageVar($name);

      if (is_array($date))
      {
        $ret = $date["year"]."-".sprintf("%02d",$date["month"])."-".sprintf("%02d",$date["day"]);
      }
      return $ret;
    }

    function handleDates(&$startdate, &$enddate)
    {
      $startdate = $this->getDate("startdate");
      $enddate = $this->getDate("enddate");

      if ($startdate=="")
      {
        $month = date("m");
        $year = date("Y");
        $startdate =  date("Y-m-d" , mktime(0,0,0,$month,1,$year));
      }

      if ($enddate=="")
      {
        $month = date("m");
        $year = date("Y");
        $enddate = date('Y-m-d',mktime(0,0,0,$month+2,0,$year));
      }
    }

    function getProjectChild($id,$employee)
    {
      $db = &$this->getDb();

      $rows = $db->getrows("
        SELECT id,name
        FROM project
        JOIN project_person l ON project.id=l.projectid
        WHERE master_project='$id' AND l.personid='$employee'");
      foreach ($rows as $r)
      {
        $this->m_parentChild['project.project'][$id]["project"][] = $r;
      }

      $rows = $db->getrows("
        SELECT id,name
        FROM package
        JOIN package_person ON package.id=package_person.package_id
        WHERE master='project.project' AND master_id='$id' AND package_person.person_id='$employee'");
      foreach ($rows as $r)
      {
        $this->m_parentChild['project.project'][$id]["package"][] = $r;
      }

      $rows = $db->getrows("SELECT id,name FROM phase WHERE master='project.project' AND master_id='$id' AND assign_to='$employee'");
      foreach ($rows as $r)
      {
        $this->m_parentChild['project.project'][$id]["task"][] = $r;
      }
    }

    function getPackageChild($id, $employee)
    {
      $db = &$this->getDb();

      $rows = $db->getrows("
        SELECT id,name
        FROM package
        JOIN package_person ON package.id=package_person.package_id
        WHERE master='project.package' AND master_id='$id' AND package_person.person_id='$employee'");
      foreach ($rows as $r)
      {
        $this->m_parentChild['project.package'][$id]["package"][] = $r;
      }

      $rows = $db->getrows("SELECT id,name FROM phase WHERE master='project.package' AND master_id='$id' AND assign_to='$employee'");
      foreach ($rows as $r)
      {
        $this->m_parentChild['project.package'][$id]["task"][] = $r;
      }
    }

  }
?>