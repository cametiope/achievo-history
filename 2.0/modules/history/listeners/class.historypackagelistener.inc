<?php
  atkimport('atk.utils.atkactionlistener');
    
  /**
   * Listener ensures we can track edits in the package node,
   * without actually having to edit this node
   */ 
  class historyPackageListener extends atkActionListener
  {  
      
    /**
     * Implement actionperformed method
     *
     * @param string $action
     * @param array $record
     * @return void
     */
    public function actionPerformed( $action, $record )
    {  
      $project = isset( $record['projectid']['id'] ) ? $record['projectid']['id'] : '';   
      switch( $action )
      {        
        case 'update':
        
          $changes = $this->determineChanges( $record );
          if( ! empty( $changes ) )
          {
            $this->storeChangesToHistory( $record['id'], $changes, $project );
          }          
        break;
      }      
    }
    
    
    /**
     * Determine package changes
     *
     * @param array $record
     * @return array
     */
    private function determineChanges( $record )
    {      
      $changes = array( );
      // Proceed if we have an atkorgrec, which will be available if the project node has the flag NF_TRACK_CHANGES
      if( array_key_exists( 'atkorgrec', $record ) )
      {        
        // Has the coordinator attribute been changed
        if( $record['coordinator_id']['id'] != $record['atkorgrec']['coordinator_id']['id'] )
        {
          $changes[] = atktext('changed_coordinator', 'history');
        }
         
        // Has the enddate attribute been changed         
        if( $record['enddate'] != $record['atkorgrec']['enddate'] )
        {
          $changes[] = atktext('changed_enddate', 'history');      
        } 

        // Has the trafficlight attribute been changed
        if( $record['trafficlight'] != $record['atkorgrec']['trafficlight'] )        
        {
          atkimport( 'module.history.utils.trafficlightutils' );
          $changes[] = atktext('changed_trafficlight', 'history') .' from '. trafficLightUtils::getTrafficLightColor( $record['atkorgrec']['trafficlight'] ).' to '. trafficLightUtils::getTrafficLightColor( $record['trafficlight'] );
        }
      }
      
      return $changes;      
    }
    
    
    /**
     * Store package changes
     *
     * @param integer $id
     * @param string $changes
     * @return void
     */
    private function storeChangesToHistory( $id, $changes, $project )
    {
      $user = atkGetUser( );
      $node = atkGetNode( 'history.history' );
      
      foreach( $changes AS $msg )
      {
        $record = array(
          'master'    => 'project.package',
          'master_id' => $id,         
          'person_id' => $user['id'],
          'timestamp' => array('year'    => date('Y'),
                               'month'   => date('m'),
                               'day'     => date('d'),
                               'hours'   => date('H'),
                               'minutes' => date('i'),
                               'seconds' => date('s'),
                         ),
          'action'    => $msg,
          'project'   => $project,
        );          
        
        $node->addDb( $record );
      }

    }
    
  }