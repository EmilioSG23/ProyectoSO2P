#ifndef CENTRAL_H
#define CENTRAL_H

#include <stdbool.h>
#include <semaphore.h>
#include "lluvia.h"

struct Central{
    int id;
    int tipo;           //Tipo H1 (1), H2 (2), H3 (3)
    int cota_minima;    //Metros (m)
    int cota_maxima;    //Metros (m)
    int generacion;     //MV/s que genera

    //Generación
    bool activado;
    int duracion_lluvia;
    int cantidad_embalse;    //Cantidad de embalse actual
    struct Lluvia* lluvia;       //Lluvia que cae, solo puede ser aguacero o diluvio

    sem_t sem_central;
};

void crear_central (struct Central* central, int tipo, int cota_minima, int cota_maxima, int cantidad_embalse, int generacion, int id);
void crear_central_tipo (struct Central* central, int tipo, int id);
void info_central_creacion (struct Central central);
void info_central (struct Central* central);

#endif