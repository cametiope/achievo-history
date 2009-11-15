SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `email_template`
--

CREATE TABLE `email_template` (
  `code` varchar(100)  NOT NULL,
  `module` varchar(100)  default NULL,
  `title` varchar(100)  default NULL,
  `subject` varchar(100)  default NULL,
  `plain_body` text,
  `html_body` text,
  `lastupdatedon` datetime default NULL,
  `lastupdatedby` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY  (`code`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Email templates';

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
