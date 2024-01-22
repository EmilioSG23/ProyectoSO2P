#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "sistema_electrico.h"

void crear_sistema_electrico (SistemaElectrico* sistema_electrico, struct Central* centrales, int num_centrales){
    sistema_electrico->centrales = centrales;
    sistema_electrico->num_centrales = num_centrales;
    sistema_electrico->generacion_total = 0;
    sistema_electrico->generacion_actual = 0;
    sistema_electrico->colapsado = false;
    sistema_electrico->inicio_generado = false;
}