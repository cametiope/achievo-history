<?php

  class mod_sflink extends achievoModule
  {
    function getNodes()
    {
      registerNode("sflink.sflink", array("accountexport","contactexport"));
    }

    function getMenuItems()
    {
      $this->menuitem("sflink","","main",array("sflink.sflink", "accountexport"),750);
      $this->menuitem("accountexport",dispatch_url("sflink.sflink", "accountexport"),"sflink",dispatch_url("sflink.sflink", "accountexport"),760);
      $this->menuitem("clear",dispatch_url("sflink.sflink", "clearsfid"),"sflink",dispatch_url("sflink.sflink", "accountexport"),780);
      $this->menuitem("contactexport",dispatch_url("sflink.sflink", "contactexport"),"sflink",dispatch_url("sflink.sflink", "contactexport"),770);
    }


  }