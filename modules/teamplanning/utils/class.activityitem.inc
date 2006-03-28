<?php

class activityItem
{
  var $m_id             = 0;
  var $m_teamid         = 0;
  var $m_projectid      = 0;
  var $m_phaseid        = 0;
  var $m_headerfields   = array();  
  var $m_weeks          = array();
  
  
  var $m_dataprefix     = "";
  var $m_olddataprefix  = "";
  
  function activityItem($id, $teamid, $projectid, $phaseid)
  {
    $this->m_id         = $id;
    $this->m_teamid     = $teamid;
    $this->m_projectid  = $projectid;
    $this->m_phaseid    = $phaseid;
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
  
  /**
   * We do not render any activities, mabey in the future...
   *    
   * @param type name description
   * @return type description
   */
  function render($headerfields, $weeks, $dataprefix, $olddataprefix)
  {
    $this->m_headerfields   = $headerfields;
    $this->m_weeks          = $weeks;
    $this->m_dataprefix     = $dataprefix;
    $this->m_olddataprefix  = $olddataprefix;
        
    /*  
      Render header field values such as the name 
      Rowid:      Teamid_projectid_phaseid_activityid_0
      Colid:      teamid_projectid_phaseid_activityid_0_fieldname
    */   
    $rowid = $this->m_teamid."_".$this->m_projectid."_".$this->m_phaseid."_".$this->m_id."_0";
    $result = '<tr class="row1" id="'.$rowid.'" style="display:none">';
    foreach($this->m_headerfields as $field)
    {
      if($field != "name")
      {
        $inputid = $this->m_dataprefix.$rowid."_".$this->_uus($field);        
        $value = (isset($this->m_headerdata[$field]) ? $this->m_headerdata[$field]/60 : "0");                
        $result.= '<td align="center"><div id="'.$inputid.'">'.$value.'</div></td>';
      } 
      else       
        $result.= "<td>        ".$this->m_headerdata[$field]."</td>";      
    }
    
    /*  
      Render planned week values       
    */    
    foreach($this->m_weeks as $weekitem)    
    {
      $result.= '<td>-</td>';
    }    
    $result.= "</tr>";

    
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
  
  function _hiddenAttr($id, $value)
  {
    //create hidden attrib
    return '<input type="hidden" name="'.$id.'" value="'.$value.'" />';
  }  
}
?>