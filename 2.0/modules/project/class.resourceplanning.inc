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
  atkimport("module.project.projectplanning");


  /**
   * Constructor.
   *
   */
  class resourceplanning extends projectplanning
  {
    public $m_projectId;

    /**
     * Project identifier
     *
     * @var string
     */
    private $project;

    public function __construct()
    {
      parent::__construct("resourceplanning");
      $this->project = sessionLoad('projectid');
      list($name,$id) = explode("=", $this->project);
      $this->m_projectId = stripQuotes($id);

      $this->m_template = 'resourceplanning.tpl';
      $this->m_additionalColumn = 2;
      $this->m_mode = $this->getViewMode();
    }

    protected function getView($startdate, $enddate,&$header, &$data, &$min_width, $employee='', $package ='')
    {
      switch ($this->getViewMode())
      {
          case "week":
            $this->getWeekView($startdate, $enddate, $header, $data, $min_width, $employee='', $package ='');
            break;
          case "month":
            $this->getMonthView($startdate, $enddate, $header, $data, $min_width, $employee='', $package ='');
            break;
      }
    }

    protected function getWeekView($startdate, $enddate,&$header, &$data, &$min_width, $employee='', $package ='')
    {
      $this->handleDateBounds($startdate, $enddate);

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
          $line[] = $this->getWorkLoad(resourceutils::str2str($w['startdate']),resourceutils::str2str($w['enddate']),$r['employee'],$this->m_projectId);
        }
        $data[] = array("data"=>$line,"id"=>$r['id'],"type"=>$r['type'],"name"=>$r['name'],"employeeid"=>$r['employee']);
      }
    }

    protected function getMonthView($startdate, $enddate,&$header, &$data, &$min_width, $employee='', $package ='')
    {
      $this->handleDateBounds($startdate, $enddate);

      $this->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employee);

      $rows = $this->getRows();

      //Header creation
      $header[] = $this->text("plan");
      $header[] = $this->text("fact");
      //Header row
      $months = resourceutils::monthsBetween($startdate, $enddate);
      $min_width = $this->getMinWidth(count($months));

      foreach ($months as $month)
      {
        $header[] = $month['month'];
      }

      //Data rows
      foreach ($rows as $r)
      {
        $line = array();
        $line[] = atkDurationAttribute::_minutes2string($this->getPlan($r['type'],$r['id'],$r['employee']));
        $line[] = atkDurationAttribute::_minutes2string($this->getFact($r['type'],$r['id'],$r['employee']));
        foreach ($months as $month)
        {
          $line[] = $this->getWorkLoad(resourceutils::str2str($month['startdate']),resourceutils::str2str($month['enddate']),$r['employee'],$this->m_projectId);

        }
        $data[] = array("data"=>$line,"id"=>$r['id'],"type"=>$r['type'],"name"=>$r['name'],"employeeid"=>$r['employee']);
      }
    }

    public function getTaskHours($start,$end, $employee='',$package='')
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

        //per employee
        $this->m_resourceHours['employee'][$r['employee']][$r['employee']]['plan'] += $plan;
        $this->m_resourceHours['employee'][$r['employee']][$r['employee']]['fact'] += $r['fact'];
      }
    }

    protected function getDateBounds(&$startdate, &$enddate)
    {
      $startdate = $this->getDate("startdate");
      $enddate = $this->getDate("enddate");

      if ($startdate=="")
      {
        $month = date("m");
        $year = date("Y");
        switch ($this->getViewMode())
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
        switch ($this->getViewMode())
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

    function handleDateBounds(&$startdate, &$enddate)
    {
      switch($this->getViewMode())
      {
        case "week":
          $startdate = dateutil::startOfWeek(dateutil::arr2str((dateutil::str2arr($startdate))));
          $enddate = dateutil::endOfWeek(dateutil::arr2str((dateutil::str2arr($enddate))));
          break;
        case "month":
          $startdate = date("Ymd", mktime(0,0,0,date("m",dateutil::str2stamp($startdate)),1,date("Y",dateutil::str2stamp($startdate))));
          $enddate = date("Ymd", mktime(0,0,0,date("m",dateutil::str2stamp($enddate))+1,0,date("Y",dateutil::str2stamp($enddate))));
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
      $view = $this->getViewMode();

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

    /**
     * Collect package and task - child of project for currect user and save it into $this->m_parentChild
     *
     * @param integer $id - projectId
     * @param integer $employee - employeeId
     */
    function getProjectChild($id,$employee)
    {
      if(!isset($this->m_parentChild['project.project'][$id]))
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
    }

    protected function registerStyle()
    {
      $page = &atkinstance("atk.ui.atkpage");
      $theme = &atkinstance("atk.ui.atktheme");
      $page->register_style($theme->stylePath("style.css"));
      $page->register_style($theme->stylePath("tabs.css"));
    }

    protected function getVarArray()
    {
      return array( 'projectmenu' => $this->getProjectMenu(),
                    'projectid'=>$this->m_projectId,
                    'viewmode'=>$this->getModeSelector(),
                    'view'=>$this->getViewMode());
    }

    protected function getRowsQuery()
    {
      return "SELECT
          'employee' type, p.id employee, p.id, p.lastname,p.firstname
        FROM
          project_person pp
  	      JOIN person p ON p.id=pp.personid
        WHERE
            pp.projectid='$this->m_projectId'
            AND p.status='active'";
    }

  }
?>
