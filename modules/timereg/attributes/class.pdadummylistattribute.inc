<?php

atkimport('atklistattribute');
class PDADummyListAttribute extends atkListAttribute 
{
  function PDADummyListAttribute($name, $options, $values, $flags=0, $size=0)
  {
    $this->atkListAttribute($name, $options,$values,$flags, $size);    
  }
  
  /**
   * This attribute does not need to be stored.
   * If we return NOSTORE at storageType and 0 (NOLOAD) at loadType, 
   * we do not need to override other functions like store(), 
   * addToQuery() etc.     
   *
   * @return int number
   */
  function storageType() { return NOSTORE; }
  function loadType()    { return 0; }
}

?>