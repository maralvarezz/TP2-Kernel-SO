#ifndef BUDDY_H
#define BUDDY_H

#include 

#define HEAPSIZE 0x10000000 

MemoryManagerADT createMemoryManagerBuddy(void * const restrict memoryForMemoryManager, uint64_t managedMemory);

void * allocMemoryBuddy(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

void freeMemoryBuddy(MemoryManagerADT const restrict memoryManager, void * const restrict memoryToFree);

memoryInfo_t memoryInfoBuddy(MemoryManagerADT const restrict memoryManager);

#endif // BUDDY_H
