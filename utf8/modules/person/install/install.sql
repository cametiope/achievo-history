
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `person`
--

CREATE TABLE `person` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `title_id` int(11) unsigned DEFAULT NULL,
  `userid` varchar(15)  default NULL,
  `lastname` varchar(50)  NOT NULL,
  `firstname` varchar(50)  default NULL,
  `initials` varchar(50)  default NULL,
  `address` varchar(100)  default NULL,
  `zipcode` varchar(20)  default NULL,
  `city` varchar(100)  default NULL,
  `state` varchar(100)  default NULL,
  `country` varchar(100)  default NULL,
  `phone` varchar(20)  default NULL,
  `cellular` varchar(20)  default NULL,
  `fax` varchar(20)  default NULL,
  `email` varchar(50)  default NULL,
  `function` varchar(50)  default NULL,
  `remark` text,
  `role` varchar(15)  NOT NULL,
  `created_by` int(11) unsigned DEFAULT NULL,
  `created_on` datetime default NULL,
  `last_modified_by` int(11) unsigned DEFAULT NULL,
  `last_modified_on` datetime default NULL,
  `birthdate` date default NULL,
  `bankaccount` text,
  `socialsecuritynumber` text,
  `employer_id` int(11) unsigned DEFAULT NULL,
  `department` int(11) unsigned DEFAULT NULL,
  `supervisor` int(11) unsigned DEFAULT NULL,
  `functionlevel` int(11) unsigned DEFAULT NULL,
  `status` varchar(9)  default NULL,
  `lng` varchar(2)  default NULL,
  `password` varchar(40)  default NULL,
  `theme` varchar(14)  default NULL,
  `company` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_PERSON_TITLE` (`title_id`),
  KEY `IDX_PERSON_ROLE` (`role`),
  KEY `FK_PERSON_ORGANIZATION` (`employer_id`),
  KEY `FK_PERSON_FUNCTIONLEVEL` (`functionlevel`),
  KEY `FK_PERSON_EMPLOYEE_DEPARTMENT` (`department`),
  CONSTRAINT `FK_PERSON_TITLE` FOREIGN KEY (`title_id`) REFERENCES title (`id`),
  CONSTRAINT `FK_PERSON_ORGANIZATION` FOREIGN KEY (`employer_id`) REFERENCES organization (`id`),
  CONSTRAINT `FK_PERSON_FUNCTIONLEVEL` FOREIGN KEY (`functionlevel`) REFERENCES functionlevel (`id`),
  CONSTRAINT `FK_PERSON_EMPLOYEE_DEPARTMENT` FOREIGN KEY (`department`) REFERENCES employee_department (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Employees and Organization contacts';

--
-- Table structure for table `title`
--

CREATE TABLE `title` (
  `id` int(11) unsigned NOT NULL,
  `title` varchar(25)  default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Titles';

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;

