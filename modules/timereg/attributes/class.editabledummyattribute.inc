<?php

  atkimport("atk.attributes.atkdummyattribute");
  class editableDummyAttribute extends atkDummyAttribute
  {    
    function editableDummyAttribute($name, $text="", $flags=0)
    {
    	parent::atkDummyAttribute($name,$text,$flags);
    }

    function edit($record="", $fieldprefix="")
    {
      return "<div ID=\"$this->m_name\">".$record[$this->fieldName()]."</div>";
    }

    function display($record)
    {
      return $record[$this->fieldName()];
    }
  }
?>
