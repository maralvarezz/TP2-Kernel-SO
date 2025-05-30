#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>
#include <stddef.h>
#include "doubleLinkedList.h"
#include "process.h"

typedef struct pipesCDT * pipesADT;

void pipesCreate();

pipesADT getPipes();

uint16_t openPipe(uint16_t pid, uint8_t use);

uint16_t closePipe(uint16_t fd);

uint16_t readPipe(uint16_t fd, char * buffer, uint16_t size);

uint16_t writePipe(uint16_t fd, char * buffer, uint16_t size);

#endif
