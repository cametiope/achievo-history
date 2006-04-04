<?php
  //@todo:patch moet ook rekening houden met public en private modules.

  //@todo:patchnummer klopt wellicht straks niet meer met HEAD branche. 
  //      nalopen voordat branches gemerged worden.  
  //@todo: patch maken voor rechten 

  $db = &atkGetDb();
  
  $setup->installNode("project.tpl_activity");
  $setup->installNode("project.activitytype");  
  
  //we need to add the columns to activity in order to retrieve anything from the activityrecords
  $setup->addColumn("activity", "phaseid", "int(10)", false, "0");
  $setup->addColumn("activity", "activitytype", "int(10)", false, "0");
    
  $activitynode         = &atkGetNode("project.activity");
  $activitytypenode     = &atkGetNode("project.activitytype");  
  
  //Get all the stored activities.
  $activities           = $activitynode->selectDb();
    
  //we get all the activity-phase combinations
  $phase_activities     = $db->getrows("SELECT * FROM phase_activity");
    
  //Activities become activitytypes.    
  $db->query("INSERT INTO activitytype (id,name,description,remarkrequired) SELECT id,name,description,remarkrequired FROM activity");    
  
  //@todo: do we need an extra check to make sure the transfer was successfull?
  
  //delete all the current activity entries
  //we use the "1" to create a 'WHERE 1' in the WHERE clause
  $activitynode->deleteDb("1", false);

  $setup->dropColumn("activity", "remarkrequired");  
  $setup->dropColumn("activity", "description");   
    
  //we create a new activity for each phase-activity combination
  foreach($phase_activities as $item)
  {    
    $activityrecord = array();
    
    //search for the corresponding activity record
    for($i=0,$_i=count($activities);$i<$_i;$i++)
    {
      //if we find it, take it and get out of this loop.
      if($activities[$i]["id"] == $item["activityid"])
      {
        $activityrecord = $activities[$i];
        break;
      }
    }
    
    //if we found a corresponding activity record, use the
    //data to create a new activity record.
    if($activityrecord != array() && isset($activityrecord))
    {      
      $rec["name"]                = $activityrecord["name"];      
      $rec["activitytype"]["id"]  = $item["activityid"];
      $rec["phaseid"]["id"]       = $item["phaseid"];
      
      $activitynode->addDb($rec, false);
    }
  } 
  
  //@todo: do we need an extra check to make sure the transfer was successfull?
  
  /* @var $setup setup */
  $setup->dropTable();
  
?>