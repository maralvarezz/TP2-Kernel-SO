#ifndef BITMAP_H
#define BITMAP_H

MemoryManagerADT createMemoryManagerBitmap(void * const restrict memoryForMemoryManager, uint64_t managedMemory);

void * allocMemoryBitmap(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

void freeMemoryBitmap(MemoryManagerADT const restrict memoryManager, void * const restrict memoryToFree);

memoryInfo_t memoryInfoBitmap(MemoryManagerADT const restrict memoryManager);

#endif // BITMAP_H