<?php

$setup->addColumn("organization","salesforce_id","CHAR(18)");
$setup->addColumn("person","salesforce_id","CHAR(18)");
$setup->executeSQL("
  CREATE TABLE `sf_dupe` (
  `type` varchar(50) NOT NULL,
  `id` int(10) NOT NULL,
  `name` varchar(100) default NULL,
  `name1` varchar(100) default NULL,
  `created_at` datetime default NULL,
  `message` varchar(200) default NULL,
  `fields` varchar(200) default NULL,
  `statuscode` varchar(200) default NULL,
  `direction` varchar(50) default NULL,
  KEY `new_index` (`type`,`id`)
)");

$setup->setVersion(1);
