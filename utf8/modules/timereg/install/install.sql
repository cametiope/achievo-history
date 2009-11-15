SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `hoursbase`
--

CREATE TABLE `hoursbase` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `virtual_time` tinyint(1) unsigned default NULL,
  `activitydate` date NOT NULL,
  `todate` date default NULL,
  `userid` int(11) unsigned NOT NULL,
  `phaseid` int(11) unsigned NOT NULL,
  `activityid` int(11) unsigned NOT NULL,
  `remark` text,
  `time` int(11) unsigned NOT NULL,
  `workperiod` int(11) unsigned DEFAULT NULL,
  `entrydate` date NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `IDX_ACTIVITYDATE` (`activitydate`),
  KEY `FK_HOURSBASE_PERSON` (`userid`),
  KEY `FK_HOURSBASE_PHASE` (`phaseid`),
  KEY `FK_HOURSBASE_ACTIVITY` (`activityid`),
  KEY `FK_HOURSBASE_WORKPERIOD` (`workperiod`),
  CONSTRAINT `FK_HOURSBASE_PERSON` FOREIGN KEY (`userid`) REFERENCES person (`id`),
  CONSTRAINT `FK_HOURSBASE_PHASE` FOREIGN KEY (`phaseid`) REFERENCES phase (`id`),
  CONSTRAINT `FK_HOURSBASE_ACTIVITY` FOREIGN KEY (`activityid`) REFERENCES activity (`id`),
  CONSTRAINT `FK_HOURSBASE_WORKPERIOD` FOREIGN KEY (`workperiod`) REFERENCES workperiod (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Hours';

-- --------------------------------------------------------

--
-- Table structure for table `hours_lock`
--

CREATE TABLE `hours_lock` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `period` int(6) NOT NULL,
  `userid` int(11) unsigned DEFAULT NULL,
  `approved` tinyint(1) unsigned default NULL,
  `coordinator` varchar(100)  default NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_HOURS_LOCK_PERSON` (`userid`),
  CONSTRAINT `FK_HOURS_LOCK_PERSON` FOREIGN KEY (`userid`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Hours locks';

-- --------------------------------------------------------
-- --------------------------------------------------------

-- --------------------------------------------------------

--
-- Table structure for table `overtime_balance`
--

CREATE TABLE `overtime_balance` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `userid` int(11) unsigned NOT NULL,
  `day` date NOT NULL,
  `balance` decimal(12,2) NOT NULL,
  `manual` varchar(100)  default NULL,
  `remark` text,
  PRIMARY KEY  (`id`),
  KEY `FK_OVERTIME_BALANCE_P` (`userid`),
  CONSTRAINT `FK_OVERTIME_BALANCE_P` FOREIGN KEY (`userid`) REFERENCES `person`(`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Overtime balance';


--
-- Table structure for table `workperiod`
--

CREATE TABLE `workperiod` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(50)  NOT NULL,
  `starttime` time NOT NULL,
  `endtime` time NOT NULL,
  `percentage` int(11) unsigned NOT NULL,
  `defaultrate` tinyint(1) unsigned default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Contract workperiod';



--
-- Structure for view `hours`
--
DROP TABLE IF EXISTS `hours`;

CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `hours` AS select `hoursbase`.`id` AS `id`,`hoursbase`.`virtual_time` AS `virtual_time`,`hoursbase`.`activitydate` AS `activitydate`,`hoursbase`.`todate` AS `todate`,`hoursbase`.`userid` AS `userid`,`hoursbase`.`phaseid` AS `phaseid`,`hoursbase`.`activityid` AS `activityid`,`hoursbase`.`remark` AS `remark`,`hoursbase`.`time` AS `time`,`hoursbase`.`workperiod` AS `workperiod`,`hoursbase`.`entrydate` AS `entrydate` from `hoursbase` where ((`hoursbase`.`virtual_time` = 0) or isnull(`hoursbase`.`virtual_time`));
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
