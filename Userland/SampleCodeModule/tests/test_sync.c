#include <stdint.h>
#include <stdio.h>
#include "syscalls.h"
#include "test_util.h"

#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 4){
    exit();
    return -1;
  }

  if ((n = satoi(argv[1])) <= 0){
    exit();
    return -1;
  }
  if ((inc = satoi(argv[2])) == 0){
    exit();
    return -1;
  }
  if ((use_sem = satoi(argv[3])) < 0){
    exit();
    return -1;
  }

  if (use_sem)
    if (!semOpen("test")) {
      printf("test_sync: ERROR opening semaphore\n");
      exit();
      return -1;
    }
    
  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem){
      semWait(semOpen("test"));
    }
    slowInc(&global, inc);
    if (use_sem){
      semPost(semOpen("test"));
    }
  }
  exit();
  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { 
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2){
    exit();
    return -1;
  }


  int8_t useSem = satoi(argv[1]);
  if(useSem){
    if (!semCreate(1, "test")) {
      printf("test_sync: ERROR creating semaphore\n");
      exit();
      return -1;
    }
    printf("Semaforo creado correctamente\n");
  } 
	char *argvDec[] = {"my_process_inc", argv[0], "-1", argv[1]};
	char *argvInc[] = {"my_process_inc", argv[0], "1", argv[1]};
  int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};

  global = 0;

  uint64_t i;
  
  printf("Empezando el testeo...\n");
  
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = createProc((uint64_t)my_process_inc, argvDec, 4, 1, fileDescriptors, 1);
    unblockProc(pids[i]);
    pids[i + TOTAL_PAIR_PROCESSES] = createProc((uint64_t)my_process_inc, argvInc, 4, 1, fileDescriptors, 1);
    unblockProc(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    waitProcess(pids[i]);
    waitProcess(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  printf("Final value: %d\n", (int)global);

  if(useSem)
    semClose(semOpen("test"));

  return 0;
}