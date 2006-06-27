<?php

atkimport('atklistattribute');
class dummyListAttribute extends atkListAttribute 
{
  function dummyListAttribute($name, $flags=0, $size=0)
  {
    $options = array(); $values= array();
    $this->_getDummyItems($options,$values);
    $this->atkListAttribute($name, $options,$values,$flags, $size); 
    $this->removeFlag(AF_OBLIGATORY);   
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
    return NULL;
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
  
  function _getDummyItems(&$options, &$values)
  {
    $options = array("dummy","dummy","dummy","dummy","dummy");
    $values = array(0,0,0,0,0);   
  }
  
  function fieldName()
  {
    return "taskid[".$this->m_name."]";    
  }
}

?>