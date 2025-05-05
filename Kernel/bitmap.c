#include "memoryManager.h"

#define FREE 0
#define USED 1
#define HEADER 2
#define BLOCKSIZE 64

typedef struct MemoryManagerCDT{
    void * bitmapStart;
    uint64_t cantBlocks;
    uint64_t blocksUsed;
    uint8_t bitmapState[];
} MemoryManagerCDT;

MemoryManagerADT createMemoryManager(void * const restrict memoryForMemoryManager, uint64_t managedMemory){
    MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
    uint64_t CDTSize = sizeof(MemoryManagerCDT) + managedMemory/(BLOCKSIZE + 1); // estimacion de lo que ocupara el CDT
    uint64_t usableMemory = managedMemory - CDTsize;  
    
    CDTsize = sizeof(MemoryManagerCDT) + usableMemory/BLOCKSIZE;
    
    memoryManager->cantBlocks = usableMemory / BLOCKSIZE; 

    memoryManager->bitmapStart = (void *)((uint8_t *)memoryForMemoryManager + CDTSize);
    memoryManager->bitmapState = (uint8_t *) (memoryForMemoryManager + sizeof(MemoryManagerCDT));

    for(int i = 0; i < memoryManager->cantBlocks; i++){
        memoryManager->bitmapState[i] = FREE; 
    }
    memoryManager->blocksUsed = 0;

    return memoryManager;
}

/*
MemoryManagerADT createMemoryManager(void * const restrict memoryForMemoryManager, uint64_t managedMemory){
    MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;

    
    uint64_t bitmapSize = managedMemory / (BLOCKSIZE + 1); 
    uint64_t CDTsize = sizeof(MemoryManagerCDT) + bitmapSize;

    
    uint64_t usableMemory = managedMemory - CDTsize;
    bitmapSize = usableMemory / BLOCKSIZE;
    CDTsize = sizeof(MemoryManagerCDT) + bitmapSize;

    memoryManager->bitmapStart = (void *)((uint8_t *)memoryForMemoryManager + CDTsize);
    memoryManager->bitmapState = (uint8_t *)((uint8_t *)memoryForMemoryManager + sizeof(MemoryManagerCDT));
    memoryManager->cantBlocks = bitmapSize;
    memoryManager->used = 0;

    for(int i = 0; i < memoryManager->cantBlocks; i++){
        memoryManager->bitmapState[i] = FREE; 
    }

    return memoryManager;
}
*/

void * allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate){
    if(memoryToAllocate + memoryManager->blocksUsed > memoryManager->cantBlocks){
        return NULL;
    }
    int flag = 0;
    int j = 0;
    for(int i = 0; i < memoryManager->cantBlocks; i++){
        flag = 0;
        if(memoryManager->bitmapState[i] == FREE){
            for(j = 0; j < memoryToAllocate && !flag; j++){
                if(memoryManager->bitMapState[i+j] == USED){
                    flag = 1;
                }
            }
            if(!flag){
                memoryManager->bitmapState[i] = HEADER;
                for(int k = 1; k < j; k++){
                    memoryManager->bitmapState[i+k] = USED;
                }
                memoryManager->blocksUsed += j;
                return (void *) &memoryManager->bitMapStart[i];
            }  
        }
        if(flag){
            i += j - 1; 
        }
    }
    return NULL;
}

void freeMemory(MemoryManagerADT const restrict memoryManager, void * const restrict memoryToFree){
    
    uint64_t i = (uint8_t *) memoryToFree - (uint8_t *) memoryManager->bitMapStart;

    if(i >= memoryManager->cantBlocks || memoryManager->bitmapState[i] != HEADER){
        return;
    }

    memoryManager->bitmapState[i] = FREE;
    memoryManager->blocksUsed--;

    for(i += 1 ; i < memoryManager->cantBlocks && memoryManager->bitMapState[i] == USED ; i++){
        memoryManager->bitmapState[i] = FREE;
        memoryManager->blocksUsed--;
    }
    
}


memoryInfo_t memoryInfo(MemoryManagerADT const restrict memoryManager){
    memoryInfo_t info;
    info.size = memoryManager->cantBlocks;
    info.used = memoryManager->blocksUsed;
    info.free = memoryManager->cantBlocks - memoryManager->blocksUsed;
    return info;
}
