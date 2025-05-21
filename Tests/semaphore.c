#include "semaphore.h"

#define CANT_SEMS 64

extern void waitingProcess(uint8_t *state);
extern void liberateProcess(uint8_t *state);

typedef struct semaphore_t{
    uint8_t idx;
    uint8_t value;
    uint8_t occupied;
    linkedListADT waitingList; 
}semaphore_t;

typedef struct semaphoreCDT{
    uint8_t cantOccupied;
    uint8_t nextIdx; 
    semaphore_t semVec[CANT_SEMS];
} semaphoreCDT;

semaphoreADT semaphore = NULL;

void createSemaphores(){
    semaphore = allocMemory(sizeof(semaphoreCDT));
    if(semaphore == NULL){
        return;
    }

    semaphore->cantOccupied = 0;
    semaphore->nextIdx = 0;

    for(int i = 0; i < CANT_SEMS; i++){
        semaphore->semVec[i].value = 0;
        semaphore->semVec[i].occupied = 0;
        semaphore->semVec[i].waitingList = createList();
        if(semaphore->semVec[i].waitingList == NULL){
            return;
        }
    }
}

TSem buildSemaphore(uint8_t value){
    semaphoreADT sem = getSemaphore();
    if(sem == NULL || sem->cantOccupied == CANT_SEMS){
        return NULL;
    }

    while(sem->semVec[sem->nextIdx].occupied == 1){
        sem->nextIdx = (sem->nextIdx + 1)  % CANT_SEMS;
    }

    sem->semVec[sem->nextIdx].occupied = 1;
    sem->semVec[sem->nextIdx].value = value;
    sem->semVec[sem->nextIdx].idx = sem->nextIdx;

    TSem toRet = &sem->semVec[sem->nextIdx];
    sem->nextIdx = (sem->nextIdx + 1)  % CANT_SEMS;
    sem->cantOccupied++;

    return toRet;
}

void waitSemaphore(TSem sem){
    if(sem == NULL || sem->occupied == 0){
        return;
    }
    uint8_t state = sem->occupied == 0;
    waitingProcess(&state);

    if(sem->value > 0){
        sem->value--;
        liberateProcess(&state);
        return;
    }
    
    uint64_t *pid = (uint64_t *) allocMemory(sizeof(uint64_t));
    if(pid == NULL){
        liberateProcess(&state);
        return;
    }
    *pid = getActualPid();
    addNode(sem->waitingList, (void *) pid);
    liberateProcess(&state);
    blockProcess(*pid);
}

void postSemaphore(TSem sem){
    if(sem == NULL || sem->occupied == 0){
        return;
    }

    uint8_t state = sem->value == 0;
    waitingProcess(&state);
    
    if(isEmpty(sem->waitingList) > 0){
        uint64_t *processPtr =  (uint64_t *) getFirst(sem->waitingList);
        TPCB process = getProcess(*processPtr);
        if(process == NULL || process->status == KILLED){
            return;
        }
        readyProcess(*processPtr);
        removeNode(sem->waitingList, (void *) processPtr);
        freeMemory(processPtr);
    }
    else{
        sem->value++;
    }
    liberateProcess(&state);
}

semaphoreADT getSemaphore(){
    return semaphore;
}
//podria no ir creemos check
uint16_t openSem(TSem sem){
    return sem->occupied * (-1);
}


uint16_t closeSem(TSem sem){
    if(sem->occupied == 0){
        return -1;
    }

    sem->occupied = 0;
    sem->value = 0;
    freeList(sem->waitingList);
    semaphore->cantOccupied--;
    sem->waitingList = createList();
    return 1;
} 

