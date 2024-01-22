#include <stdio.h>
#include <stdlib.h>

#include "lluvia.h"

void iniciar_lluvia (struct Lluvia* lluvia, int modo, int duracion, int incremento, double probabilidad){
    lluvia->modo = modo;
    lluvia->duracion = duracion;
    lluvia->incremento = incremento;
    lluvia->probabilidad = probabilidad * 100;
}

void iniciar_lluvia_modo (struct Lluvia* lluvia, int modo, double probabilidad){
    if (modo == 1)      //Aguacero
        iniciar_lluvia (lluvia, 1, 10, 2, probabilidad);
    else if (modo == 2) //Diluvio
        iniciar_lluvia (lluvia, 2, 5, 4, probabilidad);
    else                //No lluvia
        iniciar_lluvia (lluvia, 0, 5, 0, probabilidad);
}

void info_lluvia (struct Lluvia lluvia){
    printf("Lluvia de modo %d con una duracioń de %d y un incremento de %d\n", lluvia.modo, lluvia.duracion, lluvia.incremento);
}