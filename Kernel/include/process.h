#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>
#include "processDefs.h"
#include "doubleLinkedList.h"

#define STACK_SIZE 4096 // 4KB

typedef struct PCB {
	int64_t pid;        // id del proceso
	uint64_t stackBase; // ubicacion de la base de la pila
	uint64_t stackPos;  // ubicacion del ultimo elemento de la pila
	uint64_t rip;       // instruction pointer
	char **argv;        // puntero al arreglo de argumentos
	int argc;           // cantidad de argumentos
	char *name;         // nombre del proceso   
	uint8_t priority;   // prioridad del proceso
	status_t status;    // estado del proceso
	int16_t fileDescriptors[CANT_FILE_DESCRIPTORS]; // FDS chequear si podemos dejar definidos esto
	doubleLinkedListADT waitingList;        // lista de procesos bloqueados esperando por este proceso
	int ground;
} PCB_t;


int buildProcess(PCB_t *process, int16_t pid, uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground);

uint64_t setStackFrame(uint64_t stackBase, uint64_t code, int argc, char *args[]);

int changePriority(int16_t pid, uint8_t priority);

void freeProcess(PCB_t *pcb);

int waitProcess(int16_t pid);

int changeFds(int16_t pid, int16_t fileDescriptors[]);

#endif