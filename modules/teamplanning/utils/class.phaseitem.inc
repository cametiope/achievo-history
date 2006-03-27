<?php

class phaseItem
{
  //var $m_items          = array();
  var $m_id             = 0;
  var $m_teamid         = 0;
  var $m_projectid      = 0;
  var $m_headerfields   = array();
  var $m_headerdata     = array();
  var $m_weeknames      = array();
  var $m_weekdata       = array();
  
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
    $this->m_headerdata = $headerdata;
  }
  
  function setWeekData($weekdata)
  {
    $this->m_weekdata = $weekdata;
  }
    
  function render($headerfields, $weeknames)
  {
    $this->m_headerfields = $headerfields;
    $this->m_weeknames    = $weeknames;
    
    
    $rowid = $this->m_teamid."_".$this->m_projectid."_".$this->m_id;    
    $result = '<tr class="row1" id="'.$rowid.'" style="display:none">';
    foreach($this->m_headerfields as $field)
    {
      if($field != "name")
      {
        $inputid = implode("_",array($this->m_teamid,$this->m_projectid,$this->m_id,$this->_uus($field)));
        $value = (isset($this->m_headerdata[$field]) ? $this->m_headerdata[$field]/60 : "0");
        $result.= '<td align="center"><div id="'.$inputid.'">'.$value.'</div></td>';
      } 
      else       
        $result.= "<td>     ".$this->m_headerdata[$field]."</td>";      
    }
    
    foreach($this->m_weeknames as $week)    
    {
      $inputid = implode("_",array($this->m_teamid,$this->m_id,"0",$week));
      $record[$inputid] = isset($this->m_weekdata[$week]["time"]) ? $this->m_weekdata[$week]["time"]/60 : 0;
      $att = &new atkNumberAttribute($inputid,0,4,0);
      $result.= '<td>'.$att->edit($record).'</td>';
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
  
  
  
}
?>