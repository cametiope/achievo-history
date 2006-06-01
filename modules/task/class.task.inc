<?php
/**
 * One line description of the file
 *
 * Full description of the file
 *
 * @package achievo
 * @subpackage task 
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

useattrib("atklistattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");
useattrib("atkdatetimeattribute");
useattrib("atktimeattribute");
useattrib("atkdurationattribute");
useattrib("atkdummyattribute");

userelation("atkmanytoonerelation");
//useattrib("task.atkdynamicmanytoonerelation");

class task extends atkNode 
{
  var $m_dependency_config = array('activity'=>'project.activity');
  
  function task()
  {
    $this->atkNode("task");    
    $this->add(new atkNumberAttribute("id",AF_PRIMARY));
    $this->add(new atkListAttribute("priority",array("higest","high","normal","low","lowest")));
    $this->add(new atkAttribute("name", AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkListAttribute("tasktype",array("project","phase","activity","todo","agenda"),"",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkNumberAttribute("refid",AF_OBLIGATORY));
    $this->add(new atkListAttribute("status",array('activity'))); //this attribute must be before refid
    
    //$this->add(new atkDynamicManyToOneRelation("refid",'tasktype',$dep_config,AF_OBLIGATORY));
    $this->add(new atkDateAttribute("createdate", "d-m-Y","d-m-Y"));
    $this->add(new atkDateTimeAttribute("editdatetime"));
    $this->add(new atkDateAttribute("startdate", "d-m-Y","d-m-Y"));
    $this->add(new atkDateAttribute("enddate", "d-m-Y","d-m-Y"));
    $this->add(new atkTimeAttribute("starttime"));
    $this->add(new atkTimeAttribute("endtime"));    
    $this->add(new atkManyToOneRelation("owner","employee.employee",AF_SEARCHABLE));        
    $this->add(new atkDurationAttribute("initial_planning"));
    $this->add(new atkDurationAttribute("current_planning"));
    $this->add(new atkDummyAttribute("registered_hours"));    
    
    $this->setTable("task");
    $this->setIndex("name");
    $this->setExtendedSearchAction("smartsearch");
  }
  
  function initial_values()
  {
    return array(
                  "starttime"=>array("hours"=>"0","minutes"=>"0"),
                  "endtime"=>array("hours"=>"0","minutes"=>"0")
                );
  }
  
  function descriptor_def()
  {
    return "[name]";    
  } 
  
  function action_admin(&$handler)
  {
    $user = &getUser();
    
    if(isset($user))
      $this->addFilter("task.owner='{$user['id']}'");

    $handler->action_admin();
  }
  
  function getView()
  {
    /* @var $sessionManager atkSessionManager */
    $sessionManger = &atkGetSessionManager();    
    $view = $sessionManger->stackVar("view");
    if ($view=="" || !isset($view))
    {
      $view = "active";
    }
    return $view;
  }  
  
  function adminFooter()
  {
    $view = $this->getView();

    if ($view=="active")
    {
      return atktext("onlyactiverecordsareshown")." ".href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
                  '&view=nonactive', atktext('clicktoviewarchive', $this->m_module, $this->m_type))."<br>";
    }
    else
    {
      return atktext("youareinthearchive")." ".href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
                  '&view=active', atktext('clicktoviewactiverecords', $this->m_module, $this->m_type))."<br>";
    }
  }  
}
?>