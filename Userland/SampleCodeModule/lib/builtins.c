#include "builtins.h"
#include "stdlib.h"

void memInfo(int argc, char *argv[]) {
    if (argc != 0) {
        printf("Uso incorrecto de memoryInfo. No se esperan argumentos.\n");
        return;
    }
    memoryInfo_t info = getMemInfo();
    printf("Memoria total: %d bytes\n", (int)info.size);
    printf("Memoria usada: %d bytes\n", (int)info.used);
    printf("Memoria libre: %d bytes\n", (int)info.free);
}

void block(int argc, char *argv[]) {
    if (argc != 1) {
        printf("Uso incorrecto de blockProcess. Debe especificar el PID del proceso a bloquear.\n");
        return;
    }
    int pid = atoi(argv[0]);
    if (pid < 0) {
        printf("PID invalido.\n");
        return;
    }
    blockProc(pid);
    printf("Proceso %d bloqueado.\n", pid);
}

void unblock(int argc, char *argv[]) {
    if (argc != 1) {
        printf("Uso incorrecto de unblockProcess. Debe especificar el PID del proceso a desbloquear.\n");
        return;
    }
    int pid = atoi(argv[0]);
    if (pid <= 1) {
        printf("PID invalido, debe ser un numero mayor a 1.\n");
        return;
    }
    unblockProc(pid);
    printf("Proceso %d desbloqueado.\n", pid);
}

void ps(int argc, char *argv[]){
    if(argc != 0){
        printf("Uso incorrecto de ps. No se esperan argumentos.\n");
        return;
    }

    uint64_t cantProcess;

    TPInfo processes = processInfo(&cantProcess);

    if(processes == NULL || cantProcess == 0){
        printf("No hay procesos en ejecucion.\n");
        return;
    }

    printf("Los %d procesos en ejecucion son:\n", cantProcess + 1);

    for(int i = 0; i < cantProcess + 1; i++) {
        PInfo process = processes[i];
        printf("PID: %d\n Nombre: %s\n Estado: %s\n Priority: %d\n Ground: %s\n StackPos: %d\n StackBase: %d\n RIP: %d\n", process.pid, process.name, process.status == 0 ? "Bloqueado" : (process.status == 3 ? "Muerto" : "Activo"), process.priority, process.ground == 0 ? "Background" : "Foreground", process.stackPos, process.stackBase, process.rip);
    }
    freeMem(processes);
}

void kill(int argc, char *argv[]){
    if(argc != 1){
        printf("Uso incorrecto de kill. Debe especificar el PID del proceso a terminar.\n");
        exit();
        return;
    }

    int pid = atoi(argv[0]);

    if(pid <= 1){
        printf("PID invalido, tiene que ser mayor a 1.\n");
        return;
    }

    killProcess(pid);
    printf("Proceso %d terminado.\n", pid);
}

void nice(int argc, char *argv[]){
    if(argc != 2){
        printf("Uso incorrecto de nice. Debe especificar el PID del proceso y la nueva prioridad.\n");
        return;
    }
    int pid = atoi(argv[0]);
    int priority = atoi(argv[1]);
    if(pid <= 1){
        printf("PID invalido, tiene que ser mayor a 1.\n");
        return;
    }
    if(priority < 1 || priority > 5){
        printf("Prioridad invalida, debe ser un numero entre 1 y 5.\n");
        return;
    }
    if(changePrio(pid, priority) == -1){
        printf("Error al cambiar la prioridad del proceso %d.\n", pid);
        return;
    }
    printf("Prioridad del proceso %d cambiada a %d.\n", pid, priority);
    return;
}
