<?php
  $setup->executeSQL("CREATE TABLE `organization_representative` (
                      `organization_id` int(11) NOT NULL,
                      `representative_id` int(11) NOT NULL,
                      PRIMARY KEY  (`organization_id`,`representative_id`))");
  $setup->executeSQL("ALTER TABLE `organization` ADD `manager_id` INT( 11 ) NULL,
                      ADD `parent_id` INT( 11 ) NOT NULL");
  $setup->executeSQL("ALTER TABLE `employee_department` ADD `manager_id` INT( 11 ) NOT NULL,
                      ADD `total_hours` INT( 11 ) NOT NULL");