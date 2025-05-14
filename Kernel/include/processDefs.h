#ifndef _PROCESSDEFS_H
#define _PROCESSDEFS_H

//process status
typedef enum { BLOCKED = 0, READY, RUNNING, KILLED } status_t;


//standard file descriptors
#define STDIN 0
#define STDOUT 1
#define STDERR 2

//ground
#define FOREGROUND 1
#define BACKGROUND 0


#endif