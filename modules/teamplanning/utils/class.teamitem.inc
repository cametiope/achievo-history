<?php

class teamItem
{
  var $m_title              = "";
  var $m_id                 = 0;
  var $m_items              = array();
  var $m_capacity_max       = array();
  var $m_capacity_available = array();
  var $m_capacity_percentage= array();  
  var $m_headerfields       = array();
  var $m_weeknames          = array();  
    
  function teamItem($id)
  {
    $this->m_id = $id;
  }
  
  function addItem($item)
  {
    $this->m_items[] = $item;
  }
  
  function setTitle($title)
  {
    $this->m_title = $title;
  }
    
  function setMaxCapacity($teamcapacity)
  {
    $this->m_capacity_max = $teamcapacity;
  }
  
  function render($headerfields, $weeknames)
  {
    $this->m_headerfields = $headerfields;
    $this->m_weeknames    = $weeknames;

    /*  
      Calculate data    
    */    
    $this->_calculateCapacity();    
    
    /*  
      Render title  
    */
    $result = "<h1>".$this->m_title."</h1>";
    $result.= '<table id="team_'.$this->m_id.'" class="recordlist">';
    
    /*
      Render headers and weeknumbers
    */    
    $result.= '<tr class="row2">';
    foreach($headerfields as $field)  $result.= '<th>'.$field."</th>";        
    foreach($weeknames as $week)      $result.= '<th>'.$week."</th>";    
    $result.= '</tr>';
    
    /*
      Render projects
    */
    foreach($this->m_items as $item)
      $result.= $item->render($headerfields, $weeknames);
      
    /*
      Render capacity rows
    */
    //max capacity
    $result.= '<tr class="row2">';
    $result.= '<th colspan="'.count($headerfields).'">'.atktext("max_capacity","teamplanning").'</th>';
    foreach($this->m_weeknames as $week)    
      $result.= '<th>'.$this->m_capacity_max["weekspeed"][$week].'</th>';
    $result.= '</tr>';        
    
    //available capacity  
    $result.= '<tr class="row2">';    
    $result.= '<th colspan="'.count($headerfields).'">'.atktext("available_capacity","teamplanning").'</th>';
    foreach($this->m_weeknames as $week)    
      $result.= '<th>'.$this->m_capacity_available["weekspeed"][$week].'</th>';
    $result.= '</tr>';
    
    //percentage capacity
    $result.= '<tr class="row2">';    
    $result.= '<th colspan="'.count($headerfields).'">'.atktext("percentage_capacity","teamplanning").'</th>';
    foreach($this->m_weeknames as $week)
    {
      $percentage = $this->m_capacity_percentage["weekspeed"][$week];
      
      $color = "";
      if(($percentage > 100) || ($percentage<0))  $color = "#FF0000";
      else                                        $color = "#00FF00";
      
      $result.= '<th><font color="'.$color.'">'.$percentage.'%</font></th>';
    }
    $result.= '</tr>';
    
    $result.= '</table>';
    
    
    return $result;
  }  
  
  function _calculateData()
  {
    $this->_calculateCapacity();
  }
  
  function _calculateCapacity()
  {    
    foreach($this->m_weeknames as $week)
    {
      $max      = $this->m_capacity_max["weekspeed"][$week];

      $sumprojects = 0;
      //calculate the total of all projects
      for($i=0,$_i=count($this->m_items);$i<$_i;$i++)
      {
        $item = &$this->m_items[$i];
        $itemhours = isset($item->m_weekdata[$week]["time"]) ? $item->m_weekdata[$week]["time"] : 0;
        $sumprojects += $item->m_weekdata[$week]["time"];
        
        //@todo: count total hours of planned phases and take max
      }
      $planned = $sumprojects/60;

      $this->m_capacity_available["weekspeed"][$week] = $max-$planned;
      $this->m_capacity_percentage["weekspeed"][$week] = round(($planned / $this->m_capacity_max["weekspeed"][$week]) * 100,0);
    }
  }
}
?>