SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `docmanager_document`
--

CREATE TABLE `docmanager_document` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `documentcode` varchar(100)  default NULL,
  `name` varchar(100)  NOT NULL,
  `filename` varchar(255)  default NULL,
  `owner` varchar(100)  default NULL,
  `master` varchar(100)  default NULL,
  `master_id` int(11) unsigned DEFAULT NULL,
  `entrydate` date default NULL,
  `version` varchar(100)  default NULL,
  `status` varchar(8)  default NULL,
  `confidential` tinyint(1) unsigned default NULL,
  `internal` tinyint(1) unsigned default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Docmanager document';

-- --------------------------------------------------------

--
-- Table structure for table `docmanager_documenttype`
--

CREATE TABLE `docmanager_documenttype` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(100)  NOT NULL,
  `template` varchar(255)  default NULL,
  `master` varchar(255)  NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Docmanager document type';

-- --------------------------------------------------------

--
-- Table structure for table `docmanager_prjtpl_dt`
--

CREATE TABLE `docmanager_prjtpl_dt` (
  `projecttpl_id` int(11) unsigned NOT NULL ,
  `documenttype_id` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`projecttpl_id`,`documenttype_id`),
  KEY `FK_DOCMANAGER_PRJTPL_DT_PROJECTTPL` (`projecttpl_id`),
  KEY `FK_DOCMANAGER_PRJTPL_DT_DOCTYPE` (`documenttype_id`),
  CONSTRAINT `FK_DOCMANAGER_PRJTPL_DT_PROJECTTPL` FOREIGN KEY (`projecttpl_id`) REFERENCES tpl_project (`id`),
  CONSTRAINT `FK_DOCMANAGER_PRJTPL_DT_DOCTYPE` FOREIGN KEY (`documenttype_id`) REFERENCES docmanager_documenttype (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8  COMMENT='Project template document type';

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
