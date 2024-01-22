#ifndef LLUVIA_H
#define LLUVIA_H

struct Lluvia{
    int modo;               //0 no lluvia, 1 aguacero, 2 diluvio
    int duracion;           //Duración de la lluvia en segundos
    int incremento;         //Incremento de cota en m/s
    int probabilidad;
};

void iniciar_lluvia (struct Lluvia* lluvia, int modo, int duracion, int incremento, double probabilidad);
void iniciar_lluvia_modo (struct Lluvia* lluvia, int modo, double probabilidad);
void info_lluvia (struct Lluvia lluvia);

#endif