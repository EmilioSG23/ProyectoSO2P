#include <stdio.h>
#include <stdlib.h>
#include <printf.h>
#include <stdbool.h>

#include "central.h"
#include "lluvia.h"

#define MAX_PRODUCCION = 150
#define MIN_PRODUCCION = 100

#define DECREMENTO_COTA = 5

struct Electricidad{
    struct Central* centrales;
    int generacion_total;
};

int main () {
    struct Central h1 = iniciar_central_tipo (1);
    struct Central h2 = iniciar_central_tipo (2);
    struct Central h3 = iniciar_central_tipo (3);

    struct Central centrales[] = {h1, h2, h3};
    int num_centrales = sizeof(centrales)/sizeof(centrales[0]) || 0;

    for (int i = 0; i < num_centrales; i++)
        info_central (centrales[i]);

    return 0;
}