<?php

  $setup->installNode("project.activity");
  $setup->installNode("project.phase");
  $setup->installNode("project.phase_activity");
  $setup->installNode("project.phaseplanning");
  $setup->installNode("project.project");
  $setup->installNode("project.project_personcontact");
  $setup->installNode("project.project_personemployee");
  $setup->installNode("project.role");
  $setup->installNode("project.dependency");
  $setup->installNode("project.tpl_phase");
  $setup->installNode("project.tpl_phase_activity");
  $setup->installNode("project.tpl_project");
  $setup->installNode("project.tpl_project_phase");
  $setup->installNode("project.tpl_dependency");
  $setup->installNode("project.deliverable");
  $setup->installNode("project.mastergantt_colorconfig");
  $setup->installNode("project.project_category");
  $setup->executeSQL("CREATE TABLE `package` (
  `id` int(11) NOT NULL,
  `abbreviation_package` varchar(10) default NULL,
  `name` varchar(200) NOT NULL,
  `category_id` int(11) default NULL,
  `coordinator_id` int(11) NOT NULL,
  `description` text,
  `status` varchar(15) NOT NULL,
  `master` varchar(100) default NULL,
  `master_id` int(11) default NULL,
  `startdate` date default NULL,
  `enddate` date default NULL,
  `deposit` int(11) default NULL,
  `completed` int(11) default NULL,
  `initial_planning` int(11) default 0,
  `current_planning` int(11) default 0,
  `spend_hours` int(11) default 0,
  `remaining_effort_time` int(11) default 0,
  `projectid` int(11) not null,
  PRIMARY KEY  (`id`),
  KEY `master` (`master_id`),
  KEY `coordinator_id` (`coordinator_id`)
	)");

  $setup->executeSQL("CREATE TABLE `package_person` (
  `package_id` int(11) NOT NULL,
  `person_id` int(11) NOT NULL,
  PRIMARY KEY  (`package_id`,`person_id`)
	)");

  $setup->executeSQL('
  CREATE TABLE IF NOT EXISTS `project_contact` (
    `project` int(10) NOT NULL,
    `personid` int(10) NOT NULL,
    PRIMARY KEY  (`project`,`personid`)
  ) ENGINE=MyISAM');
    
  
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
  
  $setup->installNode("project.project_group");
	$setup->installNode("project.deliverable_phase");
	$setup->installNode("project.deliverable_package");
	$setup->installNode("project.deliverable_document");
	$setup->installNode("project.project_contact");

  $setup->setVersion(33);

?>
