<?php

  /** 
   * the dummyEditAttribute usefull for getting input from user without saving to db
   */
  
  class dummyEditAttribute extends atkAttribute
  {
    function dummyEditAttribute($name, $flags = 0, $size = 0)
    {
      $this->atkAttribute($name, $flags|AF_HIDE_VIEW|AF_HIDE_EDIT|AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_SELECT, $size);
    }
    
    // Dummy methods to prevent loading/storing of data.
    function load() {}    
    function addToQuery() {}
    function hide() {}
    function search() {}
    function getSearchModes() { return array();}
    function searchCondition() {}
    function getSearchCondition() {}
    function fetchMeta() {}
    function dbFieldSize() {}
    function dbFieldType() {}
  }

?>