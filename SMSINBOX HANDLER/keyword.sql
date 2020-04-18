/*
SQLyog Enterprise - MySQL GUI v8.14 
MySQL - 5.7.25 : Database - smsgw_new
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`smsgw_new` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `smsgw_new`;

/*Table structure for table `keyword` */

DROP TABLE IF EXISTS `keyword`;

CREATE TABLE `keyword` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `Keyword` varchar(30) NOT NULL,
  `SMSText` text,
  `SrcType` enum('SMS','URL') NOT NULL DEFAULT 'SMS',
  `URL` text,
  `HitCount` bigint(20) unsigned NOT NULL DEFAULT '0',
  `LastHit` datetime DEFAULT NULL,
  `ShortCode` varchar(20) NOT NULL,
  `Status` enum('Active','Inactive') NOT NULL DEFAULT 'Active',
  `AccountID` varchar(50) DEFAULT NULL,
  `ActivationStart` datetime DEFAULT NULL,
  `ActivationEnd` datetime DEFAULT NULL,
  `ServiceID` varchar(50) DEFAULT NULL,
  `IsSingleLevelService` enum('0','1') NOT NULL DEFAULT '0',
  PRIMARY KEY (`Keyword`,`ShortCode`),
  KEY `FK_keyword` (`ShortCode`),
  KEY `id` (`id`),
  CONSTRAINT `FK_keyword` FOREIGN KEY (`ShortCode`) REFERENCES `shortcode` (`shortcode`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=latin1;

/*Data for the table `keyword` */

insert  into `keyword`(`id`,`Keyword`,`SMSText`,`SrcType`,`URL`,`HitCount`,`LastHit`,`ShortCode`,`Status`,`AccountID`,`ActivationStart`,`ActivationEnd`,`ServiceID`,`IsSingleLevelService`) values (15,'BATAL','','URL','http://localhost/SMSServices_4_0/core/service.php?mn=%mn&msg=%msg&op=%op&sc=%sc&tid=%tid&retry=%retry',6073,'2013-10-01 00:03:36','2000','Active','SSD-Tech','2020-01-26 23:16:26','2040-09-09 11:00:00','','0'),(12,'ON','','URL','http://localhost/SMSServices_4_0/core/service.php?mn=%mn&msg=%msg&op=%op&sc=%sc&tid=%tid&retry=%retry',3898,'2013-09-30 14:07:06','20000','Active','SSD-Tech','2020-01-26 23:16:22','2040-09-09 11:00:00','BA-Weekly','0'),(17,'STOP',NULL,'URL','http://localhost/SMSServices_4_0/core/service.php?mn=%mn&msg=%msg&op=%op&sc=%sc&tid=%tid&retry=%retry',0,'2020-02-02 17:01:24','20000','Active','SSD-Tech','2020-02-02 17:01:38','2050-02-02 17:01:40','BA-Weekly_STOP','0');

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
