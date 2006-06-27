<?php

atkimport("atknumberattribute");

class pdaTaskAttribute extends atkNumberAttribute
{
  function pdaTaskAttribute($name, $flags=0)
  {
    /* base class constructor */
    $this->atkAttribute($name, $flags|AF_HIDE|AF_FORCE_LOAD);
  }

  function fetchValue($postvars)
  {
    return array(
      'phaseid' => $postvars[$this->fieldName()]['phaseid'],
      'activityid' => $postvars[$this->fieldName()]['activityid']
    );
  }
    
  /**
   * We don't support searching
   * @param $record array with fields
   * @return search field
   */
  function search($record="")
  {
    return "&nbsp;";
  }

  function db2value($rec)
  {
    return array("phaseid"=>$rec["phaseid"],
                 "activityid"=>$rec["activityid"],
                 );
  }

  function value2db($rec)
  {
    $phaseid = $rec[$this->fieldName()]["phaseid"];    
    $activityid = $rec[$this->fieldName()]["activityid"];    
    $user = getUser();
    
    // search task or create task and set $taskid
    $db = &$this->m_ownerInstance->getDb();
    list($row) = $db->getRows(sprintf("SELECT t.id AS taskid FROM task t, activity a WHERE t.tasktype = 'activity' AND a.id = t.refid AND a.id = %d AND t.owner = %d", $activityid, $user['id']));
    // If we have no task yet, create one.
    if ($row == NULL)
    {
      atkdebug("this user does not yet have a task related to this activity.");            
      /* @var $activityNode atkNode */
      $taskNode     = &atkGetNode('task.task');
      $activityNode = &atkGetNode('project.activity');      
      
      //get activity name
      $activityRecords    = $activityNode->selectDb("activity.id='$activityid'","","","",array("name"));
      $activityname       = $activityRecords[0]["name"];
      $dt                 = date("Y-m-d");
      $datearr            = explode("-",date("Y-m-d-H-i-s"));    
      $datetime           = array();    
      $datetime["year"]   = $datearr[0];
      $datetime["month"]  = $datearr[1];
      $datetime["day"]    = $datearr[2];
      $datetime["hours"]  = $datearr[3];
      $datetime["minutes"]= $datearr[4];
      $datetime["seconds"]= $datearr[5];
      
      /* Create a task */
      $newtask = array();
      $taskid                       = $db->nextid('task');
      $newtask["id"]                = $taskid;
      $newtask["name"]              = $activityname;
      $newtask["owner"]             = array("id"=>$user["id"]);
      $newtask["createdate"]        = $dt;      
      $newtask["editdatetime"]      = $datetime;
      $newtask["refid"]             = $activityid;
      $newtask["tasktype"]          = 'activity';
      $taskNode->addDb($newtask, false);
      
      $row = array('taskid' => $taskid);
    }
    
    $taskid = $row['taskid'];
    
    atkdebug("SAVING TASKID [$taskid]");
    
    return $taskid;    
  }
  
  function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
  {
    if ($mode == "add" || $mode == "update")
    {
      parent::addToQuery($query, $tablename, $fieldaliasprefix, $rec, $level, $mode);
    }
    else
    {      
      /* @var $query atkQuery */
      $query->addField("id","", "A", "phase", false);
      $query->addField("id","", "C", "activity", false);      
      $query->addJoin("task", "D", "D.id=hours.taskid", false);
      $query->addJoin("activity", "C", "C.id=D.refid", false);
      $query->addJoin("phase", "A","A.id=C.phaseid", true);
      $query->addJoin("project", "B","B.id=A.projectid", true);
      $query->addCondition("D.tasktype='activity'");
    }
  }

  function getOrderByStatement()
  {
    return "B.name, A.name";
    
  }
  
  function dbFieldType()
  {
    return "number";
  }

  function equal($recA, $recB)
  {
    return $recA[$this->fieldName()]["activityid"]==$recB[$this->fieldName()]["activityid"];
  }
  
  function validate(&$record, $mode)
  {    
        
  }
}
?>