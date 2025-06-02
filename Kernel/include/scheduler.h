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

uint64_t createProcess(uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground);

void killProcess(uint64_t pid);

int blockProcess(uint64_t pid);

int readyProcess(uint64_t pid);

TPCB getProcess(uint64_t pid);

void yieldProcess();

schedulerADT getScheduler();

uint64_t getActualPid();

void killActualProcess();

void timerTickInt();

typedef struct PInfo{
    uint8_t priority;
    int16_t pid;
    int ground;
    uint64_t stackPos;
    uint64_t stackBase;
    uint8_t status;
    uint64_t rip;
    char * name;
}PInfo;

typedef struct PInfo * TPInfo;

TPInfo processInformation(uint64_t * pQuantity);

#endif 