<?php

  /**
   * Module Project
   * Class resourceplanning
   *
   * This class show resourceplanning page
   *
   * $Id$
   * @author Yury Golovnya  <yury@acheivo.org>
   * @version $Revision$
   *
   */
  atkimport("module.project.utils.resourceutils");
  atkimport("module.utils.dateutil");
  atkimport("module.timereg.overtime_balance");
  useattrib("atkdurationattribute");
  include_once(atkConfig("atkroot")."achievotools.inc");


  /**
   * Constructor.
   *
   */
  class resourceplanning extends atkNode
  {
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
    public $m_resourceHours = array();

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
    public $m_parentChild =  array();
    public $m_projectId;
    private $m_mode;

    /**
     * Project identifier
     *
     * @var string
     */
    private $project;

    public function __construct()
    {
      $this->atkNode("resourceplanning");
      $this->project = sessionLoad('projectid');
      list($name,$id) = explode("=", $this->project);
      $this->m_projectId = stripQuotes($id);


      $this->m_mode = $this->getView();
    }

    public function action_report(&$handler)
    {
      $view = $this->getView();

      $this->getDateBounds($view, $startdate, $enddate);

      $dummyrec = array("startdate"=>$startdate, "enddate"=>$enddate);
      $start = new atkDateAttribute("startdate");
      $end = new atkDateAttribute("enddate");
      $timerange = $this->text("from")." : ".$start->edit($dummyrec).$this->text("till")." : ".$end->edit($dummyrec);

      $theme = &atkinstance("atk.ui.atktheme");

      $page = &atkinstance("atk.ui.atkpage");
      $page->register_style($theme->stylePath("style.css"));
      $page->register_style($theme->stylePath("tabs.css"));

      $header = array();
      $data = array();
      $min_width = 0;
      $depth = 0;

      switch ($view)
      {
          case "week":
            $this->getWeekView($startdate, $enddate, $header, $data, $min_width);
            break;
          case "month":
            $this->getMonthView($startdate, $enddate, $header, $data, $min_width);
            break;
      }

      $vars = array('session_form'=>atkSessionManager::formState(),
                    'projectmenu' => $this->getProjectMenu(),
                    'timerange'=>$timerange,
                    'planheader'=>$header,
                    'plan'=>$data,
                    'projectid'=>$this->m_projectId,
                    'min_width'=>$min_width,
                    'viewmode'=>$this->getModeSelector(),
                    'view'=>$this->getView(),
                    'width'=>190,
                    'padding'=>0,
                    'depth'=>$depth,
                      );

      $ui = &atkinstance("atk.ui.atkui");

      $content = $ui->render(moduleDir('project').'templates/resourceplanning.tpl',$vars);
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
            pp.projectid='$this->m_projectId'
            AND p.status='active'
      ");

      $node = &atkGetNode("employee.employee");
      for($i=0,$_i=count($res);$i<$_i;$i++)
      {
        $res[$i]['name'] = $node->lastname_display($res[$i], "list");
      }
      return $res;
    }

    function getWeekView($startdate, $enddate,&$header, &$data, &$min_width, $employee='', $package ='')
    {
      $this->handleDateBound($startdate, $enddate, $startweek, $endweek);

      $this->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employee);

      $rows = $this->getRows();

      //Header creation
      $header[] = $this->text("plan");
      $header[] = $this->text("fact");

      //Header columns
      $weeks = dateutil::weeksBetween($startdate,$enddate);
      $min_width = $this->getMinWidth(count($weeks));

      foreach ($weeks as $w)
      {
        $header[] = substr($w['week'],-4);
      }

      //Data rows
      foreach ($rows as $r)
      {
        $line = array();
        $line[] = atkDurationAttribute::_minutes2string($this->getPlan($r['type'],$r['id'],$r['employee']));
        $line[] = atkDurationAttribute::_minutes2string($this->getFact($r['type'],$r['id'],$r['employee']));
        //

        foreach ($weeks as $w)
        {
          $line[] = $this->getAvailability(resourceutils::str2str($w['startdate']),resourceutils::str2str($w['enddate']),$r['employee']);
        }
        $data[] = array("data"=>$line,"id"=>$r['id'],"type"=>$r['type'],"name"=>$r['name'],"employeeid"=>$r['employee']);
      }
    }

    function getMonthView($startdate, $enddate,&$header, &$data, &$min_width, $employee='', $package ='')
    {
      $this->handleDateBound($startdate, $enddate, $startmonth, $endmonth);

      $this->getTaskHours($startdate, $enddate,$employee);

      $rows = $this->getRows();

      //Header creation
      $header[] = $this->text("plan");
      $header[] = $this->text("fact");
      //Header row
      $months = resourceutils::monthsBetween(dateutil::str2stamp($startdate), dateutil::str2stamp($enddate));
      $min_width = $this->getMinWidth(count($months));

      foreach ($months as $month)
      {
        $header[] = $month;
      }

      //Data rows
      foreach ($rows as $r)
      {
        $line = array();
        $line[] = atkDurationAttribute::_minutes2string($this->getPlan($r['type'],$r['id'],$r['employee']));
        $line[] = atkDurationAttribute::_minutes2string($this->getFact($r['type'],$r['id'],$r['employee']));
        //
        $year = date("Y");
        for($w=$startmonth;$w<=$endmonth;$w++)
        {
          $line[] = $this->getAvailability(date('Y-m-d',mktime(0,0,0,$w,1,$year)),date('Y-m-d',mktime(0,0,0,$w+1,0,$year)),$r['employee']);
        }
        $data[] = array("data"=>$line,"id"=>$r['id'],"type"=>$r['type'],"name"=>$r['name'],"employeeid"=>$r['employee']);
      }
    }

    function getTaskHours($start,$end, $employee='',$package='')
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
            p.parent_id
          FROM
            phase h
            LEFT JOIN
            (
            	SELECT 'project.package' master, id master_id, name,
            	master parent,
            	master_id parent_id
              FROM package
            ) p USING (master,master_id)
            LEFT JOIN
            (
            	SELECT SUM(time) fact,phaseid
              FROM hours
              WHERE
              activitydate BETWEEN '$start' AND '$end'
              GROUP BY phaseid
            ) a ON a.phaseid=h.id
          WHERE
          h.projectid='$this->m_projectId' AND
          ".($employee ? " h.assign_to='$employee' AND " : "" )."
          ".($package ? " h.master_id='$package' AND h.master='project.package' AND " : "" )."
          ( (h.startdate BETWEEN '$start' AND '$end') OR
          (h.enddate BETWEEN '$start' AND '$end')  OR
          (h.startdate<='$start' AND h.enddate>='$end') )
      ");

      foreach ($rows as $r)
      {
        //per task
        $factdays = $this->getWorkingDaysFromTask($r['startdate'],$r['enddate'],$start, $end);
        $plandays = resourceutils::getWorkingDays($r['startdate'],$r['enddate']);
        $plan = $factdays*$r['plan']/$plandays;

        $this->m_resourceHours['task'][$r['employee']][$r['id']] = array("plan"=>$plan,"fact"=>$r['fact'],"name"=>$r['name'],"startdate"=>$r['startdate'],"enddate"=>$r['enddate'],"taskplan"=>$r['plan']);

        //per package
        if($r['master'] == 'project.package')
        {
          $this->m_resourceHours['package'][$r['employee']][$r['master_id']]['plan'] += $plan;
          $this->m_resourceHours['package'][$r['employee']][$r['master_id']]['fact'] += $r['fact'];
          $this->m_resourceHours['package'][$r['employee']][$r['master_id']]['name'] = $r['master_name'];
        }

        //per employee
        $this->m_resourceHours['employee'][$r['employee']][$r['employee']]['plan'] += $plan;
        $this->m_resourceHours['employee'][$r['employee']][$r['employee']]['fact'] += $r['fact'];
      }
    }

    function getWorkingDaysFromTask($starttask,$endtask,$startperiod,$endperiod)
    {
      if( ($starttask > $endperiod) || ($endtask < $startperiod))
      {
        return 0;
      }
      elseif($starttask <= $startperiod)
      {
        if(($endtask >= $startperiod) && ($endtask < $endperiod))
        {
          $start = $startperiod;
          $end = $endtask;
        }
        elseif($endtask >= $endperiod)
        {
          $start = $startperiod;
          $end = $endperiod;
        }
      }
      elseif(($starttask > $startperiod) && ($starttask <= $endperiod))
      {
        if(($endtask > $startperiod) && ($endtask <= $endperiod))
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

    public function getView()
    {
      if($this->m_mode) return $this->m_mode;
      // We use the session manager to retrieve the view.
      $sessionManager = &atkGetSessionManager();
      $view = $sessionManager->pageVar("view");
      if (!in_array($view, array("day", "week", "month")))
      {
        $view = atkconfig::get("project","resourceplanning_view");
        $sessionManager->pageVar("view", $view);
      }
      return $view;
    }

    function getProjectChild($id,$employee)
    {
      $db = &$this->getDb();

      $rows = $db->getrows("
        SELECT id,name
        FROM package
        JOIN package_person ON package.id=package_person.package_id
        WHERE master='project.project' AND master_id='$id' AND package_person.person_id='$employee'");
      foreach ($rows as $r)
      {
        $this->m_parentChild['project.project'][$id]["package"][] = $r;
      }
      //@todo: we can get this info from $this->m_resoureceHours
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
    /**
     * Calculate employees availability at period of time
     *
     * @param string $start - start of period yyyy-mm-dd
     * @param string $end- end of period yyyy-mm-dd
     * @param int $employee - employees id
     * @return string html code - image with title
     */
    function getPeriodPlan($start, $end, $employee, $id)
    {
      $plan = $this->getTaskPlan('task', $id, $employee);
      $startdate = $this->getStartDate('task', $id, $employee);
      $enddate = $this->getEndDate('task', $id, $employee);

      $factdays = $this->getWorkingDaysFromTask($startdate, $enddate, $start, $end);
      $plandays = resourceutils::getWorkingDays($startdate,$enddate);
      $res = $factdays*$plan/$plandays;
      return $res;
    }

    function getLinepackage($weeks, $id, $employeeId)
    {
      if(!isset($this->m_resourceHours['package'][$employeeId][$id]['line']))
      {
        $this->m_resourceHours['package'][$employeeId][$id]['line'] = array();

        //subpackage handling
        foreach ($this->m_parentChild['project.package'][$id]['package'] as $child)
        {
          $row = $this->getLinePackage($weeks,$child['id'],$employeeId);
          for($i=0,$_i=count($row);$i<$_i;$i++)
          {
            $this->m_resourceHours['package'][$employeeId][$id]['line'][$i] += $row[$i];
          }
        }

        //task handling
        foreach ($this->m_parentChild['project.package'][$id]['task'] as $child)
        {
          $row = $this->getLineTask($weeks,$child['id'],$employeeId);
          for($i=0,$_i=count($row);$i<$_i;$i++)
          {
            $this->m_resourceHours['package'][$employeeId][$id]['line'][$i] += $row[$i];
          }
        }
      }
      return $this->m_resourceHours['package'][$employeeId][$id]['line'];
    }

    function getLinetask($weeks, $id, $employeeId)
    {
      if(!isset($this->m_resourceHours['task'][$employeeId][$id]['line']))
      {
        foreach ($weeks as $w)
        {
          //$line[] = atkDurationAttribute::_minutes2string($this->getPeriodPlan(resourceutils::str2str($w['startdate']),resourceutils::str2str($w['enddate']),$employeeId,$id));
          $line[] = $this->getPeriodPlan(resourceutils::str2str($w['startdate']),resourceutils::str2str($w['enddate']),$employeeId,$id);
        }
        $this->m_resourceHours['task'][$employeeId][$id]['line'] = $line;
      }
      return $this->m_resourceHours['task'][$employeeId][$id]['line'];
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

    private function getDateBounds($view, &$startdate, &$enddate)
    {
      $startdate = $this->getDate("startdate");
      $enddate = $this->getDate("enddate");

      if ($startdate=="")
      {
        $month = date("m");
        $year = date("Y");
        switch ($view)
        {
          case "week":
            $startdate =  date("Y-m-d" , mktime(0,0,0,$month,1,$year));
            break;
          case "month":
            $startdate =  date("Y-m-d" , mktime(0,0,0,$month-2,1,$year));
            break;
        }
      }

      if ($enddate=="")
      {
        $month = date("m");
        $year = date("Y");
        switch ($view)
        {
          case "week":
            $enddate = date('Y-m-d',mktime(0,0,0,$month+2,0,$year));
            break;
          case "month":
            $enddate = date('Y-m-d',mktime(0,0,0,$month+3,0,$year));
            break;
        }
      }
    }

    function handleDateBound(&$startdate, &$enddate,&$start,&$end)
    {
      switch($this->getView())
      {
        case "week":
          $start = date("W",dateutil::str2stamp($startdate));
          $end = date("W",dateutil::str2stamp($enddate));

          $startdate = dateutil::startOfWeek(dateutil::arr2str((dateutil::str2arr($startdate))));
          $enddate = dateutil::endOfWeek(dateutil::arr2str((dateutil::str2arr($enddate))));
          break;
        case "month":
          $start = date("m",dateutil::str2stamp($startdate));
          $end = date("m",dateutil::str2stamp($enddate));
          break;
      }
    }

    /**
     * Get the project menu.
     *
     * @return string HTML
     */
    private function getProjectMenu()
    {
      atkimport('module.project.projectmenu');
      $pm = new projectmenu('project.resourceplanning', $this->m_project);
      return $pm->getTabHtml();
    }

    private function getModeSelector()
    {
      $view = $this->getView();

      $ret = $this->text("mode").": ";

      $daylink = href(dispatch_url($this->atknodetype(),"report",array("view"=>"day")),$this->text("daymode"));
      $weeklink = href(dispatch_url($this->atknodetype(),"report",array("view"=>"week")),$this->text("weekmode"));
      $monthlink = href(dispatch_url($this->atknodetype(),"report",array("view"=>"month")),$this->text("monthmode"));

      switch($view)
      {
        case "day":
          $ret .= $this->text("daymode")." ".$weeklink." ".$monthlink;
          break;
        case "week":
          $ret .= $daylink." ".$this->text("weekmode")." ".$monthlink;
          break;
        case "month":
          $ret .= $daylink." ".$weeklink." ".$this->text("monthmode");
          break;
      }
      return $ret;
    }

    public function getStartDate($type,$id,$employee)
    {
      return $this->m_resourceHours[$type][$employee][$id]['startdate'];
    }

    public function getEndDate($type,$id,$employee)
    {
      return $this->m_resourceHours[$type][$employee][$id]['enddate'];
    }

    public function getTaskPlan($type,$id,$employee)
    {
      return $this->m_resourceHours[$type][$employee][$id]['taskplan'];
    }
    
    public function getMinWidth($count)
    {
       return ($count +2)*50+190;
    }

  }
?>
