<?php

userelation("atkmanytoonerelation");

useattrib("atkdateattribute");
useattrib("atkdurationattribute");

class hrFirstAttribute extends atkDateAttribute 
{
  public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
  {
    /*@var $query atkQuery*/
    $query->addField("first_hr","","s");
    $query->addField("last_hr","","s");
    $query->addField("initial_planning","","ss");
    $query->addField("current_planning","","ss");
    $query->addField("spend_hours","","s");
    
    $filter = $this->getOwnerInstance()->m_filterrecord;
    
    $where = $where1 = array();
    $flag = true;
        
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
    if($filter['phasestatus']<>'all')
    {
      $where[] = "t.status = '".$filter['phasestatus']."'";
      $where1[] = "t.status = '".$filter['phasestatus']."'";
    }
    
    $wherestr = implode(" AND ",$where);
    if($wherestr) $wherestr = "WHERE ".$wherestr;

    $wherestr1 = implode(" AND ",$where1);
    if($wherestr1) $wherestr1 = "WHERE ".$wherestr1;
    
    $query->addJoin("
          (SELECT 
            MIN(h.activitydate) first_hr, 
            MAX(h.activitydate) last_hr, 
            SUM(h.time) spend_hours, 
            t.projectid, 
            h.userid 
          FROM
             hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr
          GROUP BY t.projectid)","s","s.projectid=project.id",$flag);
    
    $query->addJoin("
          (SELECT 
            SUM(t.initial_planning) `initial_planning`, 
            SUM(t.current_planning) `current_planning`, 
            t.projectid
          FROM 
            phase t
            $wherestr1
          GROUP BY t.projectid)","ss","ss.projectid=project.id",$flag);
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
    if($this->fieldName() == 'spend_hours') $table = 's';
    else $table = 'ss';
    return $table.".".$this->fieldName().($direction ? " {$direction}" : "");
  }
  
}

class surveyproject extends atkNode 
{
  public $m_filterrecord;
  public function __construct($name="surveyproject",$flags=NF_READONLY)
  {
    parent::atkNode($name,$flags);
    
    $this->add(new atkAttribute("name"));
    $this->add(new atkManyToOneRelation("coordinator","employee.employeeselector"));
    $this->add(new atkDateAttribute("startdate"));
    $this->add(new atkDateAttribute("enddate"));

    $this->add(new hrFirstAttribute("first_hr"));
    $this->add(new hrLastAttribute("last_hr"));
    
    $this->add(new planningAttribute("initial_planning","","",AF_TOTAL));
    $this->add(new planningAttribute("current_planning","","",AF_TOTAL));
    $this->add(new planningAttribute("spend_hours","","",AF_TOTAL));

    $this->setTable("project");   
  }
}