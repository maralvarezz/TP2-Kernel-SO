#ifndef _PROCESSDEFS_H
#define _PROCESSDEFS_H
#include "process.h"

//process status
typedef enum { BLOCKED = 0, READY, RUNNING, KILLED } status_t;
typedef PCB_t * TPCB;

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



#endif