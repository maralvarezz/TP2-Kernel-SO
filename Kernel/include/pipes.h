#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>
#include <stddef.h>
#include "doubleLinkedList.h"
#include "process.h"

typedef struct pipesCDT * pipesADT;

void pipesCreate();

pipesADT getPipes();

int64_t openPipe(int16_t pid, uint8_t use);

int16_t closePipe(int64_t fd);

int64_t readPipe(int64_t fd, char * buffer, int64_t size);

uint16_t writePipe(uint16_t fd, char * buffer, uint16_t size);

#endif
