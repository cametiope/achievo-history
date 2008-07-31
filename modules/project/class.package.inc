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

  include_once("achievotools.inc");
  atkimport("atk.atkmetanode.inc");

  userelation("atkmanytoonerelation");
  useattrib("atklistattribute");
  useattrib("atkradioattribute");
  useattrib("project.packageprojectselector");

  useattrib("project.trafficlightattribute");
  useattrib("project.trafficlightmemoattribute");

  class package extends atkMetaNode
  {

    public $securityAlias = "project.project";
    public $order = "name";
    public $descriptor = "[name]";
    public $flags = array(NF_ADD_LINK,NF_EDITAFTERADD);

    /**
     * Project identifier
     *
     * @var string
     */
    private $project;

    /**
     * Constructor.
     */
    public function __construct()
    {
    	$this->setProjectId();
    	parent::__construct();
    }

    /**
     * Meta.
     *
     * @param atkMetaPolicy $policy
     */
    public static function meta(atkMetaPolicy $policy)
    {
    	$policy->setType('trafficlight', 'trafficlightattribute');
    	$policy->setTypeAndParams('trafficlight_updatestyle', 'atkradioattribute', array('options' => array('manual', 'automatic'), 'values' => array('M', 'A')));
    	
    	$policy->addFlag('trafficlight_updatestyle', AF_FORCE_LOAD);
    	$policy->setTypeAndParams('trafficlight_memo', 'trafficlightmemoattribute', array('size' => array('rows' => '4')));

      $policy->hasOne("employee.employee",array("source"=>"coordinator_id","filter"=>"person.status='active'"));

      $policy->hasMany("package_person");

      $policy->hasOne("project.project",array("source"=>"projectid"),array(),AF_HIDE);
      $policy->addFlags('projectid', AF_HIDE);

      $policy->addFlag('id', AF_FORCE_LOAD);
      $policy->setTypeAndParams('project_package','module.project.attributes.packageprojectselector' );
      $policy->addFlags("master", AF_HIDE_ADD|AF_HIDE_EDIT|AF_FORCE_LOAD);
      $policy->addFlags("master_id", AF_HIDE|AF_FORCE_LOAD);

      $policy->setTypeAndParams("status","atk.attributes.atklistattribute",array(array("active","nonactive","archived")));
      $policy->setTypeAndParams(array("initial_planning","current_planning","remaining_effort_time", "spend_hours"),"atk.attributes.atkdurationattribute",array("15m"));
      $policy->addFlags("initial_planning","current_planning","remaining_effort_time", "spend_hours", AF_READONLY);

      $policy->addFlags("abbreviation_package","name",AF_UNIQUE);
      $policy->addFlags("abbreviation_package","coordinator_id","category_id","status","initial_planning","deposit","master_id",
                        "trafficlight", "trafficlight_updatestyle", "trafficlight_memo", AF_HIDE_LIST);
      $policy->addFlags("name",AF_SEARCHABLE);
      $policy->addFlags("description","startdate","enddate","initial_planning","current_planning","category_id", "status", "deposit", "completed", "spend_hours", "remaining_effort_time", AF_HIDE_ADD);
      $policy->addFlags("coordinator_id",AF_RELATION_AUTOLINK);
      $policy->addFlags("status",AF_LIST_NO_NULL_ITEM);
      $policy->addFlags("remaining_effort_time",AF_READONLY);
      $policy->addFlags("category_id","coordinator_id","deposit",AF_LARGE);
      $policy->addFlags('trafficlight_updatestyle', AF_DISPLAY_VERT);

      $policy->hasMany('deliverable_package', array('source' => 'package_id'));
      $policy->addFlags('package_person', 'deliverable_package', AF_NO_LABEL);

      $policy->setOrder(array('trafficlight', 'trafficlight_updatestyle', 'trafficlight_memo', 'abbreviation_package', 'name', 'projectid',
                              'category_id', 'coordinator_id', 'description', 'status', 'project_package'));
    }


    /**
     * Call custom validation methods for the startdate field
     *
     * @param array $record
     * @param string $mode
     */
    public function startdate_validate(&$record, $mode)
    {
      if( $mode == 'update' )
      {
        $this->validate_startdate_before_parent( $record, $mode);
        $this->validate_startdate_before_enddate( $record, $mode );
        $this->validate_startdate_before_project( $record, $mode );
      }
    }


    /**
     * Call custom validation methods for the enddate field
     *
     * @param array $record
     * @param string $mode
     */
    public function enddate_validate(&$record, $mode)
    {
      if( $mode == 'update' )
      {
        $this->validate_enddate_before_parent( $record, $mode );
        $this->validate_enddate_before_project( $record, $mode );
      }
    }


    /**
     * Custom validation: does the package start on the same date (or later) then its project
     *
     * @param array $record
     * @param string $mode
     */
    private function validate_startdate_before_project( &$record, $mode )
    {
      if( (int) dateutil::arr2str( $record['projectid']['startdate'] ) > (int) dateutil::arr2str($record['startdate']))
      {
        triggerError($record, 'startdate', atktext('package_starts_before_project', 'project', 'package'));
      }
    }


    /**
     * Custom validation: does the package start on the same date (or later) then its master
     *
     * @param array $record
     * @param string $mode
     */
    private function validate_startdate_before_parent( &$record, $mode )
    {
      if( $record['master'] == 'project.package' && $record['master_id'] !== NULL )
      {
        $node = &atkGetNode( 'project.package' );
        $result = $node->selectDb( 'package.id = '. (int) $record['master_id'] );

        if( (int) dateutil::arr2str( $result[0]['startdate'] ) > (int) dateutil::arr2str( $record['startdate'] ) )
        {
          triggerError($record, 'startdate', atktext('package_starts_before_parent_package', 'project', 'package'));
        }
      }
    }


    /**
     * Custom validation: does the package start before it ends
     *
     * @param array $record
     * @param string $mode
     */
    private function validate_startdate_before_enddate( &$record, $mode )
    {
      if( (int) dateutil::arr2str( $record['enddate'] ) < (int) dateutil::arr2str($record['startdate']))
      {
        triggerError($record, 'startdate', atktext('enddate_is_before_startdate', 'project', 'package'));
      }
    }


    /**
     * Custom validation: does the package end before its project ends
     *
     * @param array $record
     * @param string $mode
     */
    private function validate_enddate_before_project( &$record, $mode )
    {
      if( (int) dateutil::arr2str( $record['projectid']['enddate'] ) < (int) dateutil::arr2str($record['enddate']))
      {
        triggerError($record, 'enddate', atktext('package_ends_after_project', 'project', 'package'));
      }
    }


    /**
     * Custom validation: does the package end on the same date (or sooner) then the its master
     *
     * @param array $record
     * @param string $mode
     */
    private function validate_enddate_before_parent( &$record, $mode )
    {
      if( $record['master'] == 'project.package' && $record['master_id'] !== NULL )
      {
        $node = &atkGetNode( 'project.package' );
        $result = $node->selectDb( 'package.id = '. (int) $record['master_id'] );

        if( (int) dateutil::arr2str( $result[0]['enddate'] ) < (int) dateutil::arr2str( $record['enddate'] ) )
        {
          triggerError($record, 'enddate', atktext('package_ends_after_parent_package', 'project', 'package'));
        }
      }
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
      return array("status"=>1,
                   "coordinator_id"=>getUser(),
                   "projectid" => $selectedproject[1],
                   "trafficlight_updatestyle" => "A",
                   "trafficlight" => "0",);
    }


    /**
     * Postmeta.
     */
    public function postMeta()
    {
      $this->getAttribute("master")->addDependee("master_id");
      $this->setColumnAttribs(array('deliverable_package', 'package_person'));
      //$this->setProjectId();
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

    /**
     * We must reload menu after changes in package/task
     *
     * @param Array $postvars
     * @param Boolean $fullpage
     */
    public function dispatch($postvars, $fullpage=true)
    {
      if (!in_array($postvars['atkaction'],array('add','admin')))
      {
        reloadMenu();
      }
      return parent::dispatch($postvars, $fullpage);
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
     * Template override for custom traffic light display
     *
     * @param String $action
     * @param array $record
     * @param String $tab
     * @return String The filename of the template (without path)
     */
    public function getTemplate($action, $record=NULL, $tab="")
    {
      switch ($action)
      {
        case "add": return "editform.tpl";
        case "edit": return "editform_trafficlight.tpl";
        case "view": return "viewform_trafficlight.tpl";
      }
      return parent::getTemplate($action, $record, $tab);
    }

    /**
     * Edit action handler override.
     *
     * Add an extra param to the template
     *
     * @param atkEditHandler $handler
     * @return string HTML
     */
    public function action_edit(&$handler)
    {
      $handler->addRenderBoxVar('projectmenu', $this->getProjectMenu());
      return $handler->action_edit();
    }

    /**
     * AdminHeader override for projectmenu.
     */
    public function adminHeader()
    {
    	$this->getPage()->register_style(atkTheme::getInstance()->stylePath('tabs.css'));
      return $this->getProjectMenu().'<br/>';
    }

    /**
     * AddHeader override for projectmenu.
     */
    public function addHeader()
    {
      $this->getPage()->register_style(atkTheme::getInstance()->stylePath('tabs.css'));
      return $this->getProjectMenu().'<br/>';
    }

    /**
     * AdminHeader override for projectmenu.
     */
    public function viewHeader()
    {
      return $this->getProjectMenu().'<br/>';
    }

    /**
     * Get the project menu.
     *
     * @return string HTML
     */
    private function getProjectMenu()
    {
      atkimport('module.project.projectmenu');
      $pm = new projectmenu($this->atkNodeType(), $this->m_project);
      return $pm->getTabHtml();
    }

    /**
     * Set the project id.
     */
    private function setProjectId()
    {
    	$vars = atkGetPostVar();

      if ($vars['atknodetype'] == 'project.package' &&
          array_key_exists('atkfilter', $vars) &&
          $vars['atkfilter'] != '')
      {
        $this->project = $vars['atkfilter'];
        sessionStore('projectid', $this->project);
      }
      else
      {
        $this->project = sessionLoad('projectid');
      }
    }
  }
