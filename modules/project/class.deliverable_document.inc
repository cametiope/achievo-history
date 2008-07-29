<?php

  userelation("atkmanytoonerelation");
  
  /**
   * The node class for deliverable_document
   */
  class deliverable_document extends atkNode
  {
  	
  	/**
  	 * Constructor.
  	 */
    public function __construct()
    { 
      $this->atkNode("deliverable_document");
     
      $this->add(new atkManyToOneRelation("document_id","docmanager.document",AF_PRIMARY))->
        addListColumns("duedate"); 
      $this->add(new atkManyToOneRelation("deliverable_id","project.deliverable",AF_PRIMARY));  
             
      $this->setTable("deliverable_document");   
        
      $this->setSecurityAlias('project.deliverable');
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
    	
    	$this->getAttribute("document_id")->addDestinationFilter('docmanager_document.master = "project.project" AND
    	                                                          docmanager_document.master_id = '.$projectData["project_id"]["id"]);
    	
    	return $result;
    }    
  }