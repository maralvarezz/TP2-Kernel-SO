#include "memoryManager.h"

#define FREE 0
#define OCCUPIED 1
#define SPLIT 2
#define MINEXPONENT 4
#define POW_2(x) (1 << (x))
#define MAXEXPONENT 28

static uint8_t getExponentPtr(void *memoryToFree);
static uint8_t getExponent(uint64_t size);
static uint64_t getNewNode(uint8_t exponent);
static uint8_t getNodeLevel(uint8_t exponent);
static int64_t getNodeIndex(uint8_t *ptr, uint8_t *exponent);
static void setMerge(uint64_t node);
static void splitTree(uint64_t node);

typedef struct node{
    uint8_t state;
}node;

typedef struct node * TNode;

typedef struct MemoryManagerCDT{
    uint8_t * treeStart;
    TNode tree;
    uint64_t size;
    uint64_t used;
} MemoryManagerCDT;

static MemoryManagerADT memoryManager = NULL;

static MemoryManagerADT getMM(){
    return memoryManager;
}

MemoryManagerADT createMemoryManager(void * const restrict memoryForMemoryManager, uint64_t managedMemory){
    if(managedMemory > HEAPSIZE || managedMemory < POW_2(MINEXPONENT)){
        return NULL;
    }
    memoryManager = (MemoryManagerADT) memoryForMemoryManager;
    memoryManager->treeStart = (uint8_t *) memoryForMemoryManager + sizeof(MemoryManagerCDT);
    memoryManager->tree = (TNode) (memoryManager->treeStart + HEAPSIZE);
    memoryManager->size = HEAPSIZE;
    memoryManager->used = 0; 
    
    return memoryManager;
}

void * allocMemory(const size_t memoryToAllocate){
    MemoryManagerADT memoryManager = getMM();
    if(memoryToAllocate > memoryManager->size - memoryManager->used || memoryToAllocate < POW_2(MINEXPONENT)){
        return NULL;
    }
    uint8_t exponent = getExponent(memoryToAllocate);
    if(exponent > MAXEXPONENT){
        return NULL;
    }
    uint64_t nodo = getNewNode(exponent); 
    if(nodo == (uint64_t)-1){
        return NULL;
    }
    splitTree(nodo);
    memoryManager->tree[nodo].state = OCCUPIED;
    memoryManager->used += POW_2(exponent);
    return (void *) (memoryManager->treeStart + nodo * POW_2(exponent));
}

/* Me setea la memoria libre y en caso de ser necesario mergea */
void freeMemory(void * const restrict memoryToFree){
    MemoryManagerADT memoryManager = getMM();
    if(memoryToFree == NULL){
        return;
    }
    uint8_t exponent = getExponentPtr(memoryToFree);
    uint8_t nodo = getNodeIndex(memoryToFree, &exponent);
    memoryManager->tree[nodo].state = FREE;
    setMerge(nodo);
    memoryManager->used -= POW_2(exponent);
}

memoryInfo_t memoryInfo(){
    MemoryManagerADT memoryManager = getMM();
    memoryInfo_t info;
    info.size = memoryManager->size;
    info.used = memoryManager->used;
    info.free = memoryManager->size - memoryManager->used;
    return info;
}

static int64_t getNodeIndex(uint8_t *ptr, uint8_t *exponent) {
    MemoryManagerADT memoryManager = getMM();
	int64_t node = 0;
	uint8_t levelExponent = *exponent;
	while (levelExponent > 0 && memoryManager->tree[node].state != OCCUPIED){
		node = ((ptr - memoryManager->treeStart) >> levelExponent) + getNewNode(levelExponent);
		levelExponent--;
	}
	*exponent = levelExponent;
	return node;
}

static uint8_t getExponentPtr(void *memoryToFree) {
    MemoryManagerADT memoryManager = getMM();
	uint64_t address = (uint8_t *) memoryToFree - memoryManager->treeStart;
	uint8_t exponent = MAXEXPONENT;
	uint64_t size = POW_2(MAXEXPONENT);
	while (address % size != 0) {
		exponent--;
		size >>= 1;
	}
	return exponent;
}

static uint8_t getExponent(uint64_t size){
    uint8_t exponent = MINEXPONENT;
    while(size > 1){
        size = size >> 1;
        exponent++;
    }
    return exponent;
}

/* Me devulve la posicion en la que puedo allocar y -1 si no */
static uint64_t getNewNode(uint8_t exponent){
    MemoryManagerADT memoryManager = getMM();
    uint64_t cur = getNodeLevel(exponent);
    uint64_t n = getNodeLevel(exponent - 1);
    while(memoryManager->tree[cur].state == SPLIT || memoryManager->tree[cur].state == OCCUPIED){
        cur++;
        if(cur >= n){
            return -1;
        }
    }
    return cur;
}

static uint8_t getNodeLevel(uint8_t exponent){
    return (1 << exponent) - 1;
}

static void setMerge(uint64_t node){
    MemoryManagerADT memoryManager = getMM();
    if(node % 2 == 0 && memoryManager->tree[node - 1].state == FREE){
        memoryManager->tree[(node - 1) / 2].state = FREE;
    }
    else if(node % 2 && memoryManager->tree[node + 1].state == FREE){
        memoryManager->tree[node / 2].state = FREE;
    }
}

static void splitTree(uint64_t node){
    MemoryManagerADT memoryManager = getMM();
    while(node){
        node = (node - 1) / 2;
        if(memoryManager->tree[node].state == FREE){
            memoryManager->tree[node].state = SPLIT;
        }
        else{
            break;
        }
    }
}
