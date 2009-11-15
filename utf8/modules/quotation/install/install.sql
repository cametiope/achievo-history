SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `quotation_payment`
--

CREATE TABLE `quotation_payment` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `type` varchar(100)  NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Quotation payment type';

-- --------------------------------------------------------

--
-- Table structure for table `quotation_quotation`
--

CREATE TABLE `quotation_quotation` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `quotation_nr` varchar(10)  NOT NULL,
  `organization_id` int(11) unsigned NOT NULL,
  `contact` varchar(50)  NOT NULL,
  `method` varchar(5)  default NULL,
  `title` varchar(100)  NOT NULL,
  `description` text,
  `issue_date` date NOT NULL,
  `expire_date` date default NULL,
  `status` varchar(16)  default NULL,
  `assigned_to` int(11) unsigned NOT NULL,
  `price` decimal(15,2) NOT NULL,
  `payment_id` int(11) unsigned DEFAULT NULL,
  `success_chance` int(3) default NULL,
  `profit_expectance` decimal(12,2) default NULL,
  `campaign` int(11) unsigned DEFAULT NULL,
  `source` int(11) unsigned DEFAULT NULL,
  `created_by` int(11) unsigned DEFAULT NULL,
  `created_on` datetime default NULL,
  `last_modified_by` int(11) unsigned DEFAULT NULL,
  `last_modified_on` datetime default NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_QUOTATION_ORGANIZATION` (`organization_id`),
  KEY `FK_QUOTATION_PERSON` (`assigned_to`),
  KEY `FK_QUOTATION_PAYMENT` (`payment_id`),
  KEY `FK_QUOTATION_CAMPAIGN` (`campaign`),
  KEY `FK_QUOTATION_SOURCE` (`source`),
  CONSTRAINT `FK_QUOTATION_ORGANIZATION` FOREIGN KEY (`organization_id`) REFERENCES organization (`id`),
  CONSTRAINT `FK_QUOTATION_PERSON` FOREIGN KEY (`assigned_to`) REFERENCES person (`id`),
  CONSTRAINT `FK_QUOTATION_PAYMENT` FOREIGN KEY (`payment_id`) REFERENCES quotation_payment (`id`),
  CONSTRAINT `FK_QUOTATION_CAMPAIGN` FOREIGN KEY (`campaign`) REFERENCES crm_campaign (`id`),
  CONSTRAINT `FK_QUOTATION_SOURCE` FOREIGN KEY (`source`) REFERENCES crm_source (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Quotations';

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
