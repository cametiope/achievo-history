<?php
  $setup->executeSQL("
    ALTER TABLE `project_person` ADD `allocated_by` INT( 11 ) NOT NULL,
    ADD `allocated_date` DATE NOT NULL");