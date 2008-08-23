<?php
  $setup->installNode("organization.organization_representative");
  $setup->installNode("employee.department_representative");  
  $setup->executeSQL("ALTER TABLE `organization` ADD `manager_id` INT( 11 ) NULL,
                      ADD `parent_id` INT( 11 ) NOT NULL");
  $setup->executeSQL("ALTER TABLE `employee_department` ADD `manager_id` INT( 11 ) NOT NULL,
                      ADD `total_hours` INT( 11 ) NOT NULL");