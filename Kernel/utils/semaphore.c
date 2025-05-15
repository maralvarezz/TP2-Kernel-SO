#include "semaphore.h"

#define CANT_SEMS 50

extern void waitingProcess(uint8_t *state);
extern void liberateProcess(uint8_t *state);

typedef struct semaphore{
    uint8_t value;
    uint8_t occupied;
    linkedListADT waitingList; 
}semaphore;

typedef struct semaphoreCDT{
    semaphore semVec[CANT_SEMS];
} semaphoreCDT;

sempahoreADT semaphore = NULL;

semaphoreADT createSemaphores(){
    semaphore = allocMemory(sizeof(semaphoreCDT));
    if(semaphore == NULL){
        return NULL;
    }

    for(int i = 0; i < CANT_SEMS; i++){
        semaphore->semVec[i].value = 0;
        semaphore->semVec[i].waitingList = createList();
        if(semaphore->semVec[i].waitingList == NULL){
            return NULL;
        }
    }
}

int setSemaphore(uint16_t id, uint8_t value){
    if(id >= CANT_SEMS || id < 0){
        return 0;
    }

    semaphoreADT sem = getSemaphore();
    if(sem == NULL){
        return 0;
    }

    if(sem->semVec[id].occupied == 1){
        return 0;
    }
    sem->semVec[id].occupied = 1;
    sem->semVec[id].value = value;
    return 1;
}

void waitSemaphore(uint8_t id){
    if(id >= CANT_SEMS || id < 0){
        return;
    }

    semaphoreADT sem = getSemaphore();
    if(sem == NULL || sem->semVec[id].occupied == 0){
        return;
    }

    waitingProcess(sem->semVec[id].value == 0);

    if(sem->semVec[id].value > 0){
        sem->semVec[id].value--;
        liberateProcess(sem->semVec[id].value == 0);
        return;
    }
    uint64_t *pid = (uint64_t *) allocMemory(sizeof(uint64_t));
    if(pid == NULL){
        liberateProcess(sem->semVec[id].value == 0);
        return;
    }
    *pid = getActualPid();
    addNode(sem->semVec[id].waitingList, (void *) pid);
    liberateProcess(sem->semVec[id].value == 0);
    blockProcess(*pid);
}

void postSemaphore(uint8_t id){
    if(id >= CANT_SEMS || id < 0){
        return 0;
    }

    semaphoreADT sem = getSemaphore();
    if(sem == NULL || sem->semVec[id].occupied == 0){
        return;
    }

    waitingProcess(sem->semVec[id].value == 0);
    
    if(sem->semVec[id].value == 0){
        while(sem->semVec[id].waitingList->size > 0){
            uint64_t *ptr =  (uint64_t *)getFirst(sem->semVec[id].waitingList);
            PCB auxProcess = getProcess(*ptr);
            if(auxProcess == NULL || auxProcess->status == KILLED){
                return;
            }
            readyProcess(*ptr);
            removeNode(sem->semVec[id].waitingList, (void *) ptr);
            break;
        }
    }

    sem->semVec[id].value++;
    liberateProcess(sem->semVec[id].value == 0);

}

semaphoreADT getSemaphore(){
    return semaphore;
}

uint16_t openSem(uint8_t id){
    if(id >= CANT_SEMS){
        return -1;
    }
    semaphoreADT sem = getSemaphore();
    return sem->semVec[id].occupied * (-1);
}

uint16_t closeSem(uint8_t id){
    if(id >= CANT_SEMS){
        return -1;
    }
    semaphoreADT sem = getSemaphore();
    if(sem->semVec[id].occupied == 0){
        return -1;
    }
    sem->semVec[id].occupied = 0;
    sem->semVec[id].value = 0;
    freeList(sem->semVec[id].waitingList);
    sem->semVec[id].waitingList = createList();
    return 1;
} 

