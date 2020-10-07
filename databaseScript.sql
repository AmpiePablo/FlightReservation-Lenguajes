 
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
/*
 * consultaReserva
 *
 * Entradas:
 *    Una reservacion (identificador) y un usuarioGeneral(pasaporte)
 *
 * Salidas:
 *    información relevante sobre una reservación, como precio, informacion
 *    general del usuario, informacion del vuelo
 *
 * Restricciones:
 *    que la reservacion y el pasaporte existan de lo contrario retorna nulo
 *
 * Objetivo:
 *    consulta en la base datos si hay una reservacion relacionada al pasaporte
 *    de un usuario y a la reserva de un usuario.
 *    
 */

delimiter //
CREATE PROCEDURE consultaReserva(in pIdReservacion int, in pPasaporte varchar(25))
BEGIN
  select uxr.idReservacion, uG.pasaporte, uG.nombre,
  uG.apellido1, uG.apellido2,R.fecha,A.fila, A.nombre,
  tA.precioInfante,v.origen,v.destino,v.fechaSalida
  from usuarioXreservacion uxr
  inner join usuarioGeneral uG on (uxr.idUsuarioGeneral=uG.idUsuarioGeneral)
  inner join reservacion R on (uxr.idReservacion = R.idReservacion)
  inner join asiento A on (R.idReservacion=A.idReservacion)
  inner join tipoAsiento tA on (A.idTipoAsiento=tA.idTipoAsiento)
  inner join vuelo v on(A.idVuelo=v.idVuelo)
  where uxr.idReservacion = pIdReservacion and uxr.idUsuarioGeneral = uG.idUsuarioGeneral and uG.pasaporte=pPasaporte;
END //


/*
 * eliminarReservacion
 *
 * Entradas:
 *    datos de un usuarioGeneral (pasaporte)
 *
 * Salidas:
 *    un entero representa la edad del usuarioGeneral dado
 *
 * Restricciones:
 *    una entrada de tipo varchar(25) que en c sería un char * 
 *    de tamaño 25
 *
 * Objetivo:
 *    elimina todo lo relaciona a una reservación que contenga el mismo identificadorr
 *    de la reserva y el pasaporte que ingresa para no eliminar todo.
 *    
 */
delimiter //
CREATE PROCEDURE eliminarReservacion(in pIdReservacion int, in pPasaporte varchar(25))
BEGIN
  update asiento set idReservacion = NULL,estaOcupado=0 where idReservacion=pIdReservacion;

  declare pIdUsarioGeneral int;
  select idUsuarioGeneral into pIdUsarioGeneral from usuarioGeneral where pasaporte=pPasaporte;
  delete from usuarioXreservacion where idUsuarioGeneral = pIdUsarioGeneral;

  delete from reservacion where idReservacion=pIdReservacion;

END //







/*
 * diferenciaAnios
 *
 * Entradas:
 *    datos de un usuarioGeneral (pasaporte)
 *
 * Salidas:
 *    un entero representa la edad del usuarioGeneral dado
 *
 * Restricciones:
 *    una entrada de tipo varchar(25) que en c sería un char * 
 *    de tamaño 25
 *
 * Objetivo:
 *    consulta en la base datos la edad que tiene un usuario o un dueño de un pasaporte
 *    
 */
CREATE PROCEDURE diferenciaAnios(in pPasaporte varchar(25))
  begin 
    declare fechaNaci date; 
    select fechaNacimiento into fechaNaci from usuarioGeneral where pasaporte=pPasaporte; 
    select timestampdiff(YEAR,fechaNaci,now()); 
  end //

/*
 * getInfoVuelo
 *
 * Entradas:
 *    Una reservacion especificamente el identificador de la reserva
 *
 * Salidas:
 *    información relevante sobre un vuelo
 *    como origen,destino, fechas entre otros.
 *
 * Restricciones:
 *    Que el id de la reservación existe o se haya procesado con anterioridad
 *
 * Objetivo:
 *    consulta en la base datos los datos de un vuelo, siempre y cuando
 *    el vuelo este relacionado con una reservación.
 *    
 */
delimiter //
create procedure getInfoVuelo(in idReservacion int)
  begin
    select a.idAsiento,v.origen,v.fechaSalida,v.destino,v.fechaLlegada
    from asiento inner join reservacion r  on (a.idReservacion=r.idReservacion)
    inner join vuelo v on (a.idVuelo=v.idVuelo)
    where a.idReservacion=idReservacion and r.idReservacion=idReservacion;
  end //

/*
 * validaPasaporte
 *
 * Entradas:
 *    datos de un usuarioGeneral (pasaporte)
 *
 * Salidas:
 *    un estado 0-1 que verifica si existe o no un pasaporte
 *    retorna 1 si existe y 0 si no
 *
 * Restricciones:
 *    una entrada de tipo varchar(25) que en c sería un char * 
 *    de tamaño 25
 *
 * Objetivo:
 *    consulta en la base datos si hay un pasaporte como el que 
 *    se ingresa de entrada en la función
 *    
 */
CREATE FUNCTION validaPasaporte(in pPasaporte varchar(25))
  BEGIN
    returns int deterministic
    IF not exists(select pasaporte from usuarioGeneral where pasaporte=pPasaporte) then
      return 0;
    ELSE
      return 1;
    else if;
  END //




/*
 * identifcaInfante
 *
 * Entradas:
 *    Datos de usuario general especificamente un pasaporte
 *
 * Salidas:
 *    Un char(1) (1,0) que dice si un pasaporte pertenece a un infante
 *    en caso de pertenecerlo retorna 1 de lo contrario 0
 *
 * Restricciones:
 *    Que entre un pasaporte que exista dentro de la base de datos
 *
 * Objetivo:
 *    Verifica en la base de datos si un usuario dueño de pasaporte pertenece a un
 *    infante o a un adutlo, toma la fechaActual y verifica si es menor que 3 
 *    para ver si es infante o no
 *    
 */
CREATE PROCEDURE identificaInfante( in pPasaporte varchar(25))
  BEGIN
    DECLARE actual datetime;
    set actual = now();

    DECLARE result1 char(1);
    set result1 = '1';

    DECLARE result2 char(1);
    set result2 = '0';

    DECLARE fechaNacimiento date;
    set fechaNacimiento = select fechaNacimiento from usuarioGeneral where pasaporte=pPasaporte;

    if((select actual - select fechaNacimiento) > 3 ) BEGIN
      select result2;
    END ELSE BEGIN
      SELECT result1;
    END
  END //

/*
 * Monto
 *
 * Entradas:
 *    Un entero como id
 *
 * Salidas:
 *    Un decimal(10,2)
 *
 * Restricciones:
 *    El entero debe coincidir con un idReservacion
 *    de la tabla vuelos.reservaciones
 *
 * Objetivo:
 *    Sumar los montos de todos los asientos
 *    relacionados a la reservación, y el monto
 *    de asiento para cualquier infante que contenga
 *    la reservación, también.
 *    
 */

create function Monto(ID int)
returns decimal(10,2)
begin
  return (
    select 
      sum(ta.precioAdulto) + (count(uxr.idUsuarioGeneral) - count(at.idAsiento))*max(ta.precioInfante)
    from
      reservacion re inner join asiento at
      on re.idReservacion = at.idReservacion
      inner join tipoAsiento ta
      on at.idTipoAsiento = ta.idTipoAsiento
      inner join usuarioXreserva uxr
      on re.idReservacion = uxr.idReservacion
    where
      re.idReservacion = ID
    );
end;


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

insert into asiento(nombre,fila,estaOcupado,idTipoAsiento,idVuelo,idReservacion) 
values('C_SL_3','C',0,8,1,1);

insert into asiento(nombre,fila,estaOcupado,idTipoAsiento,idVuelo,idReservacion) 
values('C_SL_3','C',0,8,1,1);

insert into usuarioXreservacion(idUsuarioGeneral,idReservacion) values(1,1);
insert into usuarioXreservacion(idUsuarioGeneral,idReservacion) values(2,2);
insert into usuarioXreservacion(idUsuarioGeneral,idReservacion) values(3,2);
insert into usuarioXreservacion(idUsuarioGeneral,idReservacion) values(1,2);
