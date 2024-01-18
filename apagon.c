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

//Variables globales
Hidroelectrica hidroelectrica;
struct Lluvia lluvias[3];
pthread_mutex_t generacion_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cond_central_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t* cond_central;

//Headers
void reanudar_central (struct Central* central);
void suspender_central (struct Central* central);
void generar_electricidad (struct Central* central);
void iniciar_lluvia_central (struct Central* central);
void gestionar_centrales ();

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
    central->activado = true;
    printf ("\e[1m\x1b[32m¡La central #%d de tipo %d ha sido reanudado!\x1b[0m\e[m\n", central -> id, central->tipo);
}
void suspender_central (struct Central* central){
    central -> activado = false;
    printf ("\e[1m\x1b[31m¡La central #%d de tipo %d ha sido suspendido!\x1b[0m\e[m\n", central -> id, central -> tipo);
}

void generar_electricidad (struct Central* central){
    hidroelectrica.generacion_total += central -> generacion;
    //if (hidroelectrica.generacion_total + central -> generacion < MAX_PRODUCCION)

    central -> cantidad_embalse -= DECREMENTO_COTA;
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

void iniciar_sistema_electrico (double probabilidades_lluvia [3]){
    lluvias [0] = iniciar_lluvia_modo (0, probabilidades_lluvia [0]);
    lluvias [1] = iniciar_lluvia_modo (1, probabilidades_lluvia [1]);
    lluvias [2] = iniciar_lluvia_modo (2, probabilidades_lluvia [2]);

    pthread_t centrales_hilos [hidroelectrica.num_centrales];
    cond_central = (pthread_cond_t*) malloc(hidroelectrica.num_centrales * sizeof(pthread_cond_t));
    for (int i = 0; i < hidroelectrica.num_centrales; i++) 
        pthread_cond_init(&cond_central[i], NULL);
    
    int tiempo = 0;
    srand(time(NULL));

    //Iniciar centrales y generación
    for (int i = 0; i < hidroelectrica.num_centrales; i++){
        hidroelectrica.centrales[i].activado = true;
        printf ("\e[1m\x1b[36m¡La central #%d de tipo %d ha iniciado a generar electricidad!\x1b[0m\e[m\n", hidroelectrica.centrales[i].id, hidroelectrica.centrales[i].tipo);
        pthread_create (&centrales_hilos[i], NULL, iniciar_central, &hidroelectrica.centrales[i]);
    }
    usleep(50*1000);   //Delay
    while(!hidroelectrica.colapsado){
        // Enviar señales a los semáforos de las centrales
        for (int i = 0; i < hidroelectrica.num_centrales; i++)
            pthread_cond_signal(&cond_central[i]);
        
        pthread_mutex_lock (&generacion_mutex);
        printf ("\e[1m\x1b[34mElectricidad producida: %d MW, tiempo transcurrido: %d segundos.\x1b[0m\e[m\n\n", hidroelectrica.generacion_total, tiempo++);
        bool energia_minima = hidroelectrica.generacion_total > MIN_PRODUCCION;
        bool energia_maxima = hidroelectrica.generacion_total < MAX_PRODUCCION;
        pthread_mutex_unlock (&generacion_mutex);
        bool en_rango = energia_minima && energia_maxima;

        if (!hidroelectrica.inicio_generado && energia_minima)
                hidroelectrica.inicio_generado = true;

        if(en_rango){
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

void gestionar_centrales () {

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

    hidroelectrica = crear_hidroelectrica (centrales, num_centrales);
    iniciar_sistema_electrico (probabilidades_lluvia);

    return 0;
}