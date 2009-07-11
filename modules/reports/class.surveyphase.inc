<?php

atkimport("module.project.phase");

class hrFirstAttributes extends atkDateAttribute 
{
  public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
  {
    /*@var $query atkQuery*/
    $query->addField("first_hr","","s");
    $query->addField("last_hr","","s");
    $query->addField("spend_hours","","ss");
    
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
      $flag = false;
    }
    if(!empty($filter['description']))
    {
      $where[] = "LOWER(t.description) LIKE LOWER('%".$filter['description']."%')";
      $where1[] = "LOWER(t.description) LIKE LOWER('%".$filter['description']."%')";
      $flag = false;
    }
    
    $wherestr = implode(" AND ",$where);
    if($wherestr) $wherestr = "WHERE ".$wherestr;
    
    $wherestr1 = implode(" AND ",$where1);
    if($wherestr1) $wherestr1 = "WHERE ".$wherestr1;
    
    $query->addJoin("
          (SELECT 
            MIN(h.activitydate) first_hr,
            MAX(h.activitydate) last_hr,
            t.projectid,
            h.userid,
            t.id phaseid
           FROM 
             hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr
             GROUP BY t.id)","s","s.projectid=projectid.id AND phase.id=s.phaseid",$flag);

    $query->addJoin("
          (SELECT 
            SUM(h.time) spend_hours, 
            t.projectid,
            t.id phaseid
           FROM 
             hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr1
             GROUP BY t.id)","ss","ss.projectid=projectid.id AND phase.id=ss.phaseid",true);
  }

  function getOrderByStatement($extra='', $table='', $direction='ASC')
  {
    return "s.".$this->fieldName().($direction ? " {$direction}" : "");
  }
}

class hrLastAttributes extends atkDateAttribute 
{
  public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
  {
  }

  function getOrderByStatement($extra='', $table='', $direction='ASC')
  {
    return "s.".$this->fieldName().($direction ? " {$direction}" : "");
  }
}

class coordinator extends atkManyToOneRelation 
{
  public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
  {
    $alias = $fieldaliasprefix . $this->fieldName();
    $query->addJoin($this->m_destInstance->m_table,
                    $alias,
                    $alias.".id=projectid.coordinator",
                    $this->m_leftjoin);
    $this->m_destInstance->addToQuery($query, $alias, $level+1, false, $mode);
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
      $params["phaseid"] = "phase.id='".$record['id']."'";
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

      $org = href(dispatch_url("reports.hoursurvey", "report", $params), $org, SESSION_NESTED, ($mode!="list"));
    }
    return $org;
  }
}

class surveyphase extends phase 
{
  public $m_filterrecord;
    
  function __construct($name='surveyproject')
  {
    parent::phase($name,NF_NO_SEARCH);
    
    $this->add(new hrFirstAttributes("first_hr"));
    $this->add(new hrLastAttributes("last_hr"));
     
    $this->getAttribute("projectid")->removeFlag(AF_HIDE_LIST)->addFlag(AF_RELATION_AUTOLINK);
    $this->add(new coordinator("coordinator","employee.employee"));
    $this->getAttribute("virtual_time")->addFlag(AF_HIDE_LIST);  
    $this->getAttribute("status")->addFlag(AF_HIDE_LIST);  
    $this->getAttribute("duration")->addFlag(AF_HIDE_LIST);  
    $this->getAttribute("dependsondeliverable")->addFlag(AF_HIDE_LIST);  
    $this->getAttribute("name")->setLabel($this->text("phase"));  
    
    $this->remove("spend_hours");
    $this->add(new planningAttribute("spend_hours","","",AF_TOTAL));
    
    $this->setAttributesOrder("projectid","name","coordinator","startdate","enddate","first_hr","last_hr");
      
    $this->setSecurityAlias('project.phase');
  }

  /**
   * Set the order of all the attributes.
   *
   * This method supports a variable number of attributenames, the attributes are ordered in the
   * order they are given to this method. Attributes that are not provided will preserve their
   * original order, but are placed at the bottom of the page.
   */
  function setAttributesOrder()
  {
    $attributenames = func_get_args();
    // Get the attributes that were not passed to this method
    $otherattributes = array_diff($this->getAttributeNames(), $attributenames);

    $order = 5;
    foreach ($attributenames as $name)
    {
      $this->setAttributeOrder($name, $order);
      $order+=5;
    }

    foreach ($otherattributes as $name)
    {
      $this->setAttributeOrder($name, $this->getAttributeOrder($name)+($order+5));
    }
  }
  
  function rowColor($record)
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