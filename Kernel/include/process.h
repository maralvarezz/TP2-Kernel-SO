#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>
#include <stdlib.h>
#include "string.h"
#include "processDefs.h"
#include "scheduler.h"


#define STACK_SIZE 4096 // 4KB

int buildProcess(TPCB process, int16_t pid, uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground);

uint64_t setStackFrame(uint64_t stackBase, uint64_t code, int argc, char *args[]);

int changePriority(int16_t pid, uint8_t priority);

void freeProcess(TPCB process);

int waitProcess(int16_t pid);

int changeFds(int16_t pid, int16_t fileDescriptors[]);

#endif