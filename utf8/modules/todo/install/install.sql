SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `todo`
--

CREATE TABLE `todo` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `owner` int(11) unsigned NOT NULL,
  `projectid` int(11) unsigned DEFAULT NULL,
  `contactid` int(11) unsigned DEFAULT NULL,
  `title` varchar(50)  NOT NULL,
  `assigned_to` int(11) unsigned DEFAULT NULL,
  `entrydate` date NOT NULL,
  `duedate` date NOT NULL,
  `closedate` date default NULL,
  `updated` datetime default NULL,
  `priority` tinyint(1) unsigned NOT NULL,
  `completed` int(3) default NULL,
  `private` tinyint(1) unsigned default NULL,
  `description` text,
  `status` tinyint(1) unsigned NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_TODO_PERSON` (`owner`),
  KEY `FK_TODO_PROJECT` (`projectid`),
  KEY `FK_TODO_PERSON2` (`contactid`),
  KEY `FK_TODO_PERSON3` (`assigned_to`),
  CONSTRAINT `FK_TODO_PERSON` FOREIGN KEY (`owner`) REFERENCES person (`id`),
  CONSTRAINT `FK_TODO_PROJECT` FOREIGN KEY (`projectid`) REFERENCES project (`id`),
  CONSTRAINT `FK_TODO_PERSON2` FOREIGN KEY (`contactid`) REFERENCES person (`id`),
  CONSTRAINT `FK_TODO_PERSON3` FOREIGN KEY (`assigned_to`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Todos';

-- --------------------------------------------------------

--
-- Table structure for table `todo_history`
--

CREATE TABLE `todo_history` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `owner` int(11) unsigned NOT NULL,
  `projectid` int(11) unsigned DEFAULT NULL,
  `contactid` int(11) unsigned DEFAULT NULL,
  `title` varchar(50)  NOT NULL,
  `assigned_to` int(11) unsigned DEFAULT NULL,
  `entrydate` date NOT NULL,
  `duedate` date NOT NULL,
  `closedate` date default NULL,
  `priority` tinyint(1) unsigned NOT NULL,
  `completed` int(3) default NULL,
  `private` tinyint(1) unsigned default NULL,
  `description` text,
  `status` tinyint(1) unsigned NOT NULL,
  `todoid` int(11) unsigned NOT NULL,
  `updated` datetime default NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_TODO_HISTORY_PERSON` (`owner`),
  KEY `FK_TODO_HISTORY_PROJECT` (`projectid`),
  KEY `FK_TODO_HISTORY_PERSON2` (`contactid`),
  KEY `FK_TODO_HISTORY_PERSON3` (`assigned_to`),
  KEY `FK_TODO_HISTORY_TODO` (`todoid`),
  CONSTRAINT `FK_TODO_HISTORY_PERSON` FOREIGN KEY (`owner`) REFERENCES person (`id`),
  CONSTRAINT `FK_TODO_HISTORY_PROJECT` FOREIGN KEY (`projectid`) REFERENCES project (`id`),
  CONSTRAINT `FK_TODO_HISTORY_PERSON2` FOREIGN KEY (`contactid`) REFERENCES person (`id`),
  CONSTRAINT `FK_TODO_HISTORY_PERSON3` FOREIGN KEY (`assigned_to`) REFERENCES person (`id`),
  CONSTRAINT `FK_TODO_HISTORY_TODO` FOREIGN KEY (`todoid`) REFERENCES todo (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Todo history';
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
