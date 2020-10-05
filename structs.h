#ifndef STRUCTS_H
#define STRUCTS_H

struct Marca {
	char *descripcion;
} typedef Marca;

struct Modelo {
	char *descripcion;
	int anio;
	Marca *marca;
} typedef Modelo;

struct Avion {
	char *matricula;
	int anio;
	Modelo *modelo;
} typedef Avion;

struct TipoAsiento {
	char *tipo;
	float precio;
} typedef TipoAsiento;

struct Asiento {
	char *nombre;
	char fila;
	int estaOcupado;
	TipoAsiento *tipo;
} typedef Asiento;

struct Vuelo {
	char *origen;
	char *destino;
	char *fechaSalida;
	char *fechaLlegada;
	Avion *avion;
	Asiento **asientos;
} typedef Vuelo;

struct UsuarioOperativo {
	char *nombreUsuario;
	char *contrasenia;
	Avion **aviones;
} typedef UsuarioOperativo;

struct UsuarioGeneral {
	char *pasaporte;
	char *nombre;
	char *apellido1;
	char *apellido2;
	char *fechaNacimiento;
	char sexo;
} typedef UsuarioGeneral;

struct Reservacion {
	char *fecha;
	UsuarioGeneral **usuarios;
	Asiento **asientos;
} typedef Reservacion;

#endif
