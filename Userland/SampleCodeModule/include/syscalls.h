#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <stdint.h>
#include <color.h>
#include <stdio.h>
#include <stddef.h>

typedef struct PInfo{
    uint8_t priority;
    int16_t pid;
    int ground;
    uint64_t stackPos;
    uint64_t stackBase;
    uint8_t status;
    uint64_t rip;
    char * name;
}PInfo;

typedef struct PInfo * TPInfo;

typedef struct Node {
    void * info; 
    struct Node * next;
    struct Node * prev;
} Node;

typedef struct Node * TNode;

typedef struct linkedListCDT{
    TNode current;
    TNode first;
    TNode last;
    TNode iter; 
    int size;
} linkedListCDT;

typedef struct linkedListCDT * linkedListADT;

typedef struct semaphore_t{
    uint8_t idx;
    uint8_t value;
    uint8_t occupied;
    linkedListADT waitingList; 
}semaphore_t;

typedef struct semaphore_t * TSem;

typedef struct memoryInformation{
    uint64_t size;
    uint64_t used;
    uint64_t free;
}memoryInfo_t;

typedef struct MemoryManagerCDT * MemoryManagerADT;

/**
 * @brief Escribe a partir del descriptor recibido un caracter
 * @param fd: FileDescriptor (STDOUT | STDERR)
 * @param c: Caracter a escribir
 */
void write(int fd, char * c, uint64_t size);

/**
 * @brief Lee un byte a partir del descriptor recibido
 * @param fd: FileDescriptor (STDIN | KBDIN)
 * @return Byte leido
 */
uint8_t read(int32_t fd, char * c, uint64_t size);

/**
 * @brief Devuelve la hora expresada en segundos
 * @return Hora expresada en segundos
 */
uint32_t getSeconds();

/**
 * @brief Pone todos los pixeles de la pantalla en negro y limpia el buffer de video
 */
void clear(void);

/**
 * @brief 
 * @param regarr: Vector donde se llena la informacion de los registros
 * @return Puntero a la informacion de los registros
 */
uint64_t * getInfoReg(uint64_t * regarr);

/**
 * @brief Cambia el tamaño de la fuente
 * @param size: (1|2|3)
 */
void setFontSize(uint8_t size);

/**
 * @brief Devuelve las dimensiones de la pantalla
 * @return 32 bits menos significativos el ancho, 32 el alto 
 */
uint32_t getScreenResolution();

/**
 * @brief  Dibuja un rectangulo
 * @param  x: Origen en x 
 * @param  y: Origen en y 
 * @param  width: Ancho
 * @param  height: Alto
 * @param  color: Color de relleno
 */
void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);

/**
 * @brief Devuelve la cantidad de ticks actuales
 * @return Cantidad de ticks
 */
uint64_t getTicks();

/**
 * @brief Llena un vector con 32 bytes de informacion a partir de una direccion de memoria en hexa
 * @param pos: Direccion de memoria a partir de la cual se llena el vector 
 * @param vec: Vector en el cual se llena la informacion
 */
void getMemory(uint64_t pos, uint8_t * vec);

/**
 * @brief Ejecuta una excepcion de Invalid Opcode Exception
 */
void kaboom();

/**
 * @brief Establece un color de fuente
 * @param r: Color rojo
 * @param g: Color verde
 * @param b: Color azul
 */
void setFontColor(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Retorna el color de fuente que se esta usando actualmente
 * @return Color 
 */
Color getFontColor();

memoryInfo_t getMemInfo();

void exit();

uint64_t createProc(uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground);

TPInfo processInfo(uint64_t *processCant);

uint64_t getPid();

void killProcess(uint64_t pid);

uint64_t changePrio(uint64_t pid, uint8_t priority);

void blockProc(uint64_t pid);

int unblockProc(uint64_t pid);

void yield();

void chauCPU(); //no fue usada

int waitProcess(uint64_t pid);

uint16_t openPipe(uint16_t pid, uint8_t use);

uint16_t closePipe(uint16_t fd);

TSem semCreate(uint8_t value, char * name);

void semWait(TSem sem);

void semPost(TSem sem);

TSem semOpen(char * name);

uint16_t semClose(TSem sem);

void* allocMem(size_t memoryToAllocate);

void freeMem(void * const restrict memoryToFree);

void sleep(int seconds);

int changeFd(uint64_t pid, int16_t fileDescriptors[]);

#endif