<?php

  class mod_sflink extends achievoModule
  {
    function getNodes()
    {
      registerNode("sflink.sflink", array("sfexport"));
    }

    function getMenuItems()
    {
      $this->menuitem("sflink","","main",array("sflink.sflink", "sfexport"),750);
      $this->menuitem("sfexport",dispatch_url("sflink.sflink", "sfexport"),"sflink",dispatch_url("sflink.sflink", "sfexport"),760);
    }


  }