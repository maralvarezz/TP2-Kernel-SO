#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "doubleLinkedList.h"
#include "process.h"
#include "processDefs.h"

#define SHELLPID 1
#define IDLEPROCESS 0

typedef struct schedulerCDT * schedulerADT;


void createScheduler();

uint64_t changeProcess(uint64_t actualRSP);

int createProcess(uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground);

void killProcess(uint64_t pid);

int blockProcess(uint64_t pid);

int readyProcess(uint64_t pid);

TPCB getProcess(uint64_t pid);

void yieldProcess();

schedulerADT getScheduler();

uint64_t getActualPid();

#endif 