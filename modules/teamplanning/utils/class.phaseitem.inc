<?php

class phaseItem
{
  //var $m_items          = array();
  var $m_id             = 0;
  var $m_teamid         = 0;
  var $m_projectid      = 0;
  var $m_headerfields   = array();
  var $m_headerdata     = array();
  var $m_weeks          = array();
  var $m_weekdata       = array();
  
  var $m_dataprefix     = "";
  var $m_olddataprefix  = "";
    
  //calculated vars
  var $m_allactivitieshaveestimation = true;
  var $m_calculatedcurrentplanning = array("low"=>"?","high"=>"?");
  var $m_calculated     = array();
  
  function phaseItem($id, $teamid, $projectid)
  {
    $this->m_id         = $id;
    $this->m_teamid     = $teamid;
    $this->m_projectid  = $projectid;
  }
  
  function addItem($item)
  {
    $this->m_items[] = $item;
  }
  
  function setHeaderData($headerdata)
  {
    $this->m_headerdata = isset($headerdata) ? $headerdata : array();
  }
  
  function setWeekData($weekdata)
  {
    $this->m_weekdata = isset($weekdata) ? $weekdata : array();
  }
    
  function render($headerfields, $weeks, $dataprefix, $olddataprefix)
  {
    $this->m_headerfields   = $headerfields;
    $this->m_weeks          = $weeks;
    $this->m_dataprefix     = $dataprefix;
    $this->m_olddataprefix  = $olddataprefix;
    
    $this->_calculateData();
        
    /*
      Render header field values such as the name 
      Rowid:      Teamid_projectid_phaseid_0
      Colid:      teamid_projectid_phaseid_0_fieldname
    */   
    $rowid = $this->m_teamid."_".$this->m_projectid."_".$this->m_id."_0";
    $result = '<tr class="row1" id="'.$rowid.'" style="display:none">';
    foreach($this->m_headerfields as $field)
    {
      $inputid = $this->m_dataprefix.$rowid."_".$this->_uus($field);
      $output = "";
      switch ($field)
      {
        case "name":
          $output.= "<td>     ".$this->m_headerdata[$field]."</td>";
          break;
        case "current_planning":
          $output = $this->m_calculated["current_planning"]["low"]/60;
          if($this->m_calculated["current_planning"]["high"] != "")
            $output.= " / ".$this->m_calculated["current_planning"]["high"]/60;
          $output = '<td align="center"><div id="'.$inputid.'">'.$output.'</div></td>';
          break;
        case "current_min_planned":
          $output = $this->m_calculated["current_min_planned"]["low"]/60;
          if($this->m_calculated["current_min_planned"]["high"] != "")
            $output.= " / ".$this->m_calculated["current_min_planned"]["high"]/60; 
          $output = '<td align="center"><div id="'.$inputid.'">'.$output.'</div></td>';
          break;
        default:
          $output = '<td align="center">'.(isset($this->m_headerdata[$field]) ? $this->m_headerdata[$field]/60 : "0").'</td>';
      }
      $result.= $output;
    }
    
    /*  
      Render planned week values 
      Rowid:      Teamid_projectid_phaseid_0
      Colid:      teamid_projectid_phaseid_0_weekid
    */    
    foreach($this->m_weeks as $weekitem)    
    {
      $week = $weekitem["week"];
      $inputid = $rowid."_".$week;
      $record[$this->m_dataprefix.$inputid] = isset($this->m_weekdata[$week]["time"]) ? $this->m_weekdata[$week]["time"]/60 : 0;
      $att = &new atkNumberAttribute($this->m_dataprefix.$inputid,0,4,0);
      $result.= '<td>'.$att->edit($record).$this->_hiddenAttr($this->m_olddataprefix.$inputid,$record[$this->m_dataprefix.$inputid]).'</td>';
    }    
    $result.= "</tr>";    
    
    //temp: render activities
    foreach($this->m_items as $item)    
      $result.= $item->render($headerfields, $this->m_weeks, $this->m_dataprefix, $this->m_olddataprefix);
    
    //@todo: render phasebuffer
    /*  
      Render the phasebuffer  
      Rowid:      Teamid_projectid_phaseid_buffer
      Colid:      teamid_projectid_phaseid_buffer_weekid
    */    
    return $result;    
  }
  
  function _calculateData()
  {
    $this->_calculateEstimation();
    $this->_calculateCurrentPlanning();
    $this->_calculateCurrentMinPlanned();
  }
  
  function _calculateEstimation()
  {
    $this->m_allactivitieshaveestimation = true;
    $sum = 0;    
    //calculate the total current_planning(=estimations) of all activities
    for($i=0,$_i=count($this->m_items);$i<$_i;$i++)
    {
      $item = &$this->m_items[$i];
      if(!isset($item->m_headerdata["current_planning"]))
        $this->m_allactivitieshaveestimation = false;
      else
        $sum += $item->m_headerdata["current_planning"];
    }
    $this->m_headerdata["estimation"] = $sum;    
  }
  
  function _calculateCurrentMinPlanned()
  {
    $sum = 0;
    foreach($this->m_weekdata as $week=>$weekitem)
    {
      //$week = $weekitem["week"];
      //atk_var_dump($this->m_weekdata[$week],"\$this->m_weekdata[$week]");
      if(isset($this->m_weekdata[$week]))
        $sum+= $this->m_weekdata[$week]["time"];
    }
    $this->m_calculated["current_min_planned"]["low"] = $this->m_calculated["current_planned"]["low"] - $sum;
    if($this->m_calculated["current_planned"]["high"] != "")
      $this->m_calculated["current_min_planned"]["high"] = $this->m_calculated["current_planned"]["high"] - $sum;
    else 
      $this->m_calculated["current_min_planned"]["high"] = "";
  }
  
  /**
   * Get current planning values
   *
   * When all activities have an estimation, we show their total as the current planning, or else
   * we show a range where the current planning should be in depending on the current_planning that
   * was entered for the phase and the sum of the estimations of the activities.
   * If the initial_planning is omitted, we assume the current_planning is the same as the initial_planning.
   */
  function _calculateCurrentPlanning()
  {
    $value  = (isset($this->m_headerdata["current_planning"]) && $this->m_headerdata["current_planning"] !="" ) ? $this->m_headerdata["current_planning"] : "0";
    $value2 = (isset($this->m_headerdata["estimation"]) && $this->m_headerdata["estimation"] !="" ) ? $this->m_headerdata["estimation"] : "0";
    
    /* If we do not have a current_planning, we assume the initial planning is the current planning */
    if($value == "0")
    {
      $value = (isset($this->m_headerdata["initial_planning"]) && $this->m_headerdata["initial_planning"] !="") ? $this->m_headerdata["initial_planning"] : "0";
//      $this->m_calculated["current_planning"]["low"]   = (isset($this->m_headerdata["initial_planning"]) && $this->m_headerdata["initial_planning"] !="") ? $this->m_headerdata["initial_planning"] : "0";
//      $this->m_calculated["current_planning"]["high"]  = "";
    }        
        
    if(!$this->m_allactivitieshaveestimation)
    {      
      if($value != $value2)
      {
        if(max($value,$value2) == $value) 
        {
          $this->m_calculated["current_planning"]["low"]   = $value2;
          $this->m_calculated["current_planning"]["high"]  = $value;          
        }
        else 
        {
          $this->m_calculated["current_planning"]["low"]   = $value;
          $this->m_calculated["current_planning"]["high"]  = $value2;
        }          
      }
      else 
      {
        $this->m_calculated["current_planning"]["low"]   = $value;
        $this->m_calculated["current_planning"]["high"]  = "";
      }
    }    
    else    
    {
      $this->m_calculated["current_planning"]["low"]   = $value2;
      $this->m_calculated["current_planning"]["high"]  = "";
    }
  }
  
  
  /**
   * Un-UnderScore a string
   *   
   * Removes underscores from a string.
   * 
   * @param type name description
   * @return type description
   */
  function _uus($str)
  {
    return str_replace("_","",$str);
  }
  
  function _hiddenAttr($id, $value)
  {
    //create hidden attrib
    return '<input type="hidden" name="'.$id.'" value="'.$value.'" />';
  }  
}
?>