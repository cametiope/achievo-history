<?php

if($setup->columnExists("employee_department", "parent_id"))
{
  $setup->executeSQL( "ALTER TABLE `employee_department` CHANGE `parent_id` `parent_id` INT( 11 ) NULL" );
}

if($setup->columnExists("employee_department", "manager_id"))
{
  $setup->executeSQL( "ALTER TABLE `employee_department` CHANGE `manager_id` `manager_id` INT( 11 ) NULL" );
}