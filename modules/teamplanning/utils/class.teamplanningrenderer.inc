<?php

class teamPlanningRenderer
{
  var $m_items          = array();
  var $m_headerfields   = array();
  var $m_weeks          = array();  
  
  var $m_dataprefix     = "";
  var $m_olddataprefix  = "";
  
  function teamPlanningRenderer($dataprefix,$olddataprefix)
  {
    $this->m_dataprefix     = $dataprefix;
    $this->m_olddataprefix  = $olddataprefix;
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
  
  function setHeader($headerfields,$weeks)
  {
    $this->m_headerfields = $headerfields;
    $this->m_weeks        = $weeks;
  }
  
  function render()
  {
    $result = "";
    if($this->m_headerfields != array() && $this->m_weeks != array())
    {
      //render headers and weeknumbers
      foreach($this->m_items as $item)
      {        
        $result.= $item->render($this->m_headerfields,$this->m_weeks,$this->m_dataprefix,$this->m_olddataprefix);
      }
    }
    else     
      $result.= "<h1>cannot render planning, unknown header and/or weekfields</h1>";
    
    return $result;    
  }
  
}
?>