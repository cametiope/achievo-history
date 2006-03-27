<?php
class workload
{
  function workload()
  {
    
  }
  
  function getWorkLoad()
  {
    $workload = array();
    
    //wat nodig
    //1. Alle tijdregistraties 
    //2. Alle projecten/fase/activiteiten
    //3. Bereken voor elke fase
      //initial_planning
      //current_planing
    //4. Bereken voor elk project 
      //initial_planning
      //current_planing    
    
      
      
    //Get all active projects/phases and activities, all timeregistrations and all mostrecentestimations
    $projectdata            = $this->_getAllProjectData();
    $timeregistrations      = $this->_getAllTimeRegistrations();
    $mostrecentestimations  = $this->_getMostRecentEstimations();
    
    $workload = array();
    
    //reformat all data
    foreach($projectdata as $rec)
    {
      $projectid                = $rec["projectid"];
      $projectname              = $rec["projectname"];
      $phaseid                  = $rec["phaseid"];
      $phasename                = $rec["phasename"];
      $activityid               = $rec["activityid"];      
      $phaseinitialplanning     = $rec["phase_initial_planning"];
      $phasecurrentplanning     = $rec["phase_current_planning"];
      $registeredtime           = $registeredtime[$projectid]["phases"][$phaseid]["activities"][$activityid]["activitytime"];
      $activitycurrentplanning  = $mostrecentestimations[$projectid]["phases"][$phaseid]["activities"][$activityid]["current_planning"];      
      
      $workload[$projectid]["id"]       = $projectid;
      $workload[$projectid]["name"]     = $projectname;
      //add initial_planning for project
      //add current_planning for project
      $workload[$projectid]["phases"][$phaseid]["id"] = $phaseid;
      $workload[$projectid]["phases"][$phaseid]["name"] = $phasename;
      $workload[$projectid]["phases"][$phaseid]["initial_planning"] = $phaseinitialplanning;
      $workload[$projectid]["phases"][$phaseid]["current_planning"] = $phasecurrentplanning;
      $workload[$projectid]["phases"][$phaseid]["activities"][$activityid]["id"] = $activityid;
      
      $workload[$projectid]["phases"][$phaseid]["activities"][$activityid]["registeredtime"]  = $registeredtime;
      $workload[$projectid]["phases"][$phaseid]["activities"][$activityid]["estimatedtime"]   = $activitycurrentplanning;
    }
    
    //atk_var_dump($workload,"<h1>TAH WORKLLOAD</h1>");
    /*
      $timeregistration[$projectid]["phases"][$phaseid]["activities"][$activityid]["time"]      = 10;
      $timeregistration[$projectid]["phases"][$phaseid]["activities"][$activityid]["estimate"]  = 10;
      $timeregistration[$projectid]["phases"][$phaseid]["time"]                       = 10;
      $timeregistration[$projectid]["phases"][$phaseid]["estimate"]                   = 10;
      $timeregistration[$projectid]["phases"][$phaseid]["initial_planning"]           = 10;
      $timeregistration[$projectid]["phases"][$phaseid]["current_planning"]["value"]  = 10;
      $timeregistration[$projectid]["phases"][$phaseid]["allphases"]  = 1/0;  //hebben we van alle activiteiten een urenschatting?
      $timeregistration[$projectid]["time"]  = 10;
    */
    return $workload;
  }
  
  function _getAllProjectData()
  {
    /* @var $query atkNode */
    $db = &atkGetDb();
    $query = &new atkQuery();
    /*
    SELECT project.id AS projectid, phase.id AS phaseid,activity.id AS activityid
    FROM `activity`
    LEFT JOIN phase ON phase.id = activity.phaseid
    LEFT JOIN project ON project.id = phase.projectid
    WHERE 
    project.status='active' 
    AND phase.status='active'
    */
    $query->addTable("activity");
    $query->addField("id AS activityid","","activity");
    $query->addField("id AS phaseid","","phase");
    $query->addField("name AS phasename","","phase");
    $query->addField("initial_planning AS phase_initial_planning","","phase");
    $query->addField("current_planning AS phase_current_planning","","phase");
    $query->addField("id AS projectid","","project");
    $query->addField("name AS projectname","","project");
    $query->addJoin("phase","phase","activity.phaseid=phase.id",true);
    $query->addJoin("project","project","phase.projectid=project.id",true);
    $query->addCondition("project.status='active'");
    $query->addCondition("phase.status='active'");    
    $sql = $query->buildSelect();
    $records = $db->getrows($sql);
    //atk_var_dump($records,"<h1>projectdata</h1>");
    return $records;
  }
  
  function _getAllTimeRegistrations()
  {
    /* @var $query atkNode */
    $db = &atkGetDb();
    $query = &new atkQuery();
    
    $query->addTable("hours");
    $query->addField("id AS activityid","","activity");
    $query->addField("id AS phaseid","","phase");
    $query->addField("id AS projectid","","project");
    $query->addField("sum(time) AS activitytime","","");    
    $query->addJoin("task","task","hours.taskid=task.id",true);
    $query->addJoin("activity","activity","task.refid=activity.id",true);
    $query->addJoin("phase","phase","activity.phaseid=phase.id",true);
    $query->addJoin("project","project","phase.projectid=project.id",true);
    $query->addCondition("task.tasktype='activity'");
    $query->addCondition("project.status='active'");
    $query->addCondition("phase.status='active'");
    $query->addGroupBy("activity.id");
    $records = $db->getrows($query->buildSelect());
    
    $regs = array();
    foreach($records as $rec)
    {
      $projectid      = $rec["projectid"];
      $phaseid        = $rec["phaseid"];
      $activityid     = $rec["activityid"];
      $activitytime   = $rec["activitytime"];      
       
      $regs[$projectid]["phases"][$phaseid]["activities"][$activityid]["time"] = $activitytime;
    }
    //atk_var_dump($regs,"<h1>timeregs</h1>");
    return $regs;
  }
  
  /**
   * Get the most recent estimations for each activity. Estimations for activities are stored in the task table.
   * @todo: create a good query that does just that instead of first having to
   * edit the records.      
   *
   * @param type name description
   * @return type description
   */
  function _getMostRecentEstimations()
  {
    $db = &atkGetDb();    
    $query = &new atkQuery();
    
    $query->addTable("task");
    //$query->addField("id AS taskid","","task");
    $query->addField("id AS activityid","","activity");
    $query->addField("id AS phaseid","","phase");
    $query->addField("id AS projectid","","project");    
    $query->addField("current_planning","","task");    
    $query->addJoin("activity","activity","task.refid=activity.id",true);
    $query->addJoin("phase","phase","activity.phaseid=phase.id",true);
    $query->addJoin("project","project","phase.projectid=project.id",true);
    $query->addCondition("task.tasktype='activity'");
    $query->addCondition("project.status='active'");
    $query->addCondition("phase.status='active'");
    $query->addCondition("task.current_planning > 0");
    $query->addOrderBy("task.refid, task.editdatetime ASC");
    $records = $db->getrows($query->buildSelect());
    //$sql = "SELECT id,name,refid AS activityid,current_planning FROM `task` WHERE current_planning > 0 AND task.tasktype='activity' ORDER BY refid, editdatetime ASC";
    
    
    $estimations = array();
    
    //we recieve the estimation records ordered by refid and editdatetime.
    //we loop through all records and overwrite every current_planning so in the end we have the most recent one.
    foreach($records as $rec)
    {
      $projectid        = $rec["projectid"];
      $phaseid          = $rec["phaseid"];
      $activityid       = $rec["activityid"];
      $current_planning = $rec["current_planning"];      
      $estimations[$projectid]["phases"][$phaseid]["activities"][$activityid]["current_planning"] = $current_planning;
    }
    //atk_var_dump($estimations,"<h1>estimations</h1>");
    return $estimations;
  }
  
  function blah()
  {
    /* @var $projnode atkNode */
    /* @var $phasenode atkNode */
    $projnode     = &atkGetNode("project.project");
    $phasenode    = &atkGetNode("project.phase");
    //in the future we may add the activities as well.
    //$activitynode = &atkGetNode("project.activity");
    
    $workload = array();
    /* @var $query atkNode */
    $db = &atkGetDb();
    $query = &new atkQuery();
    
    //get all active projects
    $workload["projects"] = $projnode->selectDb();
  }
}
?>