SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `crm_campaign`
--

CREATE TABLE `crm_campaign` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `title` varchar(100) NOT NULL,
  `type` int(11) unsigned default NULL,
  `status` int(11) unsigned default NULL,
  `start_date` date default NULL,
  `end_date` date default NULL,
  `budget` decimal(12,2) default NULL,
  `actual_cost` decimal(12,2) default NULL,
  `expected_revenue` decimal(12,2) default NULL,
  `expected_cost` decimal(12,2) default NULL,
  `objective` text,
  `description` text,
  `created_by` int(11) unsigned default NULL,
  `created_on` datetime default NULL,
  `last_modified_by` int(11) unsigned default NULL,
  `last_modified_on` datetime default NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_CRM_CAMPAIGN_TYPE` (`type`),
  KEY `FK_CRM_CAMPAIGN_STATUS` (`status`),
  CONSTRAINT `FK_CRM_CAMPAIGN_TYPE` FOREIGN KEY (`type`) REFERENCES crm_campaign_type (`id`),
  CONSTRAINT `FK_CRM_CAMPAIGN_STATUS` FOREIGN KEY (`status`) REFERENCES crm_campaign_status (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM campaigns';

-- --------------------------------------------------------

--
-- Table structure for table `crm_campaign_status`
--

CREATE TABLE `crm_campaign_status` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `status_name` varchar(100) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM campaign status';

-- --------------------------------------------------------

--
-- Table structure for table `crm_campaign_type`
--

CREATE TABLE `crm_campaign_type` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `type_name` varchar(100) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM campaign type';

-- --------------------------------------------------------

--
-- Table structure for table `crm_eventlog`
--

CREATE TABLE `crm_eventlog` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `user_id` int(11) unsigned NOT NULL,
  `stamp` datetime NOT NULL,
  `node` varchar(100)  NOT NULL,
  `action` varchar(100)  NOT NULL,
  `primarykey` varchar(100)  NOT NULL,
  `summary` varchar(100)  NOT NULL,
  PRIMARY KEY  (`id`),
  KEY (`user_id`),
  CONSTRAINT `EVENTLOG_PERSON` FOREIGN KEY (`user_id`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM event log';

-- --------------------------------------------------------

--
-- Table structure for table `crm_former_name`
--

CREATE TABLE `crm_former_name` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `former_name` varchar(100)  NOT NULL,
  `account` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY  (`id`),
  KEY (`account`),
  CONSTRAINT `FORMER_NAME_ORGANIZATION` FOREIGN KEY (`account`) REFERENCES organization (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM organization former names';

-- --------------------------------------------------------

--
-- Table structure for table `crm_industry`
--

CREATE TABLE `crm_industry` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `industry_name` varchar(100)  NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM organization industry';

-- --------------------------------------------------------

--
-- Table structure for table `crm_lead`
--

CREATE TABLE `crm_lead` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `lead` int(11) unsigned DEFAULT NULL,
  `lead_description` text,
  `campaign` int(11) unsigned DEFAULT NULL,
  `status` varchar(9)  default NULL,
  `status_description` text,
  `title_id` int(11) unsigned DEFAULT NULL,
  `lastname` varchar(50)  NOT NULL,
  `firstname` varchar(50)  default NULL,
  `initials` varchar(50)  default NULL,
  `phone` varchar(20)  default NULL,
  `cellular` varchar(20)  default NULL,
  `fax` varchar(20)  default NULL,
  `email` varchar(50)  default NULL,
  `company` varchar(100)  default NULL,
  `address` varchar(100)  default NULL,
  `address2` varchar(100)  default NULL,
  `zipcode` varchar(20)  default NULL,
  `city` varchar(100)  default NULL,
  `state` varchar(100)  default NULL,
  `country` varchar(100)  default NULL,
  `website` varchar(100)  default NULL,
  `assigned_to` int(11) unsigned DEFAULT NULL,
  `remark` text,
  `created_by` int(11) unsigned DEFAULT NULL,
  `created_on` datetime default NULL,
  `last_modified_by` int(11) unsigned DEFAULT NULL,
  `last_modified_on` datetime default NULL,
  PRIMARY KEY  (`id`),
  KEY (`title_id`),
  CONSTRAINT `FK_LEAD_TITLE` FOREIGN KEY (`title_id`) REFERENCES title (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM leads';

-- --------------------------------------------------------

--
-- Table structure for table `crm_organization_relation`
--

CREATE TABLE `crm_organization_relation` (
  `relation_id` int(11) unsigned NOT NULL auto_increment,
  `account` int(11) unsigned default NULL,
  `relation_with` int(11) unsigned NOT NULL,
  `kind_of` int(11) unsigned NOT NULL,
  PRIMARY KEY  (`relation_id`),
  KEY (`account`),
  KEY (`relation_with`),
  KEY (`kind_of`),
  CONSTRAINT `FK_ORGANIZATION_RELATION_ORGANIZATION` FOREIGN KEY (`account`) REFERENCES organization (`id`),  
  CONSTRAINT `FK_ORGANIZATION_RELATION_ORGANIZATION2` FOREIGN KEY (`relation_with`) REFERENCES organization (`id`),  
  CONSTRAINT `FK_ORGANIZATION_RELATION_RELATION_TYPE` FOREIGN KEY (`kind_of`) REFERENCES crm_relation_type (`id`)  
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM organization relation';

-- --------------------------------------------------------

--
-- Table structure for table `crm_organization_status`
--

CREATE TABLE `crm_organization_status` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `status_name` varchar(100)  NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM organization status';

-- --------------------------------------------------------

--
-- Table structure for table `crm_rate`
--

CREATE TABLE `crm_rate` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `rate_name` varchar(100)  NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM rate';

-- --------------------------------------------------------

--
-- Table structure for table `crm_relation_type`
--

CREATE TABLE `crm_relation_type` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `type_name` varchar(100)  default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM relation type';

-- --------------------------------------------------------

--
-- Table structure for table `crm_source`
--

CREATE TABLE `crm_source` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `source_name` varchar(100)  NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='CRM source';

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
