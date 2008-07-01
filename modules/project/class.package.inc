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
  	
    public $securityAlias = "project.project";
    public $order = "name";
    public $descriptor = "[name]";
    public $flags = array(NF_ADD_LINK,NF_EDITAFTERADD);
  
    /**
     * Meta.
     *
     * @param atkMetaPolicy $policy
     */
    public static function meta(atkMetaPolicy $policy)
    {
    	//parent::meta($policy);
    	
      $policy->hasOne("employee.employee",array("source"=>"coordinator_id","filter"=>"person.status='active'"));
      //$policy->hasMany("package_person");
    
      $policy->addFlags('projectid', AF_HIDE);
      $policy->setTypeAndParams("master","atk.attributes.atklistattribute",array(array("project.package","project.project")));
      $policy->setTypeAndParams("master_id","module.project.attributes.dynamicmanytoonerelation");

      $policy->setTypeAndParams("status","atk.attributes.atklistattribute",array(array("active","nonactive","archived")));
//      $expression = "(('[spend_hours]' ? '[spend_hours]' : 0) + ('[remaining_effort_time]' ? '[remaining_effort_time]' : 0)) ? 
//        (('[spend_hours]' ? '[spend_hours]' : 0)/(('[spend_hours]' ? '[spend_hours]' : 0) + ('[remaining_effort_time]' ? '[remaining_effort_time]' : 0))*100) : 0";
//      $policy->setTypeAndParams("completed","atk.attributes.atkcalculatorattribute",array($expression));
      $policy->setTypeAndParams(array("initial_planning","current_planning","remaining_effort_time", "spend_hours"),"atk.attributes.atkdurationattribute",array("15m"));
      $policy->addFlags("initial_planning","current_planning","remaining_effort_time", "spend_hours", AF_READONLY);

      $policy->addFlags("abbreviation","name",AF_UNIQUE);
      $policy->addFlags("abbreviation","coordinator_id","category_id","status","initial_planning","deposit","master_id",
        AF_HIDE_LIST);
      $policy->addFlags("name",AF_SEARCHABLE);
      $policy->addFlags("description","startdate","enddate","initial_planning","current_planning","master",
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
    public function master_id_validate(&$record, $mode)
    {
      if(atkConfig::get("project","package_number_of_branches") != -1)
      {
        $db = &$this->getDb();
        $rows = $db->getrows("SELECT id FROM package WHERE master_id='".$record["id"]."'");
        if(count($rows)>0)
        {
          triggerError($record, "master_id", "Long_chain_possible");
        }
      }
    }
  
    public function master_id_selection($record=array(), $mode="select")
    {
      if($record['master'] == 'project.package')
      {
        $this->getAttribute("master_id")->addDestinationFilter("package.id<>[id]");
      
        if(atkConfig::get("project","package_number_of_branches") != -1)
        {
          $this->getAttribute("master_id")->addDestinationFilter($this->getPackageLevelId());
        }
      }
      elseif($record['master'] == 'project.project')
      {
        $this->getAttribute("master_id")->setDestinationFilter("");
      }
    
      return $this->getAttribute("master_id")->getSelectableRecords($record, $mode);
    }
  
    /**
     * Display the master - master_id column a bit more userfriendly
     *
     * @param array $record
     * @param string $mode
     */
    public function master_display($record, $mode)
    {
      $node   = getNode( $record['master'] );
      $node->addFilter( $node->getTable().'.id', $record['master_id'] );
      $result = $node->selectDb();

      return ucfirst( substr($record['master'], ( strpos( $record['master'], '.' ) +1 ) ) ).': '. $result[0]['name'];
    }
    
    /**
     * Packages can have sub-packages, which also can have sub-packages, etc. The limit of branches is configurable.
     * This function return filter with packages, which lower then level in config file. 
     *
     * @return string Filter
     */
    private function getPackageLevelId()
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
  
    /**
     * Initial values
     *
     * @return array with initial values
     */
    public function initial_values()
    {
    	global $ATK_VARS;
      $selectedproject = decodeKeyValueSet($ATK_VARS['atkfilter']);

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
                   "projectid" => $selectedproject[1]);
    }
  
    public function postMeta()
    {
      $this->getAttribute("master")->addDependee("master_id");
    }
  
    /**
     * Display override for the completed attribute
     *
     * @param array $record
     * @param string $mode
     * @return string
     */
    public function completed_display($record, $mode="")
    {
      return number_format($this->getAttribute("completed")->display($record, $mode))."%";
    }
  }
