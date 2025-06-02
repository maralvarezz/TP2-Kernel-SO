#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <stdint.h>
#include <syscalls.h>
#include <man.h>
#include <libasm.h>

/* Enum para la cantidad de argumentos recibidos */    
#define QTY_BYTES 32 /* Cantidad de bytes de respuesta del printmem */
#define DEFAULT_FONT_SIZE 1
#define MIN_FONT_SIZE 1
#define MAX_FONT_SIZE 3

#define WELCOME "Bienvenido a eSeMeMe SO!\n"
#define INVALID_COMMAND "Comando invalido!\n"
#define WRONG_PARAMS "La cantidad de parametros ingresada es invalida\n"
#define INVALID_FONT_SIZE "Dimension invalida de fuente\n"
#define CHECK_MAN "Escriba \"man %s\" para ver como funciona el comando\n"
#define CHECK_MAN_FONT "Escriba \"man font-size\" para ver las dimensiones validas\n"
/*
typedef void (* functionType) (int argc, char * argv[]);

typedef struct {
    char * name;                    // Nombre del comando
    char * description;             // Descripcion del comando (para help)
    functionType ftype;             // Cantidad de argumentos del comando
} Command;
*/

typedef enum {NO_PARAMS = 0, SINGLE_PARAM, DUAL_PARAM} functionType;    

typedef struct {
    char * name;                    // Nombre del comando
    char * description;             // Descripcion del comando (para help)
    union {                         // Puntero a la funcion
       int (*f)(void);
       int (*g)(char *);
       int (*h)(char *, char *);
    };
    functionType ftype;             // Cantidad de argumentos del comando
} Command;

static void help();
static void man(char * command);
static void printInfoReg();
static void time();
static int div(char * num, char * div);
static void fontSize(char * size);
static void printMem(char * pos);
static int getCommandIndex(char * command);
static void myClear();
static void testProcess();


/*static Command commands[] = {
    { "help", "Listado de comandos", (functionType) help};
    { "man", "Manual de uso de los comandos", (functionType) man};
    { "inforeg", "Informacion de los registos que fueron capturados en un momento arbitrario de ejecucion del sistema", (functionType) inforeg};
    { "time", "Despliega la hora actual UTC - 3", (functionType) time};
    { "div", "Hace la division entera de dos numeros naturales enviados por parametro",(functionType) div};
    { "kaboom", "Ejecuta una excepcion de Invalid Opcode", (functionType) kaboom};
    { "font-size", "Cambio de dimensiones de la fuente. Para hacerlo escribir el comando seguido de un numero", (functionType) fontSize};
    { "printmem", "Realiza un vuelco de memoria de los 32 bytes posteriores a una direccion de memoria en formato hexadecimal enviada por parametro", (functionType) printmem};
    { "clear", "Limpia toda la pantalla", (functionType) myClear};
    /*{ "mem", "Imprime el estado de la memoria", (functionType) mem};
    { "ps", "Imprime la lista de todos los procesos con sus propiedades", (functionType) ps};
    { "loop", "Imprime el ID del proceso con un saludo cada una determinada cantidad de segundos",(functionType) loop};
    { "kill", "Mata un proceso dado su ID", (functionType) kill};
    { "nice", "Cambia la prioridad de un proceso dado su ID y la nueva prioridad", (functionType) nice};
    { "block", "Cambia el estado de un proceso entre bloqueado y listo dado su ID", (functionType) block};
    { "cat", "Imprime el stdin tal como lo recibe", (functionType) cat};
    { "wc", "Cuenta la cantidad de lineas del input", (functionType) wc};
    { "filter", "Filtra las vocales del input", (functionType) filter};
    { "phylo", "Implementa el problema de los filósofos comensales", (functionType) phylo};
    { "testMemManager", "Corre un test para los memory managers", (functionType) testMemManager};
    { "testPrio", "Corre un test de prioridades", (functionType) testPrio};
    { "testProcesses", "Corre un test de procesos", (functionType) testProcesses};
    { "testSync", "Corre un test de sincronizacion", (functionType) testSync};
};*/

static Command commands[QTY_COMMANDS];

void init() {
    commands[0] = (Command){"help", "Listado de comandos", .f = (void*) &help, NO_PARAMS};
    commands[1] = (Command){ "man", "Manual de uso de los comandos", .g = (void*) &man, SINGLE_PARAM};
    commands[2] = (Command){"inforeg", "Informacion de los registos que fueron capturados en un momento arbitrario de ejecucion del sistema", .f = (void*)&printInfoReg, NO_PARAMS}; 
    commands[3] = (Command){"time", "Despliega la hora actual UTC - 3", .f = (void*) &time, NO_PARAMS};
    commands[4] = (Command){ "div", "Hace la division entera de dos numeros naturales enviados por parametro", .h = (void*) &div, DUAL_PARAM};
    commands[5] = (Command){ "kaboom", "Ejecuta una excepcion de Invalid Opcode", .f = (void*) &kaboom, NO_PARAMS};
    commands[6] = (Command){ "font-size", "Cambio de dimensiones de la fuente. Para hacerlo escribir el comando seguido de un numero", .g = (void*) &fontSize, SINGLE_PARAM};
    commands[7] = (Command){ "printmem", "Realiza un vuelco de memoria de los 32 bytes posteriores a una direccion de memoria en formato hexadecimal enviada por parametro", .g = (void*) &printMem, SINGLE_PARAM};
    commands[8] = (Command){ "clear", "Limpia toda la pantalla", .f = (void*) &myClear, NO_PARAMS};
    //commands[9] = (Command){ "testMemManager", "Corre un test para los memory managers", .g = (void*) &testMemManager, SINGLE_PARAM};
    commands[9] = (Command){ "prueba", "corre 2 procesos en simultaneo que imprimen en pantalla para testear prioridades", .f = (void*)&testProcess, NO_PARAMS};
}


void run_shell() {
    init();
    int index;
    puts(WELCOME);
    while(1){
        putchar('>');
        char command[MAX_CHARS] = {0};
        char arg1[MAX_CHARS];
        char arg2[MAX_CHARS];
        int qtyParams = scanf("%s %s %s", command, arg1, arg2);   
        index = getCommandIndex(command);
        if (index == -1) {
            if (command[0] != 0)
                printErr(INVALID_COMMAND);
            continue;
        }
        int funcParams = commands[index].ftype;
        if(qtyParams - 1 != funcParams){
            printErr(WRONG_PARAMS);
            printf(CHECK_MAN, command);
            continue;
        }
        switch (commands[index].ftype)
        {
            case NO_PARAMS: 
                commands[index].f();
                break;
            case SINGLE_PARAM:
                commands[index].g(arg1);
                break;
            case DUAL_PARAM:
                commands[index].h(arg1, arg2);
                break;
        }
    }
}

/* 
void run_shell() {
    int index;
    puts(WELCOME);
    while(1){
        putchar('>');
        char command[MAX_CHARS] = {0};
        scanf("%s", command); 
        index = getCommandIndex(command);
        if (index == -1) {
            if (command[0] != 0)
                printErr(INVALID_COMMAND);
            continue;
        }
        int funcParams = commands[index].ftype;
        if(qtyParams - 1 != funcParams){
            printErr(WRONG_PARAMS);
            printf(CHECK_MAN, command);
            continue;
        }
        switch (commands[index].ftype)
        {
            case NO_PARAMS: 
                commands[index].f();
                break;
            case SINGLE_PARAM:
                commands[index].g(arg1);
                break;
            case DUAL_PARAM:
                commands[index].h(arg1, arg2);
                break;
        }
    }
}

*/

/**
 * @brief  Devuelve el indice del vector de comandos dado su nombre
 * @param  command: Nombre del comando a buscar
 * @return  Indice del comando 
 */
static int getCommandIndex(char * command) {
    int idx = 0;
    for(; idx < QTY_COMMANDS; idx++){
        if(!strcmp(commands[idx].name, command))
            return idx;
    }    
    return -1;
}

static void help() {
    for (int i = 0; i < QTY_COMMANDS; i++)
        printf("%s: %s\r\n", commands[i].name, commands[i].description);
}

static int div(char * num, char * div) {
    printf("%s/%s=%d\r\n", num, div, atoi(num)/atoi(div));
    return 1;
}

static void time(){
    uint32_t secs = getSeconds();
    uint32_t h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
    printf("%2d:%2d:%2d\r\n", h, m, s);
}

static void fontSize(char * size) {
    int s = atoi(size);
    if(s >= MIN_FONT_SIZE && s <= MAX_FONT_SIZE)
        setFontSize((uint8_t)atoi(size));
    else{
        printErr(INVALID_FONT_SIZE);
        puts(CHECK_MAN_FONT);
    }
}

static void printMem(char * pos){
    uint8_t resp[QTY_BYTES];
    char * end;
    getMemory(strtoh(pos, &end), resp);
    for(int i = 0; i < QTY_BYTES; i++) {
        printf("0x%2x ", resp[i]);
        if (i % 4 == 3)
            putchar('\n');
    }
}

static char * _regNames[] = {"RIP", "RSP", "RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
static void printInfoReg() {
    int len = sizeof(_regNames)/sizeof(char *);
    uint64_t regSnapshot[len];
    getInfoReg(regSnapshot);
    for (int i = 0; i < len; i++)
        printf("%s: 0x%x\n", _regNames[i], regSnapshot[i]);
    
}

static void man(char * command){
    int idx = getCommandIndex(command);
    if (idx != -1)
        printf("%s\n", usages[idx]);
    else
        printErr(INVALID_COMMAND);
}

static void myClear(){
    clear();
}


static void proceso2(){
    int aux = 10;
    while(aux--){
        printf("Proceso 2\n");
    }
    killProc(getPid());
}


static void proceso1(){
    int aux = 5;
    while(1){
        printf("Proceso 1\n");
    }
    //killProc(getPid());
}


static void testProcess(){
    char * argv1[] = { "P1" };
    char * argv2[] = { "P2" };
	int16_t fileDescriptors[3] = { -1, 1, -1 };
    uint64_t pid1, pid2;
    pid1=createProc((uint64_t)proceso1, argv1, 1, 5, fileDescriptors, 0);
    unblockProc(pid1);
    //waitProcess(pid1);
    int i = 50000;
    while(i--){
        if(i== 25000){
            yield();
            printf("por la mitad\n");
        }
    } 
    killProc(pid1);
    //pid2=createProc((uint64_t)proceso2, argv2, 1, 5, fileDescriptors, 0);
}


//static void testMemManager()