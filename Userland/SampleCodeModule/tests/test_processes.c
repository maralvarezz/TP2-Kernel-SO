#include <stdio.h>
#include "syscalls.h"
#include "test_util.h"

enum State { RUNNING,
             BLOCKED,
             KILLED };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {"endless_loop"};
  int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};

  if (argc != 1){
    printf("Error: cantidad de argumentos incorrecta\n");
    exit();
    return -1;
  }

  if ((max_processes = satoi(argv[0])) <= 0){
    printf("Error: cantidad de procesos invalida\n");
    exit();
    return -1;
  }

  printf("Creando %d procesos...\n", (int)max_processes);
  p_rq p_rqs[max_processes];


  // Create max_processes processes
  for (rq = 0; rq < max_processes; rq++) {
    p_rqs[rq].pid = createProc((uint64_t)endless_loop, argvAux, 1, 1, fileDescriptors, 1);
    unblockProc(p_rqs[rq].pid);
    if (p_rqs[rq].pid == -1) {
      printf("ERROR creando proceso %d\n", rq);
      exit();
      return -1;
    } else {
      printf("Proceso %d creado con PID %d\n", rq, p_rqs[rq].pid);
      p_rqs[rq].state = RUNNING;
      alive++;
    }
  }

  printf("Comenzando ciclo de matanza/bloqueo/desbloqueo...\n");

  // Randomly kills, blocks or unblocks processes until every one has been killed
  while (alive > 0) {
    for (rq = 0; rq < max_processes; rq++) {
      action = GetUniform(100) % 2;
      switch (action) {
        case 0:
          if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
            printf("Matando proceso PID %d\n", p_rqs[rq].pid);
            killProcess(p_rqs[rq].pid);
            p_rqs[rq].state = KILLED;
            alive--;
          }
          break;

        case 1:
          if (p_rqs[rq].state == RUNNING) {
            printf("Bloqueando proceso PID %d\n", p_rqs[rq].pid);
            blockProc(p_rqs[rq].pid);
            p_rqs[rq].state = BLOCKED;
          }
          break;
      }
    }

    // Randomly unblocks processes
    for (rq = 0; rq < max_processes; rq++)
      if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
        printf("Desbloqueando proceso PID %d\n", p_rqs[rq].pid);
        if (unblockProc(p_rqs[rq].pid) == -1) {
          printf("ERROR desbloqueando proceso PID %d\n", p_rqs[rq].pid);
          exit();
          return -1;
        }
        p_rqs[rq].state = RUNNING;
      }
  }
  return 0;
}
