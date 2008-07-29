<?php

  $setup->installNode("timereg_advanced.timereg_advanced");
  $setup->setVersion( "1" );
  
  $setup->addColumn("project", "description_obligatory", "tinyint(3)", false, 0);
  $setup->addColumn("project", "use_activities", "tinyint(3)", false, 0);