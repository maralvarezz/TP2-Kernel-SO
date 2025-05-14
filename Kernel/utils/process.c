#include "process.h"

typedef struct PCB_t * TPCB;

int buildProcess(PCB_t *process, int16_t pid, uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground){
    
}

uint64_t setStackFrame(uint64_t stackBase, uint64_t code, int argc, char *args[]);

int changePriority(int16_t pid, uint8_t priority);

void freeProcess(TPCB block){
    if(argv != NULL){
        for(int i = 0; i < block->argc; i++){
        if(argv[i] != NULL){
            freeMemory(block->argv[i]);
        }
    }
        freeMemory(block->argv);
    }
    freeMemory(block->waitingList);
    freeMemory(block->name);
    //falta alfo con el stack seguro
}

int waitProcess(int16_t pid);

int changeFds(int16_t pid, int16_t fileDescriptors[]);
