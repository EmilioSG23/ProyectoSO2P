#include <stdio.h>
#include <stdlib.h>
#include <printf.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#include "central.h"
#include "lluvia.h"

#define MAX_PRODUCCION 150
#define MIN_PRODUCCION 100
#define CONSUMO_ELECTRICIDAD 5

#define DECREMENTO_COTA 5

typedef struct Hidroelectrica{
    struct Central* centrales;
    int num_centrales;
    double generacion_total;
    bool colapsado;
    bool inicio_generado;
}Hidroelectrica;

Hidroelectrica hidroelectrica;
struct Lluvia lluvias[3];

Hidroelectrica crear_hidroelectrica (struct Central* centrales, int num_centrales){
    Hidroelectrica hidroelectrica;
    hidroelectrica.centrales = centrales;
    hidroelectrica.num_centrales = num_centrales;
    hidroelectrica.generacion_total = 0;
    hidroelectrica.colapsado = false;
    hidroelectrica.inicio_generado = false;
    return hidroelectrica;
}

struct Lluvia seleccionar_lluvia (){
    srand(time(NULL));
    int rng = (rand() % 100);

    int prob_no_lluvia = 50;
    int prob_aguacero = 30;
    int prob_diluvio = 20;

    if (rng <= (prob_no_lluvia))
        return lluvias [0];
    else if (rng > prob_no_lluvia && rng <= ((prob_no_lluvia+prob_aguacero)))
        return lluvias [1];
    else
        return lluvias [2];
}

void* iniciarCentral (void* central_ptr){
    struct Central* central = (struct Central*) central_ptr;
    central->activado = true;
    central->lluvia = lluvias [0];
    while (1) {
        if (central -> activado){
            generarElectricidad (central);
            info_central (central);
        }else{
            if (central->cantidad_embalse >= central->cota_minima)
                reanudarCentral (central);
        }
        iniciarLluvia (central);
        if (central -> lluvia.modo != 0){
            central -> cantidad_embalse += central -> lluvia.incremento;
            central -> duracion_lluvia++ ;
            if (central -> duracion_lluvia == central -> lluvia.duracion){
                central -> duracion_lluvia = 0;
                central -> lluvia = lluvias [0];
            }
        }
        sleep(1);
    }
}

void iniciarLluvia (struct Central* central){
    struct Lluvia lluvia = seleccionar_lluvia();
    if (lluvia.modo != 0)
        central->lluvia = lluvia;
}

void reanudarCentral (struct Central* central){
    printf ("\e[1m\x1b[32m¡La central de tipo %d ha sido reanudado!\x1b[0m\e[m\n", central->tipo);
    central->activado = true;
}
void suspenderCentral (struct Central* central){
    central -> activado = false;
    printf ("\e[1m\x1b[31m¡La central de tipo %d ha sido suspendido!\x1b[0m\e[m\n", central -> tipo);
}

void generarElectricidad (struct Central* central){
    if (hidroelectrica.generacion_total + central -> generacion < MAX_PRODUCCION)
        hidroelectrica.generacion_total += central -> generacion;
    central -> cantidad_embalse -= DECREMENTO_COTA;
    if (hidroelectrica.generacion_total >= MAX_PRODUCCION ||
        central -> cantidad_embalse <= central -> cota_minima || central -> cantidad_embalse > central -> cota_maxima){
        suspenderCentral (central);
    }
}

void* monitorearElectricidad (){
    while(1){
        printf ("Electricidad producida: %f MW\n", hidroelectrica.generacion_total);
            
        if(hidroelectrica.generacion_total >= MIN_PRODUCCION && hidroelectrica.generacion_total <= MAX_PRODUCCION){
            if (!hidroelectrica.inicio_generado)
                hidroelectrica.inicio_generado = true;
            hidroelectrica.generacion_total -= CONSUMO_ELECTRICIDAD;
        }
        else if(!hidroelectrica.colapsado && hidroelectrica.inicio_generado){
            printf ("\e[1m\x1b[31m¡El sistema eléctrico COLAPSÓ, causando así un APAGÓN!\x1b[0m\e[m\n");
            printf ("\e[1mSe debe reanudar el sistema\e[m\n");
            hidroelectrica.colapsado = true;
            return 0;
        }
        sleep(1);
    }
}

void iniciarHidroelectrica (){
    lluvias [0] = iniciar_lluvia_modo (0); lluvias [1] = iniciar_lluvia_modo (1); lluvias [2] = iniciar_lluvia_modo (2);

    pthread_t sistema;
    pthread_t centrales_hilos [hidroelectrica.num_centrales];

    //Iniciar centrales y generación
    for (int i = 0; i < hidroelectrica.num_centrales; i++){
        hidroelectrica.centrales[i].activado = true;
        printf ("\e[1m\x1b[36m¡La central de tipo %d ha iniciado a generar electricidad!\x1b[0m\e[m\n", hidroelectrica.centrales[i].tipo);
        pthread_create (&centrales_hilos[i], NULL, iniciarCentral, &hidroelectrica.centrales[i]);
    }

    pthread_create (&sistema, NULL, monitorearElectricidad, NULL);

    //for (int i = 0; i < hidroelectrica.num_centrales; i++)
    //    pthread_join (centrales_hilos[i], NULL);
    
    pthread_join (sistema, NULL);

    for (int i = 0; i < hidroelectrica.num_centrales; i++)
        pthread_cancel (centrales_hilos[i]);
}

int main () {
    struct Central h1 = iniciar_central_tipo (1);
    struct Central h2 = iniciar_central_tipo (2);
    struct Central h3 = iniciar_central_tipo (3);

    struct Central centrales[] = {h1, h2, h3};
    //struct Central centrales[] = {h1};

    int num_centrales = (centrales != NULL) ? sizeof(centrales) / sizeof(centrales[0]) : 0;
    hidroelectrica = crear_hidroelectrica (centrales, num_centrales);

    iniciarHidroelectrica();

    return 0;
}