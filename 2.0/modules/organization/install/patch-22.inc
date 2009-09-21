<?php
  $setup->executeSQL("CREATE TABLE `department_representative` (
                    `department_id` int(11)  NOT NULL,
                    `representative_id` int(11)  NOT NULL,
                    PRIMARY KEY (`department_id`,`representative_id`))
  "); 
