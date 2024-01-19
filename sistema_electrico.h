#ifndef SISTEMA_ELECTRICO_H
#define SISTEMA_ELECTRICO_H

typedef struct SistemaElectrico{
    struct Central* centrales;
    int num_centrales;
    int generacion_total;
    bool colapsado;
    bool inicio_generado;
}SistemaElectrico;

SistemaElectrico crear_sistema_electrico (struct Central* centrales, int num_centrales);

#endif