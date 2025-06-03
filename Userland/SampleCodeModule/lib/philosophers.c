#include <syscalls.h>
#include <stdio.h>

#define MAXPHILOS 10 //Maxima cantidad de filósofos
#define MINPHILOS 3 //Minima cantidad de filósofos
#define RIGHT(philoId) (((philoId) + 1) % cantPhilo) //calculo del filósofo de la derecha
#define LEFT(philoId) (((philoId) + cantPhilo - 1) % cantPhilo) //calculo del filósofo de la izquierda

typedef enum {
    THINKING,
    HUNGRY,
    EATING
} philosopherState;

typedef struct{
    philoState state;
    int id;
} philosopher_t;

philosopher_t philosophers[MAXPHILOS] = {0};

uint64_t cantPhilo = 0;
TSem mutexPhilo;
TSem printSemPhilo;

void mainPhilo(int argc, char *argv[]){
    if(argc != 2){
        printf("Debe insertar un parametro con la cantidad de filósofos que desea.\n");
        exit();
        return;
    }
    
    int cantInicial = atoi(argv[1]);
    if(cantInicial < MINPHILOS || cantInicial > MAXPHILOS){
        printf("El número de filósofos debe estar entre %d y %d.\n", MINPHILOS, MAXPHILOS);
        exit();
        return;
    }

    TSem mutexPhilo = buildSemaphore(1);
    if(mutex == NULL){
        printf("Error al crear el semáforo de mutex.\n");
        exit();
        return;
    }
    TSem printSemPhilo = buildSemaphore(1);
    if(printSemPhilo == NULL){
        printf("Error al crear el semáforo de impresión.\n");
        exit();
        return;
    }

    printf("Los comandos son (A):Agregar, (Q):Quitar, (S):Salir\n");
    while(cantInicial){
        sumPhilo();
        cantInicial--;
    }

    


}

//esta es la que pide la consigna para dibujar los filósofos
void dibujarPhilo() {
	semWait(printSemPhilo);
	for (int i = 0; i < cantPhilo; i++) {
		printf(philosophers[i].state == EATING ? "E " : ". ");
	}
	putchar('\n');
	semPost(printSemPhilo);
}

void sumPhilo(){
    semWait(mutexPhilo);
    philosophers[cantPhilo].state = THINKING;

    //Quiero preguntarle a los chicos esto

    printf("Filósofo %d agregado.\n", newPhilo->id);
    cantPhilo++;
    semPost(mutexPhilo);
}

void quitarPhilo(){

    //primero les voy a preguntar a los chicos lo mismo que con sum

}

int philosopherAction(int argc, char *argv[]){
    int arg = atoi(argv[1]);
    while(1){
        waitTime();
        takeFork(arg);
        waitTime();
        putFork(arg);
    }
    return 1;
}