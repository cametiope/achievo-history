<?php
  useattrib("atktextattribute");
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  useattrib("atkradioattribute");
  useattrib("atkdurationattribute");
  useattrib("atkfieldset");
  useattrib("project.trafficlightattribute");
  userelation("project.onetomanymemorelation");
  userelation("atkonetomanyrelation");

  class dashboard extends atkNode
  {

    /**
     * Project identifier
     *
     * @var string
     */
    private $project;

  	/**
  	 * Constructor.
  	 *
  	 * @param String $name
  	 * @param Integer $flags
  	 */
    public function __construct($name="dashboard", $flags=0)
    {
      $this->atkNode($name,$flags );
      $this->project = sessionLoad('projectid');

      $this->add(new atkNumberAttribute("id",AF_AUTOKEY));

      // The next three attributes have to be the first three attributes in this node
      $this->add(new trafficLightAttribute('trafficlight'));
      $this->add(new atkRadioAttribute('trafficlight_updatestyle', array('manual', 'automatic'), array('M', 'A')));
      $this->add(new oneToManyMemoRelation( 'memo', 'project.memo', 'project_id', AF_HIDE_LIST|AF_HIDE_ADD ) );
      
      $this->add(new atkAttribute("name",AF_READONLY));
      $this->add(new atkDateAttribute("startdate","","","","",AF_READONLY));
      $this->add(new atkDateAttribute("enddate","","","","",AF_READONLY));
      $this->add(new atkDateAttribute("initial_enddate","","","","",AF_READONLY));

      $this->add(new atkNumberAttribute("completed",AF_READONLY));

      // Time registration
      $this->add(new atkDurationAttribute('initial_planning', '15m', '100h', AF_READONLY));
      $this->add(new atkDurationAttribute('current_planning', '15m', '100h', AF_READONLY));

      $this->add(new atkDurationAttribute('spend_hours', '15m', '100h', AF_READONLY));

      $this->add(new atkFieldSet('actual_planning', '[current_planning.field] &nbsp; ' . $this->text('booked') . ': [spend_hours.field]'));

      $this->add(new atkDurationAttribute('remaining_effort_time', '15m', '100h', AF_READONLY));

      $this->add(new atkOneToManyRelation("phase", "project.phase", "projectid", AF_NO_LABEL|AF_HIDE_LIST|AF_CASCADE_DELETE))
           ->setDestinationFilter("phase.completed < 100 OR phase.completed IS NULL");

      $deliverable = &$this->add(new atkOneToManyRelation("deliverable", "project.deliverable","project_id", AF_NO_LABEL|AF_HIDE_LIST|AF_CASCADE_DELETE));
      $deliverable->setDestinationFilter('deliverable.project_id=[id] AND deliverable.status <> "complete"');

      if (moduleExists('history'))
      {
        $history = &$this->add(new atkOneToManyRelation("history", "history.history","master_id", AF_NO_LABEL|AF_HIDE_LIST|AF_CASCADE_DELETE));
        $history->setDestinationFilter("master='project.project' AND master_id=[id]");
      }

      $this->setTable("project","project");
      $this->setOrder("project.abbreviation, project.name");
      $this->setIndex("name");
      $this->setDescriptorTemplate('[name]');
      $this->setColumnAttribs(array('deliverable', 'history'));
    }

    /**
     * Display override for completed field.
     *
     * @param array $record
     * @param string $mode
     * @return string
     */
    public function completed_display($record,$mode)
    {
    	return $record['completed'].'%';
    }


    /**
     * Last minute overrides.
     *
     * Hide the initial enddate when it equals the current enddate.
     *
     * @param Array $record
     * @param String $mode
     */
    public function preAddToEditArray($record, $mode)
    {
    	if ($record['enddate'] == $record['initial_enddate'] && $mode != 'add')
    	{
    		$this->getAttribute('initial_enddate')->addFlag(AF_HIDE);
    	}
    }

    /**
     * Template override for custom traffic light display.
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
     * Disable all buttons in this node.
     *
     * @param String $mode
     * @param Array $record
     * @return Array
     */
    public function getFormButtons($mode, $record)
	  {
      return array();
	  }

    /**
     * ViewHeader override for projectmenu.
     */
    public function viewHeader()
    {
      return $this->getProjectMenu();
    }

    /**
     * Get the project menu.
     *
     * @return string HTML
     */
    private function getProjectMenu()
    {
      atkimport('module.project.projectmenu');
      $pm = new projectmenu('project.dashboard', $this->project);
      return $pm->getTabHtml();
    }

  }
