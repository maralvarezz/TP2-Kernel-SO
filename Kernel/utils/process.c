#include "process.h"

static char **allocateArgv(TPCB pcb, char **argv, int argc);
static void freeArgv(PCB *pcb, char **argv, int argc);
uint64_t setStackFrame(uint64_t stackBase, uint64_t code, int argc, char *args[]);

int buildProcess(TPCB process, int16_t pid, uint64_t rip, char **args, int argc, 
                            uint8_t priority, int16_t fileDescriptors[], int ground){
    process->pid = pid;
    void * stack = allocateMemory(STACK_SIZE);
    if(stack == NULL){
        return -1;
    }
    process->stackBase = (uint64_t) stack + STACK_SIZE; // porque crece hacia abajo
	process->argv = allocateArgv(process, args, argc);
	if (process->argv == NULL) {
		free((void *) (process->stackBase - STACK_SIZE));
		return -1;
	}
	process->argc = argc;
	process->name = allocateMemory(strlen(args[0]) + 1);
	if (process->name == NULL) {
		free((void *) (process->stackBase - STACK_SIZE));
		freeArgv(process, process->argv, argc);
		return -1;
	}
	strcpy(process->name, args[0]);
	process->rip = rip;
	process->priority = priority;
	process->stackPos = setupStackFrame(process->stackBase, process->rip, argc, process->argv);
	if (process->pid > 1) {
		process->status = BLOCKED;
	}else{
		process->status = READY;
	}
	process->waitingList = createDoubleLinkedListADT();
	if (process->waitingList == NULL) {
		free((void *) (process->stackBase - STACK_SIZE));
		freeArgv(process, process->argv, argc);
		free(process->name);
		return -1;
	}
	for (int i = 0; i < CANT_FILE_DESCRIPTORS; i++) {
		process->fileDescriptors[i] = fileDescriptors[i];
	}
	process->ground = ground;
	return 0;   
}

int changePriority(int16_t pid, uint8_t priority){
	if(priority > MAX_PRIORITY || priority < MIN_PRIORITY){
		return -1;
	}
	TPCB process = getProcess(pid);
	if(process == NULL){
		return -1;
	}
	process->priority = priority;
	return 0;
}

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

int waitProcess(int16_t pid){
	TPCB process = getProcess(pid);
	uint64_t currentPid = getActualPid();
	if (process == NULL || currentPid == pid) {
		return -1;
	}
	addNode(process->waitingList, currentProcess);
	blockProcess(currentPid);
	return 0;
}

int changeFds(int16_t pid, int16_t fileDescriptors[]){
	TPCB process = getProcess(pid);
	if (process == NULL) {
		return -1;
	}
	for (int i = 0; i < CANT_FILE_DESCRIPTORS; i++) {
		process->fileDescriptors[i] = fileDescriptors[i];
	}
	return 0;
}

static char **allocateArgv(char **argv, int argc) {
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

static void freeArgv(char **argv, int argc) {
	if (argv == NULL) {
		return;
	}
	for (int i = 0; i < argc; i++) {
		if (argv[i] != NULL) {
			freeMemory(argv[i]);
		}
	}
	freeMemory(argv);
}
