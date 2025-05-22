#ifndef DOUBLE_LINKED_LIST_H
#define DOUBLE_LINKED_LIST_H

#include <stdint.h>
#include <stddef.h>
#include "memoryManager.h"

typedef struct linkedListCDT * linkedListADT;

linkedListADT createList();

int addNode(linkedListADT list, void * info);


int removeNode(linkedListADT list, void * info);

void freeList(linkedListADT list);

int isEmpty(linkedListADT list);

int setFirst(linkedListADT list, void * info);

void * getFirst(linkedListADT list);

void toBegin(linkedListADT list);

int hasNext(linkedListADT list);

void * next(linkedListADT list);

#endif
