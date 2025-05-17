#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include "process.h"
#include "doubleLinkedList.h"

typedef struct semaphoreCDT * semaphoreADT;
typedef struct semaphore_t * TSem;

void createSemaphores();

TSem buildSemaphore(uint8_t value);

void waitSemaphore(TSem sem);

void postSemaphore(TSem sem);

semaphoreADT getSemaphore();

uint16_t openSem(TSem sem);

uint16_t closeSem(TSem sem);

#endif /* SEMAPHORE_H */