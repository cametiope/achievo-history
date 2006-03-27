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
  var $m_weeks              = array();  
  
  var $m_dataprefix     = "";
  var $m_olddataprefix  = "";
    
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
  
  function render($headerfields, $weeks, $dataprefix, $olddataprefix)
  {
    $this->m_headerfields   = $headerfields;
    $this->m_weeks          = $weeks;
    $this->m_dataprefix     = $dataprefix;
    $this->m_olddataprefix  = $olddataprefix;

    $headercolcount = count($headerfields);
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
    foreach($this->m_weeks as $weekitem)
    {
      $result.= '<th><input name="weekdate_'.$weekitem["week"].'" id="weekdate_'.$weekitem["week"].'" type="hidden" value="'.$weekitem["startdate"].'" />'.$weekitem["week"]."</th>";
    }
    $result.= '</tr>';
    
    /*
      Render projects
    */
    foreach($this->m_items as $item)
      $result.= $item->render($headerfields, $this->m_weeks, $this->m_dataprefix, $this->m_olddataprefix);
      
    /*
      Render capacity rows
    */
    //max capacity
    $result.= '<tr class="row2">';
    $result.= '<th colspan="'.$headercolcount.'">'.atktext("max_capacity","teamplanning").'</th>';
    foreach($this->m_weeks as $weekitem)
    {
      $week = $weekitem["week"];
      $id = implode("_",array($this->m_id,"maxcapacity",$week));
      $att = &new atkDummyAttribute($id,$this->m_capacity_max["weekspeed"][$week]);      
      $result.= '<th>'.$att->edit(NULL).'</th>';
    }
    $result.= '</tr>';        
    
    //available capacity  
    $result.= '<tr class="row2">';    
    $result.= '<th colspan="'.$headercolcount.'">'.atktext("available_capacity","teamplanning").'</th>';
    foreach($this->m_weeks as $weekitem)
    {
      $week = $weekitem["week"];
      $id = implode("_",array($this->m_id,"availablecapacity",$week));
      $att = &new atkDummyAttribute($id,$this->m_capacity_available["weekspeed"][$week]);      
      $result.= '<th>'.$att->edit(NULL).'</th>';
    }
    $result.= '</tr>';
    
    //percentage capacity
    $result.= '<tr class="row2">';    
    $result.= '<th colspan="'.$headercolcount.'">'.atktext("percentage_capacity","teamplanning").'</th>';
    foreach($this->m_weeks as $weekitem)
    {
      $week = $weekitem["week"];
      $percentage = $this->m_capacity_percentage["weekspeed"][$week];
      
      $color = "";
      if(($percentage > 100) || ($percentage<0))  $color = "#FF0000";
      else                                        $color = "#00FF00";
      
      $id = implode("_",array($this->m_id,"percentagecapacity",$week));
      $att = &new atkDummyAttribute($id,'<font color="'.$color.'">'.$percentage.'%</font>');      
      $result.= '<th>'.$att->edit(NULL).'</th>';
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
    foreach($this->m_weeks as $weekitem)
    {
      $week = $weekitem["week"];
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