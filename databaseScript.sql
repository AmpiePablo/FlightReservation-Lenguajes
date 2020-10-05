 
CREATE DATABASE vuelos;
USE vuelos;

/**********************USUARIOS OPERATIVOS Y GENERALES************************/
CREATE TABLE `vuelos`.`usuarioOperativo` (
  idUsuarioOperativo INT NOT NULL AUTO_INCREMENT,
  username VARCHAR(35) NOT NULL,
  contrasenia VARCHAR(20) NOT NULL, /*REPASAR EL ENCRIPTADO Y DESENCRIPTADO*/
  PRIMARY KEY (`idUsuarioOperativo`)
);

CREATE TABLE `vuelos`.`usuarioGeneral`(
  idUsuarioGeneral INT NOT NULL AUTO_INCREMENT,
  pasaporte VARCHAR(25) NOT NULL UNIQUE,
  nombre VARCHAR(35) NOT NULL,
  apellido1 VARCHAR(35) NOT NULL,
  apellido2 VARCHAR(35) NOT NULL,
  fechaNacimiento date not null, 
  sexo char(1) NOT NULL CHECK( sexo in ('F', 'M') ),
  PRIMARY KEY (`idUsuarioGeneral`)
);

/*********************MARCAS, MODELOS Y AVIONES*******************************/
CREATE TABLE `vuelos`.`marca`(
  idMarca INT NOT NULL AUTO_INCREMENT,
  descripcion VARCHAR(35),
  PRIMARY KEY(`idMarca`)
);

CREATE TABLE `vuelos`.`modelo` (
  idModelo INT NOT NULL AUTO_INCREMENT, 
  descripcion VARCHAR(20) NOT NULL, 
  anio INT NOT NULL,
  idMarca INT NOT NULL,
  PRIMARY KEY(`idModelo`),
  KEY `fk_idMarca` (`idMarca`),
	CONSTRAINT `fk_idMarca` FOREIGN KEY(`idMarca`) REFERENCES `marca`(`idMarca`)
);

CREATE TABLE `vuelos`.`avion` (
  idAvion INT NOT NULL AUTO_INCREMENT,
  matricula VARCHAR(10) NOT NULL UNIQUE,
  anio INT NOT NULL,
  idModelo INT NOT NULL,
  PRIMARY KEY(`idAvion`),
  KEY `fk_idModelo` (`idModelo`),
	CONSTRAINT `fk_idModelo` FOREIGN KEY(`idModelo`) REFERENCES `modelo`(`idModelo`)
);

/**********************TIPO ASIENTO, ASIENTO Y VUELO***************************/
CREATE TABLE `vuelos`.`tipoAsiento` (
  idTipoAsiento INT NOT NULL AUTO_INCREMENT,
  tipo char(1) not null check(tipo in('B','S','E')),
  precioAdulto decimal(10,2) check(precioAdulto>0),
  precioInfante decimal(10,2) check(precioInfante>0),
  PRIMARY KEY(`idTipoAsiento`)
);

CREATE TABLE `vuelos`.`vuelo` (
  idVuelo INT NOT NULL AUTO_INCREMENT,
  origen VARCHAR(45) NOT NULL,
  destino VARCHAR(45) NOT NULL,
  fechaSalida DATETIME NOT NULL,
  fechaLlegada DATETIME NOT NULL,
  idAvion INT NOT NULL,
  PRIMARY KEY(`idVuelo`),
  KEY `fk_idAvion`(`idAvion`),
	CONSTRAINT `fk_idAvion` FOREIGN KEY(`idAvion`) REFERENCES `avion`(`idAvion`)
);

CREATE TABLE `vuelos`.`asiento` (
  idAsiento INT NOT NULL AUTO_INCREMENT,
  nombre VARCHAR(10),
  fila CHAR(1),
  estaOcupado BIT,
  idTipoAsiento INT NOT NULL,
  idVuelo INT NOT NULL,
  idReservacion INT,
  PRIMARY KEY(`idAsiento`),
  KEY `fk_idTipoAsiento` (`idTipoAsiento`),
	CONSTRAINT `fk_idTipoAsiento` FOREIGN KEY(`idTipoAsiento`) REFERENCES `tipoAsiento`(`idTipoAsiento`),
  KEY `fk_idVuelo`(`idVuelo`),
	CONSTRAINT `fk_idVuelo` FOREIGN KEY(`idVuelo`) REFERENCES `vuelo`(`idVuelo`),
  KEY `fk_idReservacion`(`idReservacion`),
	CONSTRAINT `fk_idReservacion` FOREIGN KEY(`idReservacion`) REFERENCES `reservacion`(`idReservacion`)
);

/***********************RESERVACION Y USUARIOXRESERVACION**********************/
CREATE TABLE `vuelos`.`reservacion` (
  idReservacion INT NOT NULL AUTO_INCREMENT,
  fecha DATE NOT NULL,
  PRIMARY KEY(idReservacion)
);

CREATE TABLE `vuelos`.`usuarioXreservacion` (
  idUsuarioXReservacion INT NOT NULL AUTO_INCREMENT,
  idUsuarioGeneral INT NOT NULL,
  idReservacion INT NOT NULL,
  PRIMARY KEY(`idUsuarioXReservacion`),
  KEY `fk_idUsuarioGeneral`(`idUsuarioGeneral`),
	CONSTRAINT `fk_idUsuarioGeneral` FOREIGN KEY(`idUsuarioGeneral`) REFERENCES `usuarioGeneral`(`idUsuarioGeneral`),
  KEY `fk_idReservacion2`(`idReservacion`),
	CONSTRAINT `fk_idReservacion2` FOREIGN KEY(`idReservacion`) REFERENCES `reservacion`(`idReservacion`)
);






/***********************PROCEDIMIENTOS ALMACENADOS*******************************/
delimiter //
CREATE PROCEDURE consultaReserva(in pIdReservacion int, in pPasaporte varchar(25))
BEGIN
  select usuarioXreservacion.idReservacion, usuarioGeneral.pasaporte, usuarioGeneral.nombre,
  usuarioGeneral.apellido1, usuarioGeneral.apellido2,reservacion.fecha,asiento.fila, asiento.nombre,
  tipoAsiento.precioAdulto,vuelo.origen,vuelo.destino,vuelo.fechaSalida from usuarioXreservacion
  inner join usuarioGeneral on (usuarioXreservacion.idUsuarioGeneral=usuarioGeneral.idUsuarioGeneral)
  inner join reservacion on (usuarioXreservacion.idReservacion = reservacion.idReservacion)
  inner join asiento on (reservacion.idReservacion=asiento.idReservacion)
  inner join tipoAsiento on (asiento.idTipoAsiento=tipoAsiento.idTipoAsiento)
  inner join vuelo on(asiento.idVuelo=vuelo.idVuelo)
  where usuarioXreservacion.idReservacion = pIdReservacion and usuarioGeneral.pasaporte = pPasaporte
  order by pasaporte
END //






/***********************INSERTS IN TABLES***************************************/
INSERT INTO usuarioOperativo(username,contrasenia) values('pablo','1234');
INSERT INTO usuarioOperativo(username,contrasenia) values('landon','1234');

INSERT INTO usuarioGeneral(pasaporte,nombre,apellido1,apellido2,fechaNacimiento,sexo) values('702680070','jose','lopez','mora','1999-04-04','m');
INSERT INTO usuarioGeneral(pasaporte,nombre,apellido1,apellido2,fechaNacimiento,sexo) values('702560789','landon','piedra','mora','1997-05-05','m');
INSERT INTO usuarioGeneral(pasaporte,nombre,apellido1,apellido2,fechaNacimiento,sexo) values('123640502','rosa','mora','mora','2005-01-05','m');

INSERT INTO marca(descripcion)values('airbus'),('boeing'),('bombardier');

INSERT INTO modelo(descripcion,anio,idMarca) values('boeing 747',2010,2),('boeing 777',15,2),('A330',2016,1),('B880',2011,3);


insert into avion(matricula,anio,idModelo)values('BB110-2',2018,4);
insert into avion(matricula,anio,idModelo)values('AA1234-6',2019,3);
  

insert into tipoAsiento(tipo,precioAdulto,precioInfante) values('B',1500,1000);
insert into tipoAsiento(tipo,precioAdulto,precioInfante) values('S',1000,750);
insert into tipoAsiento(tipo,precioAdulto,precioInfante) values('E',750,500);




insert into reservacion(fecha) values('2020-10-10');
insert into reservacion(fecha) values('2020-10-09');
insert into reservacion(fecha) values('2020-10-12');


insert into asiento(nombre,fila,estaOcupado,idTipoAsiento,idVuelo,idReservacion)
values('A_BL_1','A',0,7,1,1);
insert into asiento(nombre,fila,estaOcupado,idTipoAsiento,idVuelo,idReservacion) 
values('A_BL_2','A',0,7,1,1);
insert into asiento(nombre,fila,estaOcupado,idTipoAsiento,idVuelo,idReservacion)
values('B_BL_1','B',0,7,1,2);
insert into asiento(nombre,fila,estaOcupado,idTipoAsiento,idVuelo,idReservacion) 
values('B_BL_2','B',0,7,1,2);
insert into asiento(nombre,fila,estaOcupado,idTipoAsiento,idVuelo,idReservacion) 
values('C_SL_2','C',0,8,1,2);

insert into usuarioXreservacion(idUsuarioGeneral,idReservacion) values(1,1);
insert into usuarioXreservacion(idUsuarioGeneral,idReservacion) values(2,2);
insert into usuarioXreservacion(idUsuarioGeneral,idReservacion) values(3,2);
insert into usuarioXreservacion(idUsuarioGeneral,idReservacion) values(1,2);
