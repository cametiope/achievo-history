<?php

  userelation("atkmanytoonerelation");
  
  /**
   * The node class for deliverable_phase
   */
  class deliverable_phase extends atkNode
  {
  	
  	/**
  	 * Constructor.
  	 */
    public function __construct()
    {      
      $this->atkNode("deliverable_phase");
           
      $this->add(new atkManyToOneRelation("deliverable_id","project.deliverable",AF_PRIMARY));
      $this->add(new atkManyToOneRelation("phase_id","project.phase",AF_PRIMARY)); 
       
      $this->setTable("deliverable_phase");     
      
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
    	
    	$this->getAttribute("phase_id")->addDestinationFilter('phase.projectid = '.$projectData["project_id"]["id"]);
    	
    	return $result;
    }
    
   
  }