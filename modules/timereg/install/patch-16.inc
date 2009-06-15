<?php

/**
 * Patch for hours with wrong value of virtual_time flag
 * See http://www.ibuildings.nl/support/show_bug.cgi?id=17915 for more detail
 * @author Yury Golovnya  
 */

$db = &$setup->getDb();

//find wrong hours records id
$rows = $db->getrows("
  SELECT 
    h.id
  FROM
    hoursbase h
    JOIN phase p ON h.phaseid=p.id
  WHERE h.virtual_time<>p.virtual_time
");

if(count($rows))
{
  $id = array();
  foreach ($rows as $r)
  {
    $id[] = $r['id'];	
  }
  
  //build selector
  $selector = "hoursbase.id IN(".implode(",",$id).")";
  
  $hours = atknew("modules.timereg.hours");
  $hours->setTable("hoursbase");
  
  $records = $hours->selectDb($selector);
  
  foreach ($records as $r)
  {
    $hours->updateDb($r);
  }
}

?>