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
useattrib("atkdateattribute");
useattrib("atkdatetimeattribute");
useattrib("atktimeattribute");
useattrib("atkdurationattribute");
useattrib("atkdummyattribute");

userelation("atkmanytoonerelation");

class task extends atkNode 
{
  function task()
  {
    $this->atkNode("task");    
    $this->add(new atkAttribute("id",AF_PRIMARY));
    $this->add(new atkListAttribute("priority",array("higest","high","normal","low","lowest")));
    $this->add(new atkAttribute("name", AF_OBLIGATORY));
    $this->add(new atkListAttribute("tasktype",array("project","phase","activity","todo","agenda"),"",AF_OBLIGATORY));
    $this->add(new atkAttribute("refid",AF_OBLIGATORY));
    $this->add(new atkDateAttribute("createdate", "d-m-Y","d-m-Y"));
    $this->add(new atkDateTimeAttribute("editdatetime"));
    $this->add(new atkDateAttribute("startdate", "d-m-Y","d-m-Y"));
    $this->add(new atkDateAttribute("enddate", "d-m-Y","d-m-Y"));
    $this->add(new atkTimeAttribute("starttime"));
    $this->add(new atkTimeAttribute("endtime"));
    $this->add(new atkListAttribute("status",array()));   
    $this->add(new atkManyToOneRelation("owner","employee.employee"));        
    $this->add(new atkDurationAttribute("initial_planning"));
    $this->add(new atkDurationAttribute("current_planning"));
    $this->add(new atkDummyAttribute("registered_hours"));    
    
    $this->setTable("task");
    $this->setIndex("name");
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

}
?>