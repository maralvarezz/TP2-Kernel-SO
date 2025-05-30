#include "memoryManager.h"

#define FREE 0
#define USED 1
#define HEADER 2

#define MAXBLOCKSINMEMORY 1024
typedef struct MemoryManagerCDT{
    void * bitmapStart;
    uint64_t cantBlocks;
    uint64_t blocksUsed;
    uint8_t bitmapState[MAXBLOCKSINMEMORY]; // le puse valor fijo maximo
} MemoryManagerCDT;

static void * firstMemoryPlace;

static MemoryManagerADT getMM();
/*
MemoryManagerADT createMemoryManager(void * const restrict memoryForMemoryManager, uint64_t managedMemory){
    MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
    uint64_t CDTSize = sizeof(MemoryManagerCDT) + managedMemory/(BLOCKSIZE + 1); // estimacion de lo que ocupara el CDT
    uint64_t usableMemory = managedMemory - CDTSize;  
    
    CDTSize = sizeof(MemoryManagerCDT) + usableMemory/BLOCKSIZE;
    
    memoryManager->cantBlocks = usableMemory / BLOCKSIZE; 

    memoryManager->bitmapStart = (void *)((uint8_t *)memoryForMemoryManager + CDTSize);
    memoryManager->bitmapState = (uint8_t *) (memoryForMemoryManager + sizeof(MemoryManagerCDT));

    for(int i = 0; i < memoryManager->cantBlocks; i++){
        memoryManager->bitmapState[i] = FREE; 
    }
    memoryManager->blocksUsed = 0;

    return memoryManager;
}
*/
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
    memoryManager->blocksUsed = 0;

    for(int i = 0; i < memoryManager->cantBlocks; i++){
        memoryManager->bitmapState[i] = FREE; 
    }

    return memoryManager;
}
*/
MemoryManagerADT createMemoryManager(void * const restrict memoryForMemoryManager, uint64_t managedMemory){
    if(managedMemory < sizeof(MemoryManagerCDT) + BLOCKSIZE){
        return NULL; // No hay suficiente memoria para el CDT y al menos un bloque
    }
    firstMemoryPlace = memoryForMemoryManager;
    MemoryManagerADT memoryManager = (MemoryManagerADT) firstMemoryPlace;

    memoryManager->cantBlocks = (managedMemory - sizeof(MemoryManagerCDT)) / BLOCKSIZE; // Cantidad de bloques que se pueden manejar

    memoryManager->bitmapStart = (void *)((uint8_t *)memoryForMemoryManager + sizeof(MemoryManagerCDT) + memoryManager->cantBlocks);
    
    memoryManager->blocksUsed = 0;

    for(uint64_t i = 0; i < memoryManager->cantBlocks; i++){
        memoryManager->bitmapState[i] = FREE; 
    }

    return memoryManager;
}

void * allocMemory(const size_t memoryToAllocate){
    MemoryManagerADT memoryManager = getMM();
    if(memoryToAllocate + memoryManager->blocksUsed > memoryManager->cantBlocks){
        return NULL;
    }
    uint64_t flag = 0;
    uint64_t j = 0;
    for(uint64_t i = 0; i < memoryManager->cantBlocks; i++){
        flag = 0;
        if(memoryManager->bitmapState[i] == FREE){
            for(j = 0; j < memoryToAllocate && !flag; j++){
                if(memoryManager->bitmapState[i+j] == USED){
                    flag = 1;
                }
            }
            if(!flag){
                memoryManager->bitmapState[i] = HEADER;
                for(uint64_t k = 1; k < j; k++){
                    memoryManager->bitmapState[i+k] = USED;
                }
                memoryManager->blocksUsed += j;
                return (void *) ((uint8_t *)memoryManager->bitmapStart + i * BLOCKSIZE);
            }  
        }
        if(flag){
            i += j - 1; 
        }
    }
    return NULL;
}

// void *malloc(uint64_t size) {
// 	MemoryManagerADT memoryManager = getMemoryManager();

// 	if (size == 0) {
// 		return NULL;
// 	}

// 	uint64_t blocksNeeded = size / BLOCK_SIZE + ((size % BLOCK_SIZE) ? 1 : 0);

// 	uint64_t freeBlocks = 0;

// 	for (uint64_t i = 0; i < memoryManager->blocksQty; i++) {
// 		if (memoryManager->bitmap[i] == FREE) {
// 			freeBlocks++;
// 			if (freeBlocks == blocksNeeded) {
// 				memoryManager->bitmap[i - freeBlocks + 1] = BORDER;

// 				for (uint64_t j = i - freeBlocks + 2; j <= i; j++) {
// 					memoryManager->bitmap[j] = USED;
// 				}

// 				memoryManager->usedBlocks += blocksNeeded;

// 				return (uint8_t *) memoryManager->arena + (i - freeBlocks + 1) * BLOCK_SIZE;
// 			}
// 		}
// 		else {
// 			freeBlocks = 0;
// 		}
// 	}

// 	return NULL;
// }

void freeMemory(void * const restrict memoryToFree){
    MemoryManagerADT memoryManager = getMM();    
    uint64_t i = (uint8_t *) memoryToFree - (uint8_t *) memoryManager->bitmapStart;

    if(i >= memoryManager->cantBlocks || memoryManager->bitmapState[i] != HEADER){
        return;
    }

    memoryManager->bitmapState[i] = FREE;
    memoryManager->blocksUsed--;

    for(i += 1 ; i < memoryManager->cantBlocks && memoryManager->bitmapState[i] == USED ; i++){
        memoryManager->bitmapState[i] = FREE;
        memoryManager->blocksUsed--;
    }
    
}

memoryInfo_t memoryInfo(){
    MemoryManagerADT memoryManager = getMM();
    memoryInfo_t info;
    info.size = memoryManager->cantBlocks;
    info.used = memoryManager->blocksUsed;
    info.free = memoryManager->cantBlocks - memoryManager->blocksUsed;
    return info;
}

static MemoryManagerADT getMM(){
    return (MemoryManagerADT) firstMemoryPlace;
}