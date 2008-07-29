<?php

  //traffic light
  $setup->addColumn("project","traffic_updatestyle", "char(1)");
  $setup->alterColumn('project', 'project_memo', 'trafficlight_memo', 'text', true);

  $setup->executeSQL("ALTER TABLE `package`
                      ADD `trafficlight` SMALLINT( 3 ) NOT NULL DEFAULT '0',
											ADD `trafficlight_updatestyle` CHAR( 1 ) NOT NULL DEFAULT 'A',
											ADD `trafficlight_memo` TEXT NULL ;");

  $setup->addColumn("package","trafficlight", "smallint(3)");
  $setup->addColumn("package","traffic_updatestyle", "char(1)");
  $setup->addColumn("package","trafficlight_memo","text");