<?php
  $setup->executeSQL("
    CREATE TABLE memo (
      id INT( 11 ) NOT NULL ,
      project_id INT( 11 ) NOT NULL ,
      master VARCHAR(255) NOT NULL ,
      master_id INT(11) NOT NULL, 
      memo TEXT NOT NULL ,
      date_added DATETIME NOT NULL ,
      user VARCHAR( 100 ) NOT NULL ,
      location VARCHAR( 100 ) NOT NULL ,
      PRIMARY KEY ( id ) 
    )");
  
  $setup->executeSQL("ALTER TABLE `memo` ADD `updatestyle` VARCHAR( 1 ) NOT NULL");
  
  $setup->executeSQL("ALTER TABLE `project` DROP `trafficlight_memo`;");  
  $setup->executeSQL("ALTER TABLE `package` DROP `trafficlight_memo`;");
  $setup->executeSQL("ALTER TABLE `phase` DROP `trafficlight_memo`;");