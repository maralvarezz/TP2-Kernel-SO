#include "pipes.h"

#define MAXPIPES 50
#define PIPESIZE 256
#define WRITE 1
#define READ 0

typedef struct pipe_t{
    char buffer[PIPESIZE];
    int readPos;
    int writePos;
    int64_t fd;
    int cantOcu;
    int readBlock;
    int writeBlock;
    int64_t readPid;
    int64_t writePid;
} pipe_t;

typedef struct pipe_t * TPipe;

typedef struct pipesCDT{
    pipe_t pipes[MAXPIPES];
    uint16_t cantPipes;
} pipesCDT;

pipesADT pipesM = NULL;

void pipesCreate(){
    pipesM = (pipesADT) allocMemory(sizeof(pipesCDT));
    if(pipesM == NULL){
        return;
    }
    pipesM->cantPipes = 0;
    for(int i = 0; i < MAXPIPES; i++){
        pipesM->pipes[i].fd = -1;
    }
}

static pipe_t initializePipe(){
    pipe_t pipe;
    pipe.readPos = 0;
    pipe.writePos = 0;
    pipe.cantOcu = 0;
    pipe.readBlock = 0;
    pipe.writeBlock = 0;
    pipe.readPid = -1;
    pipe.writePid = -1;
    pipe.fd= -1;
    return pipe;
}

pipesADT getPipes(){
    return pipesM;
}

int64_t openPipe(int16_t pid, uint8_t use){
    pipesADT pipesM = getPipes();
    int freePipe = -1;
    if(pipesM == NULL || pipesM->cantPipes >= MAXPIPES){
        return -1;
    }
    for(int j = 0; j < MAXPIPES; j++){
        if(pipesM->pipes[j].fd == -1){
            if(pipesM->pipes[j].writePid != -1 && use == READ){
                pipesM->pipes[j].readPid = pid;
                return pipesM->pipes[j].fd;
            }
            if(pipesM->pipes[j].readPid != -1 && use == WRITE){
                pipesM->pipes[j].writePid = pid;
                return pipesM->pipes[j].fd;
            }
        }
    }
    for(int i = 0; i < MAXPIPES; i++){
        if(pipesM->pipes[i].fd == -1){
            freePipe = i;
            break;
        }
    }
    if(freePipe == -1){
        return -1;
    }
    pipe_t auxPipe = initializePipe();
    auxPipe.fd = freePipe + 3;
    if(use == WRITE){
        auxPipe.writePid = pid;
    } else if(use == READ){
        auxPipe.readPid = pid;
    }
    pipesM->pipes[freePipe] = auxPipe;
    pipesM->cantPipes++;
    return auxPipe.fd; 
}

int16_t closePipe(int64_t fd){
    pipesADT pipesM = getPipes();
    if(pipesM == NULL){
        return -1;
    }
    if(fd < 3 || fd > MAXPIPES + 2){
        return -1;
    }

    pipe_t *pipeToClose = &pipesM->pipes[fd - 3];
    if(pipeToClose->writePid == -1  && pipeToClose->readPid == -1){
        return -1;
    }
    pipeToClose->fd = -1;
    pipeToClose->readPos = 0;
    pipeToClose->writePos = 0;
    pipeToClose->cantOcu = 0;
    pipeToClose->writePid = -1;
    pipeToClose->readPid = -1;
    pipesM->cantPipes--;
    return 0;
}

int64_t readPipe(int64_t fd, char * buffer, int64_t size){
    pipesADT pipesM = getPipes();
    if(pipesM == NULL){
        return -1;
    }
    if(size < 0 || fd < 3 || fd > MAXPIPES + 2){
        return -1;
    }
    
    pipe_t *pipeToRead = &pipesM->pipes[fd - 3];
    int index;
    for(index = 0; index < size; index++){
        while(pipeToRead->cantOcu == 0){
            pipeToRead->readBlock = 1;
            blockProcess(pipeToRead->readPid);
        }
        buffer[index] = pipeToRead->buffer[pipeToRead->readPos];
        pipeToRead->readPos = (pipeToRead->readPos + 1) % PIPESIZE;
        pipeToRead->cantOcu--;
        if(pipeToRead->writeBlock == 1){
            pipeToRead->writeBlock = 0;
            readyProcess(pipeToRead->writePid);
        }
    }
    return index;
}

int64_t writePipe(int64_t fd, char * buffer, int64_t size){
    pipesADT pipesM = getPipes();
    if(pipesM == NULL){
        return -1;
    }
    if(fd < 3 || fd > MAXPIPES + 2 || size > PIPESIZE){
        return -1;
    }
    pipe_t *pipeToWrite = &pipesM->pipes[fd - 3];
    int index;
    for(index = 0; index < size; index++){
        while(pipeToWrite->cantOcu == PIPESIZE){
            pipeToWrite->writeBlock = 1;
            blockProcess(pipeToWrite->writePid);
        }
        pipeToWrite->buffer[pipeToWrite->writePos] = buffer[index];
        pipeToWrite->writePos = (pipeToWrite->writePos + 1) % PIPESIZE;
        pipeToWrite->cantOcu++;
        if(pipeToWrite->readBlock == 1){
            pipeToWrite->readBlock = 0;
            readyProcess(pipeToWrite->readPid);
        }
    }
    return index;
}