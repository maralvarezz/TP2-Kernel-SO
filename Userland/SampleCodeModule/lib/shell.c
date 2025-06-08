#include "shell.h"
#define READ 0
#define WRITE 1
#define QTY_BYTES 32
#define DEFAULT_FONT_SIZE 1
#define MIN_FONT_SIZE 1
#define MAX_FONT_SIZE 3
#define BUILT_INS 14
#define PROCESSES 9
#define QTY_ARGS 3
#define MAX_SIZE 128
#define MAX_PARAMS 3
#define QTY_COMMANDS (BUILT_INS + PROCESSES + 1) 

#define WELCOME "Bienvenido a eSeMeMe SO!\n"
#define INVALID_COMMAND "Comando invalido!\n"
#define WRONG_PARAMS "La cantidad de parametros ingresada es invalida\n"
#define INVALID_FONT_SIZE "Dimension invalida de fuente\n"
#define CHECK_MAN "Escriba \"man %s\" para ver como funciona el comando\n"
#define CHECK_MAN_FONT "Escriba \"man font-size\" para ver las dimensiones validas\n"


typedef void (* functionType) (int argc, char * argv[]);

typedef struct {
    char * name;                    
    char * description;             
    functionType ftype;             
} Command;

static void help();
static void man(char * command);
static void printInfoReg();
static void time();
static int div(int num, char * div[]);
//static int div(char * num, char * div);
static void fontSize(char * size);
static void printMem(char * pos);
static int getCommandIndex(char * command);
static void myClear();
static void testProcess();
static int scanCommand(char *commandLine, char *command, char *args[QTY_ARGS]);
static int hasPipe(char * line);
static void executePipeCommands(char *leftCom, char *rightCom, char *leftParam[], char *rightParam[], int leftParamQuantity, int rightParamQuantity, int leftId, int rightId,int isBackGroundLeft, int isBackGroundRight);


static Command commands[] = {
    { "help", "Listado de comandos", (functionType) help},
    { "man", "Manual de uso de los comandos", (functionType) man},
    { "inforeg", "Informacion de los registos que fueron capturados en un momento arbitrario de ejecucion del sistema", (functionType) printInfoReg},
    { "time", "Despliega la hora actual UTC - 3", (functionType) time},
    { "div", "Hace la division entera de dos numeros naturales enviados por parametro",(functionType) div},
    { "kaboom", "Ejecuta una excepcion de Invalid Opcode", (functionType) kaboom},
    { "font-size", "Cambio de dimensiones de la fuente. Para hacerlo escribir el comando seguido de un numero", (functionType) fontSize},
    { "printmem", "Realiza un vuelco de memoria de los 32 bytes posteriores a una direccion de memoria en formato hexadecimal enviada por parametro", (functionType) printMem},
    { "clear", "Limpia toda la pantalla", (functionType) myClear},
    { "mem", "Imprime el estado de la memoria", (functionType) memInfo},
    { "block", "Cambia el estado de un proceso entre bloqueado y listo dado su ID", (functionType) block},
    { "unblock", "Cambia el estado de un proceso entre listo y bloqueado dado su ID", (functionType) unblock},
    { "ps", "Imprime la lista de todos los procesos con sus propiedades", (functionType) ps},
    { "kill", "Mata un proceso dado su ID", (functionType) kill},
    { "nice", "Cambia la prioridad de un proceso dado su ID y la nueva prioridad", (functionType) nice},
    { "loop", "Imprime el ID del proceso con un saludo cada una determinada cantidad de segundos",(functionType) loop},
    { "cat", "Imprime el stdin tal como lo recibe", (functionType) cat},
    { "wc", "Cuenta la cantidad de lineas del input", (functionType) wc},
    { "filter", "Filtra las vocales del input", (functionType) filter},
    { "phylo", "Implementa el problema de los filosofos comensales", (functionType) mainPhilo},
    { "testMemManager", "Corre un test para los memory managers", (functionType) testMemManager},
    { "testPriority", "Corre un test de prioridades", (functionType) testPriority},
    { "testProcesses", "Corre un test de procesos", (functionType) testProcesses},
    { "testSync", "Corre un test de sincronizacion", (functionType) testSync},
};


void run_shell() {
    int index;
    puts(WELCOME);
    char *line = (char *)allocMem(MAX_CHARS* sizeof(char));
    if(line == NULL) {
        printErr("Error al asignar memoria para la linea de comandos\n");
        exit();
        return;
    }
    while(1){
        putchar('>');
        scanf("%l", line);
        char * leftCom = allocMem(MAX_CHARS * sizeof(char *));
        if(leftCom == NULL) {
            printErr("Error al asignar memoria para el comando izquierdo\n");
            continue;
        }
        char ** leftParam = (char **)allocMem(MAX_CHARS * sizeof(char *));
        if(leftParam == NULL) {
            printErr("Error al asignar memoria para los parametros del comando izquierdo\n");
            freeMem(leftCom);
            continue;
        }
        int cantLeft;
        char *pipePos = strchr(line, '|');
        if(hasPipe(line)){
            char * rightCom = allocMem(MAX_CHARS*sizeof(char *)); //ver si no poner un MAX_LENGTH
            if(rightCom == NULL) {
                printErr("Error al asignar memoria para el comando derecho\n");
                freeMem(leftCom);
                freeMem(leftParam);
                continue;
            }

            char ** rightParam = (char **)allocMem(MAX_CHARS * sizeof(char *)); //ver si no poner un MAX_LENGTH 
            if(rightParam == NULL) {
                printErr("Error al asignar memoria para los parametros del comando derecho\n");
                freeMem(leftCom);
                freeMem(leftParam);
                freeMem(rightCom);
                continue;
            }
            *pipePos = '\0';
            cantLeft = scanCommand(line, leftCom, leftParam);

            int cantRight = scanCommand(pipePos + 2, rightCom, rightParam);

            int leftIdx = getCommandIndex(leftCom);

            int rightIdx = getCommandIndex(rightCom);

            if(strcmp(leftCom, commands[leftIdx].name) == 0 && strcmp(rightCom, commands[rightIdx].name) == 0){
                char *newParams1[QTY_ARGS+1] = {0};
                newParams1[0] = commands[leftIdx].name;
                for(int i = 0; i < cantLeft; i++) {
                    newParams1[i + 1] = leftParam[i];
                }
                int isBackGroundLeft = 0;
                if(cantLeft){
                    isBackGroundLeft = (leftParam[cantLeft - 1][0] == '&');
                }
                char *newParams2[QTY_ARGS+1] = {0};
                newParams2[0] = commands[rightIdx].name;
                for(int i = 0; i < cantRight; i++) {
                    newParams2[i + 1] = rightParam[i];
                }
                int isBackGroundRight = 0;
                if(cantRight){
                    isBackGroundRight = (rightParam[cantRight - 1][0] == '&');
                }
                if(!isBackGroundLeft && !isBackGroundRight) {
                    executePipeCommands(leftCom, rightCom, newParams1, newParams2, cantLeft + 1, cantRight + 1, leftIdx, rightIdx, isBackGroundLeft, isBackGroundRight);
                }
                else {
                    printf("No se pueden tener procesos en background con pipes\n");
                }
                for(int i = 0; i < cantLeft; i++) {
                    freeMem(leftParam[i]);
                }
                for(int i = 0; i < cantRight; i++) {
                    freeMem(rightParam[i]);
                }
                freeMem(rightParam);
                freeMem(rightCom);
            }
            else {
                printf("Para ejecutar un pipe, ambos comandos deben ser procesos\n");
            }
        }
        else {
            cantLeft = scanCommand(line, leftCom, leftParam);
            int id = getCommandIndex(leftCom);
            if(id < BUILT_INS && id >=0) {
                commands[id].ftype(cantLeft, leftParam);
            }
            else if(id >= BUILT_INS && id < QTY_COMMANDS) {
                char *newParams[QTY_ARGS+1] = {0};
                int indexProc = id - BUILT_INS;
                uint64_t rip = (uint64_t)commands[id].ftype;
                newParams[0] = commands[id].name;
                for(int i = 0; i < cantLeft; i++) {
                    newParams[i + 1] = leftParam[i];
                }
                int isBackGround = 0;
                int16_t fileDescriptors[] = { STDIN, STDOUT, STDERR };
                if(cantLeft){
                    isBackGround = (leftParam[cantLeft - 1][0] == '&');
                }
                cantLeft += isBackGround ? -1 : 0;
                uint64_t pid = createProc(rip, newParams, cantLeft + 1, 2, fileDescriptors, !isBackGround);
                if(pid == -1) {
                    printErr("Error al crear el proceso\n");
                    for(int i = 0; i < cantLeft; i++) {
                        freeMem(leftParam[i]);
                    }
                    continue;
                }
                if(unblockProc(pid) == -1) {
                    printErr("Error al desbloquear el proceso\n");
                    killProcess(pid);
                    for(int i = 0; i < cantLeft; i++) {
                        freeMem(leftParam[i]);
                    }
                    freeMem(leftCom);
                    continue;
                }
                if(!isBackGround) {
                    waitProcess(pid);
                }
            }
            else{
                printf("El comando %s no es un comando valido\n", leftCom);
            }
        }
        for(int i = 0; line[i] != '\0'; i++) {
            line[i] = '\0';
        }
        for(int i = 0; i < cantLeft; i++) {
            freeMem(leftParam[i]);
        }
        freeMem(leftParam);
        freeMem(leftCom);
    }
    freeMem(line);
    exit();
}


static int hasPipe(char * line) {
    for(int i = 0; line[i] != '\0'; i++) {
        if(line[i] == '|') {
            return 1;
        }
    }
    return 0;
}

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

static int div(int num, char * div[]) {
    printf("Dividiendo %s entre %s\r\n", div[0], div[1]); 
    //printf("%s/%s=%d\r\n", num, div, n/d);
    return 1;
}

/*static int div(char * num, char * div) {
    printf("Dividiendo %s entre %s\r\n", num, div); 
    //printf("%s/%s=%d\r\n", num, div, n/d);
    return 1;
}*/

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
        for(j = 0; commandLine[i] != ' ' && commandLine[i] != '\0'; i++,j++){
            args[k][j] = commandLine[i];
        }
        args[k][j] = '\0';
        while(commandLine[i] == ' '){
            i++;
        }
    }
    return k;
}



static void executePipeCommands(char *leftCom, char *rightCom, char *leftParam[], char *rightParam[], int leftParamQuantity, int rightParamQuantity, int leftId, int rightId,int isBackGroundLeft, int isBackGroundRight) {
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
