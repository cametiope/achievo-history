SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `project_notes`
--

CREATE TABLE `project_notes` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `owner` int(11) unsigned NOT NULL,
  `projectid` int(11) unsigned NOT NULL,
  `title` varchar(50)  NOT NULL,
  `entrydate` date NOT NULL,
  `description` text NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_PROJECT_NOTES_PROJECT` (`projectid`),
  KEY `FK_PROJECT_NOTES_PERSON` (`owner`),
  CONSTRAINT `FK_PROJECT_NOTES_PROJECT` FOREIGN KEY (`projectid`) REFERENCES project (`id`),
  CONSTRAINT `FK_PROJECT_NOTES_PERSON` FOREIGN KEY (`owner`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Project notes';

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
