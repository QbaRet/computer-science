/*M!999999\- enable the sandbox mode */ 
-- MariaDB dump 10.19  Distrib 10.11.13-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: lab3
-- ------------------------------------------------------
-- Server version	10.11.13-MariaDB-0ubuntu0.24.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Ludzie`
--

DROP TABLE IF EXISTS `Ludzie`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `Ludzie` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `PESEL` char(11) NOT NULL,
  `imie` varchar(30) NOT NULL,
  `nazwisko` varchar(30) NOT NULL,
  `data_urodzenia` date DEFAULT NULL,
  `plec` enum('K','M') DEFAULT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `PESEL` (`PESEL`),
  KEY `idx_plec_imie` (`plec`,`imie`)
) ENGINE=InnoDB AUTO_INCREMENT=56 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Ludzie`
--

LOCK TABLES `Ludzie` WRITE;
/*!40000 ALTER TABLE `Ludzie` DISABLE KEYS */;
INSERT INTO `Ludzie` VALUES
(1,'20250352229','Jan','Kowalski','2020-01-01','M'),
(2,'20240988645','Anna','Nowak','2020-02-02','K'),
(3,'20210665727','Piotr','Wiśniewski','2020-03-03','M'),
(4,'20290314391','Zofia','Dąbrowska','2020-04-04','K'),
(5,'20241092938','Maria','Lewandowska','2020-05-05','K'),
(6,'97041719691','Adam','Nowak','1997-04-17','M'),
(7,'91091646426','Ewa','Kowalska','1991-09-16','K'),
(8,'96110957767','Piotr','Wiśniewski','1996-11-09','M'),
(9,'90012218753','Anna','Wójcik','1990-01-22','K'),
(10,'92112662672','Marek','Kowalczyk','1992-11-26','M'),
(11,'96123193569','Katarzyna','Kamińska','1996-12-31','K'),
(12,'99041958393','Tomasz','Lewandowski','1999-04-19','M'),
(13,'98112451423','Agnieszka','Zielińska','1998-11-24','K'),
(14,'96081447663','Paweł','Szymański','1996-08-14','M'),
(15,'98012855439','Monika','Woźniak','1998-01-28','K'),
(16,'97071114468','Łukasz','Dąbrowski','1997-07-11','M'),
(17,'90112354171','Joanna','Kozłowska','1990-11-23','K'),
(18,'98041188535','Michał','Jankowski','1998-04-11','M'),
(19,'90052587741','Magdalena','Mazur','1990-05-25','K'),
(20,'90102676254','Rafał','Krawczyk','1990-10-26','M'),
(21,'92072765217','Dorota','Piotrowska','1992-07-27','K'),
(22,'94070719854','Grzegorz','Grabowski','1994-07-07','M'),
(23,'95071995571','Sylwia','Pawlak','1995-07-19','K'),
(24,'90110243295','Krzysztof','Michalski','1990-11-02','M'),
(25,'94052393539','Beata','Nowicka','1994-05-23','K'),
(26,'90101783542','Wojciech','Adamczyk','1990-10-17','M'),
(27,'95122189245','Alicja','Dudek','1995-12-21','K'),
(28,'94062483824','Sebastian','Zawadzki','1994-06-24','M'),
(29,'93031773427','Natalia','Sikora','1993-03-17','K'),
(30,'94081265379','Patryk','Ostrowski','1994-08-12','M'),
(31,'93060393849','Karolina','Baran','1993-06-03','K'),
(32,'90052491895','Jakub','Szulc','1990-05-24','M'),
(33,'96011273346','Paulina','Włodarczyk','1996-01-12','K'),
(34,'99082898469','Mateusz','Chmielewski','1999-08-28','M'),
(35,'91050821969','Ewelina','Borkowska','1991-05-08','K'),
(36,'95040334912','Dariusz','Sokołowski','1995-04-03','M'),
(37,'98070141424','Izabela','Szczepańska','1998-07-01','K'),
(38,'96032435143','Artur','Sawicki','1996-03-24','M'),
(39,'91030219823','Agnieszka','Kucharska','1991-03-02','K'),
(40,'98030447397','Marcin','Lis','1998-03-04','M'),
(41,'92122491837','Edyta','Maciejewska','1992-12-24','K'),
(42,'95011717449','Bartłomiej','Kubiak','1995-01-17','M'),
(43,'98082088562','Justyna','Wilk','1998-08-20','K'),
(44,'93072016617','Adrian','Wysocki','1993-07-20','M'),
(45,'91032717619','Klaudia','Kaźmierczak','1991-03-27','K'),
(46,'94093092653','Radosław','Czarnecki','1994-09-30','M'),
(47,'91102544116','Aneta','Andrzejewska','1991-10-25','K'),
(48,'93102317912','Mariusz','Malinowski','1993-10-23','M'),
(49,'96100293475','Renata','Jaworska','1996-10-02','K'),
(50,'98122116679','Kamil','Głowacki','1998-12-21','M'),
(51,'59051417482','Zbigniew','Stępień','1960-01-01','M'),
(52,'56051581599','Helena','Kwiatkowska','1959-02-02','K'),
(53,'50071829279','Janusz','Wróbel','1958-03-03','M'),
(54,'52101913383','Barbara','Górska','1957-04-04','K'),
(55,'54030592833','Andrzej','Król','1956-05-05','M');
/*!40000 ALTER TABLE `Ludzie` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb3 */ ;
/*!50003 SET character_set_results = utf8mb3 */ ;
/*!50003 SET collation_connection  = utf8mb3_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER sprawdz_pesel_tylko_walidacja
BEFORE INSERT ON Ludzie
FOR EACH ROW
BEGIN
    IF czy_pesel_poprawny(NEW.PESEL) = 0 THEN
        SIGNAL SQLSTATE '45000' 
        SET MESSAGE_TEXT = 'Błąd: Nieprawidłowy numer PESEL (zła suma kontrolna).';
    END IF;
END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `Pracownicy`
--

DROP TABLE IF EXISTS `Pracownicy`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `Pracownicy` (
  `osoba_id` int(11) DEFAULT NULL,
  `zawod_id` int(11) DEFAULT NULL,
  `PESEL` char(11) DEFAULT NULL,
  `pensja` float DEFAULT NULL CHECK (`pensja` > 0),
  KEY `osoba_id` (`osoba_id`),
  KEY `zawod_id` (`zawod_id`),
  KEY `idx_pensja` (`pensja`),
  CONSTRAINT `Pracownicy_ibfk_1` FOREIGN KEY (`osoba_id`) REFERENCES `Ludzie` (`ID`),
  CONSTRAINT `Pracownicy_ibfk_2` FOREIGN KEY (`zawod_id`) REFERENCES `Zawody` (`zawod_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Pracownicy`
--

LOCK TABLES `Pracownicy` WRITE;
/*!40000 ALTER TABLE `Pracownicy` DISABLE KEYS */;
INSERT INTO `Pracownicy` VALUES
(6,1,'97041719691',5156.79),
(7,2,'91091646426',2656.64),
(8,3,'96110957767',12598.1),
(9,1,'90012218753',5417.74),
(10,1,'92112662672',3629.63),
(11,2,'96123193569',2010.09),
(12,3,'99041958393',13396.2),
(13,1,'98112451423',4355.29),
(14,2,'96081447663',2283.46),
(15,1,'98012855439',1656.49),
(16,3,'97071114468',17570.9),
(17,2,'90112354171',2060.7),
(18,2,'98041188535',2867.5),
(19,2,'90052587741',2271.7),
(20,2,'90102676254',2295.05),
(21,1,'92072765217',4243.1),
(22,2,'94070719854',2023.27),
(23,3,'95071995571',27502.4),
(24,1,'90110243295',2601.24),
(25,4,'94052393539',14619.3),
(26,3,'90101783542',14078.8),
(27,4,'95122189245',9177.44),
(28,2,'94062483824',2138.41),
(29,3,'93031773427',27804.2),
(30,4,'94081265379',11232.2),
(31,2,'93060393849',2968.57),
(32,1,'90052491895',9725.36),
(33,4,'96011273346',8577.55),
(34,4,'99082898469',8395.85),
(35,1,'91050821969',4428.77),
(36,1,'95040334912',8470.15),
(37,4,'98070141424',19247),
(38,1,'96032435143',8653.71),
(39,2,'91030219823',2284.05),
(40,4,'98030447397',16710.5),
(41,1,'92122491837',4183.35),
(42,4,'95011717449',12937.7),
(43,1,'98082088562',1635.67),
(44,2,'93072016617',2166.65),
(45,1,'91032717619',5734.06),
(46,3,'94093092653',16275.5),
(47,4,'91102544116',22930.3),
(48,2,'93102317912',2367.16),
(49,3,'96100293475',18932.7),
(50,2,'98122116679',2402.64),
(51,4,'59051417482',18393.1),
(52,4,'56051581599',14167),
(53,2,'50071829279',2391.06),
(54,2,'52101913383',2510.42),
(55,1,'54030592833',6335.26);
/*!40000 ALTER TABLE `Pracownicy` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Zawody`
--

DROP TABLE IF EXISTS `Zawody`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `Zawody` (
  `zawod_id` int(11) NOT NULL AUTO_INCREMENT,
  `nazwa` varchar(50) NOT NULL,
  `pensja_min` float DEFAULT NULL CHECK (`pensja_min` > 0),
  `pensja_max` float DEFAULT NULL CHECK (`pensja_max` > 0),
  PRIMARY KEY (`zawod_id`),
  CONSTRAINT `CONSTRAINT_1` CHECK (`pensja_min` < `pensja_max`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Zawody`
--

LOCK TABLES `Zawody` WRITE;
/*!40000 ALTER TABLE `Zawody` DISABLE KEYS */;
INSERT INTO `Zawody` VALUES
(1,'Polityk',1000,10000),
(2,'Nauczyciel',2000,3000),
(3,'Lekarz',10000,28000),
(4,'Informatyk',6000,35000);
/*!40000 ALTER TABLE `Zawody` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-12-13 22:16:56
