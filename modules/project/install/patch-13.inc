<?php  

  //@todo: do we need an extra check to make sure the transfer was successfull?
  
  //@todo:Check if patchnumber isn't the same as an existing patch number from the head branch 
  //      this is crucial when merging.
  
  //@todo: Patch userrights

  //we create another db connection to prevent that the resultlist from the first one is cleared
  //when we execute another query.  
  global $config_db;
  $config_db["second"] = $config_db["default"];
  $db   = &atkGetDb();  
  $db2  = &atkGetDb("second",true);  
  
  //Install nodes tpl_activity en activitytype
  $setup->installNode("project.tpl_activity");
  $setup->installNode("project.activitytype");
  
  //Activities become activitytypes.  The id's are still the same, we will use this information.
  $sql = "INSERT INTO activitytype
            SELECT id,name,description,remarkrequired,overtimecompensation FROM activity";  
  $db->query($sql);  
  atkdebug("query affected ".$db->m_affected_rows." rows");
    
  //Get all activities, we remember them for later use.
  $activities = $db->getrows("SELECT * FROM activity");
  atkdebug("Retrieved all activities");
  
  //Remove and add some columns to the activities
  $setup->dropColumn("activity",  "remarkrequired");  
  $setup->dropColumn("activity",  "description");
  $setup->addColumn("activity",   "phaseid","int(11)", false, 0);  
  $setup->addColumn("activity",   "activitytype","int(11)", false, 0);
    
  //Delete all the current activity entries  
  $db->query("DELETE FROM activity WHERE 1");
  atkdebug("Deleted all activity records");
  
  $db->query("SELECT * FROM phase_activity");
  atkdebug("Retrieved all phase_activity records");
  
  //We create a new activity for each phase-activity combination
  while($db->next_record())
  {
    $item = $db->m_record;    
    $activityrecord = "";
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
    if($activityrecord !="")
    { 
      $name         = escapeSQL($activityrecord["name"]);
      $activitytype = escapeSQL($item["activityid"]);   //the old activityid, is now the activitytype id
      $phaseid      = escapeSQL($item["phaseid"]);
      $id           = $db2->nextid("project.activity");
      $db2->query("INSERT INTO activity (id, name, activitytype, phaseid) VALUES ($id,'$name',$activitytype,$phaseid)");
    }
    else 
    {
      //what should we do here?....
      atkdebug("Cannot find {$item["activityid"]}");
    }
  }

  //Add a taskid column to the hours table
  //Rename activitydate to taskdate
  $setup->addColumn("hours","taskid","int(11)", false, 0);  
  $setup->alterColumn("hours","activitydate","taskdate","date",false);

  $now      = date("Ymd");    //Createdate for the tasks
  $tasktype = "activity";

  //Create a LUT (Look Up Table) to acces the activitynames easy and FAST 
  $activitynames = array();
  foreach($activities as $act)  
    $activitynames[$act["id"]] = $act["name"];
  
  //Add an index on the phaseid,activityid and userid columns to make
  //the following queries run much much faster.
  $db->query("ALTER TABLE hours ADD INDEX (phaseid, activityid, userid)");  
  
  //@todo: create a task for each phase-activity-person combination entry in hours table
  $query = $db->createQuery();      
  $query->addField("userid","","hours");
  $query->addField("activityid","","hours");
  $query->addField("phaseid","","hours");    
  $query->addGroupBy("phaseid");
  $query->addGroupBy("activityid");
  $query->addGroupBy("userid");
  $query->addTable("hours");  
  $db->query($query->buildSelect());  
  
  //Create a new task for each person-activity-phase combination  
  while($db->next_record())   
  {
    $rec = $db->m_record;       //retrieve record
    
    $userid     = $rec["userid"];
    $activityid = $rec["activityid"];
    $phaseid    = $rec["phaseid"];    
    $name       = escapeSQL($activitynames[$activityid]); //get activityname

    //Get the new activity record that belongs to this phase.    
    //The activityid in the hours table used to be linked to the id of an activity.
    //Since the id of the activitytype is the same as the old activity id, we use that one.
    $sql = "SELECT id FROM activity WHERE phaseid=$phaseid AND activitytype=$activityid";    
    $record = $db2->getrows($sql);        
    $refid  = $record[0]["id"];
    
    //if we find an hour registration that is linked to an activity/phase combination that does not exists anymore,
    //create a task linked to an activity with refid 0.
    if(!isset($refid) || $refid=="")
      $refid = 0;    
    
    $taskid = $db2->nextid("task.task");    
    $db2->query("INSERT INTO task (id,name,refid,tasktype,createdate,owner) VALUES ($taskid,'$name',$refid,'$tasktype','$now',$userid)");
    
    //update hour records    
    $db2->query("UPDATE hours SET taskid=$taskid WHERE activityid=$activityid AND phaseid=$phaseid AND userid=$userid");
  }
  
  //@todo: remove index
  //@todo: remove columns phaseid and activityid from hours  
?>