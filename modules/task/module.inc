<?php

/**
 * The task node defines atomic activities.
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
class mod_task extends atkModule 
{  
  function getNodes()
  {
    registerNode("task.task", array("admin", "add", "edit", "delete"));
  }
  
  function getMenuItems()
  {
    //no menuitems for tasks
  }
}
?>