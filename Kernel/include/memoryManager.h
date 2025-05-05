#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>

typedef struct memoryInformation{
    uint64_t size;
    uint64_t used;
    uint64_t free;
}memoryInfo_t;

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT createMemoryManager(void * const restrict memoryForMemoryManager, uint64_t managedMemory);

void * allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

void freeMemory(MemoryManagerADT const restrict memoryManager, void * const restrict memoryToFree);

memoryInfo_t memoryInfo(MemoryManagerADT const restrict memoryManager);

#endif