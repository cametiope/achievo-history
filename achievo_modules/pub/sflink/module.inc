<?php
  $modifiers[] = "organization.organization";
  $modifiers[] = "employee.employee";
  $modifiers[] = "organization.contact";

  class mod_sflink extends achievoModule
  {
    function getNodes()
    {
      registerNode("sflink.sflink", array("accountexport","contactexport","getupdated"));
      registerNode("sflink.sf_dupe", array("admin"));
    }

    function getMenuItems()
    {
      $this->menuitem("sflink","","main",array("sflink.sflink", "accountexport"),750);
      
      $this->menuitem("employeelink",dispatch_url("sflink.sflink", "employeelink"),"sflink",array("sflink.sflink", "contactexport"),755);
      $this->menuitem("accountexport",dispatch_url("sflink.sflink", "accountexport"),"sflink",array("sflink.sflink", "accountexport"),760);
      $this->menuitem("clearaccount",dispatch_url("sflink.sflink", "clearsfid"),"sflink",array("sflink.sflink", "accountexport"),770);
      $this->menuitem("contactexport",dispatch_url("sflink.sflink", "contactexport"),"sflink",array("sflink.sflink", "contactexport"),780);
      $this->menuitem("clearcontact",dispatch_url("sflink.sflink", "clearcontactsfid"),"sflink",array("sflink.sflink", "accountexport"),790);
      $this->menuitem("accountimport",dispatch_url("sflink.sflink", "Accountimport"),"sflink",array("sflink.sflink", "accountexport"),800);
      $this->menuitem("contactimport",dispatch_url("sflink.sflink", "Contactimport"),"sflink",array("sflink.sflink", "contactexport"),810);
      $this->menuitem("getUpdated",dispatch_url("sflink.sflink", "getupdated"),"sflink",array("sflink.sflink", "getupdated"),830);
      $this->menuitem("sf_dupe",dispatch_url("sflink.sf_dupe", "admin"),"sflink",array("sflink.sf_dupe", "admin"),820);
      //$this->menuitem("fix",dispatch_url("sflink.sflink", "fix"),"sflink",array("sflink.sflink", "contactexport"),830);
    }
    
    function organization_modifier(&$node)
    {
      $node->add(new atkAttribute("salesforce_id",AF_READONLY|AF_HIDE_LIST|AF_HIDE_SEARCH));
      
      $list = array(
        "invoice_city","invoice_country","invoice_zipcode","invoice_state","invoice_address","invoice_address2",
        "visit_city","visit_country","visit_zipcode","visit_state","visit_address","visit_address2","remark",
        "fax","name","employees","phone","mail_city","mail_country","mail_zipcode","mail_state","mail_address",
        "mail_address2","organizationcode","website","type","assigned_to"
      );
      foreach ($list as $l)
      {
        $node->getAttribute($l)->addFlag(AF_READONLY);
      }
    }

    function employee_modifier(&$node)
    {
      $node->add(new atkAttribute("salesforce_id",AF_READONLY|AF_HIDE_LIST|AF_HIDE_SEARCH));
    }
    
    function contact_modifier(&$node)
    {
      $node->add(new atkAttribute("salesforce_id",AF_READONLY|AF_HIDE_LIST|AF_HIDE_SEARCH));

      $list = array(
        "title_id","lastname","firstname","phone","fax","email","cellular","city","country","zipcode",
        "state","address","remark","function","company"
      );
      foreach ($list as $l)
      {
        $node->getAttribute($l)->addFlag(AF_READONLY);
      }
    }
    
  }