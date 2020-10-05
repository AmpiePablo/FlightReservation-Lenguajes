#ifndef OPERATIVAS_H
#define OPERATIVAS_H

#include <stdio.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "get_input.h"


void MenuOperativo();
void MenuAviones();
void IncluirAvion();
void EliminarAvion();
void MostrarAviones();
void DisplayResult(MYSQL_RES*);
int ExecuteQuery(MYSQL_RES**, const char*);


typedef struct {
	char *server;
	char *user;
	char *password;
	char *database;
} UserData;

UserData UD = {"localhost", "root", "ampie0422", "vuelos"};

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
		"5. Regresar\n"
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
				//caso 2;
				break;
				
			case '3':
				//case 3;
				break;
				
			case '4':
				//case 4;
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
	printf("%d\n", mysql_num_fields(res));
																//
	if (opcion <= 0 || opcion > mysql_num_fields(res)+1) {		//
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
	printf("%s\n", modelo_query);
	free(modelo_query);										//
	printf("%s\n", "sigue3...1");
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
	if (opcion <= 0 || opcion > mysql_num_fields(res)+1) {	//
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
		free(input);
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
	char *insertion_query = (char*) malloc((56 + strlen(av->matricula) + strlen(input) + strlen(idModelo) + 1) * sizeof(char));																//
																						//
	sprintf(insertion_query,															//
		"insert into avion(matricula,anio,idModelo) values ('%s', %s, %s)",										//
		av->matricula, input, idModelo);												//
																						//
	mysql_free_result(res);																//
	int error = ExecuteQuery(&res, insertion_query);									//
																						//
	free(insertion_query);																//
	free(input);
																						//
	if (error) {																		//
	    puts("\nError insertando Avion (Asegurese de que la Matricula sea unica)\n");
	    printf("%d\n", error);	//
		goto LiberarAvion;																//
	}																					//
	printf("\nAvion de matricula %s insertado exitosamente.\n", av->matricula);			//
	
	
	LiberarAvion:								//
		//free(mo->descripcion);					//
		free(mo);								//
												//
	ErrorModelo:								// LIBERACIÓN DE MEMORIA
		//free(ma->descripcion);					// Y MANEJO DE ERRORES
		free(ma);								//
												//
	ErrorMarca:									//
		//free(av->matricula);					//
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
	printf("%s\n", "algo4");
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
EN PROGRESO
void CargarUsuarios() {
	
	puts("\nIngrese la ruta del archivo con los usuarios que se desean cargar\n\n-> "
	char *ruta = GetInput();
	
	FILE *file_ug = fopen(ruta, "r");
	
	if (file_ug == NULL) {
		puts("\nError: No se encontro el archivo\n");
		goto LiberarRuta;
	}
	
	strcat(ruta, "_reporte");
	FILE *file_rp = fopen(ruta);
	
	;
	//while not EOF
	//getline
	//split (into exactly 4, else write null)
	//append UsuarioGeneral to dynamic array (this step is maybe)
	//(or otherwise) insert every one as you go, while writing into the reporte file
	
	LiberarRuta:
		free(ruta);
}
*/

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
	
	if (error)
		return error;
	
	*res = mysql_store_result(conn);
	
	mysql_close(conn);
	
	return 0;
}

#endif