#define MAX_PROCESSES 100
#define DEFAULT_PRIOTITY
#define MAX_PRIORITY
#define TQUANTUM 
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

typedef struct schedulerCDT * schedulerADT;

schedulerADT scheduler = NULL;
uint8_t createdScheduler = 0;

void createScheduler(){
    scheduler = allocMemory(sizeof(schedulerCDT));
    if(scheduler == NULL){
        return;
    }
    scheduler->runningList = createList();
    scheduler->readyList = createList();
    scheduler->blockedList = createList();
    scheduler->actualProcess = NULL;
    scheduler->quantum = TQUANTUM;
    scheduler->actualPid = 0;
    scheduler->nextPid = 1; //ponele
    scheduler->cantProcesses = 0;
    createdScheduler = 1;
    //acá habría que crear el proceso idle y agregarlo a la lista de listos
}

//este bro, va a agarrar un proceso, si es que hay, lo va a METER running agarrando uno de los readys y lo va  amandar a la cola de bloqueados o de ready dependiendo el caso
//esto va a devolver la stackPosition del procesoActual para que el scheduler lo pueda usar
uint64_t changeProcess(uint64_t actualRSP){ 
    if(createdScheduler == 0){ //si no está creado el scheduler devolvemos el puntero a la pila del proceso actual
        return actualRSP;
    }
    schedulerADT scheduler = getScheduler();
    shceduler->quantums--; //simulo el tiempo del proceso en ejecución
    if(scheduler->quantums > 0 || scheduler->cantProcesses == 0){ 
        return actualRSP;
    }
    //si el proceso actual es el kernel, entonces tengo que sacar un proceso el primer proceso de la lista de listos y ponerlo a correr
    
    if(scheduler->actualPid == KERNELPID){
        scheduler->actualProcess = getProcess(scheduler->readyList);
        if(scheduler->actualProcess == NULL){ //si el proceso no existe, entonces no hay nada para correr
            return actualRSP;
        }
        //actualizo los datos del proceso 
        scheduler->quantums = scheduler->actualProcess->priority;
        scheduler->actualProcess->state = RUNNING;
        scheduler->actualPid = scheduler->actualProcess->pid;
        return scheduler->actualProcess->stackPos;
    }

    //si el proceso actual no es el kernel y no es NULL, entonces guardo la posición del RSP del proceso actual y saco de RUNNING al proces y lo meto en la lista de READY
    //en el caso de que el proceso actual sea KILLED, entonces lo libero y no lo meto en la lista de READY
    if(scheduler->actualProcess != NULL){
        scheduler->actualProcess->stackPos = actualRSP; //guardo la posición de la pila del proceso actual
        if(scheduler->actualProcess->state == RUNNING){
            scheduler->actualProcess->sate = READY;
            addNode(scheduler->readyList, scheduler->actualProcess);
        }
        else if(scheduler->actualProcess->state == KILLED){
            freeProcess(scheduler->actualProcess);
            return actualRSP;
        }
    }
    PCB *auxProcess = getFirstProcess(scheduler->readyList);
    if(scheduler->NULL){
        //acá lo que hay que hacer es seleccionar el proceso idle y ponerlo en idle si no es NULL
        return actualRSP;
    }
    
    scheduler->actualProcess = auxProcess;
    scheduler->actualProcess->state = RUNNING;
    scheduler->quantums = scheduler->actualProcess->priority;
    scheduler->actualPid = scheduler->actualProcess->pid;
    return scheduler->actualProcess->stackPos;
}

/* Devuelve 0 si no se pudo crear y si no su pid */
int createProcess(/* Me faltan cosas para pasar cuando lo creo*/){
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

    if(buildProcess()){
        //el proceso no se pudo crear
        freeMemory(newProcess);
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

    if(process->state == BLOCKED){
        return 0;
    }

    if(process->state == READY){
        if(!removeNode(scheduler->readyList, process)){
            return 0;
        }
        if(!addNode(scheduler->blockedList, process)){
            return 0;
        }
        process->state = BLOCKED;
    }
    
    //si es el proceso que estoy corriendo, interrumpime y llama al timmertick
    if(pid == getCurrentPid()){
        yieldProcess();
    }

    return 1;
}

int readyprocess(uint64_t pid){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return 0;
    }

    TPCB process = getProcess(pid);
    if(process == NULL){
        return 0;
    }

    if(process->state == READY){
        return 0;
    }

    if(!removeNode(scheduler->blockedList, process)){
        return 0;
    }
    if(!addNode(scheduler->readyList, process)){
        return 0;
    }
    process->state = READY;

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
    scheduler->quantums = 0;
    interruptTimerTick();
}

schedulerADT getScheduler(){
    return scheduler;
}

