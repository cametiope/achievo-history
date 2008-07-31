<?php

  atkimport("module.project.utils.projectutils");
  atkimport("module.timereg.hours");

  class timereg_advanced extends atkNode
  {

  	/**
  	 * Hours.
  	 *
  	 * @var array
  	 */
  	private $m_hours = array();

    /**
     * Errors.
     *
     * @var array
     */
    private $m_errors = array();

  	/**
  	 * Constructor.
  	 *
  	 * @param string $name
  	 * @param integer $flags
  	 */
    public function __construct($name="timereg_advanced", $flags=0)
    {
      $this->atkNode($name, $flags);
    }

    /*
     * Override of the add action.
     */
    public function action_add(&$handler)
    {
      $page = &$this->getPage();
      $ui = &$this->getUi();
      $smarty = &atkinstance('atk.ui.atksmarty');
      $theme = &atkTheme::getInstance();

      $vars = atkGetPostVar();

      $this->setHours($vars);
      if ( count($this->m_hours) > 0 && $this->validate_timereg() )
      {
    		$this->processTimereg();
        $redirect = dispatch_url('timereg_advanced.timereg_advanced', 'report');
	      $this->redirect($redirect, $this->m_hours);
      }

      $smarty->assign('rows', $this->getActivePhases());
      $smarty->assign('completed', $this->getCompletedValues());
      $smarty->assign('times', $this->getRegistrationTimes());
      $smarty->assign('formstart', $this->getFormStart($handler));
      $smarty->assign('formend', $this->getFormEnd());
      $smarty->assign('buttons', $this->getFormButtons('add', ''));
      $smarty->assign('values', $vars);
      $smarty->assign('errors', $this->m_errors);

      $content = $smarty->fetch(moduleDir('timereg_advanced').'templates/timereg.tpl');

      $page->register_style($theme->stylePath("style.css"));
      $page->register_style($theme->stylePath("tabs.css"));
      $page->register_style($theme->stylePath("recordlist.css"));
      $page->register_script("./atk/javascript/recordlist.js");
      $page->register_script(moduleDir('timereg_advanced')."javascript/timereg_advanced.js");

      $output = $ui->renderBox(array("title"=>atktext('advanced_time_registration'),
                                   "content"=>$content));

      $page->addContent($this->renderActionPage('add', $output));
    }

    /**
     * The add mode only needs a save button.
     *
     * @param string $mode
     * @param array $record
     * @return array $buttons
     */
	  function getFormButtons($mode, $record)
	  {
	  	$buttons = array();
	    $controller = &atkcontroller::getInstance();
	    $controller->setNode($this);

	    if ($mode == 'add') $buttons[] = $controller->getButton('save');
	    else $button = $controller->getFormButtons($mode, $record);

	    return $buttons;
	  }

    /**
     * Report action.
     *
     */
    public function action_report()
    {
      $page = &$this->getPage();
      $ui = &$this->getUi();
      $smarty = &atkinstance('atk.ui.atksmarty');
      $theme = &atkTheme::getInstance();

      $content = atktext('timereg_finished');
      $backurl = dispatch_url('timereg_advanced.timereg_advanced', 'add');
      $content .= '<br/><br/><input type="button" name="back" value="&laquo; '.atktext('back').'" onclick="location.href=\''.$backurl.'\'" />';

      $page->register_style($theme->stylePath("style.css"));
      $page->register_style($theme->stylePath("tabs.css"));
      $page->register_style($theme->stylePath("recordlist.css"));

      $output = $ui->renderBox(array("title"=>atktext('advanced_time_registration'),
                                   "content"=>$content));

      $page->addContent($this->renderActionPage('report', $output));
    }

    /**
     * Select the active phases for the loggedin user.
     *
     * @return Array
     */
    private function getActivePhases()
    {
      $node = atkGetNode('project.phase');
      $rows = $node->select($node->getTable().'.startdate < NOW() AND '.$node->getTable().'.enddate > NOW()')
                   ->allRows();

      for ($i=0, $_i=count($rows); $i<$_i; $i++)
      {
      	$rows[$i]['activities'] = $this->getActivities($rows[$i]['id']);
      }

      return $rows;
    }

    /**
     * Get the activities by phase.
     *
     * @param integer $phase Phase Id
     * @return array
     */
    private function getActivities($phase)
    {
      $node = atkGetNode('project.phase_activity');
      return $node->select('phaseid='.$phase)
                  ->excludes('phaseid')
                  ->allRows();
    }

    /**
     * Get a list with completed values.
     *
     * @return Array List of completed values
     */
    private function getCompletedValues()
    {
      return range(0, 100, atkConfig::get('project','completed_parts'));
    }

    /**
     * Get a list with hours and minutes for time registration.
     *
     * @return Array List with hours and minutes
     */
    private function getRegistrationTimes()
    {
    	return array('hours'   => range(0, atkconfig("max_bookable", 10) , 1),
    	             'minutes' => range(0, 59, 15));
    }

    /**
     * Get the start of the form.
     *
     * @return String HTML The forms' start
     */
    private function getFormStart(&$handler)
    {
      $controller = &atkcontroller::getInstance();
      $controller->setNode($this->m_node);

      $node = &$this->m_node;

      $formstart ='<form id="entryform" name="entryform" enctype="multipart/form-data" action="'.$controller->getPhpFile().'?'.SID.'"'.
                     ' method="post" onsubmit="return globalSubmit(this)">';

      $formstart .= session_form(SESSION_DEFAULT, $handler->getReturnBehaviour(), parent::getEditFieldPrefix());
      $formstart .= $controller->getHiddenVarsString();

      if (isset($node->m_postvars['atkfilter']))
      {
        $formstart .= '<input type="hidden" name="atkfilter" value="'.$node->m_postvars['atkfilter'].'">';
      }

      return $formstart;
    }

    /**
     * Validate the registered hours.
     *
     * @return boolean
     */
    private function validate_timereg()
    {
    	$i = 0;
      foreach ($this->m_hours as $hour)
      {
      	$j = 0;

      	if ($hour['time']['hours'] == 0 && $hour['time']['minutes'] == 0 )
      	{
      		$this->m_errors[$i] = array();
      		$this->m_errors[$i]['name'] = $hour['name'];
      		$this->m_errors[$i]['err'] = array();
      		$this->m_errors[$i]['err'][$j++] = atktext('time').': ' . atktext('error_obligatoryfield');
      	}

        if (trim($hour['remark']) == '')
        {
        	if ($j == 0)
        	{
	          $this->m_errors[$i] = array();
	          $this->m_errors[$i]['name'] = $hour['name'];
	          $this->m_errors[$i]['err'] = array();
        	}
          $this->m_errors[$i]['err'][$j++] = atktext('comment').': ' . atktext('error_obligatoryfield');
        }

        $i++;
      }

      if (count($this->m_errors) > 0 ) return false;
      return true;
    }

    /**
     * Get the end of the form.
     *
     * @return String HTML The forms' end
     */
    private function getFormEnd()
    {
      return '</form>';
    }

    /**
     * Store the enabled time registrations in a private member.
     *
     * @param array $vars
     */
    private function setHours($vars)
    {
    	$nHours = count($vars['timereg']);
    	if ($nHours > 0)
    	{
    		$timereg = $vars['timereg'];
    		foreach ($timereg as $item)
    		{
    			if ($item['id'] == 'on') $this->m_hours[] = $item;
    		}
    	}
    }

    /**
     * Save the time registrations to the database.
     */
    private function processTimereg()
    {
      $node = atkGetNode('timereg.hours');
      $db = $this->getDb();
      $userid = atkGetUserId();

      foreach ($this->m_hours as $hour)
      {
        $item = array();
        $item['activitydate'] = array('day' => date('d'), 'month' => date('m'), 'year' => date('Y'));
        $item['userid']       = $userid;
        $item['phaseid']      = $hour['phaseid'];
        $item['activityid']   = $hour['activity'];
        $item['remark']       = $db->escapeSQL($hour['remark']);
        $item['time']         = ($hour['time']['hours'] * 60) + $hour['time']['minutes'];
        $item['workperiod']   = 0;
        $item['entrydate']    = array('day' => date('d'), 'month' => date('m'), 'year' => date('Y'));
        $node->addDb($item);
        $this->updatePhase($hour['phaseid'], $hour['completed']);
      }
    }

    /**
     * Update phaseid with new completed value
     *
     * @param integer $phaseid
     * @param integer $completed
     */
    private function updatePhase($phaseid, $completed)
    {
    	$this->getDb()->query("UPDATE phase SET completed=".$completed." WHERE id=".$phaseid);
    }

  }