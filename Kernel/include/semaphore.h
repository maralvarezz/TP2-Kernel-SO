#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include "process.h"
#include "doubleLinkedList.h"

typedef struct semaphoreCDT * semaphoreADT;
typedef struct semaphore_t * TSem;

void createSemaphores();

TSem buildSemaphore(uint8_t value, char *name);

void waitSemaphore(TSem sem);

void postSemaphore(TSem sem);

semaphoreADT getSemaphore();

TSem openSem(char *name);

uint16_t closeSem(TSem sem);

#endif /* SEMAPHORE_H */