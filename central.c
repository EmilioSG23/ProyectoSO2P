#include <stdio.h>
#include <stdlib.h>

#include "central.h"

void crear_central (struct Central* central, int tipo, int cota_minima, int cota_maxima, int cantidad_embalse, int generacion, int id){
    central->id = id;
    central->tipo = tipo;
    central->cota_minima = cota_minima;
    central->cota_maxima = cota_maxima;
    central->generacion = generacion;

    central->activado = false;
    central->cantidad_embalse = cantidad_embalse;
    sem_init (&central->sem_central, 0, 0);
}

void crear_central_tipo (struct Central* central, int tipo, int id){
    if (tipo == 1)
        crear_central (central, 1,50,200,(50+200)/2,15, id);
    else if (tipo == 2)
        crear_central (central, 2,25,100,(25+100)/2,5, id);
    else if (tipo == 3)
        crear_central (central, 3,10,50,(10+50)/2,2, id);
    else
        printf("No es posible crear una central de tipo %d, solo de 1, 2, 3.\n", tipo);
}

void info_central_creada (struct Central central){
    printf("Central #%d de tipo %d creada, con una cota mínima de %d m, cota máxima de %d m, embalse inicial de %d m, generación de %d MW/s.\n", 
        central.id, central.tipo, central.cota_minima, central.cota_maxima,central.cantidad_embalse,central.generacion);
}

void info_central (struct Central* central){
    printf ("La \e[1mcentral #%d\e[m de \e[1m\x1b[36mtipo %d\x1b[0m\e[m genera %d MW/s, tiene un nivel de embalse de %d m.\n", central -> id, central->tipo,
         central->activado ? central->generacion : 0, central->cantidad_embalse);
}