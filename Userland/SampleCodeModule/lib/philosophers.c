#include <syscalls.h>
#include <stdio.h>
#include <stdlib.h>
#include "processes.h"

#define MAXPHILOS 9 //Maxima cantidad de filósofos
#define MINPHILOS 3 //Minima cantidad de filósofos
#define RIGHT(philoId) (((philoId) + 1) % cantPhilo) //calculo del filósofo de la derecha
#define LEFT(philoId) (((philoId) + cantPhilo - 1) % cantPhilo) //calculo del filósofo de la izquierda
#define BUFFERSIZE 2 //Tamaño del buffer necesario para los filósofos

typedef enum {
    THINKING = 0,
    HUNGRY,
    EATING
} philoState;

typedef struct{
    philoState state;
    int pid;
    TSem semPhilo; //Semáforo para controlar el estado del filósofo
} philosopher_t;

philosopher_t philosophers[MAXPHILOS] = {0};

uint64_t cantPhilo = 0;
TSem mutexPhilo;
TSem printSemPhilo;

void startPhilosophers();
static void sumPhilo();
static void quitarPhilo();
static int philosophing(int argc, char *argv[]);
static void takeFork(int id);
static void putFork(int id);
static void testForks(int id);
static void dibujarPhilo();
static void waitTime();

void mainPhilo(int argc, char *argv[]){
    if(argc != 2){
        semWait(printSemPhilo);
        printf("Debe insertar un parametro con la cantidad de filosofos que desea.\n");
        semPost(printSemPhilo);
        exit();
        return;
    }
    int cantInicial = atoi(argv[1]);
    if(cantInicial < MINPHILOS || cantInicial > MAXPHILOS){
        semWait(printSemPhilo);
        printf("El número de filosofos debe estar entre %d y %d.\n", MINPHILOS, MAXPHILOS);
        semPost(printSemPhilo);
        exit();
        return;
    }
    mutexPhilo = semCreate(1,"mutexPhilo");
    if(mutexPhilo == NULL){
        semWait(printSemPhilo);
        printf("Error al crear el semaforo de mutex.\n");
        semPost(printSemPhilo);
        exit();
        return;
    }
    printSemPhilo = semCreate(1,"printSemPhilo");
    if(printSemPhilo == NULL){
        semWait(printSemPhilo);
        printf("Error al crear el semaforo de impresion.\n");
        semPost(printSemPhilo);
        exit();
        return;
    }
    semWait(printSemPhilo);
    printf("Los comandos son (A):Agregar, (Q):Quitar, (S):Salir\n");
    semPost(printSemPhilo);
    while(cantInicial){
        sumPhilo();
        cantInicial--;
    }
    startPhilosophers();
    cantPhilo = 0;
    exit();
    return;
}

void startPhilosophers(){
    char param;
    while(1){
        semWait(printSemPhilo);
        printf("Ingrese un comando: ");
        semPost(printSemPhilo);
        param = getchar();
        if(param == 'A' || param == 'a'){
            if(cantPhilo == MAXPHILOS){
                semWait(printSemPhilo);
                printf("No se pueden agregar mas filosofos, ya hay %d.\n", MAXPHILOS);
                semPost(printSemPhilo);
                continue;
            }
            sumPhilo();
        } else if(param == 'Q' || param == 'q'){
            quitarPhilo();
        } else if(param == 'S' || param == 's'){
            semWait(printSemPhilo);
            printf("Saliendo del programa de filosofos.\n");
            semPost(printSemPhilo);
            break;
        } else {
            semWait(printSemPhilo);
            printf("Comando no reconocido. Intente nuevamente.\n");
            semPost(printSemPhilo);
        }
    }

    for(int i = 0; i < cantPhilo; i++){
        killProcess(philosophers[i].pid);
        if(semClose(philosophers[i].semPhilo)){
            printf("Error al cerrar el semáforo del filosofo %d.\n", i);
            exit();
            return;
        }
    }
    if(semClose(mutexPhilo) == -1){
        printf("Error al cerrar el semaforo de mutex.\n");
        exit();
        return;
    }
    if(semClose(printSemPhilo) == -1){
        printf("Error al cerrar el semaforo de impresion.\n");
        exit();
        return;
    }
    return;
}

static void sumPhilo(){
    semWait(mutexPhilo);
    philosophers[cantPhilo].state = THINKING;
    philosophers[cantPhilo].semPhilo = semCreate(0, "semPhilo");
    if(philosophers[cantPhilo].semPhilo == NULL){
        semWait(printSemPhilo);
        printf("Error al crear el semaforo del filosofo %d.\n", cantPhilo);
        semPost(printSemPhilo);
        semPost(mutexPhilo);
        exit();
        return;
    }
    char bufferPhilo[BUFFERSIZE]= {0};
    itoa(cantPhilo, bufferPhilo, 10);
    char *argsPhilo[] = {"philosopher", bufferPhilo};
    int16_t fileDescriptors[] = {0,1,2};
    philosophers[cantPhilo].pid = createProc((uint64_t)philosophing, argsPhilo, 2, 1, fileDescriptors, 1);
    if(philosophers[cantPhilo].pid == -1){
        semWait(printSemPhilo);
        printf("Error al crear el proceso del filosofo %d.\n", cantPhilo);
        semPost(printSemPhilo);
        semPost(mutexPhilo);
        exit();
        return;
    }
    if(unblockProc(philosophers[cantPhilo].pid) == -1){
        semWait(printSemPhilo);
        printf("Error al desbloquear el proceso del filosofo %d.\n", cantPhilo);
        semPost(printSemPhilo);
        semPost(mutexPhilo);
        exit();
        return;
    }
    semWait(printSemPhilo);
    printf("Filosofo agregado.\n");
    semPost(printSemPhilo);
    cantPhilo++;
    semPost(mutexPhilo);
}

static void quitarPhilo(){
    semWait(mutexPhilo);
    if(cantPhilo <= MINPHILOS){
        semWait(printSemPhilo);
        printf("No se pueden quitar más filosofos, debe haber al menos %d.\n", MINPHILOS);
        semPost(printSemPhilo);
        return;
    }
    //espera activa hasta que los filósofos de la izquierda o el de la derecha no estén comiendo
    while(philosophers[LEFT(cantPhilo)].state == EATING && philosophers[RIGHT(cantPhilo)].state == EATING) {
		semPost(mutexPhilo);
		semWait(philosophers[cantPhilo].semPhilo);
		semWait(mutexPhilo);
	}
	killProcess(philosophers[cantPhilo].pid);
	if(semClose(philosophers[cantPhilo].semPhilo) == -1) {
        semWait(printSemPhilo);
		printf("Error cerrando el semaforo del filosofo %d\n", cantPhilo);
        semPost(printSemPhilo);
		exit();
		return;
	}
    semWait(printSemPhilo);
    printf("Filosofo %d eliminado.\n", cantPhilo);
    semPost(printSemPhilo);
    cantPhilo--;
    semPost(mutexPhilo);
}

/*Funcionamiento que vimos en clase de los filósofos*/

static int philosophing(int argc, char *argv[]){
    int arg = atoi(argv[1]);
    while(1){
        waitTime();
        takeFork(arg);
        waitTime();
        putFork(arg);
    }
    return 1;
}

static void takeFork(int id){
    semWait(mutexPhilo);
    philosophers[id].state = HUNGRY;
    testForks(id);
    semPost(mutexPhilo);
    semWait(philosophers[id].semPhilo);
}

static void putFork(int id){
    semWait(mutexPhilo);
    philosophers[id].state = THINKING;
    testForks(LEFT(id));
    testForks(RIGHT(id));
    semPost(mutexPhilo);
}

static void testForks(int id){
    if(philosophers[LEFT(id)].state != EATING && philosophers[RIGHT(id)].state != EATING && philosophers[id].state==HUNGRY ){
        philosophers[id].state = EATING;
        semPost(philosophers[id].semPhilo);
    }
    dibujarPhilo();
}

/*Dibujo simple que pide la consigna de los filósofos*/

static void dibujarPhilo() {
	semWait(printSemPhilo);
	for (int i = 0; i < cantPhilo; i++) {
        semWait(printSemPhilo);
		printf(philosophers[i].state == EATING ? "F " : ". ");
        semPost(printSemPhilo);
	}
	putchar('\n');
	semPost(printSemPhilo);
}

static void waitTime() {
    for (int i = 0; i < 1000000; i++); // Simula un tiempo de espera
}

