#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#define HEAPSIZE 0x10000000
#define BLOCKSIZE 64
#define STRUCTSIZE (HEAPSIZE / BLOCKSIZE)

#include <stddef.h>
#include <stdint.h>

typedef struct memoryInformation{
    uint64_t size;
    uint64_t used;
    uint64_t free;
}memoryInfo_t;

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT createMemoryManager(void * const restrict memoryForMemoryManager, uint64_t managedMemory);

void * allocMemory(const size_t memoryToAllocate);

void freeMemory(void * const restrict memoryToFree);

memoryInfo_t memoryInfo();

int setFirst(linkedListADT list, void * info);

void toBegin(linkedListADT list);

int hasNext(linkedListADT list);

void * next(linkedListADT list);

#endif