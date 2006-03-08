<?php

  class taskAttribute extends atkAttribute
  { 
    function taskAttribute($name, $flags=0)
    {
      parent::atkAttribute($name,$flags);
    }
       
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
    { 
      if($mode=="admin")
      {
        /* @var $query atkQuery */
        $query->addField($this->fieldName(),"","hours",$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));        
        $query->addField("id","", "activity", "activity_", false);
        $query->addField("name","", "project", "project_", false);        
        $query->addField("name","", "phase", "phase_", false);        
        $query->addField("name","", "activity", "activity_", false);
        
        $query->addJoin("task", "task", $tablename.".".$this->fieldName()."=task.id", false);
        $query->addJoin("activity", "activity", "activity.id=task.refid", false);
        $query->addJoin("phase", "phase","phase.id=activity.phaseid", false);
        $query->addJoin("project", "project","project.id=phase.projectid", false);
        $query->addCondition("task.tasktype='activity'");
      }
      elseif($mode=="select" || $mode=="edit" || $mode=="add")      
        $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));      
    }
    
    function db2value($rec)
    {
      return Array("taskid"       =>$rec["taskid"],
                   "projectname"  =>$rec["project_name"],
                   "phasename"    =>$rec["phase_name"],
                   "activityname" =>$rec["activity_name"],
                   "activityid"   =>$rec["activity_id"]
                   );                   
    }
//    
//    function value2db($rec)
//    {
//      atk_var_dump($rec,"<h1>VALUE 2 DB</h1>");
//      return parent::value2db($rec);
//      //return $rec["taskid"]["taskid"];
//    }
    
  }
?>