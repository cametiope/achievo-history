<?php
  useattrib('project.projectmanytoonerelation');
  useattrib('atkdateattribute');
  useattrib('atktextattribute');
  useattrib('atkdurationattribute');
  useattrib('atknumberattribute');
  useattrib('atkdummyattribute');
  useattrib('atkdurationattribute');

  userelation('atkmanytoonerelation');

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
      $this->add( new atkDurationAttribute( "remaining_effort_time", "1m", "10h", AF_DURATION_STRING|AF_OBLIGATORY) );

      $this->add(new atkNumberAttribute( 'userid', AF_HIDE ) );
      $this->add( new atkTextAttribute( 'remark', atkconfig( 'timereg_remark_lines', 1 ), ( true === $this->remark_obligatory ? AF_OBLIGATORY : 0 ) ) );

      $phase = &new atkNumberAttribute( 'phaseid', AF_OBLIGATORY|AF_FORCE_LOAD );
      
      // Only add activities if the project requires these to be specified
      if( true === $this->use_activities )
      {
        $oneActivity = (projectutils::getActivityCount() == 1);
        if(!$oneActivity)
        {
          $phase->addDependee('activityid');
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
      
      $this->add($phase,null, 30);

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

    function postAdd($rec)
    {
      $phaseid = atkArrayNvl($rec,"phaseid");
      if($phaseid)
      {
        list($row) = $this->getDb()->getrows("SELECT projectid, master, master_id FROM phase WHERE id='$phaseid'");
        $information = atkNew( 'module.project.utils.projecttreeutils', $row['master'], $row['master_id'] );

        //copy remaining_effort_time from hours to phase
        $information->updateRemainTime($phaseid, $rec['remaining_effort_time']);
        $information->updateTrafficLight($phaseid);
        //update spent time in project and package
        $information->updateTree($row['projectid']);
      }
      return true;
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

      $node    = atkGetNode('project.project');
      //during setup $ATK_VARS['projectid'] is not set, so I add check for prevent Critical: Unknown error: 1064 
      //(You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the 
      //right syntax to use near ') AND (project.status='active') ORDER BY id' at line 1).
      if($ATK_VARS['projectid'])
      {
        $project = $node->selectDb( 'project.id =' . array_shift( decodeKeyValueSet( $ATK_VARS['projectid'] ) ) );
  
        $this->remark_obligatory = ( $project[0]['description_obligatory'] == 1 );
        $this->use_activities    = ( $project[0]['use_activities'] == 1 );
      }

      return null;
    }

  }