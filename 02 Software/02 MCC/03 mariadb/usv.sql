/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

CREATE DATABASE IF NOT EXISTS `usv` /*!40100 DEFAULT CHARACTER SET latin1 COLLATE latin1_german1_ci */;
USE `usv`;

CREATE TABLE IF NOT EXISTS `as1` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Schub',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Schub';

REPLACE INTO `as1` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:29', 0),
	(2, '2024-02-06 16:14:29', 0),
	(3, '2024-02-06 16:16:38', 0);

CREATE TABLE IF NOT EXISTS `as2` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Ruder',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Ruder';

REPLACE INTO `as2` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:30', -0.28),
	(2, '2024-02-06 16:14:30', -0.28),
	(3, '2024-02-06 16:16:39', -0.28);

CREATE TABLE IF NOT EXISTS `em1` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Spannung 1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Spannung Akku 1';

REPLACE INTO `em1` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:31', 24.2),
	(2, '2024-02-06 16:14:31', 24.2),
	(3, '2024-02-06 16:16:40', 24.2);

CREATE TABLE IF NOT EXISTS `em2` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Strom 1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Strom Akku 1';

REPLACE INTO `em2` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:32', 55.4),
	(2, '2024-02-06 16:14:32', 55.4),
	(3, '2024-02-06 16:16:41', 55.4);

CREATE TABLE IF NOT EXISTS `em3` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Kapazität 1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Kapazität Akku 1';

REPLACE INTO `em3` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:33', 55),
	(2, '2024-02-06 16:14:33', 55),
	(3, '2024-02-06 16:16:42', 55);

CREATE TABLE IF NOT EXISTS `em4` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Spannung 2',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Spannung Akku 2';

REPLACE INTO `em4` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:34', 19.3),
	(2, '2024-02-06 16:14:34', 19.3),
	(3, '2024-02-06 16:16:43', 19.3);

CREATE TABLE IF NOT EXISTS `em5` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Strom 2',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Strom Akku 2';

REPLACE INTO `em5` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:35', 51.6),
	(2, '2024-02-06 16:14:35', 51.6),
	(3, '2024-02-06 16:16:44', 51.6);

CREATE TABLE IF NOT EXISTS `em6` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Kapazität 2',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Kapazität Akku 2';

REPLACE INTO `em6` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:36', 52),
	(2, '2024-02-06 16:14:36', 52),
	(3, '2024-02-06 16:16:45', 52);

CREATE TABLE IF NOT EXISTS `em7` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'U Solar',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Spannung Solaranlage';

REPLACE INTO `em7` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:37', 24.6),
	(2, '2024-02-06 16:14:37', 24.6),
	(3, '2024-02-06 16:16:46', 24.6);

CREATE TABLE IF NOT EXISTS `em8` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Bordtemperatur',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Bordtemperatur';

REPLACE INTO `em8` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:39', 18.6),
	(2, '2024-02-06 16:14:39', 18.6),
	(3, '2024-02-06 16:16:48', 18.6);

CREATE TABLE IF NOT EXISTS `em9` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Spannung Lidar',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Spannung Lidar';

REPLACE INTO `em9` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:38', 24.1),
	(2, '2024-02-06 16:14:38', 24.1),
	(3, '2024-02-06 16:16:47', 24.1);

CREATE TABLE IF NOT EXISTS `gps` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `Time` timestamp NULL DEFAULT current_timestamp(),
  `Latitude` float DEFAULT NULL,
  `Longitude` float DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='GPS-Koordinaten';

REPLACE INTO `gps` (`ID`, `Time`, `Latitude`, `Longitude`) VALUES
	(1, '2024-02-06 16:14:24', 54.2805, 13.7089),
	(2, '2024-02-06 16:14:24', 54.2805, 13.7089),
	(3, '2024-02-06 16:16:33', 54.2805, 13.7089);

CREATE TABLE IF NOT EXISTS `sb1` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float DEFAULT NULL COMMENT 'Error-Byte',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='ERROR-Byte';

REPLACE INTO `sb1` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:40', 11),
	(2, '2024-02-06 16:14:40', 11),
	(3, '2024-02-06 16:16:49', 11);

CREATE TABLE IF NOT EXISTS `sb2` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Längengrad';

REPLACE INTO `sb2` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:26', 13.7089),
	(2, '2024-02-06 16:14:26', 13.7089),
	(3, '2024-02-06 16:16:35', 13.7089);

CREATE TABLE IF NOT EXISTS `sb3` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `Time` timestamp NULL DEFAULT current_timestamp() COMMENT 'Timestamp',
  `Messung` float NOT NULL COMMENT 'Breitengrad',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Breitengrad';

REPLACE INTO `sb3` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:25', 54.2805),
	(2, '2024-02-06 16:14:25', 54.2805),
	(3, '2024-02-06 16:16:34', 54.2805);

CREATE TABLE IF NOT EXISTS `sb5` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `Time` timestamp NULL DEFAULT current_timestamp(),
  `Messung` float DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Geschwindigkeit';

REPLACE INTO `sb5` (`ID`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:27', 11.1),
	(2, '2024-02-06 16:14:27', 11.1),
	(3, '2024-02-06 16:16:36', 11.1);

CREATE TABLE IF NOT EXISTS `sb6` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `Time` timestamp NULL DEFAULT current_timestamp(),
  `Messung` float DEFAULT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Kurswinkel';

REPLACE INTO `sb6` (`Id`, `Time`, `Messung`) VALUES
	(1, '2024-02-06 16:14:28', 293),
	(2, '2024-02-06 16:14:28', 293),
	(3, '2024-02-06 16:16:37', 293);

/*!40103 SET TIME_ZONE=IFNULL(@OLD_TIME_ZONE, 'system') */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
