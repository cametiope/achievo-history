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
    $query->addJoin("
          (SELECT 
            MIN(h.activitydate) first_hr, 
            MAX(h.activitydate) last_hr, 
            t.projectid, 
            t.id,
            h.userid 
          FROM
             hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr
          GROUP BY t.projectid)","s","s.projectid=project.id AND s.id=phase.id",$flag);

    $query->addJoin("
          (SELECT 
            SUM(h.time) spend_hours, 
            
            t.projectid
          FROM
             hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr1
          GROUP BY t.projectid)","sss","sss.projectid=project.id",true);
    
    $query->addJoin("
          (SELECT 
            SUM(t.initial_planning) `initial_planning`, 
            SUM(t.current_planning) `current_planning`, 
            t.projectid
          FROM 
            phase t
            $wherestr1
          GROUP BY t.projectid)","ss","ss.projectid=project.id AND s.id=phase.id",$flag1);
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
    $this->add(new atkDateAttribute("startdate"));
    $this->add(new atkDateAttribute("enddate"));

    $this->add(new hrFirstAttribute("first_hr"));
    $this->add(new hrLastAttribute("last_hr"));
    
    $this->add(new planningAttribute("initial_planning","","",AF_TOTAL));
    $this->add(new planningAttribute("current_planning","","",AF_TOTAL));
    $this->add(new spendAttribute("spend_hours","","",AF_TOTAL));

    $this->setTable("project");   
  }

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