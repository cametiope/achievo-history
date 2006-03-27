<?php

class teamPlanningRenderer
{
  var $m_items          = array();
  var $m_headerfields   = array();
  var $m_weeknames      = array();  
  
  function teamPlanningRenderer()
  {
    
  }
  
  /**
   * Add item
   *   
   * @param teamItem $item A teamItem object
   * @return none
   */
  function addItem($item)
  {
    $this->m_items[] = $item;
  }
  
  function setHeader($headerfields,$weeknames)
  {
    $this->m_headerfields = $headerfields;
    $this->m_weeknames    = $weeknames;
  }
  
  function render()
  {    
    $result = "";
    if($this->m_headerfields != array() && $this->m_weeknames != array())
    {
      //render headers and weeknumbers
      foreach($this->m_items as $item)
      {        
        $result.= $item->render($this->m_headerfields,$this->m_weeknames);
      }
    }
    else     
      $result.= "<h1>cannot render planning, unknown header and/or weekfields</h1>";
    
    return $result;    
  }  
  
  
}
?>