<?php

  useattrib("atkboolattribute");

  class addMeAttribute extends atkBoolAttribute 
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function addMeAttribute($name, $flags=0)
    {
      // Call base class constructor. 
      $this->atkAttribute($name,$flags);
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
  }
?>
