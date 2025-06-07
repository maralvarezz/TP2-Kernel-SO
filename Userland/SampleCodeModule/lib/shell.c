#include "shell.h"
#define READ 0
#define WRITE 1
/* Enum para la cantidad de argumentos recibidos */    
#define QTY_BYTES 32 /* Cantidad de bytes de respuesta del printmem */
#define DEFAULT_FONT_SIZE 1
#define MIN_FONT_SIZE 1
#define MAX_FONT_SIZE 3
#define BUILT_INS 14
#define PROCESSES 9
#define QTY_ARGS 3
#define MAX_SIZE 128

#define WELCOME "Bienvenido a eSeMeMe SO!\n"
#define INVALID_COMMAND "Comando invalido!\n"
#define WRONG_PARAMS "La cantidad de parametros ingresada es invalida\n"
#define INVALID_FONT_SIZE "Dimension invalida de fuente\n"
#define CHECK_MAN "Escriba \"man %s\" para ver como funciona el comando\n"
#define CHECK_MAN_FONT "Escriba \"man font-size\" para ver las dimensiones validas\n"

/*
typedef void (* functionType) (int argc, char * argv[]);

typedef struct {
    char * name;                    
    char * description;             
    functionType ftype;             
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
static int scanCommand(char *commandLine, char *command, char *args[QTY_ARGS]);

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
    { "mem", "Imprime el estado de la memoria", (functionType) memInfo};
    { "block", "Cambia el estado de un proceso entre bloqueado y listo dado su ID", (functionType) block};
    { "unblock", "Cambia el estado de un proceso entre listo y bloqueado dado su ID", (functionType) unblock};
    { "ps", "Imprime la lista de todos los procesos con sus propiedades", (functionType) ps};
    { "kill", "Mata un proceso dado su ID", (functionType) kill};
    { "nice", "Cambia la prioridad de un proceso dado su ID y la nueva prioridad", (functionType) nice};
    { "loop", "Imprime el ID del proceso con un saludo cada una determinada cantidad de segundos",(functionType) loop};
    { "cat", "Imprime el stdin tal como lo recibe", (functionType) cat};
    { "wc", "Cuenta la cantidad de lineas del input", (functionType) wc};
    { "filter", "Filtra las vocales del input", (functionType) filter};
    { "phylo", "Implementa el problema de los filósofos comensales", (functionType) mainPhilo};
    { "testMemManager", "Corre un test para los memory managers", (functionType) testMemManager};
    { "testPriority", "Corre un test de prioridades", (functionType) testPriority};
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
    commands[10] = (Command){ "filosofos", "Implementa el problema de los filósofos comensales", .g = (void*)&mainPhilo, SINGLE_PARAM};
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
        else if (index == 1){
            
        
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
    for (int i = 0; i < QTY_COMMANDS; i++){
        if(i == 0)
            printf("-----------------Built-in commands-----------------\n");

        if(i == BUILT_INS)
            printf("-----------------Processes commands-----------------\n");
        
        printf("%s: %s\r\n", commands[i].name, commands[i].description);
    }
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
    char * usages [] = { "Uso: help - muestra todos los comandos que existen en la terminal. No recibe parametros",
                              
                              "Uso: man [COMANDO] - explica el funcionamiento de un comando enviado como parametro",
                              
                              "Uso: inforeg - muestra informacion de los registros en un momento arbitrario de ejecucion del sistema. No recibe parametros",
                              
                              "Uso: time - despliega la hora actual. No recibe parametros",

                              "Uso: div [OP1] [OP2] - hace la division entera de dos numeros naturales que recibe por parametro\n"
                              "Ejemplo: div 10 5",

                              "Uso: kaboom - arroja una excepcion de invalid opcode. No recibe parametros",
                              
                              "Uso: font-size [1|2|3] - cambia la medida de la fuente. Para eso se deben enviar por parametro el numero 1, 2 o 3\n" 
                              "Ejemplo: font-size 2",
                              
                              "Uso: printmem [DIR] - imprime los primeros 32 bytes de memoria a partir de una direccion de memoria enviada como parametro\n"
                              "Ejemplo: printmem 10ff8c",
                              
                              "Uso: clear - limpia la pantalla. No recibe parametros",

                              "Uso: mem - imprime el estado de la memoria. No recibe parametros",

                              "Uso: ps - muestra el PID, nombre, estado, priority, ground, stackPos, stackBase y RIP de los procesos que se estan ejecutando en el sistema. No recibe parametros",

                              "Uso: kill [PID] - mata el proceso cuyo PID se envia como parametro. Recibe un numero natural que es el PID del proceso\n",

                              "Uso: nice [PID] [PRIORIDAD] - cambia la prioridad del proceso cuyo PID se envia como parametro. Recibe un numero natural que es el PID del proceso y un numero natural que es la nueva prioridad del proceso\n",

                              "Uso: loop [SEGUNDOS] - ejecuta un bucle y cada SEGUNDOS imprime un saludo en pantalla. Recibe un numero natural que es el intervalo de segundos entre cada saludo\n",

                              "Uso: cat - imprime el STDIN como lo recibe. No recibe parametros",

                              "Uso: wc - imprime la cantidad de lineas del input. No recibe parametros",

                              "Uso: filter - imprime solamente las vocales del input. No recibe parametros",

                              "Uso: phylo - ejecuta la solucion del problema de los filosofos comensales. No recibe parametros",

                              "Uso: testMemManager - ejecuta un test de la memoria. No recibe parametros",

                              "Uso: testPriority - ejecuta un test de prioridades. No recibe parametros",

                              "Uso: testProcesses - ejecuta un test de procesos. No recibe parametros",

                              "Uso: testSync - ejecuta un test de sincronizacion. No recibe parametros"

                            };
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
    int aux = 25;
    while(aux--){
        printf("Proceso 2\n");
    }
    killProcess(getPid());
}


static void proceso1(){
    int aux = 15;
    while(aux--){
        printf("Proceso 1\n");
    }
    killProcess(getPid());
}


static void testProcess(){
    char * argv1[] = { "P1" };
    char * argv2[] = { "P2" };
	int16_t fileDescriptors[3] = { -1, 1, -1 };
    uint64_t pid1, pid2;
    pid1=createProc((uint64_t)proceso1, argv1, 1, 1, fileDescriptors, 0);
    pid2=createProc((uint64_t)proceso2, argv2, 1, 1, fileDescriptors, 0);
    unblockProc(pid1);
    unblockProc(pid2);
    waitProcess(pid1); 
    waitProcess(pid2);
}

static int scanCommand(char *commandLine, char *command, char *args[QTY_ARGS]){
    int i,j,k;
    for(i=0, j=0; commandLine[i] != ' ' && commandLine[i] != '\0'; i++){
        command[j++] = commandLine[i];
    }
    command[j] = '\0';
    if(commandLine[i] == '\0') {
        return 0;   
    }
    while(commandLine[i] == ' '){
        i++;
    }
    for(j = 0 ; j < QTY_ARGS; j++){
        args[j] = (char *)allocMem(MAX_SIZE);
        if(args[j] == NULL){
            return -1;
        }
    }
    for(k = 0; k < QTY_ARGS && commandLine[i] != '\0'; k++){
        for(j = 0; commandLine[i] != ' ' && commandLine[i] != '\0'; i++){
            args[k][j++] = commandLine[i];
        }
        args[k][j] = '\0';
        while(commandLine[i] == ' '){
            i++;
        }
    }
    return k;
}

void executePipeCommands(char *leftCom, char *rightCom, char *leftParam[], char *rightParam[], int leftParamQuantity, int rightParamQuantity, int leftId, int rightId,int isBackGroundLeft, int isBackGroundRight) {
    int16_t fileDescriptors[3] = { 0, 1, 2 };
    
    int16_t leftPipePid = createProc((uint64_t)commands[leftId].ftype, (char **)leftParam, leftParamQuantity, 1, fileDescriptors, isBackGroundLeft);
    if( leftPipePid == -1) {
        printErr("Error al crear el proceso izquierdo del pipe\n");
        return;
    }
    int writeFd, readFd;
    if((writeFd = openPipe(leftPipePid, WRITE)) == -1) {
        printErr("Error al abrir el pipe de escritura\n");
        killProcess(leftPipePid);
        return;
    }
    if(changeFd(leftPipePid, (int16_t[]) {STDIN, writeFd, STDERR}) == -1) {
        printErr("Error al cambiar los file descriptors del proceso izquierdo\n");
        closePipe(writeFd);
        killProcess(leftPipePid);
        return;
    }
    int16_t rightPipePid = createProc((uint64_t)commands[rightId].ftype, (char **)rightParam, rightParamQuantity, 1, fileDescriptors, isBackGroundRight);
    if(rightPipePid == -1) {
        printErr("Error al crear el proceso derecho del pipe\n");
        killProcess(leftPipePid);
        return;
    }
    if((readFd = openPipe(rightPipePid, READ)) == -1) {
        printErr("Error al abrir el pipe de lectura\n");
        killProcess(rightPipePid);
        closePipe(writeFd);
        killProcess(leftPipePid);
        return;
    }
    if(changeFd(rightPipePid, (int16_t[]) {readFd, STDOUT, STDERR}) == -1) {
        printErr("Error al cambiar los file descriptors del proceso derecho\n");
        closePipe(writeFd);
        killProcess(rightPipePid);
        killProcess(leftPipePid);
        return;
    }
    if(unblockProc(leftPipePid) == -1) {
        printErr("Error al desbloquear el proceso izquierdo del pipe\n");
        closePipe(writeFd);
        killProcess(leftPipePid);
        killProcess(rightPipePid);
        return;
    }
    if(unblockProc(rightPipePid) == -1) {
        printErr("Error al desbloquear el proceso derecho del pipe\n");
        closePipe(writeFd);
        killProcess(leftPipePid);
        killProcess(rightPipePid);
        return;
    }
    if(!isBackGroundLeft) {
        waitProcess(leftPipePid);
        waitProcess(rightPipePid);
    }
    if(closePipe(writeFd) == -1) {
        printErr("Error al cerrar el pipe de escritura\n");
        killProcess(leftPipePid);
        killProcess(rightPipePid);
        return;
    }
}


//static void testMemManager()