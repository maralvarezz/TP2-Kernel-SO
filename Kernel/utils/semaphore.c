#include "semaphore.h"


#define CANT_SEMS 64
#define MAX_NAME_LEN 16

extern void waitingProcess(uint8_t *state);
extern void liberateProcess(uint8_t *state);

typedef struct semaphore_t{
    uint8_t idx;
    int8_t value;
    uint8_t occupied;
    uint8_t state;
    char name[MAX_NAME_LEN];
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
        semaphore->semVec[i].name[0] = '\0';
        semaphore->semVec[i].state = 0;
    }
}

TSem buildSemaphore(uint8_t value, char *name){
    semaphoreADT sem = getSemaphore();
    if(sem == NULL || sem->cantOccupied == CANT_SEMS || name == NULL || my_strlen(name) >= MAX_NAME_LEN){
        return NULL;
    }

    while(sem->semVec[sem->nextIdx].occupied == 1){
        sem->nextIdx = (sem->nextIdx + 1)  % CANT_SEMS;
    }

    sem->semVec[sem->nextIdx].occupied = 1;
    sem->semVec[sem->nextIdx].value = value;
    sem->semVec[sem->nextIdx].idx = sem->nextIdx;
    my_strcpy(sem->semVec[sem->nextIdx].name, name);

    TSem toRet = &sem->semVec[sem->nextIdx];
    sem->nextIdx = (sem->nextIdx + 1)  % CANT_SEMS;
    sem->cantOccupied++;

    return toRet;
}

void waitSemaphore(TSem sem){
    if(sem == NULL || sem->occupied == 0){
        return;
    }
    
    waitingProcess(&sem->state);

    
    if(sem->value > 0){
        sem->value--;
        liberateProcess(&sem->state);
        return;
    }
    
    uint64_t *pid = (uint64_t *) allocMemory(sizeof(uint64_t));
    if(pid == NULL){
        liberateProcess(&sem->state);
        return;
    }
    //sem->value--;
    *pid = getActualPid();
    addNode(sem->waitingList, (void *) pid);

    liberateProcess(&sem->state);
    blockProcess(*pid);
    
}

void postSemaphore(TSem sem){
    if(sem == NULL || sem->occupied == 0){
        return;
    }
    waitingProcess(&sem->state);
    int unblocked = 0;
    while(!isEmpty(sem->waitingList) && !unblocked){
        int64_t *pidPtr =  (int64_t *) getFirst(sem->waitingList);
        int64_t pid = *pidPtr;
        TPCB process = getProcess(pid);
        if(process == NULL || process->status == KILLED){
            freeMemory(pidPtr);
            continue;
        }
        freeMemory(pidPtr);
        readyProcess(pid);
        unblocked = 1;
    }
    if(!unblocked){
        sem->value++;
    }
    liberateProcess(&sem->state);
}

semaphoreADT getSemaphore(){
    return semaphore;
}


TSem openSem(char * name){
    semaphoreADT sem = getSemaphore();
    if(sem == NULL || name == NULL){
        return NULL;
    }
    for(int i = 0; i < CANT_SEMS; i++){
        if(sem->semVec[i].occupied == 1 && strcmp(sem->semVec[i].name, name) == 0){
            return &sem->semVec[i];
        }
    }
    return NULL;
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

