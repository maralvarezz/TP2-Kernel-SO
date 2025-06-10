#include "scheduler.h"

#define MAX_PROCESSES 100

#define FQ 2

typedef struct schedulerCDT{
    linkedListADT totalProcesses;
    linkedListADT readyList;
    linkedListADT blockedList;
    TPCB actualProcess;
    int32_t quantum; 
    uint64_t actualPid;
    uint64_t nextPid;
    uint64_t cantProcesses;
} schedulerCDT;

static int16_t getPipedFD(int16_t *FDs);
static void removeKilledProcess(TPCB process);
TPCB searchPipedProcess(int16_t fd,TPCB otherPipedProcess);

static void idle();
schedulerADT scheduler = NULL;
uint8_t createdScheduler = 0;

void createScheduler(){
    
    scheduler =(schedulerADT) allocMemory(sizeof(schedulerCDT));
    if(scheduler == NULL){
        return;
    }
    
    scheduler->totalProcesses = createList();
    if(scheduler->totalProcesses == NULL){
        freeMemory(scheduler);
        return;
    }

    
    scheduler->readyList = createList();
    if(scheduler->readyList == NULL){
        freeMemory(scheduler->totalProcesses);
        freeMemory(scheduler);
        return;
    }
    
    scheduler->blockedList = createList();
    if(scheduler->blockedList == NULL){
        freeMemory(scheduler->readyList);
        freeMemory(scheduler->totalProcesses);
        freeMemory(scheduler);
        return;
    }
    
    
    scheduler->actualProcess = NULL;    
    scheduler->actualPid = -1;
    scheduler->nextPid = 0;
    scheduler->quantum = 0;
    createdScheduler = 1;
    char *idleArg[] = { "idle" };
    int16_t fileDescriptors[] = {-1,-1,2};
    createProcess((uint64_t) idle, idleArg, 1, 1, fileDescriptors, 0);
    scheduler->cantProcesses = 0;
}

uint64_t changeProcess(uint64_t actualRSP){ 
    
    if(createdScheduler == 0){ 
        return actualRSP;
    }
    schedulerADT scheduler = getScheduler();
    scheduler->quantum--; 
    
    if(scheduler->quantum > 0 || scheduler->cantProcesses == 0){ 
        
        return actualRSP;
    }

    if(scheduler->actualPid < SHELLPID){      
        scheduler->actualProcess->stackPos = actualRSP;
        scheduler->actualProcess = (TPCB) getFirst(scheduler->readyList);
        if(scheduler->actualProcess == NULL){
            return actualRSP;
        }
        scheduler->quantum = 1 * scheduler->actualProcess->priority;
        scheduler->actualProcess->status = RUNNING;
        scheduler->actualPid = scheduler->actualProcess->pid;
        return scheduler->actualProcess->stackPos;
    }

    if(scheduler->actualProcess != NULL){
        scheduler->actualProcess->stackPos = actualRSP; 
        if(scheduler->actualProcess->status == RUNNING){
            scheduler->actualProcess->status = READY;
            addNode(scheduler->readyList, scheduler->actualProcess);
        }
        else if(scheduler->actualProcess->status == KILLED){
            removeKilledProcess(scheduler->actualProcess);
        }
    }
    TPCB auxProcess = getFirst(scheduler->readyList);
    while(auxProcess != NULL){
        if(auxProcess->status != KILLED){  
            scheduler->actualProcess = auxProcess;
            scheduler->actualProcess->status = RUNNING;
            scheduler->quantum = 1 * scheduler->actualProcess->priority;
            scheduler->actualPid = scheduler->actualProcess->pid;
            return scheduler->actualProcess->stackPos;  
        }else{
            removeKilledProcess(auxProcess);
        }
        auxProcess = getFirst(scheduler->readyList);
    }
    TPCB process = getProcess(IDLEPROCESS);
    if(process == NULL){
        return actualRSP;
    }
    scheduler->actualProcess = process;
    scheduler->actualPid = process->pid;
    return scheduler->actualProcess->stackPos;
}


uint64_t createProcess(uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return 0;
    }

    if(scheduler->cantProcesses > MAX_PROCESSES){
        return 0;
    }

    TPCB newProcess = allocMemory(sizeof(PCB_t));
    if(newProcess == NULL){
        return 0;
    }

    if(buildProcess(newProcess, scheduler->nextPid, rip, args, argc, priority, fileDescriptors, ground) == -1){
        freeMemory(newProcess);
        return 0;
    }
    addNode(scheduler->totalProcesses, newProcess);
    if(newProcess->pid > SHELLPID){ 
        newProcess->status = BLOCKED;
        addNode(scheduler->blockedList, newProcess);
    } 
    else{
        newProcess->status = READY;
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
    
    int16_t fd = getPipedFD(process->fileDescriptors);
    myKill(process);
    if(fd!=-1){
        TPCB pipedProcess = searchPipedProcess(fd, process);
        if(pipedProcess!=NULL){
            myKill(pipedProcess);
        }
    }

    if(scheduler->actualProcess->pid == pid){
        yieldProcess();
    }
}

void myKill(TPCB process){
    if(process == NULL || process->pid==SHELLPID){
        return;
    }
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return;
    }
    if(process->status == READY){
        removeNode(scheduler->readyList, process);
    }
    else if(process->status == BLOCKED){
        removeNode(scheduler->blockedList, process);
    }
    toBegin(process->waitingList);
    while(hasNext(process->waitingList)){
        TPCB blockedProcess = next(process->waitingList);
        if(blockedProcess == NULL){
            continue;
        }
        readyProcess(blockedProcess->pid);
    }
    process->status = KILLED;
    if(scheduler->actualProcess != process){
        addNode(scheduler->readyList, process);
    }
}


void killActualProcess(){
    schedulerADT scheduler = getScheduler();
    killProcess(scheduler->actualProcess->pid);
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
    if(process->status == READY || process->status == RUNNING){
        if(process->status == READY ){
            if(!removeNode(scheduler->readyList, process)){
                return 0;
            }
        }
        if(!addNode(scheduler->blockedList, process)){
            return 0;
        }
        process->status = BLOCKED;
    }

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
    if(process->status == RUNNING){
        return 0;
    }
    if(!removeNode(scheduler->blockedList, process) && process->status == BLOCKED){
        return 0;
    }
    if(!addNode(scheduler->readyList, process)){
        return 0;
    }
    if(process->status != KILLED){
        process->status = READY;
    }
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

TPInfo processInformation(uint64_t * pQuantity){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        *pQuantity = 0;
        return NULL;
    }
    TPInfo infoProc = (TPInfo) allocMemory(scheduler->cantProcesses * sizeof(PCB_t));
    if(infoProc == NULL){
        *pQuantity = 0;
        return NULL;
    }
    toBegin(scheduler->totalProcesses);
    TPCB aux;
    int i = 0;
    while(hasNext(scheduler->totalProcesses)){
        aux = next(scheduler->totalProcesses);
        infoProc[i].pid = aux->pid;
        infoProc[i].priority = aux->priority;
        infoProc[i].ground = aux->ground;
        infoProc[i].stackPos = aux->stackPos;
        infoProc[i].stackBase = aux->stackBase;
        infoProc[i].status = aux->status;
        infoProc[i].rip = aux->rip;
        if(aux->name !=NULL){
            infoProc[i].name = (char *)allocMemory(my_strlen(aux->name) + 1);
            if(infoProc[i].name == NULL){
                freeMemory(infoProc[i].name);
                freeMemory(infoProc);
                *pQuantity = 0;
                return NULL;
            }
            my_strcpy(infoProc[i].name, aux->name);
        }else{
            infoProc[i].name = NULL;
        }
        i++;
    }
    *pQuantity = scheduler->cantProcesses;
    return infoProc;
}

static void idle(){
    while(1){
        _hlt();
    }
}

void killForegroundProcess(){
    schedulerADT scheduler = getScheduler();
    if(scheduler == NULL){
        return;
    }
    if(scheduler->actualProcess->ground == FOREGROUND && scheduler->actualProcess->pid != SHELLPID){
        killActualProcess();
        return;
    }

    toBegin(scheduler->totalProcesses);
    TPCB process;
    while(hasNext(scheduler->totalProcesses)){
        process = next(scheduler->totalProcesses);
        if(process->ground == FOREGROUND && process->pid != SHELLPID){
            killProcess(process->pid);
            return;
        }
    }
}

int64_t getFD(int64_t fd) {
    schedulerADT scheduler = getScheduler();
    TPCB process = scheduler->actualProcess;
    return process->fileDescriptors[fd];
}


int16_t getPipedFD(int16_t *FDs) {
	if (FDs[STDIN] != STDIN) {
		return FDs[STDIN];
	}
	else if (FDs[STDOUT] != STDOUT) {
		return FDs[STDOUT];
	}
	return -1;
}


TPCB searchPipedProcess(int16_t fd,TPCB otherPipedProcess) {
	schedulerADT scheduler = getScheduler();
	TPCB process;
	toBegin(scheduler->totalProcesses);
	while (hasNext(scheduler->totalProcesses)) {
		process = next(scheduler->totalProcesses);
		if ((process -> fileDescriptors[STDIN] == fd || process -> fileDescriptors[STDOUT] == fd) && process != otherPipedProcess) {
			return process;
		}
	}
	return NULL;
}

static void removeKilledProcess(TPCB process){
    schedulerADT scheduler = getScheduler();
    removeNode(scheduler->totalProcesses, process);
    freeProcess(process);
    scheduler->cantProcesses--;
}
