<?php

$setup->addColumn("organization","salesforce_id","CHAR(18)");
$setup->addColumn("person","salesforce_id","CHAR(18)");
$setup->executeSQL("
  CREATE TABLE `sf_dupe` (
    `type` varchar(50) NOT NULL,
    `id` int(10) NOT NULL,
    `name` varchar(100) default NULL,
    `name1` varchar(100) default NULL,
    `fields` varchar(200) default NULL,
    `message` varchar(255) default NULL,
    `statuscode` varchar(100) default NULL,
    `direction` varchar(20) default NULL,
     KEY `type_id` (`type`,`id`)
)");

$setup->setVersion(1);
