<?php

  atkimport("atk.atkmetanode");

  class project_contact extends atkMetaNode
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
      $policy->hasOne("organization.contact_projectview",array("source"=>"personid"));
    }
    
    
    public function postMeta()
    {
      $attr = $this->getAttribute('personid');
      $attr->addListColumns('phone', 'email');
    }
    
    
    /**
     * We can add contacts only from the organization the project belongs to
     * and it's children
     *
     */
    public function personid_selection($record=array(), $mode="select")
    {
      $p_attr = &$this->getAttribute('personid');
      
      if( array_key_exists( 'project', $record ) )
      {
        $projectId = $record['project']['id'];
        $db = &atkGetDb();
                      
        // Determine if the project has an organization 
        $customer = $db->getRows( 'SELECT customer FROM project WHERE id ='.$projectId );
        if( isset( $customer[0]['customer'] ) )
        {
          // Add deeper nested organisations of this customer
          $customers = $this->getOrganisationChildren( $customer[0]['customer'] );
          $customers[] = $customer[0]['customer'];          
          $filter = 'person.company IN ( '.implode(', ', $customers ).')';
          
          $p_attr->addDestinationFilter( $filter );
        }              
        else 
        {
          $p_attr->addDestinationFilter( '1=0' );
        }
      }      
      
      return $p_attr->getSelectableRecords($record, $mode);
    }
    
    
    /**
     * Fetch the nested children of an organisation
     * There's probably some native way to do this, but couldnt find it
     *
     * @param integer $customer
     * @return array
     */
    private function getOrganisationChildren( $customer )
    {
      $return = array();
      
      $db = atkGetDb();
      $rows = $db->getRows( 'SELECT id, parent_id FROM organization ORDER BY parent_id' );
      if( count( $rows ) > 0 )
      {
        foreach( $rows AS $row )
        {
          if( $row['parent_id'] == $customer || in_array( $row['parent_id'], $return ) )
          {
            $return[] = $row['id'];
          }
        }
      }
      return $return;
    }
  }