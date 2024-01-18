#include <stdio.h>
#include <stdlib.h>

#include "lluvia.h"

struct Lluvia iniciar_lluvia (int modo, int duracion, int incremento, double probabilidad){
    struct Lluvia lluvia;
    lluvia.modo = modo;
    lluvia.duracion = duracion;
    lluvia.incremento = incremento;
    lluvia.probabilidad = probabilidad * 100;
    return lluvia;
}

struct Lluvia iniciar_lluvia_modo (int modo, double probabilidad){
    struct Lluvia lluvia;
    if (modo == 1)      //Aguacero
        lluvia = iniciar_lluvia (1, 10, 2, probabilidad);
    else if (modo == 2) //Diluvio
        lluvia = iniciar_lluvia (2, 5, 4, probabilidad);
    else                //No lluvia
        lluvia = iniciar_lluvia (0, 5, 0, probabilidad);
    return lluvia;
}

void info_lluvia (struct Lluvia lluvia){
    printf("Lluvia de modo %d con una duracioń de %d y un incremento de %d\n", lluvia.modo, lluvia.duracion, lluvia.incremento);
}