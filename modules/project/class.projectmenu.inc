<?php

  class projectmenu extends atkNode
  {

  	/**
  	 * Active node
  	 *
  	 * @var string
  	 */
  	private $activeNode;

  	/**
  	 * Tabs
  	 *
  	 * @var array
  	 */
  	private $tabs;

  	/**
  	 * Project id selector
  	 *
  	 * @var string
  	 */
  	private $project;

  	/**
  	 * Constructor.
  	 *
  	 * @param string $node
  	 */
  	public function __construct($node='', $project='')
  	{
  		if ($project == '') $project = sessionLoad('projectid');

  		$this->activeNode = $node;
  		$this->tabs       = array();
  		$this->setProjectId($project);

  		$this->generateTab('project.dashboard',              'view',   atktext('dashboard'),        'atkselector', 'project.id');
  		$this->generateTab('project.project',                'edit',   atktext('project'),          'atkselector', 'project.id');
  		$this->generateTab('project.package',                'admin',  atktext('package'),          'atkfilter',   'projectid');
  		$this->generateTab('project.phase',                  'admin',  atktext('phase'),            'atkfilter',   'projectid');
  		$this->generateTab('project.resourceplanning',       'report', atktext('resourceplanning'), 'atkfilter',   'project.id');
  		$this->generateTab('project.project_personemployee', 'admin',  atktext('team'),             'atkfilter',   'projectid');
  		$this->generateTab('project.deliverable',            'admin',  atktext('milestones'),       'atkfilter',   'project_id');
  		if (moduleExists('history'))
  		  $this->generateTab('history.history',              'admin',  atktext('history'),          'atkfilter',   'project_id');

  		//$this->generateTab('project.document',               'admin', atktext('documents'));
  		//$this->generateTab('project.email',                  'admin', atktext('emails'));
  		//$this->generateTab('project.notices',                'admin', atktext('notices'));
  		//$this->generateTab('project.finance',                'admin', atktext('finance'));
  	}

  	/**
  	 * Generate project tab
  	 *
  	 * @param String $node
  	 * @param String $action
  	 * @param String $label
  	 */
  	private function generateTab($node, $action, $label, $filtertype, $filterfield)
  	{
  		if (atkGetNode($node)->allowed($action))
  		{
  			$url = dispatch_url($node, $action);
  			$active = ($node == $this->activeNode) ? 1 : 0;
  			$this->tabs[] = array('label'  => $label,
  			                      'url'    => $url,
  			                      'filter' => $this->getFilter($filtertype, $filterfield),
  			                      'active' => $active);
  		}

  		return;
  	}

  	/**
  	 * Set the project id
  	 *
  	 * @param  $project
  	 */
  	private function setProjectId($project)
  	{
  		$filter = split('=', $project);
  		$this->project = $filter[1];
  	}

  	/**
  	 * Get the filter url
  	 *
  	 * @param string $filtertype
  	 * @param string $filterfield
  	 * @return string Filter
  	 */
  	private function getFilter($filtertype, $filterfield)
  	{
      return $filtertype.'='.$filterfield.rawurlencode('='.$this->project);
  	}

  	/**
  	 * Get the HTML of the tabs
  	 *
  	 * @return string HTML
  	 */
  	public function getTabHtml()
  	{
  		$smarty = &atkinstance('atk.ui.atksmarty');
  		$theme = &atkTheme::getInstance();

  		$smarty->assign('tabs', $this->tabs);
      $html = $smarty->fetch($theme->themeDir().'/templates/projectmenu.tpl');
      return $html;
  	}

  }