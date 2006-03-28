<?php

class projectItem
{
  var $m_items          = array();  //phases
  var $m_id             = 0;
  var $m_teamid         = 0;
  var $m_headerfields   = array();  
  var $m_headerdata     = array();
  var $m_weeks          = array();
  var $m_weekdata       = array();
  var $m_conflicts      = array();
  
  //some fields
  var $m_openimagesrc   = "";
  var $m_closeimagesrc  = "";
  var $m_regopen        = "";
  var $m_regclose       = "";
  
  var $m_dataprefix     = "";
  var $m_olddataprefix  = "";
  
  //calculated fields
  var $m_unassigned_projecthours = array();
  
  var $m_allactivitieshaveestimation = true;  
  
  function projectItem($projectid, $teamid)
  {
    $this->m_id = $projectid;
    $this->m_teamid = $teamid;
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
    //atk_var_dump($this->m_weekdata,"<h1>har tah weekdata</h1>");
  }
    
  function setConflicts($conflicts)
  {
    $this->m_conflicts = $conflicts;
  }
  
  function render($headerfields, $weeks, $dataprefix, $olddataprefix)
  {
    $this->m_headerfields   = $headerfields;
    $this->m_weeks          = $weeks;
    $this->m_dataprefix     = $dataprefix;
    $this->m_olddataprefix  = $olddataprefix;
    
    $numheaderfields = count($this->m_headerfields);
    $this->_init();        
            
    /* 
      calculate data 
    */
    $this->_calculateData();        
    
    /*  
      Render header field values such as the name 
      Rowid:      Teamid_projectid_0_0
      Colid:      teamid_projectid_0_0_fieldname
    */                      
    $rowid = $this->m_teamid."_".$this->m_id."_0_0";
    $result = '<tr class="row2" id="'.$rowid.'">';    
    foreach($this->m_headerfields as $field)
    {
      
      $output = "";
      switch ($field)
      {
        case "name":
          $output.= '<td>'.$this->_clickableImages().' <b>'.(isset($this->m_headerdata[$field]) ? $this->m_headerdata[$field] : "?").'</b></td>';
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
      Rowid:      Teamid_projectid_0_0
      Colid:      teamid_projectid_0_0_weekid
    */
    $rowid = $this->m_teamid."_".$this->m_id."_0_0";
    foreach($this->m_weeks as $weekitem)
    {
      $week = $weekitem["week"];
      $inputid  = $rowid."_".$week;      
      $record[$this->m_dataprefix.$inputid] = (isset($this->m_weekdata[$week]["time"]) ? $this->m_weekdata[$week]["time"]/60 : 0);;
      $att      = &new atkNumberAttribute($this->m_dataprefix.$inputid,0,4,0);      
      $result.= '<td>'.$att->edit($record).$this->_hiddenAttr($this->m_olddataprefix.$inputid,$record[$this->m_dataprefix.$inputid]).'</td>';
    }
    $result.= "</tr>";

    /*  
      Render phases 
    */
    foreach($this->m_items as $item)    
      $result.= $item->render($headerfields, $this->m_weeks, $this->m_dataprefix, $this->m_olddataprefix);
    
    /*  
      Render unassigned project hours 
      Rowid:      Teamid_projectid_0_0_unassigned
      Colid:      teamid_projectid_0_0_unassigned_weekid
    */
    $rowid = $this->m_teamid."_".$this->m_id."_0_0_unassigned";
    $result.= '<tr class="row1" id="'.$rowid.'" style="display:none">';
    $result.= '<td colspan="'.$numheaderfields.'">  <b>'.atktext("unassignedprojecthours","teamplanning").'</b></td>';
    foreach($this->m_weeks as $weekitem)
    { 
      $week = $weekitem["week"];
      $inputid = $rowid."_".$week;
      $result.= '<td><div id="'.$inputid.'">'.($this->m_unassigned_projecthours[$week]/60).'</div></td>';
    }
    $result.= '</tr>';
      
    /*  
      Render the projectbuffer  
      Rowid:      Teamid_projectid_0_buffer
      Colid:      teamid_projectid_0_buffer_weekid
    */
    $rowid = $this->m_teamid."_".$this->m_id."_0_buffer";
    $result.= '<tr class="row3" bgcolor="#F7AD05" id="'.$rowid.'_buffer" style="display:none">';
    $result.= '<td colspan="'.$numheaderfields.'">  '.atktext("buffer","teamplanning").'</td>';
    foreach($this->m_weeks as $weekitem)
    {
      $week = $weekitem["week"];
      $inputid = $rowid."_".$week;
      $record[$this->m_dataprefix.$inputid]= isset($this->m_weekdata[$week]["buffertime"]) ? $this->m_weekdata[$week]["buffertime"]/60 : 0;
      $att = &new atkNumberAttribute($this->m_dataprefix.$inputid,0,4,0);
      $result.= '<td>'.$att->edit($record).$this->_hiddenAttr($this->m_olddataprefix.$inputid,$record[$this->m_dataprefix.$inputid]).'</td>';
    }
    $result.= '</tr>';
    
    /*
      Render conflicts
      Rowid:      Teamid_projectid_0_0_conflicts
      Colid:      teamid_projectid_0_0_conflicts_0
    */    
    $rowid = $this->m_teamid."_".$this->m_id."_0_0_conflicts";
    $result.= '<tr class="row3" id="'.$rowid.'_conflicts" style="display:none">';
    $colspan = $numheaderfields + count($this->m_weeks);
    $conflicttitle = "<b>".atktext("conflicts","teamplanning")."</b><br />";
    $conflictdata = "";
    //if we have conflicts
    if(count($this->m_conflicts))
    {
      $conflictdata.= '<div id="'.$rowid.'_0"><font color="#FF0000">';
      foreach($this->m_conflicts as $conflict)
        $conflictdata.= $conflict.'<br />';
      $conflictdata.= '</font></div>';
    }
    else
      $conflictdata.= atktext("noconflicts","teamplanning");    
    
    $result.= '<td colspan="'.$colspan.'">'.$conflicttitle.$conflictdata.'</td>';
    $result.= '</tr>';

    return $result;    
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
  
  function _calculateData()
  {
    $this->_calculateHeaderData();
    $this->_calculateUnassignedProjectHours();
    $this->_calculateEstimation();
    $this->_calculateCurrentPlanning();
    $this->_calculateCurrentMinPlanned();
  }
  
  function _calculateHeaderData()
  {
    foreach($this->m_headerfields as $field)
    {
      if($field != "name")
      {
        $sum = 0;        
        for($i=0,$_i=count($this->m_items);$i<$_i;$i++)
        {
          $item = &$this->m_items[$i];  
          $sum += $item->m_headerdata[$field];
        }        
        $this->m_headerdata[$field] = $sum;       
      }
    }
  }
  
  function _calculateEstimation()
  {
    $this->m_allactivitieshaveestimation = true;
    
    $sum = 0;    
    //calculate the total of all phases
    for($i=0,$_i=count($this->m_items);$i<$_i;$i++)
    {
      $item = &$this->m_items[$i];
      $item->_calculateEstimation();
      if(!$item->m_allitemshaveestimation)      
        $this->m_allactivitieshaveestimation = false;      
      $sum += $item->m_headerdata["estimation"];      
    }    
    $this->m_headerdata["estimation"] = $sum;
  }

  function _calculateCurrentMinPlanned()
  {
    $sum = 0;
    
    foreach($this->m_weekdata as $week=>$weekitem)
    {
      //$week = $weekitem["week"];
      if(isset($this->m_weekdata[$week]))
        $sum+= $this->m_weekdata[$week]["time"];
    }
    $this->m_calculated["current_min_planned"]["low"] = $this->m_calculated["current_planning"]["low"] - $sum;
    if($this->m_calculated["current_planning"]["high"] != "")
      $this->m_calculated["current_min_planned"]["high"] = $this->m_calculated["current_planning"]["high"] - $sum;
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
  
  function _calculateUnassignedProjectHours()
  {
    foreach($this->m_weeks as $weekitem)
    {
      $week = $weekitem["week"];
      $sum = 0;
      //calculate the total of all phases
      for($i=0,$_i=count($this->m_items);$i<$_i;$i++)
      {
        $item = &$this->m_items[$i];
        $itemhours = isset($item->m_weekdata[$week]["time"]) ? $item->m_weekdata[$week]["time"] : 0;
        $sum += $item->m_weekdata[$week]["time"];
      }      
      $projecthours = isset($this->m_weekdata[$week]["time"]) ? $this->m_weekdata[$week]["time"] : 0;
      $this->m_unassigned_projecthours[$week] = $projecthours - $sum;
    }
  }  
  
  function _init()
  {
    $this->m_openimagesrc   = atkconfig("atkroot").'atk/images/plus.gif';
    $this->m_closeimagesrc  = atkconfig("atkroot").'atk/images/minus.gif';
    
    $this->m_regopen      = "^{$this->m_teamid}_{$this->m_id}_";
    $this->m_regclose     = "^{$this->m_teamid}_{$this->m_id}_";
  }
  
  function _clickableImages()
  {  
    $imageid = implode("_",array($this->m_teamid,$this->m_id));
    
    $openimageid = "img_open_".$imageid;
    $closeimageid = "img_close_".$imageid;
    
    $hrefopen = "javascript:openObject('team_$this->m_teamid','$this->m_regopen','$openimageid','$closeimageid')";
    $hrefclose = "javascript:closeObject('team_$this->m_teamid','$this->m_regclose','$openimageid','$closeimageid')";
    
    $imgopen  = '<img id="'.$openimageid.'" src="'.$this->m_openimagesrc.'" border="0">';
    $imgclose = '<img id="'.$closeimageid.'" style="display:none" src="'.$this->m_closeimagesrc.'" border="0">';
  
    $images = "<a href=\"$hrefopen\">$imgopen</a>";
    $images.= "<a href=\"$hrefclose\">$imgclose</a>";
    return $images;    
  }
  
  function _hiddenAttr($id, $value)
  {
    //create hidden attrib
    return '<input type="hidden" name="'.$id.'" value="'.$value.'" />';
  }
  
  
}
?>