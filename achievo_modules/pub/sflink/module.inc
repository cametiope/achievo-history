<?php
  $modifiers[] = "organization.organization";
  $modifiers[] = "employee.employee";
  $modifiers[] = "organization.contact";

  //setNodeOverloader("organization.organization","sflink.sflorganization");
  //check, if project node is alredy overloaded
  /*$project = atkArrayNvl($g_overloaders,"organization.organization");
  if($project)
  {
    $overloaders[$project] = "sflink.sflorganization";
  }
  else
  {
    $overloaders["organization.organization"] = "sflink.sflorganization";
  }*/

  class mod_sflink extends achievoModule
  {
    function getNodes()
    {
      registerNode("sflink.sflink", array("accountexport","contactexport","getupdated"));
      registerNode("sflink.accountnode", array("admin","add","edit","delete"));
      registerNode("sflink.sf_dupe", array("admin"));
      registerNode("sflink.sflinkcheckupdate", array("report"));
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
      $this->menuitem("getUpdated",dispatch_url("sflink.sflink", "getupdated"),"sflink",array("sflink.sflink", "getupdated"),820);
      $this->menuitem("getAccount",dispatch_url("sflink.sflink", "getupdatedaccount"),"sflink",array("sflink.sflink", "getupdated"),830);
      $this->menuitem("getContact",dispatch_url("sflink.sflink", "getupdatedcontact"),"sflink",array("sflink.sflink", "getupdated"),840);
      $this->menuitem("checkUpdated",dispatch_url("sflink.sflinkcheckupdate", "report"),"sflink",array("sflink.sflinkcheckupdate", "report"),850);
      $this->menuitem("sf_dupe",dispatch_url("sflink.sf_dupe", "admin"),"sflink",array("sflink.sf_dupe", "admin"),860);
      $this->menuitem("check",dispatch_url("sflink.sflink", "check"),"sflink",array("sflink.sflink", "contactexport"),870);
      $this->menuitem("fix",dispatch_url("sflink.sflink", "fix"),"sflink",array("sflink.sflink", "contactexport"),880);
      $this->menuitem("fixcontact",dispatch_url("sflink.sflink", "fixcontact"),"sflink",array("sflink.sflink", "contactexport"),890);
    }

    function organization_modifier(&$node)
    {
      $node->add(new atkAttribute("salesforce_id",AF_READONLY|AF_HIDE_LIST|AF_HIDE_SEARCH));

      $list = array(
        "invoice_city","invoice_country","invoice_zipcode","invoice_state","invoice_address","invoice_address2",
        "visit_city","visit_country","visit_zipcode","visit_state","visit_address","visit_address2","remark",
        "fax","name","employees","phone","mail_city","mail_country","mail_zipcode","mail_state","mail_address",
        "mail_address2","organizationcode","website","type","assigned_to","contactperson"
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