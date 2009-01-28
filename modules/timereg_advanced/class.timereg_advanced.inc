<?php

atkimport( "module.project.utils.projectutils" );
atkimport( "module.timereg.hours" );

class timereg_advanced extends atkNode
{
  
  /**
   * Hours.
   *
   * @var array
   */
  protected $m_hours = array();
  
  /**
   * Errors.
   *
   * @var array
   */
  protected $m_errors = array();
  
  /**
   * Obligatory comments by projects.
   *
   * @var array
   */
  protected $m_obligatoryProjects = array();


  /**
   * Constructor.
   *
   * @param string $name
   * @param integer $flags
   */
  public function __construct( $name = "timereg_advanced", $flags = 0 )
  {
    $this->atkNode( $name, $flags );
    
    $this->addSecurityMap( 'favorites', 'add' );
    $this->addSecurityMap( 'addfavorite', 'add' );
    $this->addSecurityMap( 'removefavorite', 'add' );
    $this->addSecurityMap( 'times', 'add' );
  }


  /*
   * Override of the add action.
   */
  public function action_add( &$handler )
  {
    $page = &$this->getPage();
    
    $vars = atkGetPostVar();
    $this->setHours( $vars );
    
    if( (count( $this->m_hours ) > 0) && ($this->validate_timereg()) )
    {
      $this->processTimereg();
      $redirect = dispatch_url( 'timereg_advanced.timereg_advanced', 'times' );
      $this->redirect( $redirect, true );
    }
    
    $content = $this->renderRecordlist( $handler, 'add' );
    $this->registerStylesAndScripts( $page );
    
    $output = &$this->getUi()->renderBox( array( 
        'title' => atktext( 'advanced_time_registration' ), 
        'content' => $content ) );
    
    $page->addContent( $this->renderActionPage( 'add', $output ) );
  }


  /*
   * Custom action to let users only register hours on their favorite tasks.
   */
  public function action_favorites( &$handler )
  {
    $page = &$this->getPage();
    
    $vars = atkGetPostVar();
    $this->setHours( $vars );
    if( (count( $this->m_hours ) > 0) && ($this->validate_timereg()) )
    {
      $this->processTimereg();
      $redirect = dispatch_url( 'timereg_advanced.timereg_advanced', 'times' );
      $this->redirect( $redirect, true );
    }
    
    $content = $this->renderRecordlist( $handler, 'favorites' );
    $this->registerStylesAndScripts( $page );
    
    $output = &$this->getUi()->renderBox( array( 
        'title' => atktext( 'advanced_time_registration' ), 
        'content' => $content ) );
    
    $page->addContent( $this->renderActionPage( 'add', $output ) );
  }


  /**
   * Fill the smarty vars.
   *
   * @param string $action
   */
  protected function renderRecordlist( &$handler, $action = 'add' )
  {
    $favorite = ($action == 'favorites') ? 1 : 0;
    $registered = ($action == 'times') ? true : false;
    
    $smarty = &atkinstance( 'atk.ui.atksmarty' );
    
    $smarty->assign( 'rows', $this->getActivePhases( $action, $registered ) );
    $smarty->assign( 'times', $this->getRegistrationTimes() );
    $smarty->assign( 'remaining', $this->getRemainingTimes() );
    $smarty->assign( 'action', $action );
    $smarty->assign( 'sorturl', $this->getSortUrl( $action ) );
    
    $smarty->assign( 'searchformstart', $this->getFormStart( $handler, 'searchform' ) );
    $smarty->assign( 'datefilter', $this->getDateFilter() );
    
    $smarty->assign( 'entryformstart', $this->getFormStart( $handler ) );
    $smarty->assign( 'formend', $this->getFormEnd() );
    $smarty->assign( 'buttons', $this->getFormButtons( 'add', '' ) );
    $smarty->assign( 'taburl', $this->getTimeregAdvancedTabUrls() );
    
    $smarty->assign( 'project_url', dispatch_url( 'project.project', 'edit' ) );
    $smarty->assign( 'phase_url', dispatch_url( 'project.phase', 'edit' ) );
    
    $smarty->assign( 'activetab', $action );
    $smarty->assign( 'values', atkGetPostVar() );
    $smarty->assign( 'errors', $this->m_errors );
    
    $theme = &atkTheme::getInstance();
    $smarty->assign( 'addIcon', $theme->iconPath( 'select', 'recordlist' ) );
    $smarty->assign( 'removeIcon', $theme->iconPath( 'delete', 'recordlist' ) );
    
    return $smarty->fetch( moduleDir( 'timereg_advanced' ) . 'templates/timereg.tpl' );
  }


  /**
   * Register styles and javascript for the action.
   *
   * @param atkPage $page
   */
  protected function registerStylesAndScripts( &$page )
  {
    $theme = &atkTheme::getInstance();
    $page->register_style( $theme->stylePath( "style.css" ) );
    $page->register_style( $theme->stylePath( "tabs.css" ) );
    $page->register_style( $theme->stylePath( "recordlist.css" ) );
    $page->register_script( "./atk/javascript/recordlist.js" );
    $page->register_script( moduleDir( 'timereg_advanced' ) . "javascript/timereg_advanced.js" );
  }


  /**
   * The add mode only needs a save button.
   *
   * @param string $mode
   * @param array $record
   * @return array $buttons
   */
  public function getFormButtons( $mode, $record )
  {
    $buttons = array();
    $controller = &atkcontroller::getInstance();
    $controller->setNode( $this );
    
    if( $mode == 'add' )
      $buttons[] = $controller->getButton( 'save' );
    else
      $button = $controller->getFormButtons( $mode, $record );
    
    return $buttons;
  }


  /**
   * Times action.
   *
   */
  public function action_times()
  {
    $page = &$this->getPage();
    $ui = &$this->getUi();
    $smarty = &atkinstance( 'atk.ui.atksmarty' );
    $theme = &atkTheme::getInstance();
    
    $content = $this->renderRegisteredHours();
    $backurl = dispatch_url( 'timereg_advanced.timereg_advanced', 'add' );
    $content .= '<br/><br/><input type="button" name="back" value="&laquo; ' . atktext( 'back' ) . '" onclick="location.href=\'' . $backurl . '\'" />';
    
    $this->registerStylesAndScripts( $page );
    
    $output = $ui->renderBox( array( 
        "title" => atktext( 'advanced_time_registration' ), 
        "content" => $content ) );
    
    $page->addContent( $this->renderActionPage( 'times', $output ) );
  }


  /**
   * Set favorite value for phase
   *
   */
  public function action_addfavorite()
  {
    $id = atkGetPostVar( 'phaseid' );
    $this->updatePhase( $id, 1 );
    echo '<html></html>';
    die();
  }


  /**
   * Unset favorite value for phase
   *
   */
  public function action_removefavorite()
  {
    $id = atkGetPostVar( 'phaseid' );
    $this->updatePhase( $id, 0 );
    echo '<html></html>';
    die();
  }


  /**
   * Fill the smarty vars for times.
   *
   * @return string
   */
  protected function renderRegisteredHours()
  {
    $smarty = &atkinstance( 'atk.ui.atksmarty' );
    $smarty->assign( 'rows', $this->getRegisteredHours() );
    $smarty->assign( 'project_url', dispatch_url( 'project.project', 'edit' ) );
    $smarty->assign( 'phase_url', dispatch_url( 'project.phase', 'edit' ) );
    $smarty->assign( 'taburl', $this->getTimeregAdvancedTabUrls() );
    $smarty->assign( 'activetab', 'times' );
    return $smarty->fetch( moduleDir( 'timereg_advanced' ) . 'templates/timereg_registered.tpl' );
  }


  /**
   * Select the active phases for the loggedin user.
   *
   * @return Array
   */
  protected function getActivePhases( $action = 'add', $registered = false )
  {
    $node = &atkGetNode( 'project.phase' );
    $filter = $this->createActivePhasesFilter( $node, $action );
    $orderby = $this->createActivePhasesOrder();
    atk_var_dump( $orderby, 'ORDER BY' );
    $rows = $node->select( $filter )->orderby( $orderby )->allRows();
    
    for( $i = 0, $_i = count( $rows ); $i < $_i; $i++ )
    {
      $rows[$i]['activities'] = $this->getActivities( $rows[$i]['id'] );
      $rows[$i]['add_url'] = partial_url( $this->atkNodeType(), 'addfavorite', 'addfavorite', array( 
          'phaseid' => $rows[$i]['id'] ) );
      $rows[$i]['delete_url'] = partial_url( $this->atkNodeType(), 'removefavorite', 'removefavorite', array( 
          'phaseid' => $rows[$i]['id'] ) );
      $rows[$i]['comment_obligatory'] = $this->getObligatoryComments( $rows[$i]['projectid']['id'] );
    }
    
    return $rows;
  }


  /**
   * Generate filter for active phases.
   *
   * @param atkNode $node
   * @param string $action
   * @return string $filter
   */
  protected function createActivePhasesFilter( $node, $action )
  {
    $postvars = atkGetPostVar();
    
    if( array_key_exists( 'searchallprojects', $postvars ) && $postvars['searchallprojects'] != '' )
    {
      $filter = ("1=1");
    }
    else
    {
      $filter = $node->getTable() . ".assign_to=" . atkGetUserId();
    }
    
    if( $action == 'favorites' )
    {
      $filter .= " AND " . $node->getTable() . ".is_favorite=1";
      return $filter;
    }
    
    $db = $this->getDb();
    
    if( array_key_exists( 'projectphasename', $postvars ) && $postvars['projectphasename'] != '' )
    {
      $projectPhaseName = strtolower( $db->escapeSQL( $postvars['projectphasename'] ) );
      $filter .= " AND (LOWER(projectid.name) LIKE '%" . $projectPhaseName . "%' OR LOWER(phase.name) LIKE '%" . $projectPhaseName . "%')";
    }
    
    if( $this->hasValidFilterDate( $postvars ) )
    {
      $searchDate = $db->escapeSQL( $postvars['filterdate']['year'] . '-' . $postvars['filterdate']['month'] . '-' . $postvars['filterdate']['day'] );
      $filter .= " AND (" . $node->getTable() . ".startdate < '" . $searchDate . "' AND (" . $node->getTable() . ".enddate > '" . $searchDate . "'))";
    }
    else
    {
      $filter .= " AND " . $node->getTable() . ".startdate < NOW()";
      $filter .= " AND " . $node->getTable() . ".enddate > NOW()";
    }
    
    return $filter;
  }


  /**
   * Validate filterdate.
   *
   * @param array $postvars
   * @return boolean
   */
  protected function hasValidFilterDate( $postvars )
  {
    
    if( ! array_key_exists( 'filterdate', $postvars ) )
      return false;
    if( $postvars['filterdate']['day'] == 0 )
      return false;
    if( $postvars['filterdate']['month'] == 0 )
      return false;
    if( trim( $postvars['filterdate']['year'] ) == '' )
      return false;
    
    return true;
  }


  /**
   * Generate orderby for active phases.
   *
   * @return string $filter
   */
  protected function createActivePhasesOrder()
  {
    $postvars = atkGetPostVar();
    
    if( ! array_key_exists( 'filterdate', $postvars ) )
      return '';
    
    if( trim( $postvars['orderby'] == '' ) )
      return '';
    else
    {
      switch($postvars['orderby'])
      {
        case 'project':
          $orderby = 'projectid.name ASC';
          break;
        case 'phase':
          $orderby = 'phase.name ASC';
          break;
        case 'enddate':
          $orderby = 'phase.enddate ASC';
          break;
        default:
          $orderby = '';
          break;
      }
      return $orderby;
    }
    return $this->getDb()->escapeSQL( $postvars['orderBy'] );
  }


  /**
   * Get the activities by phase.
   *
   * @param integer $phase Phase Id
   * @return array
   */
  protected function getActivities( $phase )
  {
    $node = atkGetNode( 'project.phase_activity' );
    return $node->select( 'phaseid=' . $phase )->excludes( 'phaseid' )->allRows();
  }


  /**
   * Get the description_obligatory value.
   *
   * @param integer $projectid
   * @return integer (0 or 1)
   */
  protected function getObligatoryComments( $projectid )
  {
    if( array_key_exists( $projectid, $this->m_obligatoryProjects ) )
      return $this->m_obligatoryProjects[$projectid];
    else
    {
      $obligatory = atkGetNode( "project.project" )->select( "project.id=" . $projectid )->firstRow();
      $this->m_obligatoryProjects[$projectid] = $obligatory['description_obligatory'];
      return $obligatory['description_obligatory'];
    }
  }


  /**
   * Get a list with hours and minutes for time registration.
   *
   * @return Array List with hours and minutes
   */
  protected function getRegistrationTimes()
  {
    return array( 
        'hours' => range( 0, atkconfig( "max_bookable", 10 ), 1 ), 
        'minutes' => range( 0, 59, 15 ) );
  }
  
  /**
   * Get a list with hours and minutes for remaining effort time
   *
   * @return Array List with hours and minutes
   */
  protected function getRemainingTimes()
  {
    return array( 
        'hours' => range( 0, 100, 1 ), 
        'minutes' => range( 0, 59, 15 ) );
  }


  /**
   * Generate sort Url.
   *
   * @param string $action
   * @return string
   */
  protected function getSortUrl( $action )
  {
    $postvars = atkGetPostVar();
    unset( $postvars['orderby'] );
    return dispatch_url( 'timereg_advanced.timereg_advanced', $action, $postvars );
  }


  /**
   * Get the start of the form.
   *
   * @return String HTML The forms' start
   */
  protected function getFormStart( &$handler, $formname = 'entryform' )
  {
    $controller = &atkcontroller::getInstance();
    $controller->setNode( $this->m_node );
    
    $node = &$this->m_node;
    
    $method = ($formname == 'searchform') ? 'get' : 'post';
    $formstart = '<form id="entryform" name="' . $formname . '" enctype="multipart/form-data" action="' . $controller->getPhpFile() . '?' . SID . '"' . ' method="' . $method . '" onsubmit="return globalSubmit(this)">';
    
    $formstart .= session_form( SESSION_DEFAULT, $handler->getReturnBehaviour(), parent::getEditFieldPrefix() );
    $formstart .= $controller->getHiddenVarsString();
    
    if( isset( $node->m_postvars['atkfilter'] ) )
    {
      $formstart .= '<input type="hidden" name="atkfilter" value="' . $node->m_postvars['atkfilter'] . '">';
    }
    
    return $formstart;
  }


  /**
   * Validate the registered hours.
   *
   * @return boolean
   */
  protected function validate_timereg()
  {
    $i = 0;
    foreach( $this->m_hours as $hour )
    {
      $j = 0;
      
      if( $this->getObligatoryComments( $hour['projectid'] ) == 1 && trim( $hour['remark'] ) == '' )
      {
        if( $j == 0 )
        {
          $this->m_errors[$i] = array();
          $this->m_errors[$i]['name'] = $hour['projectname'] . ' - ' . $hour['name'];
          $this->m_errors[$i]['err'] = array();
        }
        $this->m_errors[$i]['err'][$j++] = atktext( 'comment' ) . ': ' . atktext( 'error_obligatoryfield' );
        $i++;
      }
    
    }
    
    if( count( $this->m_errors ) > 0 )
      return false;
    return true;
  }


  /**
   * Get the end of the form.
   *
   * @return String HTML The forms' end
   */
  protected function getFormEnd()
  {
    return '</form>';
  }


  /**
   * Store the enabled time registrations in a private member.
   *
   * @param array $vars
   */
  protected function setHours( $vars )
  {
    $nHours = count( $vars['timereg'] );
    if( $nHours > 0 )
    {
      $timereg = $vars['timereg'];
      foreach( $timereg as $item )
      {
        if( $item['time']['hours'] > 0 || $item['time']['minutes'] > 0 )
          $this->m_hours[] = $item;
      }
    }
  }


  /**
   * Save the time registrations to the database.
   */
  protected function processTimereg()
  {
    $node = atkGetNode( 'timereg.hours' );
    $db = $this->getDb();
    $userid = atkGetUserId();
    
    foreach( $this->m_hours as $hour )
    {
      $item = array();
      $item['activitydate'] = array( 
          'day' => date( 'd' ), 
          'month' => date( 'm' ), 
          'year' => date( 'Y' ) );
      $item['userid'] = $userid;
      $item['phaseid'] = $hour['phaseid'];
      $item['activityid'] = $hour['activity'];
      $item['remaining_effort_time'] = ($hour['remaining']['hours'] * 60) + $hour['remaining']['minutes'];
      $item['remark'] = $db->escapeSQL( $hour['remark'] );
      $item['time'] = ($hour['time']['hours'] * 60) + $hour['time']['minutes'];
      $item['workperiod'] = 0;
      $item['entrydate'] = array( 
          'day' => date( 'd' ), 
          'month' => date( 'm' ), 
          'year' => date( 'Y' ) );
      
      $node->addDb( $item );
      
      $favorite = ($hour['is_favorite'] == 'on') ? 1 : 0;
      $this->updatePhase( $hour['phaseid'], $favorite );
      
      $ptu = atkNew('module.project.utils.projecttreeutils');
      $ptu->updateRemainTime( $item['phaseid'], $item['remaining_effort_time'] );
      $ptu->updateTrafficLight( $item['phaseid'] );
      
    }
  }


  /**
   * Update phaseid with new is_favorite value
   *
   * @param integer $phaseid
   * @param integer $favorite
   */
  protected function updatePhase( $phaseid, $favorite )
  {
    $db = $this->getDb();
    
    $update = "UPDATE phase SET is_favorite=" . $favorite . " WHERE id=" . $db->escapeSQL( $phaseid );
    
    $db->query( $update );
    $db->commit();
  }


  /**
   * Get the urls for the advanced time registration tabs
   *
   * @return array
   */
  protected function getTimeregAdvancedTabUrls()
  {
    $url = array( 
        'add' => dispatch_url( 'timereg_advanced.timereg_advanced', 'add' ), 
        'favorites' => dispatch_url( 'timereg_advanced.timereg_advanced', 'favorites' ), 
        'times' => dispatch_url( 'timereg_advanced.timereg_advanced', 'times' ) );
    return $url;
  }


  /**
   * Get the registered hours
   *
   * @return array $hours
   */
  protected function getRegisteredHours()
  {
    $hoursNode = atkGetNode( 'timereg.hours' );
    $hours = $hoursNode->select( "hours.activitydate='" . date( 'Y-m-d' ) . "' AND hours.userid=" . atkGetUserId() )->allRows();
    
    for( $i = 0, $_i = count( $hours ); $i < $_i; $i++ )
    {
      $hours[$i]['time_full'] = $this->getFormattedTime( $hours[$i]['time'] );
    }
    
    return $hours;
  }


  /**
   * Get formatted time f
   *
   * @param unknown_type $time
   * @return unknown
   */
  protected function getFormattedTime( $time )
  {
    useattrib( 'atkdurationattribute' );
    return atkDurationAttribute::_minutes2string( $time );
  }


  /**
   * Load DateAttribute to show the date input fields
   *
   * @return string
   */
  protected function getDateFilter()
  {
    $datefilter = &new atkDateAttribute( 'filterdate' );
    return $datefilter->edit();
  }

}