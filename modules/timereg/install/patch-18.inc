<?php

$setup->executeSQL("ALTER TABLE `hoursbase` ADD INDEX `phaseid` (`phaseid`),
  ADD INDEX `userid` (`userid`),
  ADD INDEX `activitydate` (`activitydate`)");