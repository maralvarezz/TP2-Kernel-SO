#define CANT_SEMS 50

typedef struct semaphore{
    uint8_t value;
    uint8_t occupied;
    linkedListADT waitingList; 
}semaphore;

typedef struct semaphoreCDT{
    semaphore semVec[CANT_SEMS];
} semaphoreCDT;

typedef struct semaphoreCDT * semaphoreADT;

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
    semaphoreADT sem = getSemaphore();
    if(sem == NULL){
        return;
    }
    
}

void postSemaphore(uint8_t id){
    semaphoreADT sem = getSemaphore();
    if(sem == NULL){
        return;
    }
    
}


semaphoreADT getSemaphore(){
    return semaphore;
}

