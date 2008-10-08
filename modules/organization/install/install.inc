<?php

  $setup->installNode("organization.contact");
  $setup->installNode("organization.contracttype");
  $setup->installNode("organization.contracts");
  $setup->installNode("organization.organization");
  $setup->executeSQL("CREATE TABLE `organization_representative` (
                    `organization_id` int(11) NOT NULL,
                    `representative_id` int(11) NOT NULL,
                    PRIMARY KEY  (`organization_id`,`representative_id`))");
  $setup->executeSQL("CREATE TABLE `department_representative` (
                    `department_id` int(11)  NOT NULL,
                    `representative_id` int(11)  NOT NULL,
                    PRIMARY KEY (`department_id`,`representative_id`))
  "); 
  
  $setup->setVersion(23);

?>