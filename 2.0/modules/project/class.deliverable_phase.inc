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

      $this->add(new atkManyToOneRelation("deliverable_id","project.deliverable",AF_PRIMARY))->
        addListColumns("duedate", "status", "remind_days", "icon");
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

      if( array_key_exists( 'phase_id', $result ) )
      {
        $db = atkGetDb();
        $projectData = $db->getRows('SELECT projectid FROM phase WHERE phase.id = '.$result["phase_id"]["id"] );
          
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
  
      	$this->getAttribute("phase_id")->addDestinationFilter('phase.projectid = '.$projectData["project_id"]["id"]);
    	}

    	return $result;
    }
    
  }