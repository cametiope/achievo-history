<?php
  // Add modifier
  $modifiers = array( "project.project", "project.package", "project.phase" );

  class mod_history extends achievoModule
  {

    public function getNodes()
    {
      registerNode("history.history", array("admin"));
    }


    public function getMenuItems()
    {
      // menu items
    }


    /**
     * Include a listener to the project.project node
     * This ensures we can track project edits only when the history module is available,
     * without editing the project node
     *
     * @param $node node instance
     */
    public function project_project_modifier( &$node )
    {
      atkimport( "module.history.listeners.historyprojectlistener" );
      $node->addFlag( NF_TRACK_CHANGES );
      $node->addListener( new historyProjectListener( array('update') ) );
    }


    /**
     * Include a listener to the project.package node
     * This ensures we can track package edits only when the history module is available,
     * without editing the package node
     *
     * @param $node node instance
     */
    public function project_package_modifier( &$node )
    {
      atkimport( "module.history.listeners.historypackagelistener" );
      $node->addFlag( NF_TRACK_CHANGES );
      $node->addListener( new historyPackageListener( array('update') ) );
    }


    /**
     * Include a listener to the project.package node
     * This ensures we can track phase edits only when the history module is available,
     * without editing the package node
     *
     * @param $node node instance
     */
    public function project_phase_modifier( &$node )
    {
      atkimport( "module.history.listeners.historyphaselistener" );
      $node->addFlag( NF_TRACK_CHANGES );
      $node->addListener( new historyPhaseListener( array('update') ) );
    }

  }