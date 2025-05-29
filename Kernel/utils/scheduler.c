#include "scheduler.h"

#define MAX_PROCESSES 100
#define PID_SHELL 0

#define FQ 2


typedef struct schedulerCDT{
    linkedListADT totalProcesses;
    linkedListADT readyList;
    linkedListADT blockedList;
    TPCB actualProcess;
    int32_t quantum; //el quantum va a depender directamente de la prioridad del proceso
    uint64_t actualPid;
    uint64_t nextPid;
    uint64_t cantProcesses;
} schedulerCDT;


static void idle();
schedulerADT scheduler = NULL;
uint8_t createdScheduler = 0;

void createScheduler(){
    printf("creando scheduler\n");
    scheduler =(schedulerADT) allocMemory(sizeof(schedulerCDT));
    if(scheduler == NULL){
        return;
    }
    printf("creo lista\n");
    scheduler->totalProcesses = createList();
    if(scheduler->totalProcesses == NULL){
        freeMemory(scheduler);
        return;
    }

    printf("scheduler->totalProcesses = %d\n", (uint64_t)scheduler->totalProcesses);
    scheduler->readyList = createList();
    if(scheduler->readyList == NULL){
        freeMemory(scheduler->totalProcesses);
        freeMemory(scheduler);
        return;
    }
    printf("scheduler->readyList = %d\n", (uint64_t)scheduler->readyList);
    scheduler->blockedList = createList();
    if(scheduler->blockedList == NULL){
        freeMemory(scheduler->readyList);
        freeMemory(scheduler->totalProcesses);
        freeMemory(scheduler);
        return;
    }
    printf("scheduler->blockedList = %d\n", (uint64_t)scheduler->blockedList);
    
    scheduler->actualProcess = NULL;    
    scheduler->actualPid = -1;
    scheduler->nextPid = 0;
    scheduler->quantum = 0;
    createdScheduler = 1;
    char *idleArg[] = { "idle" };
    int16_t fileDescriptors[] = {-1,-1,2};
    createProcess((uint64_t) idle, idleArg, 1, 1, fileDescriptors, 1);
    scheduler->cantProcesses = 0;
}

//este bro, va a agarrar un proceso, si es que hay, lo va a METER running agarrando uno de los readys y lo va  amandar a la cola de bloqueados o de ready dependiendo el caso
//esto va a devolver la stackPosition del procesoActual para que el scheduler lo pueda usar
/*uint64_t changeProcess(uint64_t actualRSP){ 
    
    if(createdScheduler == 0){ //si no está creado el scheduler devolvemos el puntero a la pila del proceso actual
        return actualRSP;
    }
    schedulerADT scheduler = getScheduler();
    scheduler->quantum--; //simulo el tiempo del proceso en ejecución
    if(scheduler->quantum > 0 || scheduler->cantProcesses == 0){ 
        return actualRSP;
    }
    //si el proceso actual es el kernel, entonces tengo que sacar un proceso el primer proceso de la lista de listos y ponerlo a correr

    if(scheduler->actualPid == SHELLPID){      //si el proceso no existe, entonces no hay nada para correr    
        scheduler->actualProcess = (TPCB) next(scheduler->readyList);
        if(scheduler->actualProcess == NULL){   
            return actualRSP;
        }
        
        //actualizo los datos del proceso 
        
        scheduler->quantum = scheduler->actualProcess->priority;
        scheduler->actualProcess->status = RUNNING;
        scheduler->actualPid = scheduler->actualProcess->pid;
        return scheduler->actualProcess->stackPos;
    }

    //si el proceso actual no es el kernel y no es NULL, entonces guardo la posición del RSP del proceso actual y saco de RUNNING al proces y lo meto en la lista de READY
    //en el caso de que el proceso actual sea KILLED, entonces lo libero y no lo meto en la lista de READY
    printf("scheduler->actualProcess: %d\n", (uint64_t)scheduler->actualProcess);
    printf("scheduler->actualProcess->pid: %d\n", scheduler->actualProcess->pid);
    if(scheduler->actualProcess != NULL){
        scheduler->actualProcess->stackPos = actualRSP; //guardo la posición de la pila del proceso actual
        printf("scheduler->actualProcess->status: %d\n", scheduler->actualProcess->status);
        if(scheduler->actualProcess->status == RUNNING){
            scheduler->actualProcess->status = READY;
            addNode(scheduler->readyList, scheduler->actualProcess);
        }
        else if(scheduler->actualProcess->status == KILLED){
            freeProcess(scheduler->actualProcess);
        }
    }
    //si el proceso actual es NULL, entonces tengo que sacar un proceso de la lista de listos y ponerlo a correr
    //si no hay procesos en la lista de listos, entonces no hay nada para correr y lo pongo a correr al idle
    TPCB auxProcess = getFirst(scheduler->readyList);
    printf("auxProcess: %d\n", (uint64_t)auxProcess);
    if(auxProcess == NULL){
        TPCB process = getProcess(IDLEPROCESS);
        printf("process: %d\n", (uint64_t)process);
        if(process == NULL){
            return actualRSP;
        }
        else{

            scheduler->actualProcess = auxProcess;
            return scheduler->actualProcess->stackPos;
        }
        printf("No hay procesos listos, corriendo idle\n");
    }
    addNode(scheduler->readyList,auxProcess); 
    scheduler->actualProcess = auxProcess;
    scheduler->actualProcess->status = RUNNING;
    scheduler->quantum = scheduler->actualProcess->priority;
    scheduler->actualPid = scheduler->actualProcess->pid;
    return scheduler->actualProcess->stackPos;
}*/
uint64_t changeProcess(uint64_t actualRSP){ 
    
    if(createdScheduler == 0){ //si no está creado el scheduler devolvemos el puntero a la pila del proceso actual
        return actualRSP;
    }
    schedulerADT scheduler = getScheduler();
    scheduler->quantum--; //simulo el tiempo del proceso en ejecución
    
    if(scheduler->quantum > 0 || scheduler->cantProcesses == 0){ 
        return actualRSP;
    }
    //si el proceso actual es el kernel, entonces tengo que sacar un proceso el primer proceso de la lista de listos y ponerlo a correr

    if(scheduler->actualPid <= SHELLPID){      //si el proceso no existe, entonces no hay nada para correr    
        scheduler->actualProcess = (TPCB) getFirst(scheduler->readyList);
        if(scheduler->actualProcess == NULL){   
            return actualRSP;
        }
        //actualizo los datos del proceso 
        scheduler->quantum = scheduler->actualProcess->priority;
        scheduler->actualProcess->status = RUNNING;
        scheduler->actualPid = scheduler->actualProcess->pid;
        return scheduler->actualProcess->stackPos;
    }

    //si el proceso actual no es el kernel y no es NULL, entonces guardo la posición del RSP del proceso actual y saco de RUNNING al proces y lo meto en la lista de READY
    //en el caso de que el proceso actual sea KILLED, entonces lo libero y no lo meto en la lista de READY
    if(scheduler->actualProcess != NULL){
        scheduler->actualProcess->stackPos = actualRSP; //guardo la posición de la pila del proceso actual
        if(scheduler->actualProcess->status == RUNNING){
            scheduler->actualProcess->status = READY;
            addNode(scheduler->readyList, scheduler->actualProcess);
        }
        else if(scheduler->actualProcess->status == KILLED){
            freeProcess(scheduler->actualProcess);
        }
    }
    //si el proceso actual es NULL, entonces tengo que sacar un proceso de la lista de listos y ponerlo a correr
    //si no hay procesos en la lista de listos, entonces no hay nada para correr y lo pongo a correr al idle
    TPCB auxProcess = getFirst(scheduler->readyList);
    if(auxProcess == NULL){
        TPCB process = getProcess(IDLEPROCESS);
        if(process == NULL){
            return actualRSP;
        }
        else{
            scheduler->actualProcess = process;
            scheduler->actualPid = process->pid;
            return scheduler->actualProcess->stackPos;
        }
    }
    //addNode(scheduler->readyList,auxProcess); 
    scheduler->actualProcess = auxProcess;
    scheduler->actualProcess->status = RUNNING;
    scheduler->quantum = scheduler->actualProcess->priority;
    scheduler->actualPid = scheduler->actualProcess->pid;
    return scheduler->actualProcess->stackPos;
}

/* Devuelve 0 si no se pudo crear y si no su pid */
int createProcess(uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return 0;
    }

    if(scheduler->cantProcesses > MAX_PROCESSES){
        return 0;
    }

    TPCB newProcess = allocMemory(sizeof(PCB_t));
    printf("newProcess: %d\n", (uint64_t)newProcess);
    if(newProcess == NULL){
        return 0;
    }
    

    if(buildProcess(newProcess, scheduler->nextPid, rip, args, argc, priority, fileDescriptors, ground) == -1){
        freeMemory(newProcess);
        return 0;
    }
    printf("Se construyó el proceso\n");
    addNode(scheduler->totalProcesses, newProcess);
    printf("Se agregó el proceso a la lista de procesos\n");
    if(newProcess->pid < 1){ 
        newProcess->status = BLOCKED;
        addNode(scheduler->blockedList, newProcess);
    } 
    else{
        newProcess->status = READY;
        addNode(scheduler->readyList, newProcess);
    }

    scheduler->cantProcesses++;
    scheduler->nextPid++;
    printf("Termino el create\n");
    return newProcess->pid;
}

void killProcess(uint64_t pid){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return;
    }

    TPCB process = getProcess(pid);
    if(process == NULL){
        return;
    }
    //cuando implementemos sincro, tenemos que hacer aca que el proceso antes de liberar memoria, ponga en ready a los procesos que lo estan esperando
    // despues de eso, libero la memoria del proceso y lo saco de las listas
    /* Necesito pipes para cerrar fd (ayudame sancho) */
    // no
    removeNode(scheduler->readyList, process);
    scheduler->cantProcesses--;
    process->status= KILLED;
    yieldProcess();
}

int blockProcess(uint64_t pid){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return 0;
    }
    TPCB process = getProcess(pid);
    if(process == NULL){
        return 0;
    }
    if(process->status == BLOCKED){
        return 0;
    }
    if(process->status == READY){
        if(!removeNode(scheduler->readyList, process)){
            return 0;
        }
        if(!addNode(scheduler->blockedList, process)){
            return 0;
        }
        process->status = BLOCKED;
    }
    //si es el proceso que estoy corriendo, interrumpime y llama al timmertick
    if(pid == getActualPid()){
        yieldProcess();
    }
    return 1;
}

int readyProcess(uint64_t pid){
    schedulerADT scheduler = getScheduler();
    TPCB process = getProcess(pid);
    if(process == NULL){
        return 0;
    }
    if(process->status == READY){
        return 0;
    }
    if(!removeNode(scheduler->blockedList, process)){
        return 0;
    }
    if(!addNode(scheduler->readyList, process)){
        return 0;
    }
    process->status = READY;
    return 1;
}

TPCB getProcess(uint64_t pid){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return NULL;
    }
    toBegin(scheduler->totalProcesses);
    TPCB aux;
    while(hasNext(scheduler->totalProcesses)){
        aux = next(scheduler->totalProcesses);
        if(aux->pid == pid){
            return aux;
        }
    }
    return NULL;
}

void yieldProcess(){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return;
    }
    scheduler->quantum = 0;
    timerTickInt();
    //deberia directamente llamar a la funcion que se llama cuando el timerTick interrumpe  
}

schedulerADT getScheduler(){
    return scheduler;
}

uint64_t getActualPid(){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return 0;
    }
    return scheduler->actualPid;
}

static void idle(){
    while(1){
        _hlt();
    }
}

