<?php

  useattrib("atkdateattribute");
  useattrib("atklistattribute");
  useattrib("atknumberattribute");
  useattrib("atkboolattribute");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");

  /**
   * The node class for deliverable.
   */
  class deliverable extends atkNode
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
  	 */
    public function __construct()
    {
      $this->atkNode("deliverable", NF_ADD_LINK|NF_EDITAFTERADD);

      $this->project = sessionLoad('projectid');

      $this->add(new atkNumberAttribute("id", AF_AUTOKEY));

      $this->add(new atkAttribute("name", AF_OBLIGATORY, 50));
      $this->add(new atkManyToOneRelation("project_id", "project.project", AF_HIDE|AF_OBLIGATORY));
      $this->add(new atkDateAttribute("duedate", AF_OBLIGATORY));
      $this->add(new atkListAttribute("status", array("new","complete"), array(), AF_OBLIGATORY|AF_FORCE_LOAD));
      $this->add(new atkNumberAttribute("remind_days"));

      // Tracking
      $this->add(new atkManyToOneRelation("userid","employee.employee", AF_HIDE|AF_FORCE_LOAD|AF_OBLIGATORY|AF_READONLY));
      $this->add(new atkDateAttribute("entrydate",AF_HIDE|AF_READONLY|AF_OBLIGATORY));

      // Relations
      $this->add(new atkOneToManyRelation('phase', 'project.deliverable_phase','deliverable_id', AF_HIDE_ADD|AF_HIDE_LIST));
      $this->add(new atkOneToManyRelation('package', 'project.deliverable_package', 'deliverable_id', AF_HIDE_ADD|AF_HIDE_LIST));
      $this->add(new atkOneToManyRelation('document','project.deliverable_document', 'deliverable_id', AF_HIDE_ADD|AF_HIDE_LIST));

      $this->add(new atkDummyAttribute('icon', '', AF_HIDE_VIEW|AF_HIDE_ADD|AF_HIDE_EDIT|AF_FORCE_LOAD|AF_NO_LABEL));

      $this->add(new atkNumberAttribute("emailstatus", AF_HIDE|AF_FORCE_LOAD));
      $this->addSecurityMap("view","project.admin");
      $this->setTable("deliverable");
      $this->setOrder("duedate ASC", "name ASC");
    }


    /**
     * Set up default values for this node
     * @return array Associative array where the key is
     *               the attribute name, and value is the
     *               default value.
     */
    public function initial_values()
    {
      $user = &getUser();
      return array("userid"=>array("id"=>$user["id"]),
                   "remind_days" =>7,
                   "status"       =>"new",
                   "entrydate"    =>array("year"=>date("Y"),
                     "month"=>date("m"),
                     "day"=>date("d")));
    }

    /**
     * Return record descriptor (for use in relations etc.)
     * @return String A template containing fieldnames between brackets.
     */
    public function descriptor_def()
    {
      return "[name]";
    }


    /**
     * Override default admin screen to implement a different template
     *
     * @param object $handler
     */
    public function action_admin( &$handler )
    {
      $smarty = &atkinstance('atk.ui.atksmarty');
      atkimport("atk.ui.atktheme");
      $theme = &atkTheme::getInstance();

      $page = &$this->getPage();
      $vars = &atkGetPostVar();

      $page->register_style($theme->stylePath("tabs.css"));

      $milestones = array();
      $results = $this->selectDb($vars['atkfilter']);

      for ($i=0,$_i=count($results);$i<$_i;$i++)
      {
        $dispatch_params = array( 'atkselector'    => 'deliverable.id='.$results[$i]['id'],
                                  'atklevel'       => 1,
                                  'atkprevlevel'   => 0,
                                  'atkstackid'     => atkStackID() );

        $milestones[$i]['id']       = $results[$i]['id'];
        $milestones[$i]['name']     = $results[$i]['name'];
        $milestones[$i]['status']   = $results[$i]['status'];
        $milestones[$i]['duedate']  = $results[$i]['duedate'];
        $milestones[$i]['userid']['lastname'] = $results[$i]['userid']['lastname'];
        $theme->iconPath( 'edit', 'recordlist');
        $milestones[$i]['editlink']   = '<a href="'.dispatch_url('project.deliverable', 'edit', $dispatch_params ).'" title="edit"><img src="'.$theme->iconPath( 'edit', 'recordlist').'" alt="edit" border="0" /></a>';
        $milestones[$i]['deletelink'] = '<a href="'.dispatch_url('project.deliverable', 'delete', $dispatch_params ).'" title="delete"><img src="'.$theme->iconPath( 'delete', 'recordlist').'" alt="delete" border="0" /></a>';
      }

      $smarty->assign('addlink', '<a href="'.dispatch_url('project.deliverable', 'add', array('atklevel' => 1, 'atkprevlevel' => 0, 'atkstackid' => atkStackID() ) ).'" title="add">'.atktext('link_deliverable_add', 'project').'</a>');
      $smarty->assign('header_titles', array('&nbsp;', 'Date', 'Progress', 'Owner', 'Status', '&nbsp;'));
      $smarty->assign('milestones', $milestones);
      $smarty->assign('projectmenu', $this->getProjectMenu());
      $content = $smarty->fetch(moduleDir('project').'templates/milestones_admin.tpl');
      $ui = &$this->getUi();
      $this->addStyle("style.css");

      $result = $ui->renderBox(array("title" => "<strong>".atktext('deliverables')."</strong><br/><br/>", "content" => $content));
      $page->addContent($this->renderActionPage("admin", $result));
    }


    /**
     * Get the children of a milestone
     * These can be packages, phases and documents
     * Called by Ajax
     *
     * @param integer $milestone_id
     * @return array
     */
    public function getMilestoneChildren( $milestone_id )
    {
      $children['packages']   = $this->fetchPackagesByMilestone( $milestone_id );
      $children['phases']     = $this->fetchPhasesByMilestone( $milestone_id );
      $children['documents']  = $this->fetchDocumentsByMilestone( $milestone_id );

      return $children;
    }


    /**
     * Get the children of a package
     * These can be other packages or phases
     * Called by Ajax
     *
     * @param integer $package_id
     * @return array
     */
    public function getPackageChildren( $package_id )
    {
      $children['packages'] = $this->fetchPackagesByPackage( $package_id );
      $children['phases']   = $this->fetchPhasesByPackage( $package_id );

      return $children;
    }


    /**
     * Get the packages that are connected to a certain milestone from the database
     *
     * @param integer $milestone_id
     * @return array
     */
    private function fetchPackagesByMilestone( $milestone_id )
    {
      $packages = array();
      $node = &atkGetNode( 'project.deliverable_package' );
      $results = $node->selectDb( 'deliverable_package.deliverable_id = '.(int) $milestone_id );

      if( count( $results ) > 0 )
      {
        foreach( $results AS $package )
        {
          $packages[] = array( 'id'       => $package['package_id']['id'],
                               'name'     => $package['package_id']['name'],
                               'progress' => 'xx', );
        }
      }

      return $packages;
    }


    /**
     * Get the phases that are connected to a certain milestone from the database
     *
     * @param integer $milestone_id
     * @return array
     */
    private function fetchPhasesByMilestone( $milestone_id )
    {
      $phases = array();
      $node = &atkGetNode( 'project.deliverable_phase' );
      $results = $node->selectDb( 'deliverable_phase.deliverable_id = '.(int) $milestone_id );

      if( count( $results ) > 0 )
      {
        foreach( $results AS $phase )
        {
          $phases[] = array( 'id'       => $phase['phase_id']['id'],
                             'name'     => $phase['phase_id']['name'],
                             'progress' => 'xx', );
        }
      }

      return $phases;
    }


    /**
     * Get the packages that are connected to a certain package from the database
     *
     * @param integer $package_id
     * @return array
     */
    private function fetchPackagesByPackage( $package_id )
    {
      $packages = array();

      $node = &atkGetNode( 'project.package' );
      $results = $node->selectDb( 'package.master = "project.package" AND package.master_id = '.(int) $package_id );

      if( count( $results ) > 0 )
      {
        foreach( $results AS $package )
        {
          $packages[] = array( 'id'       => $package['id'],
                               'name'     => $package['name'],
                               'progress' => 'xx', );
        }
      }

      return $packages;
    }


    /**
     * Get the phases that are connected to a certain package from the database
     *
     * @param integer $package_id
     * @return array
     */
    private function fetchPhasesByPackage( $package_id )
    {
      $phases = array();

      $node = &atkGetNode( 'project.phase' );
      $results = $node->selectDb( 'phase.master = "project.package" AND phase.master_id = '.(int) $package_id );

      if( count( $results ) > 0 )
      {
        foreach( $results AS $phase )
        {
          $phases[] = array( 'id'       => $phase['id'],
                             'name'     => $phase['name'],
                             'progress' => 'xx', );
        }
      }

      return $phases;
    }


    /**
     * Get the documents that belong to a certain milestone from the database
     *
     * @param integer $milestone_id
     * @return array
     */
    private function fetchDocumentsByMilestone( $milestone_id )
    {
      $documents = array();
      $node = &atkGetNode( 'project.deliverable_document' );
      $results = $node->selectDb( 'deliverable_document.deliverable_id = '.(int) $milestone_id );

      if( count( $results ) > 0 )
      {
        foreach( $results AS $document )
        {
          $documents[] = array( 'id'       => $document['document_id']['id'],
                                'name'     => $document['document_id']['name'],
                                'file'     => $document['document_id']['filename'], );
        }
      }

      return $documents;
    }

    /**
     * Icon display override to show custom icons in the record list
     *
     * @param Array $record
     * @param String $mode
     * @return String with HTML
     */
    public function icon_display($record, $mode="")
    {

    	$today = gmmktime(0,0,0, date('m'), date('d'), date('Y'));
    	$duedate = gmmktime(0,0,0, $record['duedate']['month'], $record['duedate']['day'], $record['duedate']['year']);

    	if ($duedate < $today) return '<img src="images/milestone_red.png" alt="" />';
    	else if ($duedate == $today || $duedate < ($today + ($record['remind_days'] * 60 * 60 * 24)))  return '<img src="images/milestone_yellow.png" alt="" />';
    	else return '<img src="images/milestone_green.png" alt="" />';

    	return $record['icon'];
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

  }