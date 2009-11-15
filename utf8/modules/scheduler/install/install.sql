SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `scheduler_alarms`
--

CREATE TABLE `scheduler_alarms` (
  `scheduler_id` int(11) unsigned NOT NULL,
  `startdate` int(11) unsigned NOT NULL,
  `duration` int(11) unsigned DEFAULT NULL,
  `senddate` int(11) unsigned DEFAULT NULL,
  `send` tinyint(1) unsigned default NULL,
  `type` varchar(100)  default NULL,
  `userid` int(11) unsigned NOT NULL,
  `owner` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY  (`scheduler_id`,`startdate`,`userid`),
  KEY `FK_SCHEDULER_ALARMS_SCHEDULER` (`scheduler_id`),
  KEY `FK_SCHEDULER_ALARMS_PERSON` (`userid`),
  CONSTRAINT `FK_SCHEDULER_ALARMS_SCHEDULER` FOREIGN KEY (`scheduler_id`) REFERENCES scheduler_scheduler (`id`),
  CONSTRAINT `FK_SCHEDULER_ALARMS_PERSON` FOREIGN KEY (`userid`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler alarms';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_attendees`
--

CREATE TABLE `scheduler_attendees` (
  `scheduler_id` int(11) unsigned NOT NULL,
  `person_id` int(11) unsigned NOT NULL,
  `status` varchar(11)  NOT NULL,
  PRIMARY KEY  (`scheduler_id`,`person_id`),
  KEY `FK_SCHEDULER_ATTENDEES_SCHEDULER` (`scheduler_id`),
  KEY `FK_SCHEDULER_ATTENDEES_PERSON` (`person_id`),
  CONSTRAINT `FK_SCHEDULER_ATTENDEES_SCHEDULER` FOREIGN KEY (`scheduler_id`) REFERENCES scheduler_scheduler (`id`),
  CONSTRAINT `FK_SCHEDULER_ATTENDEES_PERSON` FOREIGN KEY (`person_id`) REFERENCES person (`id`)

) ENGINE=InnoDb DEFAULT CHARSET=utf8  COMMENT='Scheduler attendees';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_category`
--

CREATE TABLE `scheduler_category` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `description` varchar(50)  NOT NULL,
  `bgcolor` varchar(10)  NOT NULL,
  `fgcolor` varchar(10)  NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8  COMMENT='Scheduler category';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_cyclus`
--

CREATE TABLE `scheduler_cyclus` (
  `scheduler_id` int(11) unsigned NOT NULL,
  `monthly_day` int(2) default NULL,
  `yearly_day` int(2) default NULL,
  `yearly_month` int(2) default NULL,
  `yearly_month2` int(2) default NULL,
  `daily_every` int(11) unsigned DEFAULT NULL,
  `weekly_every` int(11) unsigned DEFAULT NULL,
  `monthly_every` int(11) unsigned DEFAULT NULL,
  `monthly_every2` int(11) unsigned DEFAULT NULL,
  `monthly_month_time` int(2) default NULL,
  `yearly_month_time` int(2) default NULL,
  `weekly_weekday` int(17) default NULL,
  `monthly_weekday_list` tinyint(1) unsigned default NULL,
  `yearly_weekday_list` tinyint(1) unsigned default NULL,
  `daily_choice` int(11) unsigned DEFAULT NULL,
  `monthly_choice` int(11) unsigned DEFAULT NULL,
  `yearly_choice` int(11) unsigned DEFAULT NULL,
  `end_choice` int(11) unsigned DEFAULT NULL,
  `cyclus_enddate` date default NULL,
  `cyclus_times` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY  (`scheduler_id`),
  KEY `FK_SCHEDULER_CYCLUS_SCHEDULER` (`scheduler_id`),
  CONSTRAINT `FK_SCHEDULER_CYCLUS_SCHEDULER` FOREIGN KEY (`scheduler_id`) REFERENCES scheduler_scheduler (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler cyclus';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_cyclus_not`
--

CREATE TABLE `scheduler_cyclus_not` (
  `scheduler_id` int(11) unsigned NOT NULL,
  `date` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`scheduler_id`,`date`),
  KEY `FK_SCHEDULER_CYCLUS_NOT_SCHEDULER` (`scheduler_id`),
  CONSTRAINT `FK_SCHEDULER_CYCLUS_NOT_SCHEDULER` FOREIGN KEY (`scheduler_id`) REFERENCES scheduler_scheduler (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler no cyclus';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_dates`
--

CREATE TABLE `scheduler_dates` (
  `scheduler_id` int(11) unsigned NOT NULL,
  `startdate` int(11) unsigned NOT NULL,
  `enddate` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY  (`scheduler_id`,`startdate`),
  KEY `IDX_ENDDATE` (`enddate`),
  KEY `FK_SCHEDULER_DATES_SCHEDULER` (`scheduler_id`),
  CONSTRAINT `FK_SCHEDULER_DATES_SCHEDULER` FOREIGN KEY (`scheduler_id`) REFERENCES scheduler_scheduler (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler dates';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_group`
--

CREATE TABLE `scheduler_group` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `userid` varchar(100)  default NULL,
  `name` varchar(100)  default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler group';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_group_member`
--

CREATE TABLE `scheduler_group_member` (
  `groupid` int(11) unsigned NOT NULL,
  `userid` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`groupid`,`userid`),
  KEY `FK_SCHEDULER_GROUP_MEMBER_GROUP` (`groupid`),
  KEY `FK_SCHEDULER_GROUP_MEMBER_PERSON` (`userid`),
  CONSTRAINT `FK_SCHEDULER_GROUP_MEMBER_GROUP` FOREIGN KEY (`groupid`) REFERENCES scheduler_group (`id`),
  CONSTRAINT `FK_SCHEDULER_GROUP_MEMBER_PERSON` FOREIGN KEY (`userid`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler group members';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_holidays`
--

CREATE TABLE `scheduler_holidays` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(100)  NOT NULL,
  `type` varchar(7)  default NULL,
  `day` int(2) default NULL,
  `month` int(2) default NULL,
  `special` varchar(6)  default NULL,
  `day_offset` int(3) default NULL,
  `length` int(2) default NULL,
  `weekday` tinyint(1) unsigned default NULL,
  `moveto` tinyint(1) unsigned default NULL,
  `schedulecategory` int(11) unsigned NOT NULL,
  `country` varchar(2)  NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_SCHEDULER_HOLIDAYS_CATEGORY` (`schedulecategory`),
  CONSTRAINT `FK_SCHEDULER_HOLIDAYS_CATEGORY` FOREIGN KEY (`schedulecategory`) REFERENCES scheduler_category(`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler holidays';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_notes`
--

CREATE TABLE `scheduler_notes` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `owner` int(11) unsigned NOT NULL,
  `scheduler_id` int(11) unsigned NOT NULL,
  `entrydate` date NOT NULL,
  `title` varchar(100)  NOT NULL,
  `description` text NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_SCHEDULER_NOTES_PERSON` (`owner`),
  KEY `FK_SCHEDULER_NOTES_SCHEDULER` (`scheduler_id`),
  CONSTRAINT `FK_SCHEDULER_NOTES_PERSON` FOREIGN KEY (`owner`) REFERENCES person (`id`),
  CONSTRAINT `FK_SCHEDULER_NOTES_SCHEDULER` FOREIGN KEY (`scheduler_id`) REFERENCES scheduler_scheduler (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler notes';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_scheduler`
--

CREATE TABLE `scheduler_scheduler` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `title` varchar(50)  NOT NULL,
  `location` varchar(50)  default NULL,
  `description` text,
  `recur` varchar(7)  default NULL,
  `startdate` date default NULL,
  `enddate` date default NULL,
  `allday` tinyint(1) unsigned default NULL,
  `starttime` time default NULL,
  `endtime` time default NULL,
  `category` int(11) unsigned NOT NULL,
  `all_users` tinyint(1) unsigned default NULL,
  `priority` tinyint(1) unsigned default NULL,
  `private` tinyint(1) unsigned default NULL,
  `nonblocking` tinyint(1) unsigned default NULL,
  `owner` int(11) unsigned NOT NULL,
  `owner_attendee` tinyint(1) unsigned default NULL,
  `lastdate` int(11) unsigned DEFAULT NULL,
  `times` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_SCHEDULER_PERSON` (`owner`),
  KEY `FK_SCHEDULER_CATEGORY` (`category`),
  CONSTRAINT `FK_SCHEDULER_PERSON` FOREIGN KEY (`owner`) REFERENCES person (`id`),
  CONSTRAINT `FK_SCHEDULER_CATEGORY` FOREIGN KEY (`category`) REFERENCES scheduler_category (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_userassistants`
--

CREATE TABLE `scheduler_userassistants` (
  `userid` int(11) unsigned NOT NULL,
  `employeeid` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`userid`,`employeeid`),
  KEY `FK_SCHEDULER_USERASSISTANTS_PERSON` (`userid`),
  KEY `FK_SCHEDULER_USERASSISTANT_PERSON2` (`employeeid`),
  CONSTRAINT `FK_SCHEDULER_USERASSISTANT_PERSON` FOREIGN KEY (`userid`) REFERENCES person (`id`),
  CONSTRAINT `FK_SCHEDULER_USERASSISTANT_PERSON2` FOREIGN KEY (`employeeid`) REFERENCES person (`id`)

) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler user assistents';

-- --------------------------------------------------------

--
-- Table structure for table `scheduler_userpreferences`
--

CREATE TABLE `scheduler_userpreferences` (
  `userid` int(11) unsigned NOT NULL,
  `timeschedule` int(2) NOT NULL,
  `showweeknumbers` tinyint(1) unsigned default NULL,
  `showtodo` tinyint(1) unsigned default NULL,
  `showemployeebirthdays` tinyint(1) unsigned default NULL,
  `showlunarphases` tinyint(1) unsigned default NULL,
  `autorefresh` tinyint(1) unsigned default NULL,
  `refresh_interval` int(11) unsigned DEFAULT NULL,
  `default_view` varchar(5)  NOT NULL,
  `default_userview` varchar(25)  NOT NULL,
  `default_eventtime` time default NULL,
  `default_category` int(11) unsigned DEFAULT NULL,
  `scheduler_emptycolor` varchar(10)  default NULL,
  `scheduler_emptyworkhourscolor` varchar(10)  default NULL,
  PRIMARY KEY  (`userid`),
  CONSTRAINT `FK_SCHEDULER_USERPREFERENCES_PERSON` FOREIGN KEY (`userid`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Scheduler employee preferences';

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
