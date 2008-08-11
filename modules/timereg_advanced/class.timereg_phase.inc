<?php
  useattrib('project.projectmanytoonerelation');
  useattrib('atkdateattribute');
  useattrib('atktextattribute');
  useattrib('atkdurationattribute');
  useattrib('atknumberattribute');
  useattrib('atkdummyattribute');
  //useattrib('atklistattribute');

  atkimport('module.project.utils.projectutils');

  class Timereg_Phase extends atkNode
  {

    private $remark_obligatory;
    private $use_activities;

    /**
     * Node constructor
     */
    public function __construct( )
    {
      $this->atkNode( 'timereg_phase', NF_ADD_DIALOG );

      // Fetch needed info from project node
      $this->getProjectDetails( );

      $this->add(new atkNumberAttribute( 'id', AF_AUTOKEY ) );
      $this->add(new atkDateAttribute( 'activitydate', '', '', 0, ( atkconfig('timereg_allowfuture', false )? 0 : date('Ymd') ), AF_OBLIGATORY|AF_FORCE_LOAD ), null, 10 );
      $this->add(new atkDateAttribute( 'entrydate', '', '', 0, 0, AF_HIDE|AF_OBLIGATORY ) );
      $this->add(new atkDurationAttribute( 'time', atkconfig( 'timereg_resolution', '15m' ), atkconfig( 'max_bookable',10 ).'h',AF_OBLIGATORY ) );

      $this->add(new atkNumberAttribute( 'userid', AF_HIDE ) );
      $this->add(new atkDummyAttribute( 'completed', $this->renderSelectList( $this->getCompletedValues() ), AF_HIDE_LIST|AF_DUMMY_SHOW_LABEL ) );
      $this->add( new atkTextAttribute( 'remark', atkconfig( 'timereg_remark_lines', 1 ), ( true === $this->remark_obligatory ? AF_OBLIGATORY : 0 ) ) );

      $phase = &new atkNumberAttribute( 'phaseid', AF_OBLIGATORY|AF_FORCE_LOAD );

      // Only add activities if the project requires these to be specified
      if( true === $this->use_activities )
      {
        $oneActivity = (projectutils::getActivityCount() == 1);
        if(!$oneActivity)
        {
          $phase->addDependee('activityid');
          $this->add($phase,null, 30);
        }
        if($oneActivity)
        {
          $activity_flag = AF_HIDE|AF_FORCE_LOAD;
        }
        else
        {
          $activity_flag = AF_LIST_OBLIGATORY_NULL_ITEM;
        }
        $activity = &new atkManyToOneRelation('activityid','project.activity',AF_OBLIGATORY|$activity_flag);

        $this->add($activity);
      }

      $this->setSecurityAlias('project.phase');

      $this->setTable('hours','hours');
    }


    /**
     * Set the initial attribute values
     *
     * @return array
     */
    public function initial_values( )
    {
      $user = atkGetUser();

      return
        array(
          'userid'       => $user['id'],
          'activitydate' => $this->getArrayToday(),
          'entrydate'    => $this->getArrayToday(),
        );
    }


    /**
     * Get date formatted intial value for the current day
     *
     * @return array
     */
    private function getArrayToday( )
    {
      return
        array(
          'year'  => date('Y'),
          'month' => date('m'),
          'day'   => date('d'),
      );
    }


    /**
     * Triggers before update
     *
     * @param array $record
     * @return boolean
     */
    public function preUpdate( &$record, $mode )
    {
      return $this->saveCompleted( $record );
    }


    /**
     * Triggers before insert
     *
     * @param array $record
     * @return boolean
     */
    public function preAdd( &$record )
    {
      return $this->saveCompleted( $record );
    }


    /**
     * Store progress in phase instead of the hours node
     *
     * @param array $record
     * @return boolean
     */
    private function saveCompleted( &$record )
    {
      $postvars = atkGetPostVar();

      if( array_key_exists( 'phaseid' , $record ) && array_key_exists( 'completed_select', $postvars ) )
      {
        $db = &atkGetDb();
        return $db->query( 'UPDATE phase SET completed = '.(int) $postvars['completed_select'].' WHERE id = '.(int) $record['phaseid'] );
      }
      return false;
    }


    /**
     * Get the values and options for the completed dropdown
     *
     * @return array with values and options
     */
    private function getCompletedValues()
    {
      $values = range(0,100, atkConfig::get('project','completed_parts'));
      for ($i=0, $_i=count($values); $i<$_i; $i++)
      {
        $completed['values'][$i] = $values[$i];
        $completed['options'][$i] = $values[$i].'%';
      }
      return $completed;
    }


    /**
     * Render a custom selectlist
     *
     * @todo preset selected option on validation error
     * @return string
     */
    private function renderSelectList( $optionlist )
    {
      $html = '
        <select name="completed_select">';

      for( $i = 0, $_i = count( $optionlist['options'] ); $i < $_i; $i++ )
      {
        $html .= '<option value="'.$optionlist['values'][$i].'">'.$optionlist['options'][$i].'</option>';
      }

      $html .= '</select>';

      return $html;
    }

    /**
     * Set additional details from the project node
     *
     * @todo get the projectid from URL, instead of using this dummy
     * @return null
     */
    private function getProjectDetails( )
    {
      global $ATK_VARS;

  //    $ATK_VARS['projectid'] = 'projectid=\'1\'';
      $projectid = decodeKeyValueSet( $ATK_VARS['projectid'] );

      $node    = atkGetNode('project.project');
      $project = $node->selectDb( 'project.id =' . (int) $projectid );

      $this->remark_obligatory = ( $project[0]['description_obligatory'] == 1 );
      $this->use_activities    = ( $project[0]['use_activities'] == 1 );

      return null;
    }

  }