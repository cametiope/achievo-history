<?php

class projectItem
{
  var $m_items          = array();  //phases
  var $m_id             = 0;
  var $m_teamid         = 0;
  var $m_headerfields   = array();  
  var $m_headerdata     = array();
  var $m_weeknames      = array();
  var $m_weekdata       = array();
  var $m_conflicts      = array();
  
  //some fields
  var $m_openimagesrc   = "";
  var $m_closeimagesrc  = "";
  var $m_regopen        = "";
  var $m_regclose       = "";
  
  //calculated fields
  var $m_unassigned_projecthours = array();
  
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
    $this->m_headerdata = $headerdata;
  }
  
  function setWeekData($weekdata)
  {
    $this->m_weekdata = $weekdata;
  }
    
  function setConflicts($conflicts)
  {
    $this->m_conflicts = $conflicts;
  }
  
  function render($headerfields, $weeknames)
  {
    $this->m_headerfields = $headerfields;
    $this->m_weeknames    = $weeknames;
    $numheaderfields = count($this->m_headerfields);
    $this->_init();        
            
    /* 
      calculate data 
    */
    $this->_calculateData();
            
    /*  
      Render header field values such as the name 
    */
    $rowid = $this->m_teamid."_".$this->m_id;    
    $result = '<tr class="row2" id="'.$rowid.'">';    
    foreach($this->m_headerfields as $field)
    {       
      if($field != "name")
      {
        $inputid = implode("_",array($this->m_teamid,$this->m_id,"0",$this->_uus($field)));
        $value = (isset($this->m_headerdata[$field]) ? $this->m_headerdata[$field]/60 : "0");
        $result.= '<td align="center"><div id="'.$inputid.'">'.$value.'</div></td>';
      }
      else
        $result.= '<td>'.$this->_clickableImages().' <b>'.(isset($this->m_headerdata[$field]) ? $this->m_headerdata[$field] : "?").'</b></td>';
    }
    
    /*  
      Render planned week values 
    */
    foreach($this->m_weeknames as $week)    
    {
      $inputid = implode("_",array($this->m_teamid,$this->m_id,"0",$week));      
      $record[$inputid]= (isset($this->m_weekdata[$week]["time"]) ? $this->m_weekdata[$week]["time"]/60 : 0);
      $att = &new atkNumberAttribute($inputid,0,4,0);
      $result.= '<td>'.$att->edit($record).'</td>';
    }
    $result.= "</tr>";

    /*  
      Render phases 
    */
    foreach($this->m_items as $item)    
      $result.= $item->render($headerfields, $weeknames);    
    
    /*  
      Render unassigned project hours 
    */
    $result.= '<tr class="row1" id="'.$rowid.'_unassigned" style="display:none">';
    $result.= '<td colspan="'.$numheaderfields.'">  <b>'.atktext("unassignedprojecthours","teamplanning").'</b></td>';
    foreach($this->m_weeknames as $week)
    { 
      $inputid = implode("_",array($this->m_teamid,$this->m_id,"0",$this->_uus($field)));
      $result.= '<td><div id="'.$rowid."_unassigned".$week.'">'.($this->m_unassigned_projecthours[$week]/60).'</div></td>';
    }
    $result.= '</tr>';
      
    /*  
      Render the projectbuffer  
    */    
    $result.= '<tr class="row3" bgcolor="#F7AD05" id="'.$rowid.'_buffer" style="display:none">';
    $result.= '<td colspan="'.$numheaderfields.'">  '.atktext("buffer","teamplanning").'</td>';
    foreach($this->m_weeknames as $week)
    {
      $inputid = implode("_",array($this->m_teamid,$this->m_id,"buffer",$week));
      $record[$inputid]= isset($this->m_weekdata[$week]["buffertime"]) ? $this->m_weekdata[$week]["buffertime"]/60 : 0;
      $att = &new atkNumberAttribute($inputid,0,4,0);
      $result.= '<td>'.$att->edit($record).'</td>';
    }
    $result.= '</tr>';
    
    /*
      Render conflicts
    */
    $result.= '<tr class="row3" id="'.$rowid.'_conflicts" style="display:none">';
    $colspan = $numheaderfields + count($this->m_weeknames);
    $conflicttitle = "<b>".atktext("conflicts","teamplanning")."</b><br />";
    $conflictdata = "";
    //if we have conflicts
    if(count($this->m_conflicts))
    {
      $conflictdata.= '<div id="'.$rowid.'_conflicts"><font color="#FF0000">';
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
  }
  
  function _calculateHeaderData()
  {
    foreach($this->m_headerfields as $field)
    {
      //temp: 'id' can be deleted
      if($field != "name" && $field != "id")
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
  
  function _calculateUnassignedProjectHours()
  {
    foreach($this->m_weeknames as $week)
    {
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
}
?>