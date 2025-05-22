#include "scheduler.h"

#define MAX_PROCESSES 100
#define PID_SHELL 0



typedef struct schedulerCDT{
    linkedListADT totalProcesses;
    linkedListADT readyList;
    linkedListADT blockedList;
    TPCB actualProcess;
    uint32_t quantum; //el quantum va a depender directamente de la prioridad del proceso
    uint64_t actualPid;
    uint64_t nextPid;
    uint64_t cantProcesses;
} schedulerCDT;


static void idle();
schedulerADT scheduler = NULL;
uint8_t createdScheduler = 0;

void createScheduler(){
    scheduler = allocMemory(sizeof(schedulerCDT));
    if(scheduler == NULL){
        return;
    }

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
    scheduler->actualPid = 0;
    scheduler->nextPid = 0; //ponele
    scheduler->cantProcesses = 0;
    createdScheduler = 1;
    TPCB idleProcess = allocMemory(sizeof(PCB_t));
    if(idleProcess == NULL){
        return;
    }
    char *idleArg[] = { "idle" };
    int16_t fileDescriptors[] = {-1,-1,-1};
    if(buildProcess(idleProcess, IDLEPROCESS, (uint64_t)idle, idleArg, 1, 1, fileDescriptors, BACKGROUND) != -1) {
        addNode(scheduler->totalProcesses, idleProcess);
        addNode(scheduler->blockedList, idleProcess);
        scheduler->cantProcesses++;
        scheduler->nextPid = 1; // Próximo PID será 1
    } else {
        // Si falla buildProcess, liberar la memoria
        freeMemory(idleProcess);
    }
}

//este bro, va a agarrar un proceso, si es que hay, lo va a METER running agarrando uno de los readys y lo va  amandar a la cola de bloqueados o de ready dependiendo el caso
//esto va a devolver la stackPosition del procesoActual para que el scheduler lo pueda usar
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
            return scheduler->actualProcess->stackPos;
        }
    }
    
    scheduler->actualProcess = auxProcess;
    scheduler->actualProcess->status = RUNNING;
    scheduler->quantum = scheduler->actualProcess->priority;
    scheduler->actualPid = scheduler->actualProcess->pid;
    return scheduler->actualProcess->stackPos;
}

/* Devuelve 0 si no se pudo crear y si no su pid */
int createProcess(uint64_t rip, char **args, int argc, 
                            uint8_t priority, int16_t fileDescriptors[], int ground){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return 0;
    }

    if(scheduler->cantProcesses >= MAX_PROCESSES){
        return 0;
    }

    TPCB newProcess = allocMemory(sizeof(PCB_t));
    if(newProcess == NULL){
        return 0;
    }

    if(buildProcess(newProcess, scheduler->nextPid, rip, args, argc, priority, fileDescriptors, ground) == -1){
        //el proceso no se pudo crear
        freeMemory(newProcess);
        return 0;
    }
    addNode(scheduler->totalProcesses, newProcess);

    if(newProcess->pid < 1){ 
        addNode(scheduler->blockedList, newProcess);
    } 
    else{
        addNode(scheduler->readyList, newProcess);
    }

    scheduler->cantProcesses++;
    scheduler->nextPid++;
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

    /* Necesito pipes para cerrar fd (ayudame sancho) */
    // no
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
    //scheduler->quantum = 0;
    //interruptTimerTick(); 
    // // deberia directamente llamar a la funcion que se llama cuando el timerTick interrumpe  
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

