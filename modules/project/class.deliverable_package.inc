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
        addListColumns("duedate");
             
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
    	
    	$projectData = atkGetNode("project.deliverable")->select("deliverable.id = '".$result["deliverable_id"]["id"]."'")
    	                                                 ->includes("project_id")
    	                                                 ->firstRow();
    	
    	$this->getAttribute("package_id")->addDestinationFilter('package.projectid = '.$projectData["project_id"]["id"]);
    	
    	return $result;
    }   
  }