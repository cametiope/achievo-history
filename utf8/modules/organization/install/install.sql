SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `organization`
--

CREATE TABLE `organization` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `organizationcode` varchar(100)  default NULL,
  `name` varchar(100)  NOT NULL,
  `type` varchar(8)  default NULL,
  `visit_address` varchar(100)  default NULL,
  `visit_address2` varchar(100)  default NULL,
  `visit_zipcode` varchar(20)  default NULL,
  `visit_city` varchar(100)  default NULL,
  `visit_state` varchar(100)  default NULL,
  `visit_country` varchar(100)  default NULL,
  `mail_address` varchar(100)  default NULL,
  `mail_address2` varchar(100)  default NULL,
  `mail_zipcode` varchar(20)  default NULL,
  `mail_city` varchar(100)  default NULL,
  `mail_state` varchar(100)  default NULL,
  `mail_country` varchar(100)  default NULL,
  `invoice_address` varchar(100)  default NULL,
  `invoice_address2` varchar(100)  default NULL,
  `invoice_zipcode` varchar(20)  default NULL,
  `invoice_city` varchar(100)  default NULL,
  `invoice_state` varchar(100)  default NULL,
  `invoice_country` varchar(100)  default NULL,
  `phone` varchar(20)  default NULL,
  `fax` varchar(20)  default NULL,
  `email` varchar(50)  default NULL,
  `website` varchar(100)  default NULL,
  `bankaccount` varchar(30)  default NULL,
  `bankaccount2` varchar(30)  default NULL,
  `bankaccount3` varchar(30)  default NULL,
  `bankaccount4` varchar(30)  default NULL,
  `vatnumber` varchar(25)  default NULL,
  `debtornumber` varchar(30)  default NULL,
  `member_of` int(11) unsigned DEFAULT NULL,
  `status` int(11) unsigned DEFAULT NULL,
  `source` int(11) unsigned DEFAULT NULL,
  `industry` int(11) unsigned DEFAULT NULL,
  `employees` int(5) default NULL,
  `revenue` varchar(5)  default NULL,
  `rate` int(11) unsigned DEFAULT NULL,
  `assigned_to` int(11) unsigned DEFAULT NULL,
  `remark` text,
  `created_by` int(11) unsigned DEFAULT NULL,
  `created_on` datetime default NULL,
  `last_modified_by` int(11) unsigned DEFAULT NULL,
  `last_modified_on` datetime default NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_ORGANIZATION_ORGANIZATION` (`member_of`),
  KEY `FK_ORGANIZATION_CRM_STATUS` (`status`),
  KEY `FK_ORGANIZATION_CRM_SOURCE` (`source`),
  KEY `FK_ORGANIZATION_CRM_INDUSTRY` (`industry`),
  KEY `FK_ORGANIZATION_CRM_RATE` (`rate`),
  KEY `FK_ORGANIZATION_PERSON` (`assigned_to`),
  CONSTRAINT `FK_ORGANIZATION_ORGANIZATION` FOREIGN KEY (`member_of`) REFERENCES organization (`id`),
  CONSTRAINT `FK_ORGANIZATION_CRM_ORGANIZATION_STATUS` FOREIGN KEY (`status`) REFERENCES crm_organization_status (`id`),
  CONSTRAINT `FK_ORGANIZATION_CRM_SOURCE` FOREIGN KEY (`source`) REFERENCES crm_source (`id`),
  CONSTRAINT `FK_ORGANIZATION_CRM_INDUSTRY` FOREIGN KEY (`industry`) REFERENCES crm_industry (`id`),
  CONSTRAINT `FK_ORGANIZATION_CRM_RATE` FOREIGN KEY (`rate`) REFERENCES crm_rate (`id`),
  CONSTRAINT `FK_ORGANIZATION_PERSON` FOREIGN KEY (`assigned_to`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Organizations';


--
-- Table structure for table `contract`
--

CREATE TABLE `contract` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `contractnumber` varchar(20) default NULL,
  `contractname` varchar(100) NOT NULL,
  `contracttype` int(11) unsigned NOT NULL,
  `customer` int(11) unsigned NOT NULL,
  `billing_type` varchar(8) default NULL,
  `billing_period` varchar(17) NOT NULL,
  `period_price` decimal(15,2) default NULL,
  `startdate` date NOT NULL default '0000-00-00',
  `enddate` date NOT NULL default '0000-00-00',
  `term_of_notice` date default NULL,
  `aftercalculation` tinyint(1) unsigned unsigned default 0,
  `priceperhour` decimal(12,2) default NULL,
  `status` varchar(8) NOT NULL,
  `description` text,
  PRIMARY KEY  (`id`),
  KEY `FK_CONTRACT_CONTRACTTYPE` (`contracttype`),
  CONSTRAINT `FK_CONTRACT_CONTRACTTYPE` FOREIGN KEY (`contracttype`) REFERENCES `contracttype`(`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Organization contract';


--
-- Table structure for table `contracttype`
--

CREATE TABLE `contracttype` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `description` varchar(100) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Organization contract type';

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;