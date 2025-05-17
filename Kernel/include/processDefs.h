#ifndef _PROCESSDEFS_H
#define _PROCESSDEFS_H
#include "doubleLinkedList.h"
#include "process.h"

//standard file descriptors
#define STDIN 0
#define STDOUT 1
#define STDERR 2

//ground
#define FOREGROUND 1
#define BACKGROUND 0

#define CANT_FILE_DESCRIPTORS 3

#define MAX_PRIORITY 5
#define MIN_PRIORITY 1
#define DEFAULT_PRIORITY 3



//process status
typedef enum { BLOCKED = 0, READY, RUNNING, KILLED } status_t;


typedef struct PCB_t {
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
	linkedListADT waitingList;        // lista de procesos bloqueados esperando por este proceso
	int ground;
} PCB_t;

typedef PCB_t * TPCB;







#endif