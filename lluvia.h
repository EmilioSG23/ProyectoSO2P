#ifndef LLUVIA_H
#define LLUVIA_H

struct Lluvia{
    int modo;               //0 no lluvia, 1 aguacero, 2 diluvio
    int duracion;           //Duración de la lluvia en segundos
    int incremento;         //Incremento de cota en m/s
};

struct Lluvia iniciar_lluvia (int modo, int duracion, int incremento);
struct Lluvia iniciar_lluvia_modo (int modo);
void info_lluvia (struct Lluvia lluvia);

#endif