#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "doubleLinkedList.h"
#include "process.h"
#include "processDefs.h"

typedef struct schedulerCDT * schedulerADT;



void createScheduler();

uint64_t changeProcess(uint64_t actualRSP);

int createProcess(/* Parámetros pendientes */);

void killProcess(uint64_t pid);

int blockProcess(uint64_t pid);

int readyprocess(uint64_t pid);

TPCB getProcess(uint64_t pid);

void yieldProcess();

schedulerADT getScheduler();

uint64_t getActualPid();

#endif 