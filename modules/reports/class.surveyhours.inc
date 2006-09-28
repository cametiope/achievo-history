<?php
atkimport("module.timereg.hours");
class surveyhours extends hours 
{
  function surveyhours()
  {
    $this->hours('surveyhours');
    foreach (array_keys($this->m_attribList) as $attrName)
    {
      $attr = &$this->getAttribute($attrName);
      if (is_a($attr, 'atkmanytoonerelation'))
        $attr->removeFlag(AF_RELATION_AUTOCOMPLETE);
        
      if($attrName == "phaseid")
        $attr->m_dependees = array();
        
    }
    $this->setSecurityAlias('timereg.hours');
  }  

//  function phaseid_selection($record,$mode="")
//  {
//    $att = &$this->getAttribute("phaseid");
//    return $att->getSelectableRecords($record, $mode);
//  }  
}
?>