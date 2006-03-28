<?php
  
  $modifiers[] = "timereg.hours";
  $modifiers[] = "project.phase";

  
  class mod_teamplanning extends atkModule
  {

    /**
     * Defines the nodes and the available rights you can grant to users on these nodes
     */
    function getNodes()
    {
      registerNode("teamplanning.teamplanning", array("admin", "edit", "any_user", "any_project"));
      registerNode("teamplanning.teamplanningconflictlog", array("admin", "view"));
    }
    
    function getDescription()
    {
      return "Module for making team based planning";
    }
    
    function getMenuItems()
    {
      // Add a teamplanning submenu if the user has either admin access to the teamplanning or teamplanningconflictlog node
      $this->menuitem("teamplanning", "", "main", array("teamplanning.teamplanning", "admin", "teamplanning.teamplanningconflictlog", "admin"));
      
      // Add links to the 2 teamplanning views if the user has admin access to the teamplanning node
      $this->menuitem("teamplanning_planning", dispatch_url("teamplanning.teamplanning", "admin"), "teamplanning", array("teamplanning.teamplanning", "admin"));      

      // Add a link to the teamplanningconflictlog node if the user has admin access on this node
      $this->menuitem("teamplanningconflicts", dispatch_url("teamplanning.teamplanningconflictlog", "admin"), "teamplanning", array("teamplanning.teamplanningconflictlog", "admin"));

      // Add a seperator after the last item, to create a spacing between the menu items and the "Back to the main menu" link
      $this->menuitem("-", "", "teamplanning", array("teamplanning.teamplanning", "admin", "teamplanning.teamplanningconflictlog", "admin"));

    }

    function timereg_hours_modifier(&$node)
    {
      $this->addConflictListener($node, array("save", "update"));
    }

//    function project_phase_modifier(&$node)
//    {
//      $this->addConflictListener($node, array("update"));
//
//      // Add the listener for phaseplanning shiftings when updating a phase
//      atkimport("module.teamplanning.shiftphaselistener");
//      $node->addListener(new shiftPhaseListener(array("update")));
//    }

    function addConflictListener(&$node, $actionfilter)
    {
      atkimport("module.teamplanning.conflictlistener");
      $node->addListener(new conflictListener($actionfilter));
    }

  }
?>