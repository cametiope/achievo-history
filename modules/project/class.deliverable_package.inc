<?php

  userelation("atkmanytoonerelation");
  
  /**
   * The node class for deliverable_package
   */
  class deliverable_package extends atkNode
  {
      
  	/**
  	 * Constructor.
  	 */  
    public function __construct()
    {
      $this->atkNode("deliverable_package");
     
      $this->add(new atkManyToOneRelation("package_id","project.package",AF_PRIMARY));       
      $this->add(new atkManyToOneRelation("deliverable_id","project.deliverable",AF_PRIMARY))->
        addListColumns("duedate", "status", "remind_days", "icon");
             
      $this->setTable("deliverable_package");     
      
      $this->setSecurityAlias("project.deliverable");  
    }
    
    
    /**
     * Intercept the deliverable that we are adding phases to, so we can filter the phase dropdown
     * based on the same project.
     */
    public function createForceList($handler)
    {
    	$result = $handler->createForceList();
    	
    	if( array_key_exists( 'package_id', $result ) )
    	{
    	  $db = atkGetDb();
    	  $projectData = $db->getRows('SELECT projectid FROM package WHERE package.id = '.$result["package_id"]["id"] );
          
    	  if( count( $projectData ) > 0 )
    	  {
          $this->getAttribute("deliverable_id")->addDestinationFilter('deliverable.project_id = '.$projectData[0]["projectid"].' AND deliverable.status <> "complete"'); 
    	  }   	  
    	}
    	else 
    	{
      	$projectData = atkGetNode("project.deliverable")->select("deliverable.id = '".$result["deliverable_id"]["id"]."'")
      	                                                 ->includes("project_id")
      	                                                 ->firstRow();
      	
      	$this->getAttribute("package_id")->addDestinationFilter('package.projectid = '.$projectData["project_id"]["id"]);
    	}
    	return $result;
    }   
  }