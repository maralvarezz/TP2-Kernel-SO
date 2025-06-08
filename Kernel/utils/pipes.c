#include "pipes.h"

#define MAXPIPES 50
#define PIPESIZE 256
#define WRITE 1
#define READ 0

typedef struct pipe_t{
    char buffer[PIPESIZE];
    uint16_t readPos;
    uint16_t writePos;
    uint16_t fd;
    uint16_t cantOcu;
    uint16_t readBlock;
    uint16_t writeBlock;
    uint16_t readPid;
    uint16_t writePid;
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
    if(fd < 3 || fd > MAXPIPES + 2 || size > PIPESIZE){
        return -1;
    }
    
    int index = fd - 3;
    while(pipesM->pipes[index].cantOcu <= 0){
        pipesM->pipes[index].readBlock = 1;
        blockProcess(pipesM->pipes[index].readPid);
    }
    
    for(int j = 0; j < size; j++){
        buffer[j] = pipesM->pipes[index].buffer[(pipesM->pipes[index].readPos + j) % PIPESIZE];
        if(pipesM->pipes[index].writeBlock == 1){
            pipesM->pipes[index].writeBlock = 0;
            readyProcess(pipesM->pipes[index].writePid);
        }
    }
    pipesM->pipes[index].readPos = (pipesM->pipes[index].readPos + size) % PIPESIZE;
    return 1;
}

uint16_t writePipe(uint16_t fd, char * buffer, uint16_t size){
    pipesADT pipesM = getPipes();
    if(pipesM == NULL){
        return -1;
    }
    if(fd < 3 || fd > MAXPIPES + 2 || size > PIPESIZE){
        return -1;
    }
    int index = fd - 3;

    while(pipesM->pipes[index].cantOcu <= PIPESIZE){
        pipesM->pipes[index].writeBlock = 1;
        blockProcess(pipesM->pipes[index].writePid);
    }

    for(int j = 0; j < size; j++){
        pipesM->pipes[index].buffer[(pipesM->pipes[index].writePos + j) % PIPESIZE] = buffer[j];
        if(pipesM->pipes[index].readBlock == 1){
            pipesM->pipes[index].readBlock = 0;
            readyProcess(pipesM->pipes[index].readPid);
        }
    }
    pipesM->pipes[index].writePos = (pipesM->pipes[index].writePos + size) % PIPESIZE;
    pipesM->pipes[index].cantOcu += size;
    return 1;
}