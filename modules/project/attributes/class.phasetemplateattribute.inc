<?php

userelation("atkmanytoonerelation");

// When a new phase is saved, you can base it on a template, i.e. select a set of 
// default activities. To accomplish this, we make a new attribute based on a ManyToOneRelation
// that isn't stored in the db, but used to add records to the projectactivities table.
class phasetemplateAttribute extends atkManyToOneRelation
{
  function phasetemplateAttribute($name, $target, $flags)
  {
    $this->atkManyToOneRelation($name, $target, $flags);
  }

  function addToQuery($query)
  {
    // do nothing
  }
  
  function store($db, $record, $type)
  {
    // Todo, here we must save the activities from the template to the projectactivities table
    if ($type == "add")
    {
      if ($record['template']['id']!="")
      {
        //$query = "INSERT INTO phase_activity (activityid, phaseid) SELECT activityid, ".$record['id']." FROM tpl_phase_activity WHERE phaseid = ".$record['template']['id'];

        //@todo: repareer template generatie voor phases
        //get activityids that belong to this phase
        $query = "SELECT activityid FROM tpl_phase_activity WHERE phaseid=".$record['template']['id'];
        $data = $db->getrows($query);
        $activitytemplateids = array();
        foreach($data as $a) $activitytemplateids[] = $a["activityid"];
         
        //if this phase template has activity templates, we retrieve them from the database.
        if(count($activitytemplateids) > 0)
        {            
         $query = "SELECT * FROM tpl_activity WHERE id IN (".implode(",",$activitytemplateids).")";
         $activitytemplates = $db->getrows($query);
        }         
        
        //create activities for this phase for each activitytemplate we found
        foreach ($activitytemplates as $activitytemplate)
        {
          $new_actid = $db->nextid('activity');
          
          $query = "INSERT INTO 
                    activity(id,name,phaseid,activitytype) 
                    VALUES (".$new_actid.",
                    '".$activitytemplate["name"]."',
                    ".$new_id.",
                    ".$activitytemplate["activitytype"].")";
          $db->query($query);
        }
      }
    }
    return true;
  }
  
  function dbFieldType()
  {
    return "";
  }
}

?>