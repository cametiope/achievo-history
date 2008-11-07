<?php
  atkimport('atk.atkdatanode'); 

  class pim_Allocation extends atkDataNode
  {
    
    /**
  	 * Constructor.
  	 *
  	 * @param String $name
  	 * @param Integer $flags
  	 */
    public function __construct($name="pim_allocation", $flags ='')
    {
      parent::__construct( $name, NF_NO_ADD|NF_NO_EDIT|NF_NO_DELETE ); 

      $this->add(new atkAttribute('id', AF_PRIMARY|AF_HIDE));
      $this->add(new atkAttribute('person_name'));
      $this->add(new atkAttribute('allocator_name'));
      $this->add(new atkAttribute('allocated_date'));
      $this->add(new atkAttribute('abbreviation'));
      $this->add(new atkAttribute('name'));

      $this->setSecurityAlias('project.project');
      $this->setOrder('allocated_date');  
    }
    
    public function selectDb()
    {
      $db   = atkGetDb();
      $days = atkConfig::get( 'project', 'default_days_allocation_pim' );
        
      $user = atkGetUser();
      $userId = array_key_exists( 'id', $user ) ? $user['id'] : 0;
      
      $query = '
        SELECT 
          person.id, CONCAT(person.firstname, " ", person.lastname) AS person_name,
          CONCAT(person_allocator.firstname, " ", person_allocator.lastname) AS allocator_name,
          DATE_FORMAT(project_person.allocated_date, "%d-%m-%Y") AS allocated_date, project_person.allocated_by,
          project.abbreviation, project.name          
        FROM 
          project_person, person person, person person_allocator, project
        WHERE
          project_person.personid = person.id
        AND 
          project_person.allocated_by = person_allocator.id
        AND
          project_person.projectid = project.id
        AND
          project_person.allocated_by <> '.$userId.'
        AND 
          DATE_SUB(CURDATE(),INTERVAL '.$days.' DAY) <= project_person.allocated_date
        AND 
          project_person.personid IN (
            SELECT id FROM person WHERE supervisor = '.$userId.'
        )';
  
      atk_var_dump($query);
      return $db->getRows( $query );      
    }
  }    