#include <syscalls.h>
#include "builtins.h"

void memInfo(int argc, char *argv[]) {
    if (argc != 0) {
        printf("Uso incorrecto de memoryInfo. No se esperan argumentos.\n");
        return;
    }
    memoryInfo_t info = memoryInfo();
    if(info == NULL) {
        printf("Error al obtener la información de memoria.\n");
        return;
    }
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
        printf("PID inválido.\n");
        return;
    }
    blockProcess(pid)
    printf("Proceso %d bloqueado.\n", pid);
}

void unblock(int argc, char *argv[]) {
    if (argc != 1) {
        printf("Uso incorrecto de unblockProcess. Debe especificar el PID del proceso a desbloquear.\n");
        return;
    }
    int pid = atoi(argv[0]);
    if (pid <= 1) {
        printf("PID inválido, debe ser un número mayor a 1.\n");
        return;
    }
    unblockProcess(pid)
    printf("Proceso %d desbloqueado.\n", pid);
}

void ps(int argc, char *argv[]){
    if(argc != 0){
        printf("Uso incorrecto de ps. No se esperan argumentos.\n");
        return;
    }

    uint16_t cantProcess;

    TPInfo * processes = processInfo(&cantProcess);

    if(processes == NULL || cantProcess == 0){
        printf("No hay procesos en ejecución.\n");
        return;
    }

    printf("Los %d procesos en ejecución son:\n", cantProcess);

    for(int i = 0; i < cantProcess; i++) {
        TPInfo process = processes[i];
        printf("PID: %d, Nombre: %s, Estado: %s, Priority: %d, Ground: %s, StackPos: %d, StackBase: %d, RIP: %d\n", process.pid, process.name, process.state == 0 ? "Activo" : "Bloqueado", process.priority, process.ground == 0 ? "Background" : "Foreground", process.stackPos, process.stackBase, process.rip);
    }
    
    //aca faltaria un free??????? (de processes)
    //si processInfo llama a malloc, entonces si
    //aunque capaz seria mejor que no lo haga y que sea un array estatico
}

void kill(int argc, char *argv[]){
    if(argc != 1){
        printf("Uso incorrecto de kill. Debe especificar el PID del proceso a terminar.\n");
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
    if(changePriority(pid, priority) == -1){
        printf("Error al cambiar la prioridad del proceso %d.\n", pid);
        return;
    }
    printf("Prioridad del proceso %d cambiada a %d.\n", pid, priority);
    return;
}
