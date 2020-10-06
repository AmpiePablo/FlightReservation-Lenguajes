#ifndef PDF_H
#define PDF_H

#include "pdfgen.h"
#include "pdfgen.c"
#include "structs.h"
#include <stdio.h>
#include <string.h>

#define POS_INI_X  100
#define POS_INI_Y  540

int generar(int idReserva, char *fecha, char *pCedulas[],char *pAsientos[],char *origen,char *fechaSalida, char *destino, char *fechaLlegada,int contCedulas, int contAsientos);


int generar(int idReserva, char *fecha, char *pCedulas[],char *pAsientos[],char *origen,char *fechaSalida, char *destino, char *fechaLlegada,int contCedulas, int contAsientos){
    struct pdf_info info = {
        .creator = "My software",
        .producer = "My software",
        .title = "My document",
        .author = "My name",
        .subject = "My subject",
        .date = "Today"};
    struct pdf_doc *pdf = pdf_create(PDF_A4_WIDTH, PDF_A4_HEIGHT, &info);
    pdf_set_font(pdf, "Times-Roman");
    pdf_append_page(pdf);
    //Identificador


    /*pdf_add_text(pdf,NULL,"Número de Reserva:",12,100,790,PDF_BLACK);
    pdf_add_text(pdf, NULL, idReserva, 12, 100, 770, PDF_BLACK); // el top de la pagina en y es 720 y hay que restarle para ir mas abajo*/

    pdf_add_text(pdf,NULL,"Origen del vuelo:",12,100,720,PDF_BLACK);
    pdf_add_text(pdf, NULL, origen, 12, 100, 700, PDF_BLACK); // el top de la pagina en y es 720 y hay que restarle para ir mas abajo

    pdf_add_text(pdf,NULL,"Hora de Salida:",12,100,680,PDF_BLACK);
    pdf_add_text(pdf, NULL, fechaSalida, 12, 100, 660, PDF_BLACK); // el top de la pagina en y es 720 y hay que restarle para ir mas abajo

    pdf_add_text(pdf,NULL,"Destino del vuelo:",12,100,640,PDF_BLACK);
    pdf_add_text(pdf, NULL,destino , 12, 100, 620, PDF_BLACK); // el top de la pagina en y es 720 y hay que restarle para ir mas abajo

    pdf_add_text(pdf,NULL,"Fecha de Llegada:",12,100,600,PDF_BLACK);
    pdf_add_text(pdf, NULL, fechaLlegada, 12, 100, 580, PDF_BLACK); // el top de la pagina en y es 720 y hay que restarle para ir mas abajo

    int y = POS_INI_Y;
    pdf_add_text(pdf,NULL,"Personas de la reservacion:",12,100,560,PDF_BLACK);
    for(int i = 0; i < contCedulas;i++){
		pdf_add_text(pdf, NULL, pCedulas[i], 12, 100, y, PDF_BLACK); // el top de la pagina en y es 720 y hay que restarle para ir mas abajo    	
		y-=20;
    }

    pdf_add_text(pdf,NULL,"Asientos de la reservacion:",12,100,y,PDF_BLACK);
    y-=20;
    for(int i = 0; i < contAsientos;i++){
		pdf_add_text(pdf, NULL, pAsientos[i], 12, 100, y, PDF_BLACK); // el top de la pagina en y es 720 y hay que restarle para ir mas abajo    	
		y-=20;
    }
    pdf_save(pdf, "factura");
    pdf_destroy(pdf);
    return 0;
}
#endif
