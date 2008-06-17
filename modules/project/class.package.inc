<?php

/**
 * Package
 *
 * In the package node you can create, view, edit and delete packages.
 *
 * @package achievo
 * @subpackage modules
 *
 * @author yury <yury@achievo.org>
 *
 * @copyright (c) 2008 Ibuildings.nl BV
 * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
 *
 * @version $Revision$
 * $Id$
 */

atkimport("atk.atkmetanode.inc");

userelation("atkmanytoonerelation");
useattrib("atklistattribute");

class package extends atkMetaNode
{
  var $securityAlias = "project.project";
  var $order = "name";
  var $descriptor = "[name]";
  var $flags = array(NF_EDITAFTERADD,NF_IMPORT);
  var $filter = "package.status='1'";
  
  function meta(&$policy)
  {
    /*@var $policy atkMetaPolicy*/
    $policy->hasOne("employee.employee",array("source"=>"coordinator_id","filter"=>"person.status='active'"));
    $policy->hasMany("package_person");
    $policy->hasOne("package",array("source"=>"master_id"));

    $policy->setTypeAndParams("status","atk.attributes.atklistattribute",array(array("active","nonactive","archived"),array(1,2,3)));

    $policy->addFlags("abbreviation","name",AF_UNIQUE);
    $policy->addFlags("name","coordinator_id","category_id","status","initial_planning","completed","planned_time",
      "dependsondeliverable",AF_HIDE_LIST);
    $policy->addFlags("abbreviation",AF_SEARCHABLE);
    $policy->addFlags("description","coordinator_id","startdate","enddate","initial_planning","current_planning",
      "remaining_effort_time","overall_time","planned_time",AF_HIDE_ADD);
    $policy->addFlags("coordinator_id",AF_RELATION_AUTOLINK);
    $policy->addFlags("status",AF_LIST_NO_NULL_ITEM);
    $policy->addFlags("remaining_effort_time","planned_time",AF_READONLY);

  }
  
  function initial_values()
  {
    $nextyear = strtotime("+1 year");
    return array("startdate"=>array("year"=>date("Y"),
                                    "month"=>date("m"),
                                    "day"=>date("d")),
                 "enddate"=>array("year"=>(date("Y",$nextyear)),
                                  "month"=>date("m",$nextyear),
                                  "day"=>date("d",$nextyear)),
                 "status"=>1,
                 "coordinator_id"=>getUser());
  }

}
?>