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
  
  function meta(&$policy)
  {
    /*@var $policy atkMetaPolicy*/
    $policy->hasOne("employee.employee",array("source"=>"coordinator_id","filter"=>"person.status='active'"));
    $policy->hasMany("package_person");
    
    $policy->addFlags('projectid', AF_HIDE);
    $policy->setTypeAndParams("master","atk.attributes.atklistattribute",array(array("project.package","project.project")));
    $policy->setTypeAndParams("master_id","module.project.attributes.dynamicmanytoonerelation");

    $policy->setTypeAndParams("status","atk.attributes.atklistattribute",array(array("active","nonactive","archived")));
    $expression = "(('[spend_hours]' ? '[spend_hours]' : 0) + ('[remaining_effort_time]' ? '[remaining_effort_time]' : 0)) ? 
    (('[spend_hours]' ? '[spend_hours]' : 0)/(('[spend_hours]' ? '[spend_hours]' : 0) + ('[remaining_effort_time]' ? '[remaining_effort_time]' : 0))*100) : 0";
    $policy->setTypeAndParams("completed","atk.attributes.atkcalculatorattribute",array($expression));
    $policy->setTypeAndParams(array("initial_planning","current_planning","remaining_effort_time"),"atk.attributes.atkdurationattribute",array("15m"));

    $policy->addFlags("abbreviation","name",AF_UNIQUE);
    $policy->addFlags("abbreviation","coordinator_id","category_id","status","initial_planning","completed","deposit",
      AF_HIDE_LIST);
    $policy->addFlags("name",AF_SEARCHABLE);
    $policy->addFlags("description","coordinator_id","startdate","enddate","initial_planning","current_planning","master",
      "master_id","category_id", "status", "deposit", "completed", "spend_hours", "remaining_effort_time", AF_HIDE_ADD);
    $policy->addFlags("coordinator_id",AF_RELATION_AUTOLINK);
    $policy->addFlags("status",AF_LIST_NO_NULL_ITEM);
    $policy->addFlags("remaining_effort_time",AF_READONLY);
    $policy->addFlags("category_id","coordinator_id","deposit",AF_LARGE);
  }
  
  /**
   * Privent over branch limit
   *
   * @param array $record
   * @param string $mode
   */
  function master_id_validate(&$record, $mode)
  {
    $db = &$this->getDb();
    $rows = $db->getrows("SELECT id FROM package WHERE master_id='".$record["id"]."'");
    if(count($rows)>0)
    {
      triggerError($record, "master_id", "Long_chain_possible");
    }
  }
  
  function master_id_selection($record=array(), $mode="select")
  {
    if($record['master'] == 'project.package')
    {
      $this->getAttribute("master_id")->addDestinationFilter("package.id<>[id]");
      
      if(atkConfig::get("project","package_number_of_branches") != -1)
      {
        $this->getAttribute("master_id")->addDestinationFilter($this->_getPackageLevelId());
      }
    }
    elseif($record['master'] == 'project.project')
    {
      $this->getAttribute("master_id")->setDestinationFilter("");
    }
    
    return $this->getAttribute("master_id")->getSelectableRecords($record, $mode);
  }
  
  /**
   * Packages can have sub-packages, which also can have sub-packages, etc. The limit of branches is configurable.
   * This function return filter with packages, which lower then level in config file. 
   *
   * @return string Filter
   */
  function _getPackageLevelId()
  {
    $id = atkConfig::get("project","package_number_of_branches");
    $db = &$this->getDb();
    
    $query = "SELECT p0.id FROM package p0 ";
    
    for($i=1; $i<$id; $i++)
    {
      $r = $i-1;
      
      $query .= "LEFT JOIN package p$i ON p$r.master_id=p$i.id AND p$r.master='project.package' ";
    }
    $query .= "WHERE p".($i-1).".id IS NULL";
    
    $rows = $db->getrows($query);
    
    foreach ($rows as $row)
    {
      $arr[] = $row["id"];
    }
    
    return "package.id IN(".implode(",",$arr).")";
  }
  
  function initial_values()
  {
    $selectedproject = atkGetPostVar("projectid") != ''  ? atkGetPostVar("projectid") : 0;

    $nextyear = strtotime("+1 year");
    return array("startdate"=>array("year"=>date("Y"),
                                    "month"=>date("m"),
                                    "day"=>date("d")),
                 "enddate"=>array("year"=>(date("Y",$nextyear)),
                                  "month"=>date("m",$nextyear),
                                  "day"=>date("d",$nextyear)),
                 "status"=>1,
                 "master"=>"project.package",
                 "coordinator_id"=>getUser(),
                 'projectid' => $selectedproject);
  }
  
  function postMeta()
  {
    $this->getAttribute("master")->addDependee("master_id");
  }
  
  function completed_display($record, $mode="")
  {
    return number_format($this->getAttribute("completed")->display($record, $mode))."%";
  }
}
?>