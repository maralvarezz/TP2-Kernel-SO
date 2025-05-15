#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include "doubleLinkedList.h"

typedef struct semaphoreCDT * semaphoreADT;

semaphoreADT createSemaphores();

int setSemaphore(uint16_t id, uint8_t value);

void waitSemaphore(uint8_t id);

void postSemaphore(uint8_t id);

semaphoreADT getSemaphore();

#endif /* SEMAPHORE_H */