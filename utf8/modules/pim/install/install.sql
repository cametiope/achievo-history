SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
--
-- Table structure for table `userpimitems`
--

CREATE TABLE `userpimitems` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `userid` int(11) unsigned DEFAULT NULL,
  `pimitem` varchar(255)  NOT NULL,
  `orderby` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY  (`id`),
  KEY `FK_USERPIMITEMS_PERSON` (`userid`),
  CONSTRAINT `FK_USERPIMITEMS_PERSON` FOREIGN KEY (`userid`) REFERENCES person (`id`)
) ENGINE=InnoDb DEFAULT CHARSET=utf8 COMMENT='Employee pimitems';

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
