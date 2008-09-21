<?php

if(!$setup->columnExists("employee_department", "parent_id"))
{
  $setup->executeSQL( "ALTER TABLE `employee_department` ADD `parent_id` INT( 11 ) NOT NULL AFTER `id`" );
}