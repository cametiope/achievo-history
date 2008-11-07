<?php

  atkimport("atk.atkmetanode");

  class memo extends atkMetaNode
  {
    public $securityAlias = "project.project";
    
    
    /**
     * Meta.
     *
     * @param atkMetaPolicy $policy
     */
    public static function meta(atkMetaPolicy $policy)
    {      
      $policy->hasOne("project.project");
      $policy->addFlags('master', 'master_id', 'date_added', 'user', 'location', 'project_id', AF_HIDE_ADD|AF_HIDE_EDIT|AF_FORCE_LOAD);
    }
    
    
    public function postMeta()
    {
      //$attr = $this->getAttribute('personid');
      //$attr->addListColumns('phone', 'email');
    }   

    /**
     * We need $record to set additional info; we don't have this available
     * in initial_values() and therefore we add the master and master_id to the record here
     *
     * @param array $record
     * @param string $mode
     */
    public function preAddToEditArray( &$record, $mode )
    {
      $record = array_merge( $record, $this->getMasterNode( $record, $mode ) );
    }
    
    
    /**
     * Unset the recordactions; since we will add these manually
     *
     * @param array $record
     * @param array $actions
     */
    public function recordActions( $record, &$actions )
    {
      unset( $actions['view'], $actions['edit'], $actions['delete'] );
    }
    
    
    /**
     * Preset initial formvalues
     *
     * @return array
     */
    public function initial_values( )
    {
      $user     = atkGetUser();
      $username = array_key_exists( 'firstname', $user ) && array_key_exists( 'lastname', $user )  
                    ? $user['firstname'] . ' ' . $user['lastname'] 
                    : 'administrator';                   
                    
      return array(
        "date_added" => array("year"=>date("Y"),
                              "month"=>date("m"),
                              "day"=>date("d"),
                              "hours" => date('H'),
                              "minutes" => date('i')),
        "user"       => $username,
        "location"   => 'location' );
        
    }
        
    
    /**
     * Private method to determine from where a memo was added
     *
     * @param array $record
     * @param string $mode
     * @return arrayy
     */
    private function getMasterNode( $record, $mode )
    {
      if( array_key_exists( 'project_id', $record ) )
      {
        return array( 'master'    => 'project.project',
                      'master_id' =>  $record['project_id']['id']
        );
      }
      return array();
    }
    
  }