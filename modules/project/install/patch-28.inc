<?php

  //traffic light
  $setup->addColumn("project","trafficlight_updatestyle", "char(1)");
  
  if($setup->columnExists('project','traffic_light'))
  {
    $setup->alterColumn('project', 'traffic_light', 'trafficlight', 'smallint(3)', true);
  }
  
  if($setup->columnExists('project','project_memo'))
  {
    $setup->alterColumn('project', 'project_memo', 'trafficlight_memo', 'text', true);
  }
  
  $setup->executeSQL("ALTER TABLE `package`
                      ADD `trafficlight` SMALLINT( 3 ) NOT NULL DEFAULT '0',
											ADD `trafficlight_updatestyle` CHAR( 1 ) NOT NULL DEFAULT 'A',
											ADD `trafficlight_memo` TEXT NULL ;");

  $setup->addColumn("phase","trafficlight", "smallint(3)");
  $setup->addColumn("phase","trafficlight_updatestyle", "char(1)");
  $setup->addColumn("phase","trafficlight_memo","text");