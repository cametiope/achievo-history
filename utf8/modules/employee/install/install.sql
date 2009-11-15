SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `employeerole`
--

CREATE TABLE `employeerole` (
  `employee_id` int(11) unsigned NOT NULL,
  `role_id` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`employee_id`,`role_id`),
  CONSTRAINT `FK_EMPLOYEEROLE_PERSON` FOREIGN KEY (`employee_id`) REFERENCES person (`id`),
  CONSTRAINT `FK_EMPLOYEEROLE_PROFILE` FOREIGN KEY (`role_id`) REFERENCES profile (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Employee roles';

-- --------------------------------------------------------

--
-- Table structure for table `employee_department`
--

CREATE TABLE `employee_department` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(100)  NOT NULL,
  `description` text,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Employee department';

--
-- Table structure for table `accessright`
--
CREATE TABLE `accessright` (
  `node` varchar(100) NOT NULL,
  `action` varchar(25) NOT NULL,
  `role_id` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`node`,`action`,`role_id`),
  KEY `FK_ACCESSRIGHT_ROLE` (`role_id`),
  CONSTRAINT `FK_ACCESSRIGHT_ROLE` FOREIGN KEY (`role_id`) REFERENCES role (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Employee accessrights';



--
-- Table structure for table `functionlevel`
--

CREATE TABLE `functionlevel` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(50)  NOT NULL,
  `description` text,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Function level';

--
-- Table structure for table `profile`
--

CREATE TABLE `profile` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(50)  NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Security profiles';


--
-- Table structure for table `usercontract`
--

CREATE TABLE `usercontract` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `userid` int(11) unsigned NOT NULL,
  `uc_hours` decimal(6,2) NOT NULL,
  `startdate` date NOT NULL,
  `enddate` date default NULL,
  `description` text,
  `workingdays` text,
  `workstarttime` time NOT NULL,
  `workendtime` time NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_USERCONTRACT_PERSON` (`userid`),
  CONSTRAINT `FK_USERCONTRACT_PERSON` FOREIGN KEY (`userid`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Employee contract';
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;