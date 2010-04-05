<?php

userelation("atkmanytoonerelation");

useattrib("atkdateattribute");
useattrib("atkdurationattribute");

class hrFirstAttribute extends atkDateAttribute 
{
  public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
  {
    /*@var $query atkQuery*/
    $query->addJoin("phase","","phase.projectid=project.id");
    
    $query->addField("first_hr","","s");
    $query->addField("last_hr","","s");
    $query->addField("initial_planning","","ss");
    $query->addField("current_planning","","ss");
    $query->addField("startdate","","ss");
    $query->addField("enddate","","ss");
    $query->addField("spend_hours","","sss");
    
    $filter = $this->getOwnerInstance()->m_filterrecord;
    
    $where = $where1 = array();
    $flag = $flag1 = true;    
    
    //filter for hours only
    if(!empty($filter['from']))
    {
      $from = dateutil::arr2str($filter["from"],"Y-m-d");
      $where[] = "h.activitydate >= '$from'";
      $flag = false;
    }
    if(!empty($filter['to']))
    {
      $to = dateutil::arr2str($filter["to"],"Y-m-d");
      $where[] = "h.activitydate <= '$to'";
      $flag = false;
    }
    if(isset($filter['worked_on_by']['id']))
    {
      $where[] = "h.userid='".$filter['worked_on_by']['id']."'";
      $flag = false;
    }
    //filter for phase only
    if($filter['phasestatus']<>'all')
    {
      $where[] = "t.status = '".$filter['phasestatus']."'";
      $where1[] = "t.status = '".$filter['phasestatus']."'";
      $flag = $flag1 = false;
    }
    if(!empty($filter['description']))
    {
      $where[] = "LOWER(t.description) LIKE LOWER('%".$filter['description']."%')";
      $where1[] = "LOWER(t.description) LIKE LOWER('%".$filter['description']."%')";
      $flag = $flag1 = false;
    }
    
    $wherestr = implode(" AND ",$where);
    if($wherestr) $wherestr = "WHERE ".$wherestr;

    $wherestr1 = implode(" AND ",$where1);
    if($wherestr1) $wherestr1 = "WHERE ".$wherestr1;
    
    //always left join, because we don't add this subquery for adding some limits, we simply add columns 
    /*$query->addJoin("
          (SELECT DISTINCT
            t.projectid, 
            t.id phaseid
          FROM
             hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr
          )","s1","s1.projectid=project.id AND s1.phaseid=phase.id",false);
*/
    $query->addJoin("
          (SELECT
            MIN(h.activitydate) first_hr, 
            MAX(h.activitydate) last_hr, 
            t.projectid,
            t.id phaseid
          FROM
             hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr
          GROUP BY t.projectid)","s","s.projectid=project.id AND s.phaseid=phase.id",false);

    $query->addJoin("
          (SELECT 
            SUM(h.time) spend_hours, 
            t.projectid,
            t.id phaseid
          FROM
             hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr1 AND t.id IN 
             (SELECT 
              h.phaseid
              FROM hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr)
          GROUP BY t.projectid)","sss","sss.projectid=project.id AND sss.phaseid=phase.id",false);
    
    $query->addJoin("
          (SELECT 
            SUM(t.initial_planning) `initial_planning`, 
            SUM(t.current_planning) `current_planning`, 
            MIN(t.startdate) startdate,
            MAX(t.enddate) enddate,
            t.projectid,
            t.id phaseid
          FROM 
            hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr
          GROUP BY t.projectid)","ss","ss.projectid=project.id AND ss.phaseid=phase.id",false);
  }

  function getOrderByStatement($extra='', $table='', $direction='ASC')
  {
    return "s.".$this->fieldName().($direction ? " {$direction}" : "");
  }
}

class hrLastAttribute extends atkDateAttribute 
{
  public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
  {
  }

  function getOrderByStatement($extra='', $table='', $direction='ASC')
  {
    return "s.".$this->fieldName().($direction ? " {$direction}" : "");
  }
}

class planningAttribute extends atkDurationAttribute 
{
  public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
  {
  }

  function getOrderByStatement($extra='', $table='', $direction='ASC')
  {
    return "ss.".$this->fieldName().($direction ? " {$direction}" : "");
  }
}

class spendAttribute extends atkDurationAttribute 
{
  public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
  {
  }

  function getOrderByStatement($extra='', $table='', $direction='ASC')
  {
    return "s.".$this->fieldName().($direction ? " {$direction}" : "");
  }
  /**
   * Display override. If possible, displays a link to the hoursurvey to view details.
   * @param array $record Record
   * @param string $mode mode
   * @return string
   */
  function display($record, $mode)
  {
    $value = $record[$this->fieldName()];
    $org = parent::display($record, $mode);

    $filter = $this->getOwnerInstance()->m_filterrecord;
    
    if ($value>0)
    {
      $params["projectid"] = "project.id='".$record['id']."'";
      $params["startdate"] = "2000-01-01";
      $params["enddate"] = date("Y-m-d");
      
      if(isset($filter['worked_on_by']['id']))
      {
        $params["userid"] = $filter['worked_on_by']['id'];
      }
      else
      {
        $params["userid"] = "all";
      }

      $org = atkHref(dispatch_url("reports.hoursurvey", "report", $params), $org, SESSION_NESTED, ($mode!="list"));
    }
    return $org;
  }
}

class surveyproject extends atkNode 
{
  public $m_filterrecord;
  public function __construct($name="surveyproject",$flags=NF_READONLY)
  {
    parent::atkNode($name,$flags);
    
    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("abbreviation", AF_HIDE_LIST));
    $this->add(new atkAttribute("name"));
    $this->add(new atkManyToOneRelation("coordinator","employee.employeeselector"));
    
    /**
     * If we display only the project, it's the first and last hour booked on the
     * project. If we show all phases, then it's the first and last hour on the phase.
     * Same for the dates.
     */
    
    $this->add(new hrLastAttribute("startdate"));
    $this->add(new hrLastAttribute("enddate"));
    
    /**
     * First hr/last hr are the dates of the first and last registration
     */
    $this->add(new hrFirstAttribute("first_hr"));
    $this->add(new hrLastAttribute("last_hr"));
    
    /**
     * Initial, current and actual are the value columns that need to have a total at
     * the bottom of the report.
     */
    $this->add(new planningAttribute("initial_planning","","",AF_TOTAL));
    $this->add(new planningAttribute("current_planning","","",AF_TOTAL));
    
    /**
     * Similar to project admin phase list, the 'actual' should be a link 
     * to the hoursurvey to view details.
     * If we set filter for period (from/to dates) - anyway we have to see
     * total number there - I want to select projects that have been worked on last week for
     * example, but then I still want to see their total numbers so I can know if it's
     * on track.
     */
    $this->add(new spendAttribute("spend_hours","","",AF_TOTAL));

    $this->setTable("project");   
  }

  /**
   * Project should be a link to project edit (take security into account, 
   * not everyone is allowed to edit projects; if not, use view, but also with
   * security)
   *
   * @param unknown_type $record
   * @param unknown_type $mode
   * @return unknown
   */
  public function name_display($record, $mode)
  {
    $value = $record["name"];
    $org = project::descriptor($record);

    if ($value>'')
    {
      $node = atkGetNode("project.project");
      if($node->allowed("edit"))
      {
        $action = "edit";
      }
      elseif($node->allowed("view"))
      {
        $action = "view";
      }
      
      if(isset($action))
      {
        $params["atkselector"] = "project.id='".$record['id']."'";
        $org = atkHref(dispatch_url("project.project", $action, $params), $org, SESSION_NESTED, ($mode!="list"));
      }
    }
    return $org;
  }

  /**
   * Row should be colored the same as in the project admin phase list
   *
   * @param array $record for coloring
   * @return string with color code
   */
  public function rowColor($record)
  {
    if($record["current_planning"]>0)
    {
      if($record["spend_hours"]>($record["current_planning"]))
      {
        return COLOR_ERROR;
      }
      elseif($record["spend_hours"]>=($record["current_planning"])*0.9)
      {
        return COLOR_WARNING;
      }
    }
  }
}