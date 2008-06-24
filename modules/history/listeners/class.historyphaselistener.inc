<?php
  atkimport('atk.utils.atkactionlistener');
    
  /**
   * Listener ensures we can track edits in the phase node,
   * without actually having to edit this node
   */ 
  class historyPhaseListener extends atkActionListener
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
      switch( $action )
      {
        case 'update':
          $changes = $this->determineChanges( $record );
          if( ! empty( $changes ) )
          {
            $this->storeChangesToHistory( $record['id'], $changes );
          }
        break;
      }
    }
    
    
    /**
     * Determine phase changes
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
        
        // Has the enddate attribute been changed         
        if( $record['enddate'] != $record['atkorgrec']['enddate'] )
        {
          $changes[] = atktext('changed_enddate', 'history');
        }        
      }
      
      return $changes;
    }
    
    
    /**
     * Store phase changes
     *
     * @param integer $id
     * @param string $changes
     * @return void
     */
    private function storeChangesToHistory( $id, $changes )
    {
      $user = atkGetUser( );
      $node = atkGetNode( 'history.history' );
      
      foreach( $changes AS $msg )
      {
        $record = array(
          'master'    => 'project.phase',
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
        );          
        
        $node->addDb( $record );
      }

    }
    
  }