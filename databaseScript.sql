CREATE DATABASE vuelos;
USE vuelos;

/**********************USUARIOS OPERATIVOS Y GENERALES************************/
CREATE TABLE `vuelos`.`usuarioOperativo` (
  idUsuarioOperativo INT NOT NULL AUTO_INCREMENT,
  username VARCHAR(35) NOT NULL,
  contrasenia VARCHAR(20) NOT NULL,
  PRIMARY KEY (`idUsuarioOperativo`)
);

CREATE TABLE `vuelos`.`usuarioGeneral`(
  pasaporte INT NOT NULL,
  nombre VARCHAR(35) NOT NULL,
  apellido1 VARCHAR(35) NOT NULL,
  apellido2 VARCHAR(35) NOT NULL,
  fechaNacimiento date not null, 
  sexo char(1),
  PRIMARY KEY (`pasaporte`)
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
  anio DATE NOT NULL,
  idMarca INT NOT NULL,
  PRIMARY KEY(`idModelo`),
  KEY `fk_idMarca` (`idMarca`),
	CONSTRAINT `fk_idMarca` FOREIGN KEY(`idMarca`) REFERENCES `marca`(`idMarca`)
);

CREATE TABLE `vuelos`.`avion` (
  idAvion INT NOT NULL AUTO_INCREMENT,
  matricula INT NOT NULL,
  anio VARCHAR(5) NOT NULL,
  idModelo INT NOT NULL,
  PRIMARY KEY(`idAvion`),
  KEY `fk_idModelo` (`idModelo`),
	CONSTRAINT `fk_idModelo` FOREIGN KEY(`idModelo`) REFERENCES `modelo`(`idModelo`)
);

/**********************TIPO ASIENTO, ASIENTO Y VUELO***************************/
CREATE TABLE `vuelos`.`tipoAsiento` (
  idTipoAsiento INT NOT NULL AUTO_INCREMENT,
  tipo VARCHAR(35) NOT NULL,
  precio DECIMAL(10,2),
  PRIMARY KEY(`idTipoAsiento`)
);

CREATE TABLE `vuelos`.`vuelo` (
  idVuelo INT NOT NULL,
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
  idAsiento INT NOT NULL,
  nombre VARCHAR(10),
  fila VARCHAR(5),
  estaOcupado BIT,
  idTipoAsiento INT NOT NULL,
  idVuelo INT NOT NULL,
  PRIMARY KEY(`idAsiento`),
  KEY `fk_idTipoAsiento` (`idTipoAsiento`),
	CONSTRAINT `fk_idTipoAsiento` FOREIGN KEY(`idTipoAsiento`) REFERENCES `tipoAsiento`(`idTipoAsiento`),
  KEY `fk_idVuelo`(`idVuelo`),
	CONSTRAINT `fk_idVuelo` FOREIGN KEY(`idVuelo`) REFERENCES `vuelo`(`idVuelo`)
);

/***********************RESERVACION Y USUARIOXRESERVACION**********************/
CREATE TABLE `vuelos`.`reservacion` (
  idReservacion INT NOT NULL,
  fecha DATE NOT NULL,
  idAsiento INT NOT NULL,
  PRIMARY KEY(idReservacion),
  KEY `fk_idAsiento`(`idAsiento`),
	CONSTRAINT `fk_idAsiento` FOREIGN KEY(`idAsiento`) REFERENCES `asiento`(`idAsiento`)
);

CREATE TABLE `vuelos`.`usuarioXreservacion` (
  idUsuarioXReservacion INT NOT NULL AUTO_INCREMENT,
  pasaporte INT NOT NULL,
  idReservacion INT NOT NULL,
  PRIMARY KEY(`idUsuarioXReservacion`),
  KEY `fk_pasaporte`(`pasaporte`),
	CONSTRAINT `fk_pasaporte` FOREIGN KEY(`pasaporte`) REFERENCES `usuarioGeneral`(`pasaporte`),
  KEY `fk_idReservacion`(`idReservacion`),
	CONSTRAINT `fk_idReservacion` FOREIGN KEY(`idReservacion`) REFERENCES `reservacion`(`idReservacion`)
);
