#include <stdio.h>
#include <stdlib.h>
#include <printf.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#include "central.h"
#include "lluvia.h"

#define MAX_PRODUCCION 150
#define MIN_PRODUCCION 100
#define CONSUMO_ELECTRICIDAD 10

#define DECREMENTO_COTA 5

typedef struct Hidroelectrica{
    struct Central* centrales;
    int num_centrales;
    int generacion_total;
    bool colapsado;
    bool inicio_generado;
}Hidroelectrica;

Hidroelectrica hidroelectrica;
struct Lluvia lluvias[3];
pthread_mutex_t generacion_mutex = PTHREAD_MUTEX_INITIALIZER;

//Headers
void reanudar_central (struct Central* central);
void suspender_central (struct Central* central);
void generar_electricidad (struct Central* central);
void iniciar_lluvia_central (struct Central* central);

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

void* iniciar_central (void* central_ptr){
    struct Central* central = (struct Central*) central_ptr;
    central->activado = true;
    iniciar_lluvia_central (central);

    while (!hidroelectrica.colapsado) {
        info_central (central);
        if (central -> activado){
            pthread_mutex_lock (&generacion_mutex);
            generar_electricidad (central);
            pthread_mutex_unlock (&generacion_mutex);
        }
        
        if ((central -> activado) && (central -> cantidad_embalse < central -> cota_minima))
            suspender_central (central);

        if (!(central -> activado) && (central->cantidad_embalse >= central->cota_minima))
            reanudar_central (central);

        central -> cantidad_embalse = central -> cantidad_embalse < central -> cota_maxima ? 
            central -> cantidad_embalse + central -> lluvia.incremento: central -> cota_maxima;
        central -> duracion_lluvia ++ ;
        if (central -> duracion_lluvia >= central -> lluvia.duracion){
            iniciar_lluvia_central (central);
        }
        sleep(1);
    }
}

void iniciar_lluvia_central (struct Central* central){
    struct Lluvia lluvia = seleccionar_lluvia();
    central -> duracion_lluvia = 0;
    central -> lluvia = lluvia;
}

void reanudar_central (struct Central* central){
    central->activado = true;
    printf ("\e[1m\x1b[32m¡La central de tipo %d ha sido reanudado!\x1b[0m\e[m\n", central->tipo);
}
void suspender_central (struct Central* central){
    central -> activado = false;
    printf ("\e[1m\x1b[31m¡La central de tipo %d ha sido suspendido!\x1b[0m\e[m\n", central -> tipo);
}

void generar_electricidad (struct Central* central){
    hidroelectrica.generacion_total += central -> generacion;
    //if (hidroelectrica.generacion_total + central -> generacion < MAX_PRODUCCION)

    central -> cantidad_embalse -= DECREMENTO_COTA;
}

void gestionar_centrales () {

}

void iniciar_sistema_electrico (){
    lluvias [0] = iniciar_lluvia_modo (0); lluvias [1] = iniciar_lluvia_modo (1); lluvias [2] = iniciar_lluvia_modo (2);
    pthread_t centrales_hilos [hidroelectrica.num_centrales];
    int tiempo = 0;
    srand(time(NULL));

    //Iniciar centrales y generación
    for (int i = 0; i < hidroelectrica.num_centrales; i++){
        hidroelectrica.centrales[i].activado = true;
        printf ("\e[1m\x1b[36m¡La central de tipo %d ha iniciado a generar electricidad!\x1b[0m\e[m\n", hidroelectrica.centrales[i].tipo);
        pthread_create (&centrales_hilos[i], NULL, iniciar_central, &hidroelectrica.centrales[i]);
    }
    usleep(50*1000);   //Delay
    while(!hidroelectrica.colapsado){
        
        pthread_mutex_lock (&generacion_mutex);
        printf ("\e[1m\x1b[34mElectricidad producida: %d MW, tiempo transcurrido: %d segundos.\x1b[0m\e[m\n\n", hidroelectrica.generacion_total, tiempo++);
        bool en_rango = hidroelectrica.generacion_total > MIN_PRODUCCION && hidroelectrica.generacion_total < MAX_PRODUCCION;
        pthread_mutex_unlock (&generacion_mutex);

        if(en_rango){
            if (!hidroelectrica.inicio_generado)
                hidroelectrica.inicio_generado = true;
                
            /*pthread_mutex_lock (&generacion_mutex);
            hidroelectrica.generacion_total -= CONSUMO_ELECTRICIDAD;
            pthread_mutex_unlock (&generacion_mutex);*/

        }else{if(!hidroelectrica.colapsado && hidroelectrica.inicio_generado){
            printf ("\e[1m\x1b[31m¡El sistema eléctrico COLAPSÓ, causando así un APAGÓN!\x1b[0m\e[m\n");
            printf ("\e[1mSe debe reanudar el sistema\e[m\n");
            hidroelectrica.colapsado = true;
            break;
        }}
        gestionar_centrales();
        sleep(1);
    }

    for (int i = 0; i < hidroelectrica.num_centrales; i++)
        pthread_cancel (centrales_hilos[i]);

}

int main () {
    struct Central h1 = crear_central_tipo (1);
    struct Central h2 = crear_central_tipo (2);
    struct Central h3 = crear_central_tipo (3);

    struct Central centrales[] = {h1, h2, h3};
    //struct Central centrales[] = {h1};

    int num_centrales = (centrales != NULL) ? sizeof(centrales) / sizeof(centrales[0]) : 0;
    hidroelectrica = crear_hidroelectrica (centrales, num_centrales);

    iniciar_sistema_electrico ();

    return 0;
}