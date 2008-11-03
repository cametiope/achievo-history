<?php
$setup->executeSQL('
CREATE TABLE IF NOT EXISTS `project_contact` (
  `project` int(10) NOT NULL,
  `personid` int(10) NOT NULL,
  PRIMARY KEY  (`project`,`personid`)
) ENGINE=MyISAM');