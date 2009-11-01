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
      
      $this->menuitem("accountexport",dispatch_url("sflink.sflink", "accountexport"),"sflink",dispatch_url("sflink.sflink", "accountexport"),760);
      $this->menuitem("clearaccount",dispatch_url("sflink.sflink", "clearsfid"),"sflink",dispatch_url("sflink.sflink", "accountexport"),780);
      $this->menuitem("clearcontact",dispatch_url("sflink.sflink", "clearcontactsfid"),"sflink",dispatch_url("sflink.sflink", "accountexport"),790);
      $this->menuitem("contactexport",dispatch_url("sflink.sflink", "contactexport"),"sflink",dispatch_url("sflink.sflink", "contactexport"),770);
      $this->menuitem("sf_dupe",dispatch_url("sflink.sf_dupe", "admin"),"sflink",array("sflink.sf_dupe", "admin"),750);
      $this->menuitem("getUpdated",dispatch_url("sflink.sflink", "getupdated"),"sflink",array("sflink.sflink", "getupdated"),750);
    }


  }