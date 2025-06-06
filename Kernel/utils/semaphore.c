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
    printf("Process %d trying to acquire semaphore %s\n", getActualPid(), sem->name);
    waitingProcess(&sem->state);
    
    if(sem->value > 0){
        sem->value--;
        printf("proceso %d acquired semaphore %s\n", getActualPid(), sem->name);
        liberateProcess(&sem->state);
        return;
    }
    
    uint64_t *pid = (uint64_t *) allocMemory(sizeof(uint64_t));
    if(pid == NULL){
        printf("Error allocating memory for process ID\n");
        liberateProcess(&sem->state);
        return;
    }
    *pid = getActualPid();
    addNode(sem->waitingList, (void *) pid);
    printf("Process %d added to semaphore %s waiting list\n", *pid, sem->name);
    liberateProcess(&sem->state);
    blockProcess(*pid);
    printf("Process %d blocked on semaphore %s\n", *pid, sem->name);
}

void postSemaphore(TSem sem){
    if(sem == NULL || sem->occupied == 0){
        return;
    }
    waitingProcess(&sem->state);
    
    while(!isEmpty(sem->waitingList)){
        uint64_t *pidPtr =  (uint64_t *) getFirst(sem->waitingList);
        TPCB process = getProcess(*pidPtr);
        if(process == NULL || process->status == KILLED){
            printf("process %d not found or killed, removing from semaphore %s waiting\n",*pidPtr ,sem->name);
            freeMemory(pidPtr);
            continue;
        }
        printf("Process %d unblocked %s\n", *pidPtr, sem->name);
        readyProcess(*pidPtr);
        freeMemory(pidPtr);
        break;
    }
    if(isEmpty(sem->waitingList)){
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

