<?php

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
      
      $this->menuitem("accountexport",dispatch_url("sflink.sflink", "accountexport"),"sflink",array("sflink.sflink", "accountexport"),760);
      $this->menuitem("clearaccount",dispatch_url("sflink.sflink", "clearsfid"),"sflink",array("sflink.sflink", "accountexport"),770);
      $this->menuitem("clearcontact",dispatch_url("sflink.sflink", "clearcontactsfid"),"sflink",array("sflink.sflink", "accountexport"),780);
      $this->menuitem("contactexport",dispatch_url("sflink.sflink", "contactexport"),"sflink",array("sflink.sflink", "contactexport"),790);
      $this->menuitem("sf_dupe",dispatch_url("sflink.sf_dupe", "admin"),"sflink",array("sflink.sf_dupe", "admin"),800);
      $this->menuitem("getUpdated",dispatch_url("sflink.sflink", "getupdated"),"sflink",array("sflink.sflink", "getupdated"),810);
      $this->menuitem("accountimport",dispatch_url("sflink.sflink", "Accountimport"),"sflink",array("sflink.sflink", "accountexport"),820);
      $this->menuitem("contactimport",dispatch_url("sflink.sflink", "Contactimport"),"sflink",array("sflink.sflink", "contactexport"),830);
      $this->menuitem("fix",dispatch_url("sflink.sflink", "fix"),"sflink",array("sflink.sflink", "contactexport"),830);
    }


  }