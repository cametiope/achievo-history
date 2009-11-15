SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;

--
-- Table structure for table `tpl_dependency`
--

CREATE TABLE `tpl_dependency` (
  `phaseid_row` int(11) unsigned NOT NULL,
  `phaseid_col` int(11) unsigned NOT NULL,
  `projectid` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`phaseid_row`,`phaseid_col`,`projectid`),
  KEY `FK_TPL_DEPENDENCY_TPL_PROJECT` (`projectid`),
  CONSTRAINT `FK_TPL_DEPENDENCY_TPL_PROJECT` FOREIGN KEY (`projectid`) REFERENCES tpl_project (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Dependency template';

-- --------------------------------------------------------

--
-- Table structure for table `tpl_phase`
--

CREATE TABLE `tpl_phase` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(50)  NOT NULL,
  `description` text,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Phase template';

-- --------------------------------------------------------

--
-- Table structure for table `tpl_phase_activity`
--

CREATE TABLE `tpl_phase_activity` (
  `activityid` int(11) unsigned NOT NULL,
  `phaseid` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`activityid`,`phaseid`),
  KEY `FK_TPL_PHASE_ACTIVITY_ACTIVITY` (`activityid`),
  KEY `FK_TPL_PHASE_ACTIVITY_TPL_PHASE` (`phaseid`),
  CONSTRAINT `FK_TPL_PHASE_ACTIVITY_ACTIVITY` FOREIGN KEY (`activityid`) REFERENCES activity (`id`),
  CONSTRAINT `FK_TPL_PHASE_ACTIVITY_TPL_PHASE` FOREIGN KEY (`phaseid`) REFERENCES tpl_phase (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Phase activity template';

-- --------------------------------------------------------

--
-- Table structure for table `tpl_project`
--

CREATE TABLE `tpl_project` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(50)  NOT NULL,
  `description` text,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Project template';

-- --------------------------------------------------------

--
-- Table structure for table `tpl_project_phase`
--

CREATE TABLE `tpl_project_phase` (
  `projectid` int(11) unsigned NOT NULL,
  `phaseid` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`projectid`,`phaseid`),
  KEY `FK_TPL_PROJECT_PHASE_TPL_PROJECT` (`projectid`),
  KEY `FK_TPL_PROJECT_PHASE_TPL_PHASE` (`phaseid`),
  CONSTRAINT `FK_TPL_PROJECT_PHASE_TPL_PROJECT` FOREIGN KEY (`projectid`) REFERENCES tpl_project (`id`),
  CONSTRAINT `FK_TPL_PROJECT_PHASE_TPL_PHASE` FOREIGN KEY (`phaseid`) REFERENCES tpl_phase (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Project phase template';


--
-- Table structure for table `phase`
--

CREATE TABLE `phase` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(50)  NOT NULL,
  `projectid` int(11) unsigned NOT NULL,
  `description` text,
  `status` varchar(9)  NOT NULL,
  `virtual_time` tinyint(1) unsigned default NULL,
  `startdate` date default NULL,
  `enddate` date default NULL,
  `dependsondeliverable` int(11) unsigned DEFAULT NULL,
  `initial_planning` int(11) unsigned DEFAULT NULL,
  `current_planning` int(11) unsigned DEFAULT NULL,
  `spend_hours` text,
  PRIMARY KEY  (`id`),
  KEY `FK_PHASE_PROJECT` (`projectid`),
  CONSTRAINT `FK_PHASE_PROJECT` FOREIGN KEY (`projectid`) REFERENCES project (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Project phases';

-- --------------------------------------------------------

--
-- Table structure for table `phase_activity`
--

CREATE TABLE `phase_activity` (
  `phaseid` int(11) unsigned NOT NULL,
  `activityid` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`phaseid`,`activityid`),
  CONSTRAINT `FK_PHASE_ACTIVITY_PHASE` FOREIGN KEY (`phaseid`) REFERENCES phase (`id`),
  CONSTRAINT `FK_PHASE_ACTIVITY_ACTIVITY` FOREIGN KEY (`activityid`) REFERENCES activity (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Phase activity';

-- --------------------------------------------------------


-- --------------------------------------------------------

--
-- Table structure for table `project`
--

CREATE TABLE `project` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `abbreviation` varchar(10)  default NULL,
  `name` varchar(200)  NOT NULL,
  `project_category` int(11) unsigned DEFAULT NULL,
  `coordinator` int(11) unsigned DEFAULT NULL,
  `master_project` int(11) unsigned DEFAULT NULL,
  `contract_id` int(11) unsigned DEFAULT NULL,
  `description` text,
  `quotation_number` text,
  `fixed_price` decimal(14,5) default NULL,
  `status` varchar(15)  default NULL,
  `customer` int(11) unsigned DEFAULT NULL,
  `timereg_limit` tinyint(1) unsigned default NULL,
  `alwaysvisibleintimereg` tinyint(1) unsigned default NULL,
  `startdate` date default NULL,
  `enddate` date default NULL,
  `template` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY  (`id`),
  KEY `IDX_NAME` (`name`(25)),
  KEY `FK_PROJECT_PROJECT_CATEGORY` (`project_category`),
  KEY `FK_PROJECT_PERSON` (`coordinator`),
  KEY `FK_PROJECT_PROJECT` (`master_project`),
  KEY `FK_PROJECT_CONTRACT` (`contract_id`),
  KEY `FK_PROJECT_PERSON2` (`customer`),
  CONSTRAINT `FK_PROJECT_PROJECT_CATEGORY` FOREIGN KEY (`project_category`) REFERENCES project_category (`id`),
  CONSTRAINT `FK_PROJECT_PERSON` FOREIGN KEY (`coordinator`) REFERENCES person (`id`),
  CONSTRAINT `FK_PROJECT_PROJECT` FOREIGN KEY (`master_project`) REFERENCES project (`id`),
  CONSTRAINT `FK_PROJECT_CONTRACT` FOREIGN KEY (`contract_id`) REFERENCES contract (`id`),
  CONSTRAINT `FK_PROJECT_PERSON2` FOREIGN KEY (`customer`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Projects';

-- --------------------------------------------------------

--
-- Table structure for table `project_category`
--

CREATE TABLE `project_category` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `project_category` varchar(50)  NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Project category';

-- --------------------------------------------------------


-- --------------------------------------------------------

--
-- Table structure for table `project_person`
--

CREATE TABLE `project_person` (
  `projectid` int(11) unsigned NOT NULL,
  `personid` int(11) unsigned NOT NULL,
  `role` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`projectid`,`personid`,`role`),
  KEY `FK_PROJECT_PERSON_PROJECT` (`projectid`),
  KEY `FK_PROJECT_PERSON_PERSON` (`personid`),
  CONSTRAINT `FK_PROJECT_PERSON_PROJECT` FOREIGN KEY (`projectid`) REFERENCES project (`id`),
  CONSTRAINT `FK_PROJECT_PERSON_PERSON` FOREIGN KEY (`personid`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Project employees / contacts';

-- --------------------------------------------------------

--
-- Table structure for table `project_phaseplanning`
--

CREATE TABLE `project_phaseplanning` (
  `phaseid` int(11) unsigned NOT NULL,
  `personid` int(11) unsigned NOT NULL,
  `initial_planning` int(11) unsigned DEFAULT NULL,
  `current_planning` int(11) unsigned DEFAULT NULL,
  `spend_hours` varchar(100)  default NULL,
  PRIMARY KEY  (`phaseid`,`personid`),
  KEY `FK_PROJECT_PHASEPLANNING_PHASE` (`phaseid`),
  KEY `FK_PROJECT_PHASEPLANNING_PERSON` (`personid`),
  CONSTRAINT `FK_PROJECT_PHASEPLANNING_PHASE` FOREIGN KEY (`phaseid`) REFERENCES phase (`id`),
  CONSTRAINT `FK_PROJECT_PHASEPLANNING_PERSON` FOREIGN KEY (`personid`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Project phase planning';


--
-- Table structure for table `mastergantt_colorconfig`
--

CREATE TABLE `mastergantt_colorconfig` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `hours_min` int(11) unsigned DEFAULT NULL,
  `hours_max` int(11) unsigned DEFAULT NULL,
  `color` varchar(10)  default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Mastergantt color config';


--
-- Table structure for table `dependency`
--

CREATE TABLE `dependency` (
  `phaseid_row` int(11) unsigned NOT NULL,
  `phaseid_col` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`phaseid_row`,`phaseid_col`),
  CONSTRAINT `FK_DEPENDENCY_PHASE` FOREIGN KEY (`phaseid_row`) REFERENCES phase (`id`),
  CONSTRAINT `FK_DEPENDENCY_PHASE2` FOREIGN KEY (`phaseid_col`) REFERENCES phase (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Dependency';


--
-- Table structure for table `activity`
--

CREATE TABLE `activity` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(50) NOT NULL,
  `description` varchar(100) default NULL,
  `remarkrequired` tinyint(1) unsigned default NULL,
  `overtimecompensation` tinyint(1) unsigned default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Activities';


--
-- Table structure for table `deliverable`
--

CREATE TABLE `deliverable` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(50)  NOT NULL,
  `project_id` int(11) unsigned NOT NULL,
  `duedate` date NOT NULL,
  `status` varchar(8)  NOT NULL,
  `remind_days` int(11) unsigned DEFAULT NULL,
  `userid` int(11) unsigned NOT NULL,
  `entrydate` date NOT NULL,
  `emailstatus` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY  (`id`),
  KEY (`project_id`),
  KEY (`userid`),
  CONSTRAINT `FK_DELIVERABLE_PROJECT` FOREIGN KEY (`project_id`) REFERENCES project (`id`),
  CONSTRAINT `FK_DELIVERABLE_PERSON` FOREIGN KEY (`userid`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Deliverable';



--
-- Table structure for table `role`
--

CREATE TABLE `role` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(100) NOT NULL,
  `description` text NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT = "Roles" ;

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
