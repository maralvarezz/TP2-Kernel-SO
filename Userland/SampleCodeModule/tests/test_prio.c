#include <stdint.h>
#include <stdio.h>
#include "syscalls.h"
#include "test_util.h"

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  
#define MEDIUM 3  
#define HIGHEST 5 

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {"endless_loop_print"};
  int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++){
    pids[i] = createProc((uint64_t)endless_loop_print, argv, 1, LOWEST, fileDescriptors, 1);
    unblockProc(pids[i]);
  }
  bussy_wait(WAIT);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    changePrio(pids[i], prio[i]);

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    blockProc(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    changePrio(pids[i], MEDIUM);

  printf("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    unblockProc(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    killProcess(pids[i]);

  printf("TEST PRIORITY COMPLETED\n");
}