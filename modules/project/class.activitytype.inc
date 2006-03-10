<?php

/**
 * Type of an activity
 *
 * Node contains the activitytype that can be linked to a activity.
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
class activitytype extends atkNode 
{
  function activitytype()
  {
    $this->atkNode("activitytype");
    $this->add(new atkAttribute("id", AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_UNIQUE));
    $this->add(new atkAttribute("description",AF_SEARCHABLE, 50));
    $this->add(new atkBoolAttribute("remarkrequired",AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("overtimecompensation"));    
    $this->setTable("activitytype");
    $this->setOrder("name");
  }
  
  function descriptor_def()
  {
    return "[name]";
  }  
}
?>