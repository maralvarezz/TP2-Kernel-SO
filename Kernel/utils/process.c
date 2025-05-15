#include "process.h"

//-----------------------------------------------
// es para ver meintras hago las cosas sacar 

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
//-----------------------------------------------

static char **allocArgv(TPCB pcb, char **argv, int argc);

int buildProcess(TPCB process, int16_t pid, uint64_t rip, char **args, int argc, 
                            uint8_t priority, int16_t fileDescriptors[], int ground){
    process->pid = pid;
    void * stack = allocateMemory(STACK_SIZE);
    if(stack == NULL){
        return -1;
    }
    process->stackBase = (uint64_t) stack + STACK_SIZE; // porque crece hacia abajo


    
}

uint64_t setStackFrame(uint64_t stackBase, uint64_t code, int argc, char *args[]);

int changePriority(int16_t pid, uint8_t priority);

void freeProcess(TPCB process){
    if(argv != NULL){
        for(int i = 0; i < process->argc; i++){
        if(argv[i] != NULL){
            freeMemory(process->argv[i]);
        }
    }
        freeMemory(process->argv);
    }
    freeMemory(process->waitingList);
    freeMemory(process->name);
    //falta algo con el stack seguro
}

int waitProcess(int16_t pid);

int changeFds(int16_t pid, int16_t fileDescriptors[]);

static char **allocArgv(TPCB pcb, char **argv, int argc) {
	char **toReturn = allocateMemory((argc + 1) * sizeof(char *));
	if (toReturn == NULL) {
		return NULL;
	}
	for (int i = 0; i < argc; i++) {
		toReturn[i] = allocateMemory(strlen(argv[i]) + 1);

		if (toReturn[i] == NULL) {
			for (int j = 0; j < i; j++) {
				freeMemory(toReturn[j]);
			}
			freeMemory(toReturn);
			return NULL;
		}

		strcpy(toReturn[i], argv[i]);
	}
	toReturn[argc] = NULL;
	return toReturn;
}

