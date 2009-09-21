<?php

  /**
   * Module organization
   * Class contact_projectview
   *
   * Overrides some methods to be able to show a different listview of the 
   * project_personemployee node
   */

  atkimport('module.organization.contact');

  class contact_projectview extends contact
  {
    
  
    /**
     * Constructor
     *
     */  
    public function __construct()
    {      
      parent::__construct('contact_projectview', NF_NO_SEARCH);
      
      $this->remove('vcard');
      
      // Reset email attribute to assign new flags
      $this->remove('email');
      $this->add(new atkEmailAttribute("email", false, AF_HIDE_LIST|AF_FORCE_LOAD, 50));   

      // Reset function attribute to assign new flags     
      $this->remove('function');
      $this->add(new atkAttribute("function", 0, 50));
          
      $this->add(new atkAttribute('function'));
      $this->setIndex( null );
      $this->setSecurityAlias('organization.contact');
    
      // Make sure the email icon is viewable for everyone
      $this->addSecurityMap('email', 'view');
    }
    
    
    /**
     * Override recordactions to remove the view and edit icons
     *
     * @param array $record
     * @param array $actions
     * @param array $mraactions
     */
    public function recordActions($record, &$actions, &$mraactions) 
    {
      $delete = $actions['delete']; // temp store delete action to change the order of recordlist icons
      $actions = array();
            
      if( '' != $record['email'] )
      {
        $actions['email'] = 'javascript:document.location.href=\'mailto:'.$record['email'].'\';';
      }
      $actions['delete'] = $delete;
    }    
    
    
    /**
     * Workaround because the key in atkfilter is incorrect
     *
     * @param array $record
     * @return boolean
     */
    public function preAdd( &$record )
    {
      global $ATK_VARS;
      $record['company'] = array_shift( decodeKeyValueSet( $ATK_VARS['atkfilter'] ) );
      
      return true; 
    }
  }