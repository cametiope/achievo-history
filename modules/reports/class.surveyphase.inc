<?php

atkimport("module.project.phase");

class hrFirstAttributes extends atkDateAttribute 
{
  public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
  {
    /*@var $query atkQuery*/
    $query->addField("first_hr","","s");
    $query->addField("last_hr","","s");
    
    $filter = $this->getOwnerInstance()->m_filterrecord;
    
    $where = array();
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
    
    $wherestr = implode(" AND ",$where);
    if($wherestr) $wherestr = "AND ".$wherestr;
    
    $query->addJoin("
          (SELECT 
            min(h.activitydate) first_hr,
            max(h.activitydate) last_hr,
            t.projectid,
            h.userid,
            t.id phaseid
           FROM 
             hoursbase h JOIN phase t ON h.phaseid=t.id $wherestr
             GROUP BY t.id)","s","s.projectid=projectid.id AND phase.id=s.phaseid",$flag);
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
  

  
}
?>