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
userelation("atkmanytoonerelation");

class activity extends atkNode
{
  function activity()
  {
    $this->atkNode("activity", NF_MRA);

    $this->add(new atkAttribute("id",AF_AUTOKEY));        
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE, 50));    
    
    //@todo: remarkrequired en description is verhuist naar activitytype
    //$this->add(new atkAttribute("description",AF_SEARCHABLE, 50));
    //$this->add(new atkBoolAttribute("remarkrequired"));
    
    $this->add(new atkManyToOneRelation("phaseid","project.phase"));    
    //@todo: vlaggen controleren
    $this->add(new atkManyToOneRelation("activitytype","project.activitytype"));

    $this->setTable("activity","activity");
    $this->setOrder("name");

    atkdebug("activity::activity()");
  }

  function descriptor_def()
  {
    return "[name]";
  }

//  @todo: verplaatsen naar activitytype
//  function action_stats(&$handler)
//  {
//    include_once moduleDir("project")."activitystats.inc";
//  }
}
?>