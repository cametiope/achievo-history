SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `atk_searchcriteria`
--

CREATE TABLE `atk_searchcriteria` (
  `nodetype` varchar(100) NOT NULL,
  `name` varchar(100) NOT NULL,
  `criteria` text NOT NULL,
  `handlertype` varchar(100) default NULL,
  `userid` int(11) unsigned default NULL,
  `is_for_all` tinyint(1) unsigned unsigned default 0,
  PRIMARY KEY  (`nodetype`,`name`),
  KEY `FK_ATK_SEARCHCRITERIA_USER` (`userid`),
  CONSTRAINT `FK_ATK_SEARCHCRITERIA_USER` FOREIGN KEY (`userid`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='User searchcriteria';


--
-- Table structure for table `db_sequence`
--

CREATE TABLE `db_sequence` (
  `seq_name` varchar(50)  NOT NULL,
  `nextid` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`seq_name`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='ATK DB Sequence';



--
-- Table structure for table `versioninfo`
--

CREATE TABLE `versioninfo` (
  `module` varchar(50)  NOT NULL,
  `version` varchar(15)  default NULL,
  PRIMARY KEY  (`module`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Setup version information';

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;