<?php

  $setup->executeSQL( "ALTER TABLE `employee_department` ADD `parent_id` INT( 11 ) NOT NULL AFTER `id`" );