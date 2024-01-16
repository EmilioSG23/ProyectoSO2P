#ifndef CENTRAL_H
#define CENTRAL_H

#include <stdbool.h>

struct Central{ 
    int tipo;           //Tipo H1 (1), H2 (2), H3 (3)
    int cota_minima;    //Metros (m)
    int cota_maxima;    //Metros (m)
    double cantidad_embalse;//Cantidad de embalse actual
    int generacion;     //MV/s que genera
    bool activado;
};

struct Central iniciar_central (int tipo, int cota_minima, int cota_maxima, double cantidad_embalse, int generacion);
struct Central iniciar_central_tipo (int tipo);
void info_central (struct Central central);


#endif