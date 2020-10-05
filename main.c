#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <mysql/mysql.h>
//#include "operativas.h"


#define clear() printf("\033[H\033[J")
#define CONSULTA_RESERVA "CALL consultaReserva("


void menuGeneral();
void consultaReservacion();
void DisplayResult(MYSQL_RES *res);
int ExecuteQuery(MYSQL_RES **res, const char *query);
int ejecutarConsulta(MYSQL_RES **res, const char *consulta);
void imprimirConsultaReserva(MYSQL_RES *res);

typedef struct {
	char *server;
	char *user;
	char *password;
	char *database;
} UserData;
UserData UD = {"localhost", "root", "ampie0422", "vuelos"};


int main(void){
	menuGeneral();
	return 0;
}


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
	  	printf("-%c -:-%c-\n",opcion,discard );
		opcion = '-';
		while(getchar()!='\n');
	  }

	  switch (opcion) {
	  	case '1':
		  printf("%s\n", "|===========Reservación de vuelos===========|");
		  break;
				
		case '2':
		  //clear();
		  printf("%s\n", "|==========Consulta de Reservación==========|");
		  consultaReservacion();
		  break;
				
		case '3':
		  printf("%s\n", "¡Ha salido exitosamente, gracias!");
		  //case 3;
		  free(texto);			
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
	if(!mysql_fetch_row(res)){
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
 *		esperados por el programa.
 *		
 */
int ejecutarConsulta(MYSQL_RES **res, const char *consulta){
	MYSQL *conn;

	conn = mysql_init(NULL);

	if(!mysql_real_connect(conn,UD.server, UD.user, UD.password, UD.database,0,NULL,0)){
		terminarError(conn);
	}
	if(mysql_query(conn,(consulta))){
		terminarError(conn);
	}
	*res = mysql_store_result(conn);
	mysql_close(conn);
	return 0;
}