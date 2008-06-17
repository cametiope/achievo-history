<?php
  $setup->executeSQL("CREATE TABLE `package` (
  `id` int(11) NOT NULL,
  `abbreviation` varchar(10) NOT NULL,
  `name` varchar(200) NOT NULL,
  `master` varchar(100) default NULL,
  `master_id` int(11) default NULL,
  `description` text,
  `coordinator_id` int(11) NOT NULL,
  `category_id` int(11) default NULL,
  `status` smallint(6) NOT NULL,
  `startdate` date default NULL,
  `enddate` date default NULL,
  `traffic_light` smallint(6) default NULL,
  `initial_planning` time default NULL,
  `completed` smallint(6) default NULL,
  `current_planning` time default NULL,
  `overall_time` time default NULL,
  `remaining_effort_time` time default NULL,
  `planned_time` time default NULL,
  `dependsondeliverable` int(11) default NULL,
  PRIMARY KEY  (`id`),
  KEY `master` (`master_id`),
  KEY `coordinator_id` (`coordinator_id`)
)");
  
  $setup->executeSQL("CREATE TABLE `package_person` (
  `package_id` int(11) NOT NULL,
  `person_id` int(11) NOT NULL,
  PRIMARY KEY  (`package_id`,`person_id`)
)");
?>