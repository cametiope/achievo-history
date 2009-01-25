<?php

  /**
   * @internal modifiers
   */
  $modifiers[] = "timereg.hours";
  
  class mod_project extends achievoModule
  {
    function getNodes()
    {
      /* register nodes */
      registerNode("project.dashboard", array("admin"));
      registerNode("project.project", array("admin", "add", "edit", "delete", "stats","planning", "initialplanning", "any_project", "changeabbreviation"), array("finance", "planning"));
      registerNode("project.package", array("admin", "add", "edit", "delete")):
      registerNode("project.phase", array("admin", "add", "edit", "delete"));
      registerNode("project.deliverable", array("admin", "add", "edit", "delete"));
      registerNode("project.activity", array("admin", "add", "edit", "delete", "stats"));
      registerNode("project.tpl_phase", array("admin", "add", "edit", "delete"));
      registerNode("project.tpl_project", array("admin", "add", "edit", "delete"));
      registerNode("project.role", array("admin", "add", "edit", "delete"));
      registerNode("project.resourceplanning", array("admin", "report"));
      registerNode("project.projectplanning", array("admin", "report"));
      registerNode("project.pim_allocation", array("admin"));
    }

    function getMenuItems()
    {
      $selectedproject = array_key_exists("selectedproject", $_REQUEST) ? $_REQUEST["selectedproject"] : 0;
      if ($selectedproject == 0)
      {
        $this->menuitem("projectmanagement",dispatch_url("project.project", "admin"),"main",array("project.project", "admin"),400); // projects submenu
        $target_planning = dispatch_url("project.project","planning",array("selectedprojectid"=>"[id]"));
        $this->menuitem("project_admin", dispatch_url("project.project", "admin"), "projectmanagement", array("project.project", "admin"));
        $this->menuitem("project_planning",  dispatch_url("project.project", "select", array("atktarget" => atkurlencode($target_planning))), "projectmanagement", array("project.project", "planning"));
        $this->menuitem("projectplanning", dispatch_url("project.projectplanning", "report"), "projectmanagement", array("project.projectplanning", "admin"));
        $this->menuitem("-","","projectmanagement");
        $this->menuitem("project_stats", dispatch_url("project.projectstats", "stats"), "projectmanagement", array("project.project", "stats"));
        $this->menuitem("activity_stats", dispatch_url("project.activity", "stats"), "projectmanagement", array("project.activity", "stats"));
      }
      else
      {
        $this->menuitem("projectmanagement",dispatch_url("project.project", "admin"),"main",array("project.project", "admin"),400); // projects submenu
        $this->menuitem("dashboard", dispatch_url("project.dashboard", "view", array("atkselector" => "project.id='".$selectedproject."'")), 'projectmanagement');
        $this->menuitem("project_edit", dispatch_url("project.project", "edit", array("atkselector" => "project.id='".$selectedproject."'")), "projectmanagement", array("project.project", "edit"));
        if(atkConfig::get("project","package_enabled"))
        {
          $this->menuitem("package", dispatch_url("project.package", "admin", array("atkfilter" => "projectid='".$selectedproject."'")), "projectmanagement", array("project.project", "admin"));
        }
        $this->menuitem("phase", dispatch_url("project.phase", "admin", array("atkfilter" => "projectid='".$selectedproject."'")), 'projectmanagement');
        $this->menuitem("team", dispatch_url("project.project_personemployee", "admin", array("atkfilter" => "projectid='".$selectedproject."'")), "projectmanagement", array("project.project", "admin"));
        $this->menuitem("milestones", dispatch_url("project.deliverable", "admin", array("atkfilter" => "project_id='".$selectedproject."'")), 'projectmanagement');
        $this->menuitem("resourceplanning", dispatch_url("project.resourceplanning", "report", array("atkfilter" => "project.id='".$selectedproject."'")), "projectmanagement", array("project.resourceplanning", "admin"));
        if (moduleExists('history'))
          $this->menuitem("history", dispatch_url("history.history", "admin", array("atkfilter" => "project_id='".$selectedproject."'")), 'projectmanagement', array("history.history", "admin"));

         $this->menuitem("project_planning", dispatch_url("project.ganttchart", "planning", array("atkselector" => "project.id='".$selectedproject."'")), "projectmanagement", array("project.project", "admin"));
         
        $this->menuitem("-","","projectmanagement");
        //$this->menuitem("project_stats", dispatch_url("project.projectstats", "stats", array("projectid" => $selectedproject)), "projectmanagement", array("project.project", "stats"));
      }
      $this->menuitem("mastergantt", dispatch_url("project.mastergantt", "planning"), "projectmanagement", array("project.project", "planning"));
      $this->menuitem("activity_admin", dispatch_url("project.activity", "admin"), "setup", array("project.activity", "admin"));
      $this->menuitem("project_group", dispatch_url("project.project_group", "admin"), "setup", array("project.activity", "admin"));
      $this->menuitem("project_categories", dispatch_url("project.project_category", "admin"), "setup", array("project.activity", "admin"));
      $this->menuitem("tpl_phase_admin", dispatch_url("project.tpl_phase", "admin"), "setup", array("project.tpl_phase", "admin"));
      $this->menuitem("tpl_project_admin", dispatch_url("project.tpl_project", "admin"), "setup", array("project.tpl_project", "admin"));
      $this->menuitem("role_admin", dispatch_url("project.role", "admin"), "setup", array("project.role", "admin"));
      $this->menuitem("mastergantt_colorconfig", dispatch_url("project.mastergantt_colorconfig", "admin"), "setup", array("project.mastergantt_colorconfig", "admin"));
    }

    function getDependencies($mode)
    {
      return array("employee", "organization");
    }

    function search($expression)
    {
      $prjnode = &atkGetNode("project.project");
      return array("projects"=>$this->recLinks($prjnode->searchDb($expression), "project.project"));
    }

    /**
     * Adds listeners to the timereg.hours node
     *
     * @param atkNode $node hours node instance
     */
    function timereg_hours_modifier(&$node)
    {
      atkimport("module.project.hourslistener");
      $node->addListener(new hoursListener());

//      atkimport("module.project.trafficlightlistener");
//      $node->addListener(new trafficlightListener());
    }

    function getPimItems()
    {
      return array("allocations"=>"getAllocations",);
    }
    
    function getAllocations()
    {
      include_once(moduleDir("project")."pim.inc");
      return pimAllocations();
    }
    
    
  }
