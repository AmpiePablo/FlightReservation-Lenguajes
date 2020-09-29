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
  sexo char(1),
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
  tipo CHAR(2) NOT NULL CHECK(tipo in('BI','BA','SI','SA','EI','EA')),
  precio DECIMAL(10,2) CHECK(precio>0),
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
  idReservacion INT NOT NULL,
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
  idReservacion INT NOT NULL,
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
