 
#ifndef OPERATIVAS_H
#define OPERATIVAS_H

#include <stdio.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "get_input.h"
#include "crypt.h"


void LoginOperativo();
void MenuOperativo();
void MenuAviones();
void IncluirAvion();
void EliminarAvion();
void MostrarAviones();
void DisplayResult(MYSQL_RES*);

void CargarUsuarios();
char** Split(char*, char, int*);
void FreeStrArr(char**, int);
char* GetLine(FILE *fp);

void EstadoVuelo();

void VerEstadisticas();
void TopMayoresVentas();
void TopMasPasajeros();

int ExecuteQuery(MYSQL_RES**, const char*);


typedef struct {
	char *server;
	char *user;
	char *password;
	char *database;
} UserData;

UserData UD = {"localhost", "root", "ampie0422", "vuelos"};

/*
 * LoginOperativo
 *
 * Entradas:
 *		Un nombre de usuario y una contraseña, digitados
 *		por el usuario por consola.
 *
 * Objetivo:
 *		Permitir que el usuario del programa intente
 *		loguearse como un usuario operativo, bajo la
 *		condición de que ingrese un usuario que exista
 *		y su contraseña respectiva correctamente.
 *		
 */
void LoginOperativo() {
	
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	char *us;
	char *pw;
	
	char *query;
	
	short no_salir = 1;
	
	do {
		puts("Usuario: \t");
		us = GetInput();
		puts("Contrasenia: \t");
		pw = GetInput();
		Encrypt(pw);
		
		query = (char*) malloc ( (84 + strlen(us) + strlen(pw) + 1) * sizeof(char) );
		
		sprintf(query, 
			"select "
				"idUsuarioOperativo "
			"from "
				"usuarioOperativo "
			"where "
				"username='%s' and contrasenia='%s'", us, pw);
		
		ExecuteQuery(&res, query);
		if (mysql_num_rows(res)>0) {
			printf("Bienvenido, %s!\n", us);
			
			MenuOperativo();
			//no_salir = 0;
			
		} else {
			puts(
				"Usuario o Contrasenia Incorrecta.\n"
				"Desea volver a intentar? (s/n): ");
			
			char confirmacion = tolower(getchar());
			while (getchar() != '\n');
			
			if (confirmacion != 's')
				no_salir = 0;
		}
		
		free(us);
		free(pw);
		free(query);
		mysql_free_result(res);
		
	} while (no_salir);
}

/*
 * MenuOperativo
 *
 * Entradas:
 *		Caracteres digitados por consola para la selección
 *		de opciones.
 *
 * Restricciones:
 *		Los caracteres que el usuario digite lo llevarán
 *		a un mensaje de error si no coinciden con
 *		alguna de las opciones presentadas.
 *
 * Objetivo:
 *		Presentar las opciones operativas al usuario,
 *		(Administración de Aviones, Carga de Usuarios,
 *		Observación de Vuelos y Ver Datos Estadísticos);
 *		así como permitir que seleccione cualquiera,
 *		dirigirlo a la opción seleccionada y Regresar.
 *		
 */
void MenuOperativo() {
	char opcion;
	char discard;
	
	char *texto =
		"Menu Operativo\n"
		"\n"
		"1. Registro de Aviones\n"
		"2. Carga de Usuarios\n"
		"3. Estado de Vuelo\n"
		"4. Estadisticas\n"
		"5. Salir\n"
		"\n"
		"-> ";
	
	for (;;) {
		
		puts(texto);
		
		opcion = getchar();
		discard = getchar();
		
		if (discard != '\n') {
			
			opcion = '-';
			while (getchar() != '\n');
		}
		
		switch (opcion) {
			
			case '1':
				MenuAviones();
				break;
				
			case '2':
				CargarUsuarios();
				break;
				
			case '3':
				EstadoVuelo();
				break;
				
			case '4':
				VerEstadisticas();
				break;
				
			case '5':
				return;
				
			default:
				puts("Error: Entrada Invalida");
		}
	}
}

/*
 * MenuAviones
 *
 * Entradas:
 *		Caracteres digitados por consola para la selección
 *		de opciones.
 *
 * Restricciones:
 *		Los caracteres que el usuario digite lo llevarán
 *		a un mensaje de error si no coinciden con
 *		alguna de las opciones presentadas.
 *
 * Objetivo:
 *		Presentar las opciones de administración de
 *		aviones al usuario, (Incluir, Eliminar y
 *		Mostrar); así como permitir que seleccione
 *		cualquiera, dirigirlo a la opción seleccionada
 *		y Regresar.
 *		
 */
void MenuAviones() {
	
	char opcion;
	char discard;
	
	char *texto =
		"Registro de Aviones\n"
		"\n"
		"1. Incluir\n"
		"2. Eliminar\n"
		"3. Mostrar\n"
		"4. Regresar\n"
		"\n"
		"-> ";
	
	for (;;) {
		
		puts(texto);
		
		opcion = getchar();
		discard = getchar();
		
		if (discard != '\n') {
			
			opcion = '-';
			while (getchar() != '\n');
		}
		
		switch (opcion) {
			
			case '1':
				IncluirAvion();
				break;
				
			case '2':
				EliminarAvion();
				break;
				
			case '3':
				MostrarAviones();
				break;
				
			case '4':
				return;
				
			default:
				puts("Error: Entrada Invalida");
		}
	}
}

/*
 * InsertarAvion
 *
 * Entradas:
 *		Datos de Avión (Matrícula, Marca, Modelo y Año)
 *		(por consola)
 *
 * Salidas:
 *		Una nuevo tupla Avión a la Base de Datos
 *
 * Restricciones:
 *		La Matrícula no debe existir aún en la Base de
 *		Datos, la Marca debe seleccionarse válidamente
 *		entre el conjunto de opciones presentadas,
 *		el Modelo también debe seleccionarse
 *		válidamente entre el conjunto de opciones
 *		presentadas y el Año debe ser entero positivo.
 *
 * Objetivo:
 *		Solicitar al usuario todos los datos correspondientes
 *		a un Avión (Matrícula, Marca, Modelo y Año),
 *		para insertarlo a la Base de Datos. Facilitarle
 *		la selección de la Marca mediante un despliegue
 *		de las Marcas existentes, al igual que
 *		facilitarle la selección del Modelo mediante un
 *		despliegue de los Modelos pertenecientes a la
 *		Marca seleccionada. Asegurarse de no insertar
 *		si existen datos inválidos.
 *		
 */
void IncluirAvion() {
	
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	Avion *av = (Avion*) malloc (sizeof(Avion));
	
	puts("\nMatricula: ");
		
	av->matricula = GetInput();
	
	ExecuteQuery(&res,											//
		"select descripcion, idMarca "							//
		"from marca ");											//
																//
	puts("\nSeleccione una Marca:\n");							//
																//
	DisplayResult(res); //Print Marcas							//
																//
	puts("\n-> ");												//
																//
	char *input = GetInput();									//
	int opcion = atoi(input);									// MARCAS
	free(input);												//
																//
	if (opcion <= 0 || opcion > mysql_num_rows(res)) {			//
		puts("\nError: Marca Invalida\n");						//
		goto ErrorMarca;										//
	}															//
																//
	Marca *ma = (Marca*) malloc (sizeof(Marca));				//
	mysql_data_seek(res, opcion - 1);							//
	row = mysql_fetch_row(res);									//
	ma->descripcion = row[0];									//
	char *idMarca = row[1];										//
	
	
	char *modelo_query = (char*) malloc((57 + strlen(idMarca)+1) * sizeof(char));										//
															//
	sprintf(modelo_query,									//
		"select descripcion, idModelo "						//
		"from modelo "										//
		"where idMarca = %s", idMarca);						//
															//
	mysql_free_result(res);									//
	ExecuteQuery(&res, modelo_query);						//
															//
	free(modelo_query);										//
															//
	puts("\nSelecione un Modelo:\n");						//
															//
	DisplayResult(res); //Print Modelos						//
															// MODELOS
	puts("\n-> ");											//
															//
	input = GetInput();										//
	opcion = atoi(input);									//
	free(input);											//
															//
	if (opcion <= 0 || opcion > mysql_num_rows(res)) {		//
		puts("\nError: Modelo Invalido\n");					//
		goto ErrorModelo;									//
	}														//
															//
	Modelo *mo = (Modelo*) malloc (sizeof(Modelo));			//
	mysql_data_seek(res, opcion - 1);						//
	row = mysql_fetch_row(res);								//
	mo->descripcion = row[0];								//
	char* idModelo = row[1];								//
	
	
	puts("\nAnio: ");											//
																//
	input = GetInput();											//
	av->anio = atoi(input);										//
																// AÑO
	if (av->anio <= 0) {										//
		puts("\nError: Anio Invalido\n");						//
		free(input);											//
		goto LiberarAvion;										//
	}															//
	
	
	printf("\n"																			//
		"Matricula: %s\n"																//
		"Marca: %s\n"																	//
		"Modelo: %s\n"																	//
		"Anio: %d\n"																	//
		"\n"																			//
		"Confirmar Avion (s/n): ",														//
		av->matricula, ma->descripcion, mo->descripcion, av->anio);						//
																						//
	char confirmacion = tolower(getchar());												//
	while(getchar() != '\n');															//
																						//
	if (confirmacion != 's') {															//
		puts("\nEl avion no fue insertado.");											// 
		goto LiberarAvion;																// CONFIRMACIÓN
	}																					// E INSERCIÓN
																						//
	char *insertion_query = (char*) malloc((56 + strlen(av->matricula) + strlen(input) + strlen(idModelo) + 1) * sizeof(char));
																						//
	sprintf(insertion_query,															//
		"insert into avion(matricula,anio,idModelo) values ('%s', %s, %s)",				//
		av->matricula, input, idModelo);												//
																						//
	mysql_free_result(res);																//
	int error = ExecuteQuery(&res, insertion_query);									//
																						//
	free(insertion_query);																//
	free(input);																		//
																						//
	if (error) {																		//
	    puts("\nError insertando Avion (Asegurese de que la Matricula sea unica)\n");	//
	    printf("%d\n", error);															//
		goto LiberarAvion;																//
	}																					//
	printf("\nAvion de matricula %s insertado exitosamente.\n", av->matricula);			//
	
	
	LiberarAvion:								//
		free(mo);								//
												//
	ErrorModelo:								// LIBERACIÓN DE MEMORIA
		free(ma);								// Y MANEJO DE ERRORES
												//
	ErrorMarca:									//
		free(av);								//
												//
		mysql_free_result(res);					//
}

/*
 * EliminarAvion
 *
 * Entradas:
 *		Matrícula de un Avión (por consola)
 *
 * Restricciones:
 *		Debe existir un Avión con la Matrícula
 *		ingresada, y tal Avión debe no estar asociado
 *		a ningún vuelo.
 *
 * Objetivo:
 *		Solicitar al usuario la Matrícula de un Avión,
 *		para eliminarlo de la Base de Datos.
 *		
 */
void EliminarAvion() {
	
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	puts("\nIngrese la Matricula del Avion que desea eliminar:\n\n-> ");
	char *matricula = GetInput();
	
	char *query = (char*) malloc((195 + strlen(matricula) + 1) * sizeof(char)); 
	
	sprintf(query,
		"select " 
			"av.matricula, ma.descripcion, mo.descripcion, av.anio " 
		"from " 
			"avion av inner join modelo mo " 
			"on av.idModelo = mo.idModelo " 
			"inner join marca ma " 
			"on mo.idMarca = ma.idMarca " 
		"where " 
			"av.matricula = '%s'", matricula);
	
	ExecuteQuery(&res, query);
	
	free(query);
	
	if (res == NULL) {
		puts("\nNo se encontraron aviones con esa matricula\n");
		goto LiberarMatricula;
	}
	
	row = mysql_fetch_row(res);
	
	printf("\n"
		"Matricula: %s\n"
		"Marca: %s\n"
		"Modelo: %s\n"
		"Anio: %d\n"
		"\n"
		"Confirmar Borrado (s/n): ",
		row[0], row[1], row[2], row[3]);
	
	mysql_free_result(res);
	
	if (tolower(getchar()) != 's') {
		puts("\nEl avion no fue eliminado.");
		goto LiberarMatricula;
	}
	query = (char*) malloc((38 + strlen(matricula)+1)*sizeof(char));
	sprintf(query, "delete from avion where matricula = '%s'", matricula);
	
	int error = ExecuteQuery(&res, query);
	
	free(query);
	
	if (error) {
		puts("\nError eliminando el Avion (Asegurese de que no este asociado a algun Vuelo)\n");
	} else {
		printf("\nEl avion de matricula %s fue eliminado exitosamente.\n", matricula);
	}
	
	LiberarMatricula:
		free(matricula);
}

/*
 * MostrarAviones
 *
 * Salidas:
 *		Todos los datos de todos los aviones
 *		(Matrícula, Marca, Modelo y Año)
 *
 * Objetivo:
 *		Mostrar al usuario todos los datos
 *		de todos los aviones que existen
 *		en la base de datos por consola.
 *		
 */
void MostrarAviones() {
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	int error = ExecuteQuery(&res,
		"select "
			"av.matricula, ma.descripcion, mo.descripcion, av.anio "
		"from "
			"avion av inner join modelo mo "
			"on av.idModelo = mo.idModelo "
			"inner join marca ma "
			"on mo.idMarca = ma.idMarca ");
	while (row = mysql_fetch_row(res)) {
		
		printf("Avion: %s\n"
			"Marca: %s, Modelo: %s, Anio: %d\n\n",
			row[0], row[1], row[2], row[3]);
	}
	if(!res)
		mysql_free_result(res);
}

/*
 * DisplayResult
 *
 * Entradas:
 *		Un puntero a un MYSQL_RES
 *		(Respuesta de una consulta MySQL)
 *
 * Salidas:
 *		Una lista enumerada de toda la primera
 *		columna de la respuesta de consulta.
 *		(por consola)
 *
 * Restricciones:
 *		El puntero entrante nunca puede ser NULL
 *
 * Objetivo:
 *		Tomar una respuesta de una consulta
 *		SQL, e imprimir el primer campo de
 *		todos los registros, enumerados de
 *		1 hasta n, en consola.
 *		
 */
void DisplayResult(MYSQL_RES *res) {
	MYSQL_ROW row;
	
	int i = 1;
	while (row = mysql_fetch_row(res))
		printf("%d. %s\n", i++, row[0]);
}

/*
 * CargarUsuarios
 *
 * Entradas:
 *		Una ruta a un archivo de texto plano
 *		(por consola)
 *
 * Salidas:
 *		Un conjunto de tuplas de usuarios generales
 *		en la base de datos, y un archivo con el
 *		reporte en el directorio del archivo original
 *
 * Restricciones:
 *		La ruta tiene que existir, y los datos
 *		que contengan tienen que ser correctos y
 *		completos.
 *
 * Objetivo:
 *		Recibir la ruta a un archivo, con un 
 *		conjunto de líneas con datos de usuarios
 *		en un formato específico (especificado
 *		en el enunciado del Proyecto), e insertar
 *		todos los usuarios especificados (que sean
 *		válidos) a la bse de datos. También
 *		crear un archivo de reporte, y en él escribir
 *		por cada usuario que se intentó insertar, si
 *		fue insertado, si estaba repetido o si estaba
 *		erróneo.
 *		
 */
void CargarUsuarios() {
	
	puts("\nIngrese la ruta del archivo con los usuarios que se desean cargar\n\n-> ");
	char *ruta = GetInput();
	
	FILE *file_ug = fopen(ruta, "r");
	
	if (file_ug == NULL) {
		puts("\nError: No se encontro el archivo\n");
		goto LiberarRuta;
	}
	
	char *dot = strrchr(ruta, '.');
	char *ruta_reporte = (char*) malloc ( ( strlen(ruta) + strlen("_reporte") ) * sizeof(char) );
	int i;
	for (i = 0; (ruta + i) < dot; i++) ruta_reporte[i] = ruta[i];
	strcpy(ruta_reporte + i, "_reporte");
	strcpy(ruta_reporte + i + strlen("_reporte"), dot);
	
	FILE *file_rp = fopen(ruta_reporte, "w+");
	
	/************************************************
	 *				Recorrer el Archivo				*
	 ************************************************/
	
	MYSQL_RES *res;
	
	char *line;
	int s_count;
	char **split;
	
	int q_size;
	char *query;
	const char *init_query = "insert into usuarioGeneral(pasaporte,nombre,apellido1,apellido2,sexo,fechaNacimiento)values(";
	char *q_cursor;
	
	int error;
	while ((line = GetLine(file_ug))!=NULL) {
		
		split = Split(line, ',', &s_count);
		
		/****************************************
		 *			  Formar Query				*
		 ****************************************/
		
		q_size = strlen(init_query) + 3*s_count + 1;
		for (int i = 0; i < s_count; i++)
			q_size += strlen(split[i]);
		
		query = (char*) malloc (q_size * sizeof(char));
		
		q_cursor = query;
		
		strcpy(q_cursor, init_query);
		q_cursor += strlen(init_query);

		printf("%s\n","si entra3" );

		for (int i = 0; i < s_count - 1; i++) {
			
			(*q_cursor) = '\'';
			q_cursor++;
			strcpy(q_cursor, split[i]);
			q_cursor += strlen(split[i]);
			(*q_cursor) = '\'';
			q_cursor++;
			(*q_cursor) = ',';
			q_cursor++;
		}

		(*q_cursor) = '\'';
		q_cursor++;
		strcpy(q_cursor, split[s_count - 1]);
		q_cursor += strlen(split[s_count - 1]);
		(*q_cursor) = '\'';
		q_cursor++;
		(*q_cursor) = ')';
		q_cursor++;
		*q_cursor = '\0';
		
		/****************************************
		 *	Ejecutar Query y Escribir a Reporte	*
		 ****************************************/
		printf("%s\n", query);
		error = ExecuteQuery(&res, query);
		printf("%d\n", error);
		if (error == 1062) {
			
			fputs("Repetido\n", file_rp);
			
		} else if (error != 0) {
			
			fputs("Error\n", file_rp);
			
		} else {
			
			fputs("Insertado\n", file_rp);
		}
		
		free(line);
		FreeStrArr(split,s_count);
		free(query);
	}
	printf("Se ha terminado de recorrer el archivo, revise en %s para ver mas detalles.", ruta_reporte);
	
	/************************************************
	 *	Cierre de Archivos y Liberación de Memoria	*
	 ************************************************/
	
	fclose(file_ug);
	fclose(file_rp);	
	free(ruta_reporte);
	
	LiberarRuta:
		free(ruta);
}

/*
 * GetLine
 *
 * Entradas:
 *		Un puntero a un objeto FILE
 *
 * Salidas:
 *		Una línea del archivo (secuencia de
 *		caracteres hasta encontrar un salto
 *		de línea)
 *
 * Restricciones:
 *		El archivo tiene que haber sido abierto
 *		en un modo que permita que sea leído.
 *
 * Objetivo:
 *		Avanzar el cursor del archivo, escribiendo
 *		caracteres a un arreglo de caracteres, formando
 *		un char* y en donde hubiera un salto de línea,
 *		delimitarlo con '\0'
 *		
 */
char* GetLine(FILE *fp) {
	
	unsigned int size = INIT_SIZE;
	
	char *line = (char*) malloc (size * sizeof(char));
	
	unsigned int length = 0;
	
	char c;
	
	while ( ( c = fgetc(fp) ) != '\n' && c != EOF ) {
		
		if (length == size) {
			
			size += size/2;
			line = (char*) realloc (line, size * sizeof(char));
		}
		line[length++] = c;
	}
	
	if (length == 0) {
		return NULL;
		
	} else {
		line = (char*) realloc (line, (length + 1) * sizeof(char));
		line[length] = '\0';
		
		return line;
	}
}

/*
 * Split
 *
 * Entradas:
 *		Una cadena de caracteres, un caracter
 *		delimitador, y un puntero a un entero
 *
 * Salidas:
 *		Un arreglo de cadenas de caracteres
 *		(retorno de función), y la cantidad de
 *		cadenas de caracteres en el arreglo
 *		(parámetro de retorno)
 *
 * Objetivo:
 *		Tomar una cadena de caracteres, y generar
 *		un resultado con todas las cadenas de
 *		caracteres que se obtendrían al dividir
 *		la cadena original en cada lugar donde se
 *		encuentre el caracter delimitador.
 *		
 */
char** Split(char *str, char splitter, int *return_count) {
	
	int str_count = 1;
	int i, j, k, l;
	
	i = 0;
	while (str[i] != '\0') {
		while ((str[i] != splitter) & (str[i] != '\0')) i++;
		str_count += (str[i++] != '\0');
	}
	
	char **result = (char**) malloc (str_count * sizeof(char*));
	for (i = 0; i < str_count; i++) result[i] = NULL;
	
	i = j = k = 0;
	while (str[k] != '\0') {
		while ((str[k] != splitter) & (str[k] != '\0')) k++;
		
		result[i] = (char*) malloc ( (k - j + 1) * sizeof(char) );
		
		l = 0;
		while (j < k) result[i][l++] = str[j++];
		
		result[i++][l] = '\0';
		
		j = k = ( k + (str[k] != '\0') );
	}
	
	*return_count = str_count;
	
	return result;
}

/*
 * FreeStrArr
 *
 * Entradas:
 *		Un arreglo de cadenas de caracteres y un entero
 *
 * Restricciones:
 *		El entero debe tener la cantidad de cadenas de
 *		caracteres que posee el arreglo
 *
 * Objetivo:
 *		Liberar la memoria de todas las cadenas de
 *		caracteres del arreglo, y luego liberar el
 *		puntero del arreglo.
 *		
 */
void FreeStrArr(char **str_arr, int count) {
	while (--count >= 0)
		free(str_arr[count]);
	free(str_arr);
}

/*
 * EstadoVuelo
 *
 * Entradas:
 *		Una entrada de texto digitada por el usuario
 *		(por consola)
 *
 * Salidas:
 *		Un gran conjunto de información por la
 *		salida estándar.
 *
 * Restricciones:
 *		La entrada debe ser un entero y debe coincidir
 *		con el identificador de un vuelo en la base de datos.
 *
 * Objetivo:
 *		A partir de un identificador de un vuelo, extraerlo
 *		de la base de datos, y desplegar legiblemente en
 *		la salida estándar, un conjunto enorme de datos
 *		relacionados con el vuelo (los datos son especificados
 *		en el enunciado del proyecto)
 *		
 */
void EstadoVuelo() {
	
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	puts("Ingrese el identificador del vuelo: ");
	char *idVuelo = GetInput();
	
	/********************************************
	 *			  Primera Consulta				*
	 ********************************************/
	
	char *query = (char*) malloc ( ( 162 + strlen(idVuelo) + 1 ) * sizeof(char) );
	
	sprintf(query,
		"select "
			"vu.origen, vu.fechaSalida, vu.destino, vu.fechaLlegada, "
			"av.matricula "
		"from "
			"vuelo vu inner join avion av "
			"on vu.idAvion=av.idAvion "
		"where "
			"vu.idVuelo=%s", idVuelo);
			
	int error = ExecuteQuery(&res, query);
	
	if (error) {
		puts("Error: Identificador de vuelo invalido\n");
		goto ErrorID;
	}
	
	row = mysql_fetch_row(res);
	
	printf(
		"Ciudad de Salida:\t%s\n"
		"Fecha y Hora de Salida:\t%s\n"
		"Ciudad de Arribo:\t%s\n"
		"Fecha y Hora de Arribo:\t%s\n"
		"\n"
		"Avion:\t\t\t%s\n\n",
		row[0], row[1], row[2], row[3], row[4]);
		
	free(query);
	mysql_free_result(res);
	
	/********************************************
	 *			  Segunda Consulta				*
	 ********************************************/
	 
	query = (char*) malloc ( (172 + strlen(idVuelo) + 1) * sizeof(char) );
	
	sprintf(query,
		"select distinct "
			"ta.tipo, ta.precioAdulto, ta.precioInfante "
		"from "
			"tipoAsiento ta inner join asiento at "
			"on ta.idTipoAsiento=at.idTipoAsiento "
		"where "
			"at.idVuelo=%s "
		"order by "
			"ta.tipo", idVuelo);
	
	ExecuteQuery(&res, query);
	puts("\tAdulto\tInfante");
	while (row = mysql_fetch_row(res))
		printf("Tipo %s:\t%s\t%s\n", row[0], row[1], row[2]);
	
	free(query);
	mysql_free_result(res);
	
	/********************************************
	 *			  Tercera Consulta				*
	 ********************************************/
	
	query = (char*) malloc ( (168 + strlen(idVuelo) + 1) * sizeof(char) );
	
	sprintf(query,
		"select "
			"at.nombre, at.fila, ta.tipo, at.estaOcupado "
		"from "
			"tipoAsiento ta inner join asiento at "
			"on ta.idTipoAsiento = at.idTipoAsiento "
		"where "
			"at.idVuelo=%s "
		"order by "
			"at.nombre", idVuelo);
	
	ExecuteQuery(&res, query);
	row = mysql_fetch_row(res);
	char fila_actual;
	
	while (row) {
		fila_actual = *(row[1]);
		printf("%c: ", fila_actual);
		
		do {
			printf("%s%c,", row[2], (atoi(row[3])) ? 'O' : 'L');
			row = mysql_fetch_row(res);
			
		} while ((row) && *(row[1]) == fila_actual);
		
		putchar('\n');
	}
	free(query);
	mysql_free_result(res);
	
	/********************************************
	 *			  Cuarta Consulta				*
	 ********************************************/
	 
	query = (char*) malloc ( (165 + strlen(idVuelo) + 1) * sizeof(char) );
	
	sprintf(query,
		"select "
			"ta.tipo, sum(-estaOcupado+1), sum(estaOcupado) "
		"from "
			"tipoAsiento ta inner join asiento at "
			"on ta.idTipoAsiento=at.idTipoAsiento "
		"where "
			"at.idVuelo=%s "
		"group by "
			"ta.tipo", idVuelo);
	
	ExecuteQuery(&res, query);
	
	while (row = mysql_fetch_row(res)) {
		printf("%sL: %s\n", row[0], row[1]);
		printf("%sO: %s\n", row[0], row[2]);
	}
	
	free(query);
	mysql_free_result(res);
	
	/********************************************
	 *			  Quinta Consulta				*
	 ********************************************/
	 
	query = (char*) malloc ( (456 + strlen(idVuelo) + 1) * sizeof(char) );
	
	sprintf(query,
		"select "
			"re.idReservacion, group_concat(ug.pasaporte),"
			"ta.tipo,"
			"count(at.idAsiento), group_concat(at.nombre)"
		"from "
			"reservacion re inner join asiento at "
			"on re.idReservacion=at.idReservacion "
			"inner join tipoAsiento ta "
			"on at.idTipoAsiento=ta.idTipoAsiento "
			"inner join usuarioXreservacion uxr "
			"on uxr.idReservacion=re.idReservacion "
			"inner join usuarioGeneral ug "
			"on uxr.idUsuarioGeneral=ug.idUsuarioGeneral "
		"where "
			"at.idVuelo=%s "
		"group by "
			"re.idReservacion, ta.tipo with rollup", idVuelo);
	//printf("%s\n",query);
	
	ExecuteQuery(&res, query);
	row = mysql_fetch_row(res);
	while (row[0]!=NULL) {
		printf("Reservacion %s:\n", row[0]);
		
		do {
			printf("%s: %s - %s\n", row[2], row[3], row[4]);
			row = mysql_fetch_row(res);
			
		} while (row[2] != NULL);
		
		printf("Pasaportes: %s\n", row[1]);
		printf("Cantidad de Asientos: %s\n", row[3]);
		putchar('\n');
		row = mysql_fetch_row(res);
	}
	
	free(query);
	mysql_free_result(res);
	
	//Fin / liberación de memoria
	
	ErrorID:
		free(idVuelo);
}

/*
 * VerEstadisticas
 *
 * Entradas:
 *		Caracteres digitados por consola para la selección
 *		de opciones.
 *
 * Restricciones:
 *		Los caracteres que el usuario digite lo llevarán
 *		a un mensaje de error si no coinciden con
 *		alguna de las opciones presentadas.
 *
 * Objetivo:
 *		Presentar las opciones de estadísticas al usuario,
 *		(Top 3 de vuelos de mayor venta y más pasajeros);
 *		así como permitir que seleccione cualquiera,
 *		dirigirlo a la opción seleccionada y Regresar.
 *		
 */
void VerEstadisticas() {
	char opcion;
	char discard;
	
	char *texto =
		"Estadisticas\n"
		"\n"
		"A. Top 3 Vuelos de Mayor Venta\n"
		"B. Top 3 Vuelos con Mas Pasajeros\n"
		"C. Regresar\n"
		"\n->";
	
	for (;;) {
		
		puts(texto);
		
		opcion = tolower(getchar());
		discard = getchar();
		
		if (discard != '\n') {
			
			opcion = '-';
			while (getchar() != '\n');
		}
		
		switch (opcion) {
			
			case 'a':
				TopMayorVenta();
				break;
				
			case 'b':
				TopMasPasajeros();
				break;
				
			case 'c':
				return;
				
			default:
				puts("Error: Entrada Invalida");
		}
	}
}

/*
 * TopMayorVenta
 *
 * Salidas:
 *		El top 3 de vuelos con mayores ventas
 *		por medio de la consola.
 *
 * Objetivo:
 *		Seleccionar de la base de datos los 3
 *		vuelos cuyas sumas de ventas en reservaciones
 *		sea la más alta, para desplegarlas en
 *		la consola para que el usuario los
 *		visualice.
 *		
 */
void TopMayorVenta() {
	
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	ExecuteQuery(res, 
		"select top 3 "//distinct?
			"vu.idVuelo, sum(Monto(re.idReservacion)) "
		"from "
			"vuelo vu inner join asiento at "
			"on vu.idVuelo = at.idVuelo "
			"inner join reservacion re "
			"on at.idReservacion = re.idReservacion "
		"order by "
			"sum(Monto(re.idReservacion)) desc");
	
	int i = 1;
	
	while (row = mysql_fetch_row(res))
		printf("%d. Vuelo: %s, Venta: %s\n", i++, row[0], row[1]);
	
	mysql_free_result(res);
}

/*
 * TopMasPasajeros
 *
 * Salidas:
 *		El top 3 de vuelos con más pasajeros
 *		por medio de la consola.
 *
 * Objetivo:
 *		Seleccionar de la base de datos los 3
 *		vuelos cuyas sumas de pasajeros (incluyendo
		infantes) sea la más alta, para desplegarlas en
 *		la consola para que el usuario los
 *		visualice.
 *		
 */
void TopMasPasajeros() {
	
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	ExecuteQuery(res, 
		"select top 3 "//distinct?
			"vu.idVuelo, sum(uxr.idUsuarioGeneral) "
		"from "
			"vuelo vu inner join asiento at "
			"on vu.idVuelo = at.idVuelo "
			"inner join reservacion re "
			"on at.idReservacion = re.idReservacion "
			"inner join usuarioXreservacion uxr "
			"on re.idReservacion = uxr.idReservacion "
		"order by "
			"sum(uxr.idUsuarioGeneral) desc");
	
	int i = 1;
	
	while (row = mysql_fetch_row(res))
		printf("%d. Vuelo: %s, Pasajeros: %s\n", i++, row[0], row[1]);
	
	mysql_free_result(res);
}

/*
 * ExecuteQuery
 *
 * Entradas:
 *		Un puntero a un puntero de un MYSQL_RES
 *		(Respuesta de una consulta MySQL),
 *		Un string de una consulta SQL
 *
 * Salidas:
 *		- El resultado de la ejecución de la consulta
 *		  SQL escrito en el puntero al puntero del
 *		  MYSQL_RES (parámetro de retorno).
 *		- Un entero con un -1 si la conexión falló,
 *		  un número de error si la ejecución de la
 *		  consulta SQL falló y un 0 si todo
 *		  salió bien (retorno de función). 
 *
 * Objetivo:
 *		Realizar todos los pasos necesarios para
 *		ejecutar una consulta SQL en la base de
 *		datos de MySQL:
 *			1. Conectarse a la Base de Datos
 *			2. Ejecutar la Consulta
 *			3. Almacenar el Resultado
 *			4. Cerrar la Conexión
 *		
 */
int ExecuteQuery(MYSQL_RES **res, const char *query) {
	
	MYSQL *conn = mysql_init(NULL);
	
	if (mysql_real_connect(conn, UD.server, UD.user, UD.password, UD.database, 0, NULL, 0) == NULL)
		return -1;
	
	int error = mysql_query(conn, query);
	
	if (error){
		unsigned int error = mysql_errno(conn);
		mysql_close(conn);
		return error;
	}
	
	*res = mysql_store_result(conn);
	
	mysql_close(conn);

	
	return 0;
}

#endif
