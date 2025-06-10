#include <stdio.h>
#include <stdlib.h>
#include "syscalls.h"
#include <stdio.h>
#include "philosophers.h"
#include "string.h"

#define MAXPHILOS 9
#define MINPHILOS 3

typedef enum {
    THINKING=0,
    HUNGRY,
    EATING
} state_t;

typedef struct Philosopher {
    int pid;
    int rightForkId;
    int leftForkId;
    int haveToLeave;
    state_t state;
}philo_t;

typedef struct shared_data {
    philo_t philosophers[MAXPHILOS];
    TSem forks[MAXPHILOS];
    int cantPhilos; // Puntero a la cantidad de filósofos compartida
    TSem mutex; // Semáforo para proteger el acceso a la lista de filósofos
    char names[MAXPHILOS][14]; // Array de nombres para los filósofos
    char aux[MAXPHILOS][2]; // Array auxiliar para convertir el id a string
} shared_data_t;

shared_data_t *shared_data;
int16_t fileDescriptors[3] = {0, 1, 2};

void philo_init(int id, int pid);
void init_sems();
void destroy_sems();
void take_forks(int id);
void put_forks(int id);
void thinking();
void eating();
void philosophing(char ** args, int argc);
void printPhilos();
void createPhilosopher(int id);

void init_sems() {
    char sem_name[2];
    for (int i = 0; i < MAXPHILOS; i++) {
        sem_name[0] = '0' + i; 
        sem_name[1] = '\0'; 
        shared_data->forks[i]=semCreate(1, sem_name);
    }
    shared_data->mutex = semCreate(1, "mutexSem");
}

void destroy_sems() {
    for (int i = 0; i < MAXPHILOS; i++) {
        semClose(shared_data->forks[i]);
    }
    semClose(shared_data->mutex);
}

void take_forks(int id) {
    
    if(id%2 == 0){
        semWait(shared_data->mutex);
        shared_data->philosophers[id].leftForkId =id;
        semPost(shared_data->mutex);
        
        semWait(shared_data->forks[id]);

        semWait(shared_data->mutex);
        shared_data->philosophers[id].rightForkId = (id + 1) % (shared_data->cantPhilos);
        semPost(shared_data->mutex);
        semWait(shared_data->forks[shared_data->philosophers[id].rightForkId]);
        
    }else{
        semWait(shared_data->mutex);
        shared_data->philosophers[id].rightForkId = (id + 1) % (shared_data->cantPhilos);
        semPost(shared_data->mutex);
        semWait(shared_data->forks[shared_data->philosophers[id].rightForkId]);
        
        semWait(shared_data->mutex);
        shared_data->philosophers[id].leftForkId =id;
        semPost(shared_data->mutex);
        semWait(shared_data->forks[id]);
    }
    return;
}

void put_forks(int id) {
    semPost(shared_data->forks[shared_data->philosophers[id].leftForkId]);
    semPost(shared_data->forks[shared_data->philosophers[id].rightForkId]);
}


void thinking() {
    bussy_wait(8000000);
}

void eating(){
    bussy_wait(8000000);
}

void philo_init(int id, int pid) {
    semWait(shared_data->mutex);
    shared_data->philosophers[id].pid = pid;
    shared_data->philosophers[id].state = THINKING;
    shared_data->philosophers[id].rightForkId = -1;
    shared_data->philosophers[id].leftForkId = -1;
    shared_data->philosophers[id].haveToLeave = 0;
    semPost(shared_data->mutex);
}

void createPhilosopher(int id) {
    strcpy(shared_data->names[id], "philosopher ");
    shared_data->names[id][12] = '0' + id; 
    shared_data->names[id][13] = '\0';
    shared_data->aux[id][0] = '0'+ id;
    shared_data->aux[id][1] = '\0'; 
    char ** args = (char **) allocMem(2 * sizeof(char *));
    args[0]=shared_data->names[id];
    args[1]=shared_data->aux[id];
    uint64_t pid;
    pid = createProc((uint64_t) philosophing, args, 2, 5,fileDescriptors, 0);
    shared_data->philosophers[id].pid = pid;
}

void philosophing(char ** args, int argc) {
    if(argc != 2){
        printf("LA PUTA QUE TE PARIO, PASAME EL ID DEL FILOSOFO.\n");
        exit();
    }
    int id = atoi(args[1]);
    philo_init(id, getPid());
    int haveToLeave = 0;
    while (!haveToLeave) {
        thinking();
        semWait(shared_data->mutex);
        shared_data->philosophers[id].state = HUNGRY;
        semPost(shared_data->mutex);
        take_forks(id);
        semWait(shared_data->mutex);
        shared_data->philosophers[id].state = EATING;
        semPost(shared_data->mutex);
        eating();
        put_forks(id);
        semWait(shared_data->mutex);
        shared_data->philosophers[id].state = THINKING;
        if(shared_data->philosophers[id].haveToLeave) {
            haveToLeave = 1;
        }
        semPost(shared_data->mutex);
    }
    exit();
    freeMem(args);
    return;
}



void printPhilos() {
    semWait(shared_data->mutex);
    for (int i = 0; i < shared_data->cantPhilos; i++) {
        printf("%s",shared_data->philosophers[i].state == EATING? "E ": ". ");    
    }
    putchar('\n');
    semPost(shared_data->mutex);

}



void addPhilo() {
    if(shared_data->cantPhilos < MAXPHILOS) {
        semWait(shared_data->mutex);
        int aux = (shared_data->cantPhilos)++;
        semPost(shared_data->mutex);
        createPhilosopher(aux);
        printf("Agregado filosofo %d\n", aux );
    } else {
        printf("No se puede agregar mas filosofos, el maximo es nueve.\n");
    }
}

void removePhilo(){
    if(shared_data->cantPhilos > MINPHILOS) {
        semWait(shared_data->mutex);
        shared_data->philosophers[shared_data->cantPhilos - 1].haveToLeave = 1;
        shared_data->cantPhilos--;
        semPost(shared_data->mutex);
        waitProcess(shared_data->philosophers[shared_data->cantPhilos].pid);
        printf("Removido el filosofo %d\n", shared_data->cantPhilos);
    } else {
        printf("No se puede quitar un filosofo, el minimo es tres.\n");
    }
}

void mainPhilo(int argc, char * argv[]) {
    if (argc != 2) {
        printf( "Incorrecto pasaje de parametros, indicar solamente la cantidad de filosofos.\n", argv[1]);
        exit();
        return ;
    }
    shared_data = (shared_data_t *) allocMem(sizeof(shared_data_t));
    shared_data->cantPhilos = atoi(argv[1]);
    if (shared_data->cantPhilos < MINPHILOS || shared_data->cantPhilos > MAXPHILOS) {
        printf("La cantidad de filosofos debe ser entre %d y  %d\n",MINPHILOS, MAXPHILOS);
        exit();
        return;
    }
    init_sems();
    

    for (int i = 0; i < shared_data->cantPhilos; i++) {
        createPhilosopher(i); // Crear filósofos
    }
    //para que 
    for(int i = 0; i < shared_data->cantPhilos; i++) {
        unblockProc(shared_data->philosophers[i].pid);
    }
    //sleep(5); // Give time for all philosophers to initialize
    int aux=0;
    int exitFlag = 0;
    printf("Presione 'a' para agregar un filosofo, 'q' para quitar un filosofo, 's' para parar el proceso: \n");
    while(!exitFlag ) {
        printPhilos();
        char c;
                if(((c=getchar())== 'A' || c == 'a') || (c == 'q' || c == 'Q') || (c =='s' || c == 'S') || (c == 'c' || c == 'C')){}
                    if(c== 'q' || c == 'Q') {
                        removePhilo();
                    } else if(c == 's' || c == 'S') {
                        exitFlag = 1;
                        break;
                    } else if(c == 'A' || c == 'a') {
                        addPhilo();
                    }
                
        }
    semWait(shared_data->mutex);
    for(int i = 0; i < shared_data->cantPhilos; i++) {
        shared_data->philosophers[i].haveToLeave = 1; // Marcar todos los filósofos para que terminen
    }
    semPost(shared_data->mutex);
    
    for(int i = 0; i < shared_data->cantPhilos; i++) {
        waitProcess(shared_data->philosophers[i].pid);
    }
    destroy_sems();
    freeMem(shared_data);
    exit();
    return ;
}