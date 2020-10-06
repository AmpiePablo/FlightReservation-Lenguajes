#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <mysql/mysql.h>
#include "operativas.h"

//MACROS
#define clear() printf("\033[H\033[J")
#define CONSULTA_RESERVA "CALL consultaReserva("
#define CONSULTA_EDAD "CALL diferenciaAnios("
#define TAM_MAXIMO 1000



void menuGeneral();
void consultaReservacion();
int ExecuteQuery(MYSQL_RES **res, const char *query);
int ejecutarConsulta(MYSQL_RES **res, const char *consulta);
void imprimirConsultaReserva(MYSQL_RES *res);
int tomarDatos(char *array[],int iter);
void reservarVuelo();
int validarCedulas(char *array[],int cont);
int validarAsiento(char *pAsientos[], int cont);
void creaReservacion(char *pCedulas[],char *pAsientos[],int contCedulas, int contAsientos);

UserData data = {"localhost", "root", "ampie0422", "vuelos"};

/*
 * main
 *
 * Entradas:
 *		Sin entradas
 *
 * Restricciones:
 *		Sin restricciones
 *
 * Objetivo:
 *		IDireccionar al menu del programa para generar
 *		los diferentes funcionalidades del sistema
 *		
 */
int main(void){
	//menuGeneral();
	LoginOperativo();
	return 0;
}


/*
 * terminarError
 *
 * Entradas:
 *		La conexion con la base de datos en mysql
 *
 * Restricciones:
 *		que entre una conexion o un valor nulo
 *
 * Objetivo:
 *		Identificar si algun proceso, consulta, que se realiza
 *		para leer, escribir la base de datos es valido o no
 *		
 */
void terminarError(MYSQL *con){
	fprintf(stderr, "%s\n",mysql_error(con));
	exit(1);
}
/*
 * menuGeeneral
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
 *		Presentar las opciones ogenerakes al usuario para
 *		que este pueda reservar, consultar información
 *		acerca de los vuelos que ha reservado o desea reservar
 *		
 */
void menuGeneral(){
	char opcion;
	char discard;
	char *texto=
	  "|==========Menu General==========|\n"
	  "\n"
	  "1. Reservar Vuelo\n"
      "2. Consulta de Reservación\n"
	  "3. Salir\n"
	  "Digite la opcion que desea realizar:  ";
	for(;;){
	  printf("%s",texto);
	  opcion = getchar();
	  discard = getchar();
	  if (discard != '\n') {	
		opcion = '-';
		while(getchar()!='\n');
	  }

	  switch (opcion) {
	  	case '1':
		  printf("%s\n", "|===========Reservación de vuelos===========|");
		  reservarVuelo();
		  break;
				
		case '2':
		  //clear();
		  printf("%s\n", "|==========Consulta de Reservación==========|");
		  consultaReservacion();
		  break;
				
		case '3':
		  printf("%s\n", "¡Ha salido exitosamente, gracias!");
		  //case 3;
		 // free(texto);			
		  return;
		default:
		   puts("Error: Entrada Invalida");
	  }
	}
}

/*
 * consultaReservacion
 *
 * Entradas:
 *		Un identificador en entero y un pasaporte de caracteres
 *
 * Restricciones:
 *		si no hay resultados con los parametros ingresados, 
 *		el sistema mostrara un mensaje de que no existen 
 *		rersultados del mismo
 *
 * Objetivo:
 *		Consultar información relacionada con la reserva de un vuelo
 *		dentro de la base de datos solicita los paremetros para ejecutar
 *		un store procedure de la base de datos de mysql
 *		
 */
void consultaReservacion(){
	MYSQL_RES *res;
	MYSQL_ROW row;

	char consulta[100];
	char idReserva[5];
	char pasaporte[25];
	strcpy(consulta,CONSULTA_RESERVA);

	printf("%s","Digite el identificador de la reserva: ");
	scanf("%s",&idReserva);

	strcat(consulta,idReserva);
	strcat(consulta,",");

	printf("%s","Digite su pasaporte: ");
	scanf("%s",&pasaporte);

	strcat(consulta,pasaporte);
	strcat(consulta,")");

	ejecutarConsulta(&res,consulta);
	imprimirConsultaReserva(res);
	mysql_free_result(res);
}

/*
 * imprimirConsultaReserva
 *
 * Entradas:
 *		Un arreglo de resultados de una consulta realizada
 *
 * Restricciones:
 *		Que entre un tipo mysql_res que es el resultado de una 
 *		consulta realizada con anterioridad
 *
 * Objetivo:
 *		Representar en pantalla toda la información de la consulta realizada
 *		que este relacionada a una reservación de un vuelo.
 *		
 */

void imprimirConsultaReserva(MYSQL_RES *res){
	MYSQL_ROW row;
	char *arrayColumn[] = {"Reservación","Pasaporte","Nombre","Apellido1","Apellido2","fecha de Reservación","Fila","Asiento","Precio","Origen","Destino" ,"Fecha de Salida"};
	int fields = mysql_num_fields(res);
	if(mysql_num_rows(res)<1){
		printf("%s\n", "NO SE ENCONTRÓ NINGUNA INFORMACIÓN CON LOS DATOS BRINDADOS");
	}
	while((row = mysql_fetch_row(res))){
		for(int i = 0; i < fields; i++){
			printf("%s: %s\n",arrayColumn[i], row[i]);
		}printf("\n");
	}
}



/*
 * ejecutarConsulta
 *
 * Entradas:
 *		un conjunto de caracteres que representan una consulta y 
 *		un arreglo que representa el resultado de la consulta 
 *
 * Restricciones:
 *		Que entre un tipo mysql_res que es el resultado de la 
 *		consulta que se va a realizar y la consulta sea valida
 *
 * Objetivo:
 *		Conectar el programa con la base de datos de mysql
 *		para así poder realizar la consulta y optener los resultados
 *		esperados por el programa
 *		
 */
int ejecutarConsulta(MYSQL_RES **res, const char *consulta){
	MYSQL *conn;

	conn = mysql_init(NULL);

	if(!mysql_real_connect(conn,data.server, data.user, data.password, data.database,0,NULL,0)){
		terminarError(conn);
	}
	if(mysql_query(conn,(consulta))){
		terminarError(conn);
	}
	*res = mysql_store_result(conn);
	mysql_close(conn);
	return 0;
}



/*
 * reservarVuelo
 *
 * Entradas:
 *		Un conjunto de caracteres que son, cedulas, asientos...
 *
 * Restricciones:
 *		Que el tamaño del arreglo siempre sea el correcto y que no se pase
 *		del limite del mismo.
 *
 * Objetivo:
 *		tomar un conjunto de pasaportes de usuarios, y nombres de asientos en un vuelo
 *		para poder hacer la reservación de un asiento, direcciona a diferentes funciones
 *		de recolección de datos, validaciones y demás.

 */	

void reservarVuelo(){
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *arrayCedulas[TAM_MAXIMO];
	char *arrayAsientos[TAM_MAXIMO];

	printf("%s\n","Digite los pasaportes de la reserva en formato {pasaporte1,pasaporte2,....,pasaporteN}" );
	int cantCedulas = tomarDatos(arrayCedulas,1);
	if(cantCedulas == -1) return;

	printf("%s\n","Digite los asientos de reserva para cada pasaporte asignado {asiento1,asiento2,....,asientoN}" );
	int cantAsientos = tomarDatos(arrayAsientos,2);
	if(cantAsientos == -1) return;

	if(cantCedulas > cantAsientos){
		int edad;
		for(int i = 0; i < cantCedulas; i++){
			char consulta[100];
			strcpy(consulta,CONSULTA_EDAD);
			strcat(consulta,arrayCedulas[i]);
			strcat(consulta,")");

			ejecutarConsulta(&res,consulta);
			row = mysql_fetch_row(res);
			edad = atoi(row[0]);
			printf("%d\n",edad );
			//edad = strtol(res[0],NULL,10);
			if(edad <= 3){
				printf("%s\n","ERROR, No ingresó ningun infante, por favor digite los asientos completos" );
				break;
			}
		}
	}
	creaReservacion(arrayCedulas,arrayAsientos,cantCedulas,cantAsientos);

}

void creaReservacion(char *pCedulas[],char *pAsientos[],int contCedulas, int contAsientos){
	MYSQL_RES *res;
	MYSQL_ROW row;
	//inserto la reservacion
	char *insertQuery = (char*) malloc((46 * sizeof(char)));
	sprintf(insertQuery,
		"insert into reservacion(fecha) "
		"values(now()) ");
	ejecutarConsulta(&res,insertQuery);
	mysql_free_result(res);


	//extraigo ultima reserva
	int id = 0;
	ejecutarConsulta(&res,"select MAX(idReservacion) from reservacion;");
	row = mysql_fetch_row(res);
	id = atoi(row[0]);

	char *idUsuario;
	int idUsuarioInt;

	if(contCedulas>contAsientos){
		for(int i = 0; i < contCedulas; i++){
			idUsuario = (char*) malloc((64 +strlen(pCedulas[i])+1) * sizeof(char));
			sprintf(idUsuario,
				"select idUsuarioGeneral "
				"from usuarioGeneral "
				"where pasaporte = '%s'", pCedulas[i]);

			ejecutarConsulta(&res,idUsuario);
			row = mysql_fetch_row(res);
			idUsuarioInt = atoi(row[0]);
		}
	}

	printf("%d\n->",id );


	for(int i = 0; i < contCedulas; i++){
		printf("%s\n", pCedulas[i]);
	}
	free(insertQuery);
}

/*
 * tomarDatos
 *
 * Entradas:
 *		un arreglo, y un tipo de iterador
 *
 * Restricciones:
 *		que los arreglos sean los correctos, el iter sea el necesario
 *		que el formato de entrada del texto sea el correcto
 *
 * Objetivo:
 *		tomar un conjunto de cedulas o pasaportes del usuario
 *		para poder realizar una reservación de un asiento en un vuelo
 *		llama a funciones para validar todo lo que sea necesario
 */	
int tomarDatos(char *array[], int iter){
	char *cadena;
	printf("%s","-->" );
	cadena = GetInput();
	char delimiter[] = ",";

	char *actual = strtok(cadena,delimiter);
	int cont = 0;

	while (actual != NULL){
		actual = strtok(NULL, delimiter);
		cont++;
	}

	array[0] = cadena;

	for (int i = 1; i < cont; i++) {
		while (*cadena != '\0') {
			cadena++;
		}
		cadena++;
		array[i] = cadena;
	}
	if(iter==1){
		if(validarCedulas(array,cont)==-1) return -1;
		else return cont;
	}
	else{
		if(validarAsiento(array,cont)==-1)return -1;
		else return cont;	
	} 
	return cont;
}



/*
 * validarCedulas
 *
 * Entradas:
 *		un arreglo, y un tipo de iterador
 *
 * Restricciones:
 *		que los arreglos sean los correctos, el iter sea el necesario
 *		que el formato de entrada del texto sea el correcto
 *
 * Objetivo:
 *		ejecuta consultas en la base de datos lo cual usa un procedure
 *		que valida si hay una cedula que no exista o que no esté registrada en la base de datos del sistema
 *
 */	
int validarCedulas(char *pCedulas[],int cont){
	MYSQL_RES *res;
	MYSQL_ROW row;

	char *cedulasQuery;

	for(int i = 0; i < cont; i++){

		cedulasQuery = (char*) malloc((58 + strlen(pCedulas[i])+1) * sizeof(char));

		sprintf(cedulasQuery,
		"select pasaporte "
		"from usuarioGeneral "
		"where pasaporte = '%s'", pCedulas[i]);


		mysql_free_result(res);
		ejecutarConsulta(&res, cedulasQuery);

		if(mysql_num_rows(res) < 1) {

			printf("%s%s%s\n", "ERROR, la cedula: ",pCedulas[i]," No existe en la base de datos");
			return -1;

		}
	}
	free(cedulasQuery);

	return 0;

}


/*
 * validarAsiento
 *
 * Entradas:
 *		un arreglo, y un contador
 *
 * Restricciones:
 *		que los arreglos sean los correctos, el iter sea el necesario
 *		que el formato de entrada del texto sea el correcto
 *
 * Objetivo:
 *		ejecuta consultas en la base de datos lo cual usa un procedure
 *		que valida si hay un asiento que no exista o que no esté registrada en la base de datos del sistema
 *		o bien verifica que el asiento este disponible
 *
 */	
int validarAsiento(char *pAsientos[], int cont){
	MYSQL_RES *res;
	MYSQL_ROW row;

	char *asientoQuery;

	for(int i = 0; i < cont; i++){
		asientoQuery = (char*) malloc((79+ strlen(pAsientos[i])+1) * sizeof(char));

		sprintf(asientoQuery,
		"select nombre "
		"from asiento "
		"where estaOcupado = 0 and nombre = '%s'", pAsientos[i]);
		//mysql_free_result(res);
		ejecutarConsulta(&res, asientoQuery);

		if(mysql_num_rows(res) <1 ){
			printf("%s%s%s\n", "ERROR, el asiento: ",pAsientos[i]," No existe en la base de datos/Se encuentra reservado");
			return -1;
		}
	}
	return 0;
}
