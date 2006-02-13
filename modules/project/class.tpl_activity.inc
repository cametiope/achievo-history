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

userelation("atkmanytoonerelation");
class tpl_activity extends atkNode 
{
  //@todo: vlaggen voor node controleren
  //@todo: vlaggen voor attributen controleren
  function tpl_activity()
  {
    $this->atkNode("tpl_activity");
    
    $this->add(new atkAttribute("id",AF_AUTOKEY));        
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE, 50));
    $this->add(new atkAttribute("description",AF_SEARCHABLE, 50));        
    $this->add(new atkManyToOneRelation("activitytype","project.activitytype", AF_OBLIGATORY|AF_RELATION_AUTOLINK));
    $this->setTable("tpl_activity");
    $this->setOrder("name");
  }
  
  function descriptor_def()
  {
    return "[name]";
  }
}
?>