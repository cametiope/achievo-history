<?php

  useattrib("atkboolattribute");

  class dummyBoolAttribute extends atkBoolAttribute 
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function dummyBoolAttribute($name, $flags=0)
    {
      //we do not want to search on a dummy
      //if we provide this flag we do not need to implement the function searchModes()
      $flags |= AF_HIDE_SEARCH; 
      // Call base class constructor. 
      $this->atkBoolAttribute($name,$flags);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="", $fieldprefix="")
    {
      $checked = "";
      if (isset($record[$this->fieldName()]) && $record[$this->fieldName()]>0) $checked="checked";
      $id = $fieldprefix.$this->fieldName();
      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_CURSOR);
      return '<input type="checkbox" id="'.$id.'" name="'.$id.'" value="1" '.$checked.'>';
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
