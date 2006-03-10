<?php
/**
 * One line description of the file
 *
 * Full description of the file
 *
 * @package achievo
 * @subpackage modules
 *
 * @author dennis <dennis@ibuildings.nl>
 *
 * @copyright (c) 2006 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing/ ATK open source license
 * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
 *
 * @version $Revision$
 * $Id$
 */
useattrib("atkboolattribute");
useattrib("atkdummyattribute");
userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");

class activity extends atkNode
{
  function activity()
  {
    $this->atkNode("activity");
    //@todo: add config activitynamereadonly to a config file.    
    $this->add(new atkAttribute("id",AF_AUTOKEY));        
    $this->add(new atkAttribute("name",AF_SEARCHABLE|(atkconfig("activitynamereadonly",false) ? AF_READONLY|AF_HIDE_ADD : AF_OBLIGATORY), 50));    
    $this->add(new atkManyToOneRelation("phaseid","project.phase", AF_HIDE));    
    //tasks with the tasktype 'activity' can be linked to an activity.
    $rel = &new atkOneToManyRelation("taskid","task.task","refid", AF_HIDE);
    $rel->setDestinationFilter("task.tasktype='activity'");
    $this->add($rel);
        
    //this attribute is used when use the config activitynamereadonlyis  set to true, and there aren't any
    //activitytypes left that aren't already added to the current phase. In that case this attribute
    //contains a warning message. At default it is hidden.
    $this->add(new atkDummyAttribute("noactivitypes",'<font style="color:red">'.atktext("noactivitytypes").'</font>',AF_HIDE));
    
    $this->add(new atkManyToOneRelation("activitytype","project.activitytype",AF_OBLIGATORY|AF_RELATION_AUTOLINK));
    
    $this->setTable("activity","activity");
    $this->setOrder("activity.name");

    atkdebug("activity::activity()");
  }

  function descriptor_def()
  {
    return "[name]";
  }

  function preAdd(&$rec)
  {
    //update the activityname to be the same as the activitytype name when the activity name has 
    //no value (in case of the activityname being readonly.
    if(!isset($rec["name"]) || $rec["name"]=="")
    {      
      $node = &getNode("project.activitytype");
      $activitytype = $node->selectDb("id='".$rec["activitytype"]["id"]."'","","","",array("name"));      
      $rec["name"] = $activitytype[0]["name"];
    }
    return true;
  }
 
  function action_add(&$handler)
  {
    $this->handle();    
    $handler->action_add();
  }  
  
  function action_save(&$handler)
  {
    $this->handle();
    $handler->action_save();
  }
     
  function validate(&$record, $mode, $ignoreList=array())
  {
    if ($mode == "update" || $mode == "add")
    {
      /* @var $node atkNode */
      $node = &getNode("project.activity");      
      
      //look for an activity with the same name that is linked to the same phase      
      $phaseid = $this->_getPhaseIdFromPostVars();
      $records = $node->selectDb("activity.phaseid=".$phaseid." AND activity.name='".$record["name"]."'","","","",array("id"));      
      
      // Trigger an error if we already have an activity in this phase with the same name
      if (count($records))      
        triggerError($record, "name", "error_activityname_already_exists_for_this_phase");
    }

    // Call the parents validate function to perform the default validation on the record
    atkNode::validate($record, $mode, $ignoreList);
  }
  
  function handle()
  {
    /**
     * if we have the config activitynamereadonly set to true, we must make sure we give it a unique name
     * within the phase. Because the name is copied from the activitytype, we must make sure that the user 
     * cannot select an activity which is already added to this phase.
     * Therefore we should filter out the selectable options in the dropdownlist for activitytypes.
     */
    if(atkconfig("activitynamereadonly",false))
    {
      //get all activitytypes that this phase already have      
      $node     = &getNode("project.activity");
      $typenode = &getNode("project.activitytype");      
      
      $phaseid = $this->_getPhaseIdFromPostVars();
      $activities = $node->selectDb("activity.phaseid=".$phaseid,"","","",array("id","activitytype"));
      
      $filter = "";
      $ids = array();
      if(count($activities))
      {
        //retrieve only the ids
        foreach($activities as $a)
          $ids[] = $a["activitytype"]["id"];
        
          //create filter
        $filter = "id NOT IN (".implode(", ",$ids).")";

        $att = &$this->getAttribute("activitytype");   
        $att->setDestinationFilter($filter);
        
        //We test the filter to see if it results in any possible activitytype. If we find none, 
        //we show the warning message 'noactivitytypes';
        /* @var $typenode atkNode */
        $activitytypes = $typenode->selectDb($filter,"","","",array("id"));
        if(!count($activitytypes))
        {
          $att = &$this->getAttribute("noactivitypes");
          $att->removeFlag(AF_HIDE_ADD);
        }
      }      
    }   
  }
   
  function action_stats(&$handler)
  {
    include_once moduleDir("project")."activitystats.inc";
  }
  
  function _getPhaseIdFromPostVars()
  {
    //the phaseid is found in the postvars in the format: "atkselector" => phase.id='number'
    $selector = explode("=",$this->m_postvars["atkselector"]);
    return $selector[1];
  }
}
?>