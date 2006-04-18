<?php

atkimport('atklistattribute');
class scheduletypeDummyListAttribute extends atkListAttribute 
{
  function scheduletypeDummyListAttribute($name, $flags=0, $size=0)
  {
    $options = array(); $values= array();
    $this->_getScheduleItems($options,$values);
    $this->atkListAttribute($name, $options,$values,$flags, $size);    
  }

  /**
    * No function, but is neccesary
    */
  function store($a, $b)
  {
    return true;
  }

  function db2value($record)
  {
    $scheduleid = $record["scheduleid"]["id"];   
    /* @var $db atkDb */
    $db = &atkGetDb();
    $query = $db->createQuery();
    $query->addField("scheduletype","","schedule");
    $query->addTable("schedule");
    $query->addCondition("id='$scheduleid'");
    $schedulerecord = $db->getrows($query->buildSelect());
    $scheduletypeid = $schedulerecord[0]["scheduletype"];
    return $scheduletypeid;
  }

  /**
    * No function, but is neccesary
    */
  function addToQuery(&$query, $tablename="", $fieldaliasprefix="",$level)
  {
  }

  function getSearchModes()
  {    
    return array();
  }

  function dbFieldType()
  {
    return "";
  }
  
  function _getScheduleItems(&$options, &$values)
  {
    /* @var $node atkNode */
    $node = &getNode("calendar.schedule_types");
    $records = $node->selectDb();
    
    foreach($records as $rec)
    {
      $values[]   = $rec["id"];
      $options[]  = $node->descriptor($rec);
    }    
  }
}

?>