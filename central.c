#include <stdio.h>
#include <stdlib.h>

#include "central.h"

struct Central crear_central (int tipo, int cota_minima, int cota_maxima, int cantidad_embalse, int generacion){
    struct Central central;
    central.tipo = tipo;
    central.cota_minima = cota_minima;
    central.cota_maxima = cota_maxima;
    central.generacion = generacion;

    central.activado = false;
    central.cantidad_embalse = cantidad_embalse;
    central.duracion_lluvia = 0;
    
    return central;
}

struct Central crear_central_tipo (int tipo){
    struct Central central;
    if (tipo == 1)
        central = crear_central (1,50,200,(50+200)/2,15);
    else if (tipo == 2)
        central = crear_central (2,25,100,(25+100)/2,5);
    else if (tipo == 3)
        central = crear_central (3,10,50,(10+50)/2,2);
    else
        printf("No es posible crear una central de tipo %d, solo de 1, 2, 3.\n", tipo);
    return central;
}

void info_central_creada (struct Central central){
    printf("Central de tipo %d creada, con una cota mínima de %d m, cota máxima de %d m, embalse inicial de %d m, generación de %d MW/s.\n",
            central.tipo, central.cota_minima, central.cota_maxima,central.cantidad_embalse,central.generacion);
}

void info_central (struct Central* central){
    printf ("La central de tipo %d genera %d MW/s, tiene un nivel de embalse de %d m.\n", central->tipo,
         central->activado ? central->generacion : 0, central->cantidad_embalse);
}