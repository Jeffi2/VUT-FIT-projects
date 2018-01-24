
-- login: 1 = admin, 2 = zamestnanec, 3 = dodavatel
CREATE TABLE users(
  login varchar(100) PRIMARY KEY,
  type int,
  can_log int,
  heslo varchar(100)
) ENGINE=InnoDB CHARSET=utf8;

CREATE TABLE zamestnanec(
  id_zam int NOT NULL AUTO_INCREMENT,
  jmeno varchar(50) NOT NULL,
  prijmeni varchar(50) NOT NULL,
  telefon numeric(9,0),
  login varchar(100) NOT NULL,
  plat int NOT NULL,
  ulice varchar(100),
  mesto varchar(100),
  psc int,
  cislo_popisne int,
  PRIMARY KEY(id_zam)
) ENGINE=InnoDB CHARSET=utf8;

CREATE TABLE bochnik(
  id_boch int NOT NULL AUTO_INCREMENT,
  pocatecni_hmotnost float(11) NOT NULL ,
  aktualni_hmotnost float(11) NOT NULL ,
  datum_dodani DATETIME NOT NULL,
  trvanlivost DATETIME NOT NULL ,
  id_pro int NOT NULL,
  id_syr int NOT NULL,
  id_obj int NOT NULL,
  PRIMARY KEY(id_boch)
  ) ENGINE=InnoDB CHARSET=utf8;

CREATE TABLE prodejna(
  id_pro int NOT NULL AUTO_INCREMENT,
  nazev varchar(50) NOT NULL,
  ulice varchar(20),
  mesto varchar(30),
  psc int,
  cislo_popisne int ,
  PRIMARY KEY(id_pro)
) ENGINE=InnoDB CHARSET=utf8;

CREATE TABLE zeme(
  id_zem int NOT NULL AUTO_INCREMENT,
  nazev varchar(50) NOT NULL,
  rozloha int,
  export_syru int,
  PRIMARY KEY(id_zem)
) ENGINE=InnoDB CHARSET=utf8;

CREATE TABLE syr(
  id_syr int NOT NULL AUTO_INCREMENT PRIMARY KEY,
  nazev varchar(50) NOT NULL,
  zivocich varchar(40) NOT NULL,
  tuk int,
  typ varchar(40),
  syr_trvanlivost int NOT NULL,
  id_zem int NOT NULL
) ENGINE=InnoDB CHARSET=utf8;

/*
zaplacena -> 1=ano, 0=ne
druh_platby -> 1=faktura, 0=hotove
*/
CREATE TABLE objednavka(
  id_obj int NOT NULL AUTO_INCREMENT PRIMARY KEY,
  celkova_cena int,
  datum_vytvoreni DATETIME DEFAULT CURRENT_TIMESTAMP,
  dodano int(1),
  ico decimal(12,0) NOT NULL,
  id_zam int NOT NULL,
  id_pro int NOT NULL,
  zaplacena int DEFAULT 0,
  datum_platby DATETIME,
  druh_platby int DEFAULT 1
) ENGINE=InnoDB CHARSET=utf8;

CREATE TABLE obsah(
  id_obs int NOT NULL AUTO_INCREMENT PRIMARY KEY,
  id_obj int NOT NULL,
  id_syr int NOT NULL,
  hmotnost float(11)
) ENGINE=InnoDB CHARSET=utf8;

create table dodavatel(
  ico decimal(12,0) PRIMARY KEY NOT NULL,
  nazev varchar(100) NOT NULL,
  ulice varchar(100),
  cislo_popisne int DEFAULT NULL,
  mesto varchar(100),
  psc int NOT NULL,
  cislo_uctu varchar(100) NOT NULL,
  telefon numeric(9,0) DEFAULT NULL,
  login varchar(100) NOT NULL,
  id_zem int
  ) ENGINE=InnoDB CHARSET=utf8;

CREATE TABLE nabidka (
  ico decimal(12,0) NOT NULL,
  id_syr int NOT NULL,
  cena int NOT NULL
  ) ENGINE=InnoDB CHARSET=utf8;

CREATE TABLE pracuje (
  id_zam int NOT NULL,
  id_pro int NOT NULL
  ) ENGINE=InnoDB CHARSET=utf8;


ALTER TABLE syr ADD CONSTRAINT FK_syr_zem FOREIGN KEY(id_zem) REFERENCES zeme(id_zem) ON DELETE CASCADE;
ALTER TABLE dodavatel ADD CONSTRAINT FK_dodavatel_zem FOREIGN KEY(id_zem) REFERENCES zeme(id_zem) ON DELETE CASCADE;
ALTER TABLE nabidka ADD CONSTRAINT FK_nabidka_dod FOREIGN KEY(ico) REFERENCES dodavatel(ico) ;
ALTER TABLE nabidka ADD CONSTRAINT FK_nabidka_syr FOREIGN KEY(id_syr) REFERENCES syr(id_syr);
ALTER TABLE nabidka ADD CONSTRAINT PK_nabidka PRIMARY KEY(id_syr, ico, cena);
ALTER TABLE pracuje ADD CONSTRAINT PK_pracuje PRIMARY KEY(id_zam, id_pro);
ALTER TABLE pracuje ADD CONSTRAINT FK_pracuje_zam FOREIGN KEY(id_zam) REFERENCES zamestnanec(id_zam)ON DELETE CASCADE ;
ALTER TABLE pracuje ADD CONSTRAINT FK_pracuje_pro FOREIGN KEY(id_pro) REFERENCES prodejna(id_pro)ON DELETE CASCADE ;
ALTER TABLE objednavka ADD CONSTRAINT FK_objednavka_zam FOREIGN KEY(id_zam) REFERENCES zamestnanec(id_zam)ON DELETE CASCADE ;
ALTER TABLE objednavka ADD CONSTRAINT FK_objednavka_dod FOREIGN KEY(ico) REFERENCES dodavatel(ico) ON DELETE CASCADE;
ALTER TABLE objednavka ADD CONSTRAINT FK_objednavka_pro FOREIGN KEY(id_pro) REFERENCES prodejna(id_pro) ON DELETE CASCADE ;
ALTER TABLE obsah ADD CONSTRAINT FK_obsah_obj FOREIGN KEY(id_obj) REFERENCES objednavka(id_obj) ON DELETE CASCADE;
ALTER TABLE obsah ADD CONSTRAINT FK_obsah_syr FOREIGN KEY(id_syr) REFERENCES syr(id_syr) ON DELETE CASCADE;
ALTER TABLE bochnik ADD CONSTRAINT FK_bochnik_syr FOREIGN KEY(id_syr) REFERENCES syr(id_syr);
ALTER TABLE bochnik ADD CONSTRAINT FK_bochnik_obj FOREIGN KEY(id_obj) REFERENCES objednavka(id_obj) ;
ALTER TABLE bochnik ADD CONSTRAINT FK_bochnik_pro FOREIGN KEY(id_pro) REFERENCES prodejna(id_pro);
ALTER TABLE zamestnanec ADD CONSTRAINT FK_login_zam FOREIGN KEY(login) REFERENCES users(login) ON DELETE CASCADE;
ALTER TABLE dodavatel ADD CONSTRAINT FK_login_dod FOREIGN KEY(login) REFERENCES users(login) ON DELETE CASCADE;

INSERT INTO users (login, type, heslo, can_log) VALUES ('a', 1, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);
INSERT INTO users (login, type, heslo, can_log) VALUES ('admin@a.cz', 1, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);
INSERT INTO users (login, type, heslo, can_log) VALUES ('a@a.cz', 1, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);
INSERT INTO users (login, type, heslo, can_log) VALUES ('root@localhost.cz', 1, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);

INSERT INTO users (login, type, heslo, can_log) VALUES ('tomas@vomacka.cz', 2, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K', 1);
INSERT INTO users (login, type, heslo, can_log) VALUES ('martin@leonov.cz', 2, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);
INSERT INTO users (login, type, heslo, can_log) VALUES ('jan@musil.cz', 2, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);
INSERT INTO users (login, type, heslo, can_log) VALUES ('franta@velky.com', 2, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',0);
INSERT INTO users (login, type, heslo, can_log) VALUES ('z@z.cz', 2, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);
INSERT INTO users (login, type, heslo, can_log) VALUES ('z', 2, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);

INSERT INTO users (login, type, heslo, can_log) VALUES ('syr@agrofert.sk', 3, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);
INSERT INTO users (login, type, heslo, can_log) VALUES ('d@d.cz', 3, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);
INSERT INTO users (login, type, heslo, can_log) VALUES ('dod@albert.cz', 3, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);
INSERT INTO users (login, type, heslo, can_log) VALUES ('d@lidl.com', 3, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',0);
INSERT INTO users (login, type, heslo, can_log) VALUES ('d', 3, '$2y$10$6ka4hQXe5w8pGkOlXeLFbepcvmAQfYcqfCXMplWnrfmJvK/we3Y5K',1);


INSERT INTO prodejna (nazev, ulice, mesto, psc, cislo_popisne) VALUES ('Praha střed', 'Apoštolská', 'Praha', 10200, 1);
INSERT INTO prodejna (nazev, ulice, mesto, psc, cislo_popisne) VALUES ('Praha západ', 'Temná', 'Praha', 10100, 13);
INSERT INTO prodejna (nazev, ulice, mesto, psc, cislo_popisne) VALUES ('Brno střed', 'Kozí', 'Brno', 60200, 9);
INSERT INTO prodejna (nazev, ulice, mesto, psc, cislo_popisne) VALUES ('Brno jih', 'Komárov', 'Brno', 61700, 103);
INSERT INTO prodejna (nazev, ulice, mesto, psc, cislo_popisne) VALUES ('Ostrava', 'Krásného', 'Ostrava', 70030, 65);

INSERT INTO zamestnanec (jmeno,
  prijmeni, telefon, login, plat, ulice, mesto, psc, cislo_popisne) VALUES ('Tomáš', 'Vomáčka', 777666555, 'tomas@vomacka.cz', 20000, 'Vaclavska', 'Brno', 60200, 32);
INSERT INTO zamestnanec (jmeno,
  prijmeni, telefon, login, plat, ulice, mesto, psc, cislo_popisne) VALUES ('Martin', 'Leonov', 603543234, 'martin@leonov.cz',  18000, 'Novakova', 'Brno', 73412, 87);
INSERT INTO zamestnanec (jmeno,
  prijmeni, telefon, login, plat, ulice, mesto, psc, cislo_popisne) VALUES ('Jan', 'Musil', 765123744, 'jan@musil.cz', 16500, 'Pirátská', 'Praha', 16020, 26);
INSERT INTO zamestnanec (jmeno,
  prijmeni, telefon, login, plat, ulice, mesto, psc, cislo_popisne) VALUES ('František', 'Velký', 566234764, 'franta@velky.com', 21000, 'Kroucená', 'Ostrava', 70031, 65);
INSERT INTO zamestnanec (jmeno,
  prijmeni, telefon, login, plat, ulice, mesto, psc, cislo_popisne) VALUES ('Zamestnanec', 'Roku', 654234654, 'z@z.cz', 17500, 'Stará', 'Hradec Králové', 93812, 1);
INSERT INTO zamestnanec (jmeno,
  prijmeni, telefon, login, plat, ulice, mesto, psc, cislo_popisne) VALUES ('Petr', 'Veselý', 724654233, 'z', 26500, 'Smutná', 'Pardubice', 46534, 17);

INSERT INTO pracuje (id_zam, id_pro) VALUES (1,3);
INSERT INTO pracuje (id_zam, id_pro) VALUES (1,4);

INSERT INTO pracuje (id_zam, id_pro) VALUES (2,3);

INSERT INTO pracuje (id_zam, id_pro) VALUES (3,1);
INSERT INTO pracuje (id_zam, id_pro) VALUES (3,2);

INSERT INTO pracuje (id_zam, id_pro) VALUES (4,3);
INSERT INTO pracuje (id_zam, id_pro) VALUES (4,5);

INSERT INTO pracuje (id_zam, id_pro) VALUES (5,1);
INSERT INTO pracuje (id_zam, id_pro) VALUES (5,2);
INSERT INTO pracuje (id_zam, id_pro) VALUES (5,3);
INSERT INTO pracuje (id_zam, id_pro) VALUES (5,5);

INSERT INTO pracuje (id_zam, id_pro) VALUES (6,4);
INSERT INTO pracuje (id_zam, id_pro) VALUES (6,3);
INSERT INTO pracuje (id_zam, id_pro) VALUES (6,2);
INSERT INTO pracuje (id_zam, id_pro) VALUES (6,1);


INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Německo', 1324432, 82);
INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Velká Británie', 2203042, 91);
INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Polsko', 2215532, 32);
INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Švýcarsko', 43012, 97);
INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Slovensko', 865456, 18);
INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Česká Republika', 4123012, 23);
INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Rakousko', 32123, 41);
INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Holandsko', 7567, 21);
INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Belgie', 45674, 7);
INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Maďarsko', 5345, 76);
INSERT INTO zeme (nazev, rozloha, export_syru) VALUES ('Itálie', 7656,98);

INSERT INTO syr (nazev, zivocich, tuk, typ, id_zem, syr_trvanlivost) VALUES ('Ementál', 'kráva', 48, 'dírkovaný', 1,14);
INSERT INTO syr (nazev, zivocich, tuk, typ, id_zem, syr_trvanlivost) VALUES ('Romadůr', 'kráva', 31, 'smradlavý', 2,1);
INSERT INTO syr (nazev, zivocich, tuk, typ, id_zem, syr_trvanlivost) VALUES ('Hermelín', 'kráva', 48, 'plesnivý', 2, 31);
INSERT INTO syr (nazev, zivocich, tuk, typ, id_zem, syr_trvanlivost) VALUES ('Ovčí sýr', 'ovce', 48, 'bio', 1,10);
INSERT INTO syr (nazev, zivocich, tuk, typ, id_zem, syr_trvanlivost) VALUES ('Eidam', 'kráva', 45, 'plátkový', 5,11);
INSERT INTO syr (nazev, zivocich, tuk, typ, id_zem, syr_trvanlivost) VALUES ('Niva', 'kráva', 45, 'plesnivý', 5,12);
INSERT INTO syr (nazev, zivocich, tuk, typ, id_zem, syr_trvanlivost) VALUES ('Brie', 'kráva', 45, 'smradlavý', 5,5);


INSERT INTO dodavatel(ico,nazev,ulice,cislo_popisne,mesto,psc,cislo_uctu,telefon,login,id_zem) VALUES(121122121221,
'Agrofert','Státní',20,'Bratislava',89231,'6464646466','565464664','syr@agrofert.sk',5);
INSERT INTO dodavatel(ico,nazev,ulice,cislo_popisne,mesto,psc,cislo_uctu,telefon,login,id_zem) VALUES(121212198221,
'VUT','Antonínská',2,'Brno',60212,'6464646466','566544532','d@d.cz', 5);
INSERT INTO dodavatel(ico,nazev,ulice,cislo_popisne,mesto,psc,cislo_uctu,telefon,login, id_zem) VALUES(143412198221,
'Albert','Hornoměstská',32,'Pardubice',43153,'6464646466','452876175','dod@albert.cz', 3);
INSERT INTO dodavatel(ico,nazev,ulice,cislo_popisne,mesto,psc,cislo_uctu,telefon,login, id_zem) VALUES(121632121221,
'Lidl','Maderstrasse',52,'Frankfurt',64254,'6464646466','123512765','d@lidl.com', 1);
INSERT INTO dodavatel(ico,nazev,ulice,cislo_popisne,mesto,psc,cislo_uctu,telefon,login, id_zem) VALUES(905690721567,
'Farma Novák','Ticha',3,'Londýn',60232,'35235234523','740234123','d', 2);


INSERT INTO nabidka (ico, id_syr, cena) VALUES (121122121221, 3, 1000);
INSERT INTO nabidka (ico, id_syr, cena) VALUES (121122121221, 1, 3000);
INSERT INTO nabidka (ico, id_syr, cena) VALUES (121122121221, 2, 2200);

INSERT INTO nabidka (ico, id_syr, cena) VALUES (121212198221, 2, 1000);
INSERT INTO nabidka (ico, id_syr, cena) VALUES (121212198221, 3, 2000);
INSERT INTO nabidka (ico, id_syr, cena) VALUES (121212198221, 4, 1500);
INSERT INTO nabidka (ico, id_syr, cena) VALUES (121212198221, 5, 500);

INSERT INTO nabidka (ico, id_syr, cena) VALUES (143412198221, 2, 1600);
INSERT INTO nabidka (ico, id_syr, cena) VALUES (143412198221, 7, 2100);
INSERT INTO nabidka (ico, id_syr, cena) VALUES (143412198221, 5, 800);

INSERT INTO nabidka (ico, id_syr, cena) VALUES (121632121221, 4, 1000);
INSERT INTO nabidka (ico, id_syr, cena) VALUES (121632121221, 2, 500);
INSERT INTO nabidka (ico, id_syr, cena) VALUES (121632121221, 3, 1400);

INSERT INTO nabidka (ico, id_syr, cena) VALUES (905690721567, 6, 1150);

INSERT INTO objednavka (celkova_cena, datum_vytvoreni, dodano, ico, id_zam, id_pro, druh_platby,zaplacena,datum_platby) VALUES (33000, '2016-05-03 21:02:44', 1, 121122121221, 3, 1,0,1,'2016-05-05 14:21:01');
INSERT INTO objednavka (celkova_cena, datum_vytvoreni, dodano, ico, id_zam, id_pro, druh_platby,zaplacena,datum_platby) VALUES (32250, '2017-11-03 14:06:12', 1, 121212198221, 6, 1,1,1,'2017-11-12 14:06:12');
INSERT INTO objednavka (celkova_cena, datum_vytvoreni, dodano, ico, id_zam, id_pro,zaplacena) VALUES (7500, '2017-11-11 10:42:34', 0, 121632121221, 6, 1,0);

INSERT INTO objednavka (celkova_cena, datum_vytvoreni, dodano, ico, id_zam, id_pro, druh_platby) VALUES (29450, '2017-10-21 15:32:31', 0, 121632121221, 3, 2,1);
INSERT INTO objednavka (celkova_cena, datum_vytvoreni, dodano, ico, id_zam, id_pro, druh_platby,zaplacena,datum_platby) VALUES (15500, '2016-11-17 11:21:42', 1, 121212198221, 5, 2,0,1,'2016-11-22 17:17:01');

INSERT INTO objednavka (celkova_cena, datum_vytvoreni, dodano, ico, id_zam, id_pro, druh_platby,zaplacena,datum_platby) VALUES (5750, '2017-11-01 11:21:42', 1, 905690721567, 5, 3,1,1,'2017-11-15 8:42:15');
INSERT INTO objednavka (celkova_cena, datum_vytvoreni, dodano, ico, id_zam, id_pro,zaplacena) VALUES (30250, '2017-11-05 12:42:34', 0, 121632121221, 6, 3,0);

INSERT INTO objednavka (celkova_cena, datum_vytvoreni, dodano, ico, id_zam, id_pro, druh_platby,zaplacena,datum_platby) VALUES (2200, '2017-11-04 11:51:21', 1, 143412198221, 1, 4,0,1,'2017-11-08 16:01:12');

INSERT INTO objednavka (celkova_cena, datum_vytvoreni, dodano, ico, id_zam, id_pro,zaplacena) VALUES (33500, '2017-10-27 18:15:52', 0, 121212198221, 5, 5,0);

INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (1, 3, 12);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (1, 1, 7);

INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (2, 2, 1.5);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (2, 3, 10);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (2, 2, 8);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (2, 5, 11.5);

INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (3, 2, 3);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (3, 4, 6);

INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (4, 4, 10);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (4, 2, 12.3);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (4, 3, 9.5);

INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (5, 2, 2);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (5, 3, 5);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (5, 4, 3.5);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (5, 5, 4.5);

INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (6, 6, 5);

INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (7, 4, 10);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (7, 2, 12.5);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (7, 3, 10);

INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (8, 2, 6);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (8, 7, 4);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (8, 5, 5);

INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (9, 3, 8.5);
INSERT INTO obsah (id_obj, id_syr, hmotnost) VALUES (9, 4, 11);

INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (12, 8, '2016-05-05 14:21:01', '2013-05-19 14:21:01', 1, 1, 3);
INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (7, 5, '2016-05-05 14:21:01', '2015-6-02 14:21:01', 1, 1, 1);

INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (1.5, 1, '2017-11-12 14:06:12', '2017-12-12 14:06:12', 1, 2, 2);
INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (10, 7.5, '2017-11-12 14:06:12', '2018-01-03 14:06:12', 1, 2, 3);
INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (8, 8, '2017-11-12 14:06:12', '2017-12-12 14:06:12', 1, 2, 2);
INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (11.5, 10, '2017-11-12 14:06:12', '2018-01-11 14:06:12', 1, 2, 5);

INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (2, 2, '2016-11-22 17:17:01', '2016-12-22 17:17:01', 2, 5, 2);
INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (5, 3.1, '2016-11-22 17:17:01', '2016-11-31 17:17:01', 2, 5, 3);
INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (3.5, 1, '2016-11-22 17:17:01', '2018-01-22 17:17:01', 2, 5, 4);
INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (4.5, 4.5, '2016-11-22 17:17:01', '2018-01-11 14:06:12', 2, 5, 5);

INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (5, 4.5, '2017-11-15 8:42:15', '2018-01-5 8:42:16', 3, 6, 6);

INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (6, 6, '2017-11-08 16:01:12', '2018-01-5 16:01:12', 4, 8, 2);
INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (4, 4, '2017-11-08 16:01:12', '2017-11-16 16:01:12', 4, 8, 7);
INSERT INTO bochnik (pocatecni_hmotnost, aktualni_hmotnost, datum_dodani, trvanlivost, id_pro, id_obj, id_syr) VALUES (6, 5, '2017-11-08 16:01:12', '2018-01-5 16:01:12', 4, 8, 5);

