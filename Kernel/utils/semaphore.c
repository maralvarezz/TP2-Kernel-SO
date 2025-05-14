


typedef struct semaphoreCDT{
    uint8_t value;
    linkedListADT waitingList; //lista de procesos esperando por el semáforo

} semaphoreCDT;

typedef struct semaphoreCDT * semaphoreADT;

sempahoreADT semaphore = NULL;

semaphoreADT createSemaphore(uint8_t value){
    semaphore = allocMemory(sizeof(semaphoreCDT));
    if(semaphore == NULL){
        return NULL;
    }
    semaphore->value = value;
    semaphore->waitingList = createList();
    if(semaphore->waitingList == NULL){
        freeMemory(semaphore);
        return NULL;
    }
    return semaphore;
}

void waitSemaphore(TPCB process){
    semaphoreADT sem = getSemaphore();
    if(sem == NULL){
        return;
    }
    if(sem->value == 0 && process->status != BLOCKED){
        if(!addNode(sem->waitingList, process)){
            return;
        }
        blockProcess(process->pid);
    }
    else{
        sem->value--;
    }
    return;
}

void postSemaphore(TPCB process){
    semaphoreADT sem = getSemaphore();
    if(sem == NULL){
        return;
    }
    if(sem->value == 0){
        while(sem->waitinglist->size > 0){
            TPCB aux = getNode(sem->waitingList);
            if(aux == NULL){
                return;
            }
            readyProcess(aux->pid);
        }
    }
    sem->value++;
}


semaphoreADT getSemaphore(){
    return semaphore;
}

