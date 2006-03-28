<?php

  atkimport("atk.utils.atkactionlistener");

  class conflictListener extends atkActionListener
  {

    function conflictListener($actionfilter=array())
    {
      $this->atkActionListener($actionfilter);
    }

    function notify($action, $record)
    {      
      $node = &getNode("teamplanning.planningconflictlog");
      $nodetype = $this->m_node->atkNodeType();
      
      atkdebug("Conflictlistener notify ".$nodetype);
      
      if ($nodetype=="timereg.hours")
      {
        // check conflicts CONFLICT_BOOKED_NEARS_PLANNED and CONFLICT_BOOKED_EXCEEDS_PLANNED
        $node->checkConflictBooked($record["phaseid"]["phaseid"]);
      }
      else if ($nodetype=="project.phase")
      {
        //$node->checkConflictBooked($record["id"]);
        $node->checkConflictPlanned($record["id"]);        
      }
      else if ($nodetype=="teamplanning.planning")
      {        
        // watch out, values are stored 'flat' in $record because this notify is manually called from
        // weekplanning node, with flat record.
        $node->checkConflictPlanned($record["phaseid"]);
        $node->checkConflictEmployee($record["employeeid"], $record["monday"]);
      }
    }


  }

?>