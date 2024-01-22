#include <stdio.h>
#include <stdlib.h>
#include <printf.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>

#include "sistema_electrico.h"
#include "central.h"
#include "lluvia.h"

#define MAX_PRODUCCION 150
#define MIN_PRODUCCION 100
#define CONSUMO_ELECTRICIDAD 10
#define DECREMENTO_COTA 5

//Variables globales
SistemaElectrico sistema_electrico;
struct Lluvia lluvias[3];
pthread_mutex_t generacion_mutex = PTHREAD_MUTEX_INITIALIZER;

int turnos_finalizados = 0;
pthread_mutex_t mutex_turnos_finalizados = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_turnos_finalizados;

//Headers
void reanudar_central (struct Central* central);
void suspender_central (struct Central* central);
void generar_electricidad (struct Central* central);
void iniciar_lluvia_central (struct Central* central);
void gestionar_centrales ();


struct Lluvia seleccionar_lluvia (){
    int rng = (rand() % 100);

    int prob_no_lluvia = lluvias[0].probabilidad;
    int prob_aguacero = lluvias[1].probabilidad;
    int prob_diluvio = lluvias[2].probabilidad;

    if (rng <= (prob_no_lluvia))
        return lluvias [0];
    else if (rng > prob_no_lluvia && rng <= ((prob_no_lluvia+prob_aguacero)))
        return lluvias [1];
    else
        return lluvias [2];
}

void iniciar_lluvia_central (struct Central* central){
    struct Lluvia lluvia = seleccionar_lluvia();
    central -> duracion_lluvia = 0;
    central -> lluvia = lluvia;
}

void reanudar_central (struct Central* central){
    central -> activado = true;
    sistema_electrico.generacion_actual += central -> generacion;
    printf ("\e[1m\x1b[32m¡La central #%d de tipo %d ha sido reanudado!\x1b[0m\e[m\n", central -> id, central -> tipo);
}
void suspender_central (struct Central* central){
    central -> activado = false;
    sistema_electrico.generacion_actual -= central -> generacion;
    printf ("\e[1m\x1b[31m¡La central #%d de tipo %d ha sido suspendido!\x1b[0m\e[m\n", central -> id, central -> tipo);
}

void generar_electricidad (struct Central* central){
    sistema_electrico.generacion_total += central -> generacion;
    //if (sistema_electrico.generacion_total + central -> generacion < MAX_PRODUCCION)

    central -> cantidad_embalse -= DECREMENTO_COTA;
}

void* iniciar_central (void* central_ptr){
    struct Central* central = (struct Central*) central_ptr;
    central->activado = true;
    sistema_electrico.generacion_actual += central -> generacion;
    iniciar_lluvia_central (central);
    sem_post (&central->sem_central);

    while (!sistema_electrico.colapsado) {
        sem_wait (&central->sem_central);
        info_central (central);

        if (central -> activado){
            pthread_mutex_lock (&generacion_mutex);
            generar_electricidad (central);
            pthread_mutex_unlock (&generacion_mutex);
        }
        // Indicación de turno finalizado al sistema eléctrico
        sem_post(&sem_turnos_finalizados);
        
        //Lluvia cayendo en la central
        central -> cantidad_embalse = central -> cantidad_embalse < central -> cota_maxima ? central -> cantidad_embalse + central -> lluvia.incremento: central -> cota_maxima;
        central -> duracion_lluvia ++ ;
        if (central -> duracion_lluvia >= central -> lluvia.duracion)
            iniciar_lluvia_central (central);

        sleep(1);
    }
}

void iniciar_sistema_electrico (double probabilidades_lluvia [3]){
    lluvias [0] = iniciar_lluvia_modo (0, probabilidades_lluvia [0]);
    lluvias [1] = iniciar_lluvia_modo (1, probabilidades_lluvia [1]);
    lluvias [2] = iniciar_lluvia_modo (2, probabilidades_lluvia [2]);

    pthread_t centrales_hilos [sistema_electrico.num_centrales];
    sem_init (&sem_turnos_finalizados, 0, 0);
    
    int tiempo = 0;
    srand(time(NULL));

    //Iniciar centrales y generación
    for (int i = 0; i < sistema_electrico.num_centrales; i++){
        //printf ("\e[1m\x1b[36m¡La central #%d de tipo %d ha iniciado a generar electricidad!\x1b[0m\e[m\n", sistema_electrico.centrales[i].id, sistema_electrico.centrales[i].tipo);
        pthread_create (&centrales_hilos[i], NULL, iniciar_central, &sistema_electrico.centrales[i]);
    }

    while(!sistema_electrico.colapsado){ 
        //Esperar a que los turnos de las centrales terminen
        for (int i = 0; i < sistema_electrico.num_centrales; i++)
            sem_wait (&sem_turnos_finalizados);

        pthread_mutex_lock (&generacion_mutex);
        printf ("\e[1m\x1b[34mElectricidad producida en total: %d MW, Electricidad producida: %d MW, tiempo transcurrido: %d segundos.\x1b[0m\e[m\n", sistema_electrico.generacion_total, sistema_electrico.generacion_actual, tiempo++);
        pthread_mutex_unlock (&generacion_mutex);
        bool energia_minima = sistema_electrico.generacion_total > MIN_PRODUCCION;
        bool energia_maxima = sistema_electrico.generacion_total < MAX_PRODUCCION;
        
        bool en_rango = energia_minima && energia_maxima;

        if (!sistema_electrico.inicio_generado && energia_minima)
                sistema_electrico.inicio_generado = true;

        if(en_rango){
            /*pthread_mutex_lock (&generacion_mutex);
            sistema_electrico.generacion_total -= CONSUMO_ELECTRICIDAD;
            pthread_mutex_unlock (&generacion_mutex);*/
        }else{if(!sistema_electrico.colapsado && sistema_electrico.inicio_generado){
            printf ("\e[1m\x1b[31m¡El sistema eléctrico COLAPSÓ, causando así un APAGÓN!\x1b[0m\e[m\n");
            printf ("\e[1mSe debe reanudar el sistema\e[m\n");
            sistema_electrico.colapsado = true;
            break;
        }}
        gestionar_centrales();
        printf ("\n");
        sleep(1);
    }

    for (int i = 0; i < sistema_electrico.num_centrales; i++)
        pthread_cancel (centrales_hilos[i]);
}

void gestionar_centrales () {
    for (int i = 0; i < sistema_electrico.num_centrales; i++){
        struct Central* central = &sistema_electrico.centrales [i];
        if (central -> activado){
            //Se suspende central, si no hay suficiente agua o si la generación próxima será mayor a MAX_PRODUCCION
            if (central -> cantidad_embalse < central -> cota_minima || sistema_electrico.generacion_total + sistema_electrico.generacion_actual >= MAX_PRODUCCION)
                suspender_central (central);
        }else{
            //Se reanuda central si hay suficiente agua
            if (central -> cantidad_embalse >= (central -> cota_minima + central -> cota_maxima)*0.40 
                && !(sistema_electrico.generacion_total+ sistema_electrico.generacion_actual + central -> generacion >= MAX_PRODUCCION))
                reanudar_central (central);
        }
        //printf ("Activado? %s\n", central -> activado ? "Si" : "No");
        sem_post (&sistema_electrico.centrales[i].sem_central);
    }
}

int main () {
    int cantidad_h1, cantidad_h2, cantidad_h3 = 0;
    int num_centrales = -1;

    //Ingreso de cantidad de centrales
    printf ("Ingrese cantidad de centrales de tipo:\n");
    while (num_centrales < 0){
        printf ("H1: "); scanf ("%d", &cantidad_h1);
        printf ("H2: "); scanf ("%d", &cantidad_h2);
        printf ("H3: "); scanf ("%d", &cantidad_h3);

        num_centrales = cantidad_h1 + cantidad_h2 + cantidad_h3;

        if(num_centrales < 0)
            printf ("La cantidad de centrales ingresada no es válida. Intentelo otra vez\n");
        else
            printf ("Cantidad total de centrales: %d\n", num_centrales);
    }

    struct Central centrales[num_centrales];
    for (int i = 0; i < num_centrales; i++){
        if (i < cantidad_h1)
            centrales [i] = crear_central_tipo (1, i+1);
        else if (i < cantidad_h1 + cantidad_h2)
            centrales [i] = crear_central_tipo (2, i+1);
        else
            centrales [i] = crear_central_tipo (3, i+1);
    }
    
    if (num_centrales == 0){
        printf ("\e[1mNo hay centrales eléctricas para producir energía...\e[m\n");return 0;
    }

    //Ingreso de probabilidades de lluvia
    double probabilidades_lluvia [3] = {-1,-1,-1};

    printf ("Ingrese probabilidades de lluvia (No lluvia, aguacero, diluvio) (Probabiidad entre 0 y 1):\n");
    while (probabilidades_lluvia [0] < 0 || probabilidades_lluvia [0] > 1){
        printf ("No lluvia: "); scanf ("%lf", &probabilidades_lluvia [0]);
    }

    if (probabilidades_lluvia [0] != 1){
        while (probabilidades_lluvia [1] < 0 || probabilidades_lluvia [1] > (1 - probabilidades_lluvia [0])){
            printf ("Aguacero: "); scanf ("%lf", &probabilidades_lluvia [1]);
        }
        probabilidades_lluvia [2] = 1 - (probabilidades_lluvia [0] + probabilidades_lluvia [1]);
        printf ("Diluvio (se asigna automáticamente): %0.2f\n", probabilidades_lluvia [2]);
    }else{
        probabilidades_lluvia [1] = 0;
        probabilidades_lluvia [2] = 0;
        printf ("Aguacero asignado a %0.2f\n", probabilidades_lluvia [1]);
        printf ("Diluvio asignado a %0.2f\n", probabilidades_lluvia [2]);
    }
    printf("\n");

    sistema_electrico = crear_sistema_electrico (centrales, num_centrales);
    iniciar_sistema_electrico (probabilidades_lluvia);

    return 0;
}