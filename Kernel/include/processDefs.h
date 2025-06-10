#ifndef _PROCESSDEFS_H
#define _PROCESSDEFS_H
#include "doubleLinkedList.h"
#include "process.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define FOREGROUND 1
#define BACKGROUND 0

#define CANT_FILE_DESCRIPTORS 3

#define MAX_PRIORITY 5
#define MIN_PRIORITY 1
#define SHELL_PRIORITY 8

typedef enum { BLOCKED = 0, READY, RUNNING, KILLED } status_t;

typedef struct PCB_t {
	int64_t pid;        
	uint64_t stackBase; 
	uint64_t stackPos;  
	uint64_t rip;       
	char **argv;        
	int argc;           
	char *name;         
	uint8_t priority;   
	status_t status;    
	int16_t fileDescriptors[CANT_FILE_DESCRIPTORS]; 
	linkedListADT waitingList;        
	int ground;
} PCB_t;

typedef PCB_t * TPCB;

#endif