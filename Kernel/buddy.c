#include "memoryManager.h"

#define FREE 0
#define OCCUPIED 1
#define SPLIT 2

/*
typedef struct node{
    uint64_t size;
    uint8_t ocu;
    struct node * left;
    struct node * right;
}node;

typedef struct node * TNode;

typedef struct memoryManagerCDT{
    uint8_t * treeStart;
    TNode tree;
    uint64_t size;
    uint64_t used;
} memoryManagerCDT;

MemoryManagerADT createMemoryManager(void * const restrict memoryForMemoryManager, uint64_t managedMemory){
    if(managedMemory > HEAPSIZE){
        return NULL;
    }
    MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
    memoryManager->treeStart = (uint8_t *) memoryForMemoryManager + sizeof(memoryManagerCDT);
    memoryManager->tree = (TNode *) (memoryManager->treeStart + HEAPSIZE);
    memoryManager->size = HEAPSIZE;
    memoryManager->used = 0; 
    return memoryManager;
}   


void * allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate){
    if(memoryToAllocate > memoryManager->size - memoryManager->used){
        return NULL;
    }

    while(memoryToAllocate > memorymanager->tree->size){
        
    }
}

void freeMemory(MemoryManagerADT const restrict memoryManager, void * const restrict memoryToFree){

}

memoryInfo_t memoryInfo(MemoryManagerADT const restrict memoryManager){
    memoryInfo_t info;
    info.size = memoryManager->size;
    info.used = memoryManager->used;
    info.free = memoryManager->size - memoryManager->used;
    return info;
}


*/