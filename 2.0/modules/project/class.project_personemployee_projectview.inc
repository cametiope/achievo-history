<?php

  /**
   * Module Project
   * Class project_personemployee_projectview
   *
   * Overrides some methods to be able to show a different listview of the 
   * project_personemployee node
   */

  atkimport('module.project.project_personemployee');
  
  class project_personemployee_projectview extends project_personemployee
  {
    
    
    /**
     * Constructor
     *
     */
    public function __construct()
    {      
      parent::__construct( 'project_personemployee_projectview' );
             
      $this->remove('personid');
      
      $rel = $this->add(new atkManyToOneRelation("personid", "employee.employee", AF_OBLIGATORY|AF_PRIMARY|AF_LARGE));
      $rel->addListColumns("phone");
      $rel->createDestination();
      $rel->getDestination()
        ->getAttribute('email')
        ->addFlag(AF_FORCE_LOAD);  
        
      $this->setDescriptorTemplate('[personid.lastname], [personid.firstname]');
      $this->setAttributeOrder('personid', 'role');      
      
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
            
      if( '' != $record['personid']['email'] )
      {
        $actions['email'] = 'javascript:document.location.href=\'mailto:'.$record['personid']['email'].'\';';
      }
      $actions['delete'] = $delete;
    }
    
    
    /**
     * Override personid display
     *
     * @param array $record
     * @param string $mode
     * @return string
     */
    public function personid_display( $record, $mode="" )
    {
      return $this->getAttribute("personid")->display($record, $fieldprefix, $mode);
    }

  }