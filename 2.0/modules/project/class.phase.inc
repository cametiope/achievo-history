<?php

include_once ("achievotools.inc");
useattrib( "project.phasetemplateattribute" );
useattrib( "atktextattribute" );
useattrib( "atklistattribute" );
useattrib( "atknumberattribute" );
useattrib( "atkRadioAttribute" );
useattrib( "atkdurationattribute" );
useattrib( "atkdateattribute" );
useattrib( "atkdummyattribute" );
useattrib( "atkfuzzysearchattribute" );
useattrib( "project.spendhoursattribute" );
useattrib( "project.dependencyattrib" );
useattrib( "project.packageprojectselector" );
useattrib( "project.trafficlightattribute" );
useattrib( "project.memoattribute" );
useattrib( "timereg.remainingefforttimeattribute" );

userelation( "project.onetomanymemorelation" );
userelation( "atkmanytoonerelation" );
userelation( "atkmanyboolrelation" );
userelation( "atkonetomanyrelation" );

/**
 * Override of default atkManyBoolRelation because isEmpty depends not only on
 * setting of phase_activity but also of template.
 */
class phaseManyBoolRelation extends atkManyBoolRelation
{


  function isEmpty( $record )
  {
    return (parent::isEmpty( $record ) && $record["template"]["id"] == "");
  }
}

class phase extends atkNode
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
  public function __construct( $name = "phase", $flags = 0 )
  {
    global $ATK_VARS;
    $securitymanager = &atkGetSecurityManager();
    $initialplanningallowed = isset( $securitymanager ) && $securitymanager->allowed( "project.project", "initialplanning" );

    $this->atkNode( $name, $flags | NF_AUTOSELECT | NF_EDITAFTERADD | NF_ADD_LINK | NF_TRACK_CHANGES ); // node() constructor is *not* called automatically!


    $this->setProjectId();

    $this->add( new atkNumberAttribute( "id", AF_AUTOKEY ) );

    // The next three attributes have to be the first three attributes in this node
    $this->add( new trafficLightAttribute( 'trafficlight', AF_HIDE_LIST ) );
    $radio = &$this->add( new atkRadioAttribute( 'trafficlight_updatestyle', array(
        'manual',
        'automatic' ), array(
        'M',
        'A' ), AF_FORCE_LOAD | AF_HIDE_LIST | AF_HIDE_ADD | AF_DISPLAY_VERT ) );
    $radio->addDependee( 'trafficlight_selection' );
    $this->add( new atkDummyAttribute( 'trafficlight_selection', '', AF_NO_LABEL | AF_HIDE_LIST | AF_HIDE_ADD ) );

    $memo = &$this->add( new oneToManyMemoRelation( 'memo', 'project.memo', 'project_id', AF_HIDE_LIST | AF_HIDE_ADD ) );

    if( $ATK_VARS['atkaction'] != 'add' && $ATK_VARS['atkprevaction'] != 'add' )
    {
      $this->add( new memoAttribute( 'memo_add', AF_HIDE_LIST | AF_HIDE_ADD | AF_HIDE_VIEW, 60 ) );
    }

    //$this->add(new atkDummyAttribute('trafficlight_memo_field', '', AF_NO_LABEL|AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add( new atkManyToOneRelation( "projectid", "project.project", AF_OBLIGATORY | AF_HIDE_LIST | AF_HIDE_EDIT | AF_FORCE_LOAD ) );

    $this->add( new atkAttribute( "name", AF_OBLIGATORY | AF_SEARCHABLE | AF_FORCE_LOAD, 50 ) );
    $this->add( new atkTextAttribute( "description", TEXT_LARGE, AF_HIDE_LIST ) );
    $this->add( new atkListAttribute( "status", array(
        "active",
        "nonactive" ), "", AF_OBLIGATORY | AF_HIDE_LIST ) );

    $this->add( new packageProjectSelector( 'project_package' ) );
    $this->add( new atkAttribute( "master", AF_HIDE_ADD | AF_HIDE_EDIT | AF_FORCE_LOAD ) );
    $this->add( new atkNumberAttribute( "master_id", AF_HIDE | AF_FORCE_LOAD ) );

    $this->add( new dependencyAttribute( "dependencies", AF_HIDE_ADD | AF_HIDE_LIST | AF_BLANKLABEL | AF_HIDE_VIEW ) );

    $this->add( new atkListAttribute( 'priority', range( 1, 10 ), '', AF_HIDE_LIST ) );

    $assign_to = &$this->add( new atkManyToOneRelation( 'assign_to', 'person.person', AF_LARGE | AF_HIDE_LIST ) );
    $assign_to->setDestinationFilter( ' person.id IN (SELECT personid FROM project_person WHERE projectid = [projectid.id])' );

    $this->add( new atkDateAttribute( "startdate", "", "" ) );
    $this->add( new atkDateAttribute( "enddate", "", "" ) );

    $this->add( new atkNumberAttribute( "completed", AF_HIDE_ADD | AF_FORCE_LOAD | AF_OBLIGATORY | AF_READONLY ) );

    $this->add( new atkDurationAttribute( "initial_planning", '1m', '10h', ($initialplanningallowed ? 0 : AF_READONLY_EDIT) | AF_TOTAL | AF_DURATION_STRING ) );
    $current = &$this->add( new atkDurationAttribute( "current_planning", "1m", "10h", AF_READONLY|AF_HIDE_ADD|AF_FORCE_LOAD | AF_DURATION_STRING ) );
    $current->setForceUpdate(true);
    
    $this->add( new remainingEffortTimeAttribute( "remaining_effort_time", "1m", "10h", AF_DURATION_STRING ) );

    // deliverable dependency
    $depends = &$this->add( new atkManyToOneRelation( "dependsondeliverable", "project.deliverable", AF_HIDE_LIST ) );
    $depends->setDestinationFilter( "project_id=[projectid.id]" );

    $this->add( new spendHoursAttribute( "spend_hours", "id", AF_TOTAL ) );

    //$this->add(new atkOneToManyRelation("phaseplanning", "project.phaseplanning", "phaseid", AF_HIDE_LIST));
    //$this->add(new atkFuzzySearchAttribute("phaseplanning_add", "employee.employee", "storePhaseplanning", "multiselect", AF_HIDE_ADD|AF_BLANK_LABEL, 20));


    $cascadingdeletehours = atkConfig::get( "project", "project_cascading_delete_hours", false );
    if( $cascadingdeletehours )
      $this->add( new atkOneToManyRelation( "hours", "timereg.hours", "phaseid", AF_HIDE | AF_HIDE_ADD | AF_CASCADE_DELETE ) );

    // list of linked deliverables for this phase
    $this->add( new atkOneToManyRelation( "deliverable_phase", "project.deliverable_phase", "phase_id", AF_NO_LABEL | AF_HIDE_LIST | AF_CASCADE_DELETE ) )->setDestinationFilter( 'deliverable_id.status <> "complete"' );

    $this->addSecurityMap( "view", "project.admin" );
    $this->setOrder( 'id', 'trafficlight', 'trafficlight_updatestyle', 'memo', 'trafficlight_selection', 'memo_add' );
    $this->setTable( "phase", "phase" );

    $this->setColumnAttribs( array(
        'deliverable_phase' ) );
        
    /* 
     * For Support tasks the remaining effort time will be calculated automatically using this formula:
     * remaining effort time = actual planning - booked time. 
     * For Support tasks the remaining effort time field is read-only."
     */
    $this->add(new atkBoolAttribute("is_support_task",AF_HIDE_LIST|AF_FORCE_LOAD));

    $this->setDescriptorTemplate( "[name]" );

    global $ATK_VARS;
    if( $this->atkNodeType() != $ATK_VARS['atknodetype'] )
    {
      $this->getAttribute( 'startdate' )->addFlag( AF_HIDE_LIST );
      $this->getAttribute( 'master' )->addFlag( AF_HIDE_LIST );
      $this->getAttribute( 'initial_planning' )->addFlag( AF_HIDE_LIST );
      $this->getAttribute( 'current_planning' )->addFlag( AF_HIDE_LIST );
      $this->addFlag( NF_NO_VIEW | NF_NO_EDIT | NF_NO_DELETE | NF_NO_SEARCH );
    }
  }


  /**
   * Add flag to memo_add attribute if trafficlight-updatestyle is "manual".
   *
   * @param array $record
   * @return boolean
   */
  public function preUpdate( &$record )
  {
    
    if( $record['trafficlight_updatestyle'] == 'M' )
    {
      $this->getAttribute( 'memo_add' )->addFlag( AF_OBLIGATORY );
    }

    return true;
  }


  /**
   * PostAdd override.
   *
   * Add memo to database if field is filled.
   * Recalculate planing information.
   *
   * @param array $record
   * @return boolean
   */
  public function postUpdate( $record )
  {
    if( array_key_exists( 'memo_add', $record ) && $record['memo_add'] != '' )
      $this->addMemo( $record );

    $this->recalculateInformation( $record );
    
    return true;
  }

  public function postAdd( $rec )
  {
    if($rec['initial_planning'])
    {
      //We add new phase with initial, current planning, and with remain efforts time - so we must update all upper accordingly
      $this->recalculateInformation( $rec );
    }
    return true;
  }

  /**
   * Add memo to database.
   *
   * @param array $record
   */
  protected function addMemo( $record )
  {
    $user = atkGetUser();
    $username = array_key_exists( 'firstname', $user ) && array_key_exists( 'lastname', $user ) ? $user['firstname'] . ' ' . $user['lastname'] : 'administrator';

    $memo = array();
    $memo['project_id'] = $record['projectid']['id'];
    $memo['master'] = 'project.phase';
    $memo['master_id'] = $record['id'];
    $memo['memo'] = $this->getDb()->escapeSQL( $record['memo_add'] );
    $memo['date_added'] = array(
        'year' => date( 'Y' ),
        'month' => date( 'm' ),
        'day' => date( 'd' ) );
    $memo['user'] = $username;
    $memo['location'] = 'Task: ' . $this->getDb()->escapeSQL( $record['name'] );
    $memo['updatestyle'] = 'm';

    $memoNode = atkGetNode( 'project.memo' );
    $memoNode->addDb( $memo );
  }


  /**
   * Recalculate planning information.
   *
   * @param array $record
   */
  protected function recalculateInformation( $record )
  {
    list($master,$master_id) = explode("s_",$record['project_package']);
    if($master) 
    {
      $master = "project.".$master;
      
      /*@var $information projecttreeutils*/
      if(isset($record['atkorgrec']) && ($master != $record['atkorgrec']['master'] || $master_id != $record['atkorgrec']['master_id']))
        $information = atkNew( 'module.project.utils.projecttreeutils', $record['atkorgrec']['master'], $record['atkorgrec']['master_id'], $master, $master_id );
      else
        $information = atkNew( 'module.project.utils.projecttreeutils', $record['master'], $record['master_id'] );
  
      $information->updateTree($record['projectid']['id']);
      $information->updateTrafficLight($record['id']);
    }
  }


  /**
   * Generate output to show or hide a selectlist for the trafficlight attribute
   *
   * @param array
   * @param string
   * @return string
   */
  public function trafficlight_selection_edit( &$record )
  {
    $output = '<div id="project_phase_trafficlight_selection">';
    if( isset( $record['trafficlight_updatestyle'] ) && $record['trafficlight_updatestyle'] == 'M' )
    {
      $red = atkConfig::get( 'project', 'trafficlight_red' );
      $yellow = atkConfig::get( 'project', 'trafficlight_yellow' );
      $green = atkConfig::get( 'project', 'trafficlight_green' );

      $output .= '<select name="trafficlight" ' . (! $record['trafficlight'] ? 'selected' : '') . '>';
      $output .= '<option value="">' . atktext( 'list_null_value_obligatory' ) . '</option>';
      $output .= '<option value="' . $red['end'] . '">' . atktext( 'red' ) . '</option>';
      $output .= '<option value="' . $yellow['start'] . '">' . atktext( 'yellow' ) . '</option>';
      $output .= '<option value="' . $green['start'] . '">' . atktext( 'green' ) . '</option>';

      $output .= '</select></div><br />';
    }
    return $output;
  }


  /**
   * Generate output to change the memo field attribute depending on the selected updatestyle
   *
   * @param array
   * @param string
   * @return string
   */
  public function trafficlight_memo_field_edit( &$record, $fieldprefix = '' )
  {
    $output = '<div id="project_phase_trafficlight_memo_field">';
    $this->memo_attr->setForceUpdate( true );

    if( isset( $record['trafficlight_updatestyle'] ) && $record['trafficlight_updatestyle'] == 'A' )
    {
      // Get memo attribute's content by executing a query
      // This value should probably come directly from $record, but somehow this wont work
      $db = &atkGetDb();
      $row = $db->getRows( 'SELECT trafficlight_memo FROM phase WHERE id = ' . ( int ) $record['id'] );
      $record['trafficlight_memo'] = $row[0]['trafficlight_memo'];

      $this->memo_attr->addFlag( AF_READONLY );
      $output .= '<textarea cols="70" rows="4" disabled="disabled" name="trafficlight_memo">';
      $output .= $record['trafficlight_memo'];
      $output .= '</textarea>';
    }
    else
    {
      $this->memo_attr->removeFlag( AF_READONLY );
      $output .= $this->memo_attr->getEdit( 'edit', $record, $fieldprefix );
    }

    $output .= '</div>';
    return $output;
  }


  public function name_display( $record, $mode )
  {
    // Do not return a html link in plain or csv mode or select screen
    if( ($mode == "plain") || ($mode == "csv") )
      return $record["name"];

    // Special select handling
    if( atkArrayNvl( $this->m_postvars, "atkaction" ) == "select" )
    {
      // Decode the given target url
      $url = atkArrayNvl( $this->m_postvars, "atktarget" );
      $url = atkurldecode( $url );
      $url = str_replace( "%5B", "[", $url );
      $url = str_replace( "%5D", "]", $url );
      $url = str_replace( "_15B", "[", $url );
      $url = str_replace( "_15D", "]", $url );

      // Parse it using the record
      atkimport( "atk.utils.atkstringparser" );
      $parser = &new atkStringParser( $url );
      $url = $parser->parse( $record, true );

      // Return a link
      return href( $url, $record["name"], SESSION_NESTED );
    }
    else
    {
      // Make all project links clickable
      $defaultaction = $this->allowed( "edit" ) ? "edit" : "view";
      $url = dispatch_url( "project.phase", $defaultaction, array(
          "atkselector" => "phase.id='{$record["id"]}'" ) );
      return href( $url, $record["name"], SESSION_NESTED );
    }
  }


  public function duration_display( $record, $mode )
  {
    if( is_array( $record["startdate"] ) && is_array( $record["enddate"] ) )
    {
      $startdate = mktime( 12, 0, 0, $record["startdate"]["month"], $record["startdate"]["day"], $record["startdate"]["year"] );
      $enddate = mktime( 12, 0, 0, $record["enddate"]["month"], $record["enddate"]["day"], $record["enddate"]["year"] );

      $seconds_in_a_day = 86400;
      $sunday_val = "0";
      $saturday_val = "6";
      $workday_counter = 0;

      $ptr_year = intval( date( "Y", $start_date ) );

      for( $day_val = $startdate; $day_val <= $enddate; $day_val += $seconds_in_a_day )
      {
        $pointer_day = date( "w", $day_val );
        if( ($pointer_day != $sunday_val) && ($pointer_day != $saturday_val) )
          $workday_counter++;

      }
      return $workday_counter . " " . ($workday_counter == 1 ? atktext( "day", "project" ) : atktext( "days", "project" ));
    }
    return "";
  }


  /**
   * Display the master - master_id column a bit more userfriendly
   *
   * @param array $record
   * @param string $mode
   */
  public function master_display( $record, $mode )
  {
    $node = getNode( $record['master'] );
    $node->addFilter( $node->getTable() . '.id', $record['master_id'] );
    $result = $node->selectDb();

    return ucfirst( substr( $record['master'], (strpos( $record['master'], '.' ) + 1) ) ) . ': ' . $result[0]['name'];
  }


  public function initial_values()
  {
    // Compose the default initual values array
    $initial_values = array(
        "status" => "active" );

    // If there is just one activity, it should be selected by default:
    $activitynode = &atkGetNode( "project.activity" );
    $count = $activitynode->countDb( "" );
    if( $count == 1 )
    {
      $activity = $activitynode->selectDb( "", "", "", "", array(
          "id" ) );
      $initial_values["phase_activity"] = array(
          array(
              "activityid" => $activity[0]["id"] ) );
    }
    $initial_values["trafficlight_updatestyle"] = "A";
    $initial_values['completed'] = 0;
    // Return the initual values:
    return $initial_values;
  }


  public function addDb( &$record, $exectrigger = true, $mode = "add" )
  {
    // This override makes sure that the current_planning and remaining_effort_time 
    // is set equal to the number of initial_planning.
    $record["current_planning"] = $record["initial_planning"];
    $record["remaining_effort_time"] = $record["initial_planning"];
    return parent::addDb( $record, $exectrigger, $mode );
  }


  /**
   * Enter description here...
   *
   * @param unknown_type $rec
   * @return unknown
   */
  public function postDel( $rec )
  {
    if( isset( $rec["id"] ) )
    {
      $node = &atkGetNode( "project.dependency" );
      $delphase = $rec["id"];
      $deprec = $node->deleteDb( "phaseid_row = $delphase OR phaseid_col = $delphase" );
    }
    $this->recalculateInformation( $rec );
    return true;
  }


  public function action_select( &$handler )
  {
    // In select mode, we place a filter, because we may only select active phases.
    $this->addFilter( "phase.status='active'" );

    // Also, we hide the status field (we're showing only active phases,
    // so this column is not necessary.
    $this->m_attribList["status"]->m_flags |= AF_HIDE;

    // call parent method.
    return $handler->action_select();
  }


  public function phaseplanning_add_edit( $record, $mode )
  {
    $org = $this->m_attribList["phaseplanning_add"]->edit( $record, $mode );
    useattrib( "atkDurationAttribute" );
    $hours = new atkDurationAttribute( "phaseplanning_initial", '', '', AF_DURATION_STRING );

    $dummy = array();
    return atktext( "plan_employee", "phase" ) . " " . $org . " " . atktext( "for" ) . " " . $hours->edit( $dummy ) . " " . atktext( "hours" );

  }


  /**
   * Call custom validation methods for the startdate field
   *
   * @param array $record
   * @param string $mode
   */
  public function startdate_validate( &$record, $mode )
  {
    if( $mode == 'update' )
    {
      $this->fixMasterFields($record);
      $this->validate_startdate_timeregistration( $record, $mode );
      $this->validate_startdate_before_parent( $record, $mode );
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
  public function enddate_validate( &$record, $mode )
  {
    if( $mode == 'update' )
    {
      $this->fixMasterFields($record);
      $this->validate_enddate_before_parent( $record, $mode );
      $this->validate_enddate_before_project( $record, $mode );
    }
  }


  /**
   * Custom validation: does the phase start on the same date (or later) then its project
   *
   * @param array $record
   * @param string $mode
   */
  private function validate_startdate_before_project( &$record, $mode )
  {
    if( ( int ) dateutil::arr2str( $record['projectid']['startdate'] ) > ( int ) dateutil::arr2str( $record['startdate'] ) )
    {
      triggerError( $record, 'startdate', atktext( 'phase_starts_before_project', 'project', 'phase' ) );
    }
  }


  /**
   * Custom validation: startdate should be before the first timeregistration
   *
   * @param array $record
   * @param string $mode
   */
  private function validate_startdate_timeregistration( &$record, $mode )
  {
    // Get the date of the first registration for this node (or set $earliesttimeregdate to null if not found)
    $hoursNode = &atkGetNode( "timereg.hours" );
    $earliesttimeregforphase = $hoursNode->selectDb( "phaseid='" . $record["id"] . "'", "hours.activitydate ASC", 1 );
    atkimport( "module.utils.dateutil" );
    $earliesttimeregdate = empty( $earliesttimeregforphase ) ? null : dateutil::arr2stamp( $earliesttimeregforphase["0"]["activitydate"] );

    // Get the start date of this phase record
    $phasestartdate = dateutil::arr2stamp( $record["startdate"] );

    // Trigger an error on the startdate attribute if the earliesttimeregdate is not null and the startdate of this record is higher than de first earliesttimeregdate
    if( (! is_null( $earliesttimeregdate )) && ($phasestartdate > $earliesttimeregdate) )
    {
      triggerError( $record, "startdate", "error_cannot_set_startdate_later_than_first_timereg" );
    }
  }

  /**
   * Custom validation: does the phase start on the same date (or later) then its master
   *
   * @param array $record
   * @param string $mode
   */
  private function validate_startdate_before_parent( &$record, $mode )
  {
    if( $record['master'] == 'project.package' && $record['master_id'] !== NULL )
    {
      $node = &atkGetNode( 'project.package' );
      $result = $node->selectDb( 'package.id = ' . ( int ) $record['master_id'] );

      if( ( int ) dateutil::arr2str( $result[0]['startdate'] ) > ( int ) dateutil::arr2str( $record['startdate'] ) )
      {
        triggerError( $record, 'startdate', atktext( 'phase_starts_before_parent_package', 'project', 'phase' ) );
      }
    }
  }


  /**
   * Custom validation: does the phase start before it ends
   *
   * @param array $record
   * @param string $mode
   */
  private function validate_startdate_before_enddate( &$record, $mode )
  {
    if( ( int ) dateutil::arr2str( $record['enddate'] ) < ( int ) dateutil::arr2str( $record['startdate'] ) )
    {
      triggerError( $record, 'startdate', atktext( 'enddate_is_before_startdate', 'project', 'phase' ) );
    }
  }


  /**
   * Custom validation: does the phase end on the same date (or sooner) then the its master
   *
   * @param array $record
   * @param string $mode
   */
  private function validate_enddate_before_parent( &$record, $mode )
  {
    if( $record['master'] == 'project.package' && $record['master_id'] !== NULL )
    {
      $node = &atkGetNode( 'project.package' );
      $result = $node->selectDb( 'package.id = ' . ( int ) $record['master_id'] );

      if( ( int ) dateutil::arr2str( $result[0]['enddate'] ) < ( int ) dateutil::arr2str( $record['enddate'] ) )
      {
        triggerError( $record, 'enddate', atktext( 'phase_ends_after_parent_package', 'project', 'phase' ) );
      }
    }
  }


  /**
   * Custom validation: does the phase end before its project ends
   *
   * @param array $record
   * @param string $mode
   */
  private function validate_enddate_before_project( &$record, $mode )
  {
    if( ( int ) dateutil::arr2str( $record['projectid']['enddate'] ) < ( int ) dateutil::arr2str( $record['enddate'] ) )
    {
      triggerError( $record, 'enddate', atktext( 'phase_end_after_project', 'project', 'phase' ) );
    }
  }


  public function storePhaseplanning( $rec, $people )
  {
    useattrib( "atkDurationAttribute" );
    $tmp_attrib = new atkDurationAttribute( "phaseplanning_initial" );
    $initial_planning = $tmp_attrib->_string2minutes( $this->m_postvars["phaseplanning_initial"] );

    if( $initial_planning > 0 )
    {
      for( $i = 0, $_i = count( $people ); $i < $_i; $i++ )
      {
        $this->_addPlanningRecord( $rec["id"], $people[$i]["id"], $initial_planning );
      }
    }
    return true;
  }


  private function _addPlanningRecord( $phase_id, $person_id, $initial_planning )
  {
    $db = &atkGetDb();

    // attendee may already exist.
    $recs = $db->getrows( "SELECT count(*) AS cnt FROM project_phaseplanning WHERE phaseid = $phase_id AND personid = $person_id" );
    if( ! count( $recs ) || $recs[0]["cnt"] == 0 )
    {
      $db->query( "INSERT INTO project_phaseplanning (phaseid, personid, current_planning, initial_planning)
                           VALUES ($phase_id, $person_id, $initial_planning, $initial_planning)" );
    }
    $this->_addTeamMemberRecord( $phase_id, $person_id );
  }


  /**
   * Check if the person is allready a teammember
   * if not, add the person
   *
   * @param int $phase_id
   * @param int $person_id
   */
  private function _addTeamMemberRecord( $phase_id, $person_id )
  {
    $db = &atkGetDb();
    // person may allready exist
    $recs = $db->getrows( "SELECT count(*) as cnt
                              FROM phase, project,project_person
                              WHERE phase.projectid = project.id
                                AND project.id = project_person.projectid
                                AND phase.id =$phase_id
                                AND project_person.personid = $person_id
                              GROUP BY project.id" );
    if( ! count( $recs ) || $recs[0]["cnt"] == 0 )
    {
      $recs = $db->getrows( "SELECT project.id FROM phase,project
                                WHERE phase.projectid = project.id
                                  AND phase.id = $phase_id" );
      $project_id = $recs[0]["id"];
      // Add person to members, role will be empty
      $db->query( "INSERT INTO project_person (projectid,personid)
                      values ($project_id,$person_id)" );
    }
  }


  public function rowColor( $record )
  {
    if( $record["current_planning"] > 0 )
    {
      if( $record["spend_hours"] > ($record["current_planning"] / 60) )
      {
        return COLOR_ERROR;
      }
      elseif( $record["spend_hours"] >= ($record["current_planning"] / 60) * 0.9 )
      {
        return COLOR_WARNING;
      }
    }
  }


  public function recordActions( $record, &$actions, &$mraactions )
  {
    //when we have a phase that belongs to a template, the records cannot be deleted in an admin screen.
    if( isset( $record["template"]["id"] ) && ($record["template"]["id"] != "") && $record["template"]["id"] != 0 )
    {
      unset( $actions["delete"] );
    }
  }


  public function preAdd( &$rec )
  {
    //if the phase is added, check if the name is filled in and the template is
    if( empty( $rec["name"] ) )
    {
      if( ! empty( $rec["template"]["id"] ) )
      {
        $node = &atkGetNode( "project.tpl_phase" );
        /* @var $node atkNode */
        $select = sprintf( $node->m_table . "." . $node->primaryKeyField() . "='%s'", $rec["template"]["id"] );
        list( $record ) = $node->selectDb( $select, "", "", "", array(
            "name" ) );
        if( $record != NULL )
          $rec["name"] = $record["name"];
      }
    }

    return true;
  }

  /**
   * We must reload menu after changes in package/task.
   *
   * @param Array $postvars
   * @param Bool $fullpage
   */
  public function dispatch( $postvars, $fullpage = true )
  {
    if( ! in_array( $postvars['atkaction'], array(
        'add',
        'admin' ) ) )
    {
      reloadMenu();
    }
    return parent::dispatch( $postvars, $fullpage );
  }


  /**
   * Last minute overrides
   *
   * @param Array $record
   * @param String $mode
   * @return Bool
   */
  public function preAddToEditArray( $record, $mode )
  {
    $initalplaning_attr = $this->getAttribute( 'initial_planning' );
    $currentplanning_attr = $this->getAttribute( 'current_planning' );
    $remainingefforttime_attr = $this->getAttribute( 'remaining_effort_time' );

    if( $mode == 'add' )
    {
      $initalplaning_attr->addFlag( AF_OBLIGATORY );
      $currentplanning_attr->addFlag( AF_HIDE_ADD | AF_FORCE_LOAD );
      $remainingefforttime_attr->addFlag( AF_HIDE_ADD | AF_FORCE_LOAD );
    }
    else
    {
      $initalplaning_attr->addFlag( AF_READONLY );
    }
    
    /*    
     * For Support tasks the remaining effort time field is read-only."
     */
    if ($record['completed'] == 100 || $record['is_support_task'])
    {
      $remainingefforttime_attr->addFlag( AF_READONLY );
      $currentplanning_attr->removeFlag( AF_READONLY_EDIT );
    }
    
    atk_var_dump($record['completed'], 'COMPLETED');

    if( $this->atkNodeType() != $this->m_postvars['atknodetype'] )
    {
      $this->getAttribute( 'startdate' )->addFlag( AF_HIDE_LIST );
      $this->getAttribute( 'master' )->addFlag( AF_HIDE_LIST );
      $this->getAttribute( 'master' )->addFlag( AF_HIDE_LIST );
      $initalplaning_attr->addFlag( AF_HIDE_LIST );
      $currentplanning_attr->addFlag( AF_HIDE_LIST );
    }
    return true;
  }


  /**
   * Template override for custom traffic light display
   *
   * @param String $action
   * @param array $record
   * @param String $tabi need a proposal fort he following small functionality

We want to show in the normal Time registration the high and holidays (for example Christmas) in calendar and Date Attribute. The days must be coloerd red in the JavaScript calendar (must have). The Date dropdown box shows the day red (must have) and is flowed by the comment (could have).

To add the dates we need a small node where we can define high days (Edit, Delete actions is also necessary). To define them we select a date and write a short comment like Christmas or something else.
   * @return String The filename of the template (without path)
   */
  public function getTemplate( $action, $record = NULL, $tab = "" )
  {
    switch($action)
    {
      case "add":
        return "editform.tpl";
      case "edit":
        return "editform_trafficlight.tpl";
      case "view":
        return "viewform_trafficlight.tpl";
    }
    return parent::getTemplate( $action, $record, $tab );
  }


  /**
   * Edit action handler override.
   *
   * Add an extra param to the template
   *
   * @param atkEditHandler $handler
   * @return string HTML
   */
  public function action_edit( &$handler )
  {
    $handler->addRenderBoxVar( 'projectmenu', $this->getProjectMenu() );
    return $handler->action_edit();
  }


  /**
   * AdminHeader override for projectmenu.
   */
  public function adminHeader()
  {
    $this->getPage()->register_style( atkTheme::getInstance()->stylePath( 'tabs.css' ) );
    return $this->getProjectMenu() . '<br/>';
  }


  /**
   * AddHeader override for projectmenu.
   */
  public function addHeader()
  {
    $this->getPage()->register_style( atkTheme::getInstance()->stylePath( 'tabs.css' ) );
    return $this->getProjectMenu() . '<br/>';
  }


  /**
   * AdminHeader override for projectmenu.
   */
  public function viewHeader()
  {
    return $this->getProjectMenu() . '<br/>';
  }


  /**
   * Get the project menu.
   *
   * @return string HTML
   */
  private function getProjectMenu()
  {
    atkimport( 'module.project.projectmenu' );
    $pm = new projectmenu( 'project.phase', $this->m_project );
    return $pm->getTabHtml();
  }


  /**
   * Set the project id.
   */
  private function setProjectId()
  {
    $vars = atkGetPostVar();

    if( $vars['atknodetype'] == 'project.phase' && array_key_exists( 'atkfilter', $vars ) && $vars['atkfilter'] != '' )
    {
      $this->project = $vars['atkfilter'];
      sessionStore( 'projectid', $this->project );
    }
    else
    {
      $this->project = sessionLoad( 'projectid' );
    }
  }
  
  public function fixMasterFields(&$record)
  {
    if(isset($record['project_package']))
    {
      list($master,$master_id) = explode("s_",$record['project_package']);
      $master = "project.".$master;
      if($master!=$record['master'] || $master_id!=$record['master_id'])
      {
        $record['master'] = $master;
        $record['master_id'] = $master_id;
      }
    }
  }
  /**
   * Display override for completed field
   *
   * @param array $record
   * @param string $mode
   * @return string
   */
  public function completed_display($record,$mode)
  {
    return $record['completed'].'%';
  }
  
  public function is_support_task($id)
  {
    $is = $this->getDb()->getrows("SELECT is_support_task FROM phase WHERE id='$id'");
    return $is[0]['is_support_task'] == '1';
  }
  
}