# ProyectoSO2P
Para la compilación del programa convolucion basta con ingresar el comando make en la terminal y el programa será compilado.
Para eso, es necesario tener instalado el compliador gcc en su sistema operativo. Para comprobar si está instalado, use el comando:
	gcc --version
En caso de no reconocer la versión, siga los siguientes pasos para la instalación en Linux:
1. Ingrese el siguiente comando: sudo apt update
2. Ingrese el comando: sudo apt install build-essential
3. Verificar la versión, confirmando así su instalación

Para la compilación del programa, haga lo siguiente:
1. Ingresar a la terminal (consola)
2. Ingresar al directorio del programa
3. Ingresar el comando: make

Uso del programa:
Ingrese los parámetros correspondientes que el programa le pide. SOLO podrá ingresar la cantidad de centrales de cierto cierto y las probabilidades de cielo despejado y aguacero, el diluvio se completa automáticamente.

Tests:
Pruebe con algo sencillo:
H1: 1, H2: 1, H3: 1
Probabilidad de clima: 0.33 cada uno.