<?php
   
  class memoAttribute extends atkAttribute
  {
    
    public function __construct( $name, $flags = 0, $size = 60 )
    {
      parent::atkAttribute( $name, $flags, $size );
    }    
    
    
    /**
     * Override the addToQuery method; since the memoattribute does not have a related
     * column in the node's table (memo has it's own table)
     * 
     * @return null;
     */
    public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      return;
    }
    
  }