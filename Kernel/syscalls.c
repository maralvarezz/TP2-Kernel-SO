#include <stdint.h>
#include <video.h>
#include <keyboard.h>
#include <lib.h>
#include <color.h>
#include <time.h>
#include <memory.h>
#include <scheduler.h>
#include <pipes.h>
#include <semaphore.h>
#include <memoryManager.h>
/* File Descriptors*/
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define KBDIN 3

/* IDs de syscalls */
#define READ 0
#define WRITE 1
#define CLEAR 2
#define SECONDS 3
#define GET_REGISTER_ARRAY 4
#define SET_FONT_SIZE 5
#define GET_RESOLUTION 6
#define DRAW_RECT 7
#define GET_TICKS 8
#define GET_MEMORY 9
#define SET_FONT_COLOR 11
#define GET_FONT_COLOR 12

static uint8_t syscall_read(uint32_t fd);
// static void syscall_read(int64_t fd, char *buffer, uint64_t size);
static void syscall_write(uint32_t fd, char c);
static void syscall_clear();
static uint32_t syscall_seconds();
static uint64_t * syscall_registerArray(uint64_t * regarr);
static void syscall_fontSize(uint8_t size);
static uint32_t syscall_resolution();
static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
static uint64_t syscall_getTicks();
static void syscall_getMemory(uint64_t pos, uint8_t * vec);
static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b);
static uint32_t syscall_getFontColor();
static void syscall_exit();
static uint64_t syscall_createProcess(uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground);
static TPInfo syscall_processInfo(uint64_t *processCant);
static uint64_t syscall_getPid();
static void syscall_killProcess(uint64_t pid);
static uint64_t syscall_changePriority(uint64_t pid, uint8_t priority);
static void syscall_blockProcess(uint64_t pid);
static int syscall_unblockProcess(uint64_t pid);
static void syscall_chauCPU();
static int syscall_waitProcess(uint64_t pid);
static TSem syscall_semCreate(uint8_t value, char *name);
static void syscall_semWait(TSem sem);
static void syscall_semPost(TSem sem);
static TSem syscall_semOpen(char * name);
static uint16_t syscall_semClose(TSem sem);
static void syscall_yield();
static uint16_t syscall_openPipe(uint16_t pid, uint8_t use);
static uint16_t syscall_closePipe(uint16_t fd);
static memoryInfo_t syscall_memoryInfo();
static void syscall_malloc(const size_t memoryToAllocate);
static void syscall_free(void * const restrict memoryToFree);
static void syscall_sleep(int seconds);
static int syscall_changeFd(uint64_t pid, int16_t fileDescriptors[]);

typedef uint64_t (*syscallFunc)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {

    static syscallFunc syscalls[] = {
        (syscallFunc) syscall_read,
        (syscallFunc) syscall_write,
        (syscallFunc) syscall_clear,
        (syscallFunc) syscall_seconds,
        (syscallFunc) syscall_registerArray,
        (syscallFunc) syscall_fontSize,
        (syscallFunc) syscall_resolution,
        (syscallFunc) syscall_drawRect,
        (syscallFunc) syscall_getTicks,
        (syscallFunc) syscall_getMemory,
        (syscallFunc) syscall_setFontColor,
        (syscallFunc) syscall_getFontColor,
        (syscallFunc) syscall_memoryInfo,
        (syscallFunc) syscall_exit,
        (syscallFunc) syscall_createProcess,
        (syscallFunc) syscall_processInfo,
        (syscallFunc) syscall_getPid,
        (syscallFunc) syscall_killProcess,
        (syscallFunc) syscall_changePriority,
        (syscallFunc) syscall_blockProcess,
        (syscallFunc) syscall_unblockProcess,
        (syscallFunc) syscall_yield, 
        (syscallFunc) syscall_chauCPU,
        (syscallFunc) syscall_waitProcess, 
        (syscallFunc) syscall_openPipe,
        (syscallFunc) syscall_closePipe,
        (syscallFunc) syscall_semCreate,
        (syscallFunc) syscall_semWait,
        (syscallFunc) syscall_semPost,
        (syscallFunc) syscall_semOpen,
        (syscallFunc) syscall_semClose,
        (syscallFunc) syscall_malloc,
        (syscallFunc) syscall_free,
        (syscallFunc) syscall_sleep,
        (syscallFunc) syscall_changeFd,
    };

    return syscalls[nr](arg0, arg1, arg2, arg3, arg4, arg5);

	// switch (nr) {
    //     case READ:
    //         return syscall_read((uint32_t)arg0);
	// 	case WRITE:
	// 		syscall_write((uint32_t)arg0, (char)arg1);
    //         break;
    //     case CLEAR:
    //         syscall_clear();
    //         break;
    //     case SECONDS:
    //         return syscall_seconds();
    //     case GET_REGISTER_ARRAY:
    //         return (uint64_t) syscall_registerArray((uint64_t *) arg0);
    //     case SET_FONT_SIZE:
    //         syscall_fontSize((uint8_t)arg0);
    //         break;
    //     case GET_RESOLUTION:
    //         return syscall_resolution();
    //     case DRAW_RECT:
    //         syscall_drawRect((uint16_t)arg0, (uint16_t)arg1, (uint16_t)arg2, (uint16_t)arg3, (uint32_t)arg4);
    //         break;
    //     case GET_TICKS:
    //         return syscall_getTicks();
    //     case GET_MEMORY:
    //         syscall_getMemory((uint64_t) arg0, (uint8_t *) arg1);
    //         break;
    //     case SET_FONT_COLOR:
    //         syscall_setFontColor((uint8_t) arg0, (uint8_t) arg1, (uint8_t) arg2);
    //         break;
    //     case GET_FONT_COLOR:
    //         return syscall_getFontColor();
	// }
	// return 0;
}

// Read char
static uint8_t syscall_read(uint32_t fd){
    switch (fd){
        case STDIN:
            return getAscii();
        case KBDIN:
            return getScancode();
    }
    return 0;
}

// static void syscall_read(int64_t fd, char *buffer, uint64_t size) {
// 	int64_t fdProcess = fd; //getFileDescriptor(fd);
// 	if (fd >= 3) {
// 		readPipe(fdProcess, buffer, size);
// 	}
// 	else if (fd == STDIN) {
// 		char c;
// 		while( (c = getAscii()) == 0){
// 			blockProcess(getActualPid());
// 		}
// 		buffer[0] = c;
// 	}
// }

// Write char
static void syscall_write(uint32_t fd, char c){
    Color prevColor = getFontColor();
    if(fd == STDERR)
        setFontColor(ERROR_COLOR);
    else if(fd != STDOUT)
        return;
    printChar(c);
    setFontColor(prevColor);
}

// Clear
static void syscall_clear(){
    videoClear();
}

// Get time in seconds
static uint32_t syscall_seconds(){
    uint8_t h, m, s;
    getTime(&h, &m, &s);
    return s + m * 60 + ((h + 24 - 3) % 24) * 3600;
}

// Get register snapshot array
static uint64_t * syscall_registerArray(uint64_t * regarr){
    uint64_t * snapshot = getLastRegSnapshot();
    for(int i = 0; i < QTY_REGS; i++)
        regarr[i] = snapshot[i];
    return regarr;
}

// Set fontsize
static void syscall_fontSize(uint8_t size){
    setFontSize(size - 1);
}

// Get screen resolution
static uint32_t syscall_resolution(){
    return getScreenResolution();
}

// DrawRect
static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    ColorInt myColor = { bits: color };
    drawRect(x, y, width, height, myColor.color);
}

// GetTicks
static uint64_t syscall_getTicks(){
    return ticksElapsed();
}

//PrintMem
static void syscall_getMemory(uint64_t pos, uint8_t * vec){
    memcpy(vec, (uint8_t *) pos, 32);
}

//Set fontsize
static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b){
    setFontColor((Color){b, g, r});
}

//Get fontsize
static uint32_t syscall_getFontColor(){
    ColorInt c = { color: getFontColor() };
    return c.bits;
}

static memoryInfo_t syscall_memoryInfo(){
    return memoryInfo();
}

static void syscall_exit(){
    killActualProcess();
}

static uint64_t syscall_createProcess(uint64_t rip, char **args, int argc, uint8_t priority, int16_t fileDescriptors[], int ground){
    return createProcess(rip, args, argc, priority, fileDescriptors, ground);
}

static TPInfo syscall_processInfo(uint64_t *processCant){
    return processInformation(processCant);
}

static uint64_t syscall_getPid(){
    return getActualPid();
}

static void syscall_killProcess(uint64_t pid){
    killProcess(pid);
}

static uint64_t syscall_changePriority(uint64_t pid, uint8_t priority){
    return changePriority(pid, priority);
}

static void syscall_blockProcess(uint64_t pid){
    blockProcess(pid);
    yieldProcess();
}

static int syscall_unblockProcess(uint64_t pid){
    return readyProcess(pid);
}

static void syscall_yield(){
    yieldProcess();
}

static void syscall_chauCPU(){
    //hay que ver qué hacer acá, por ahora lo dejamos así
}

static int syscall_waitProcess(uint64_t pid){
    return waitProcess(pid);
}

static uint16_t syscall_openPipe(uint16_t pid, uint8_t use){
    return openPipe(pid, use);
}

static uint16_t syscall_closePipe(uint16_t fd){
    return closePipe(fd);
}

static TSem syscall_semCreate(uint8_t value, char *name){
    return buildSemaphore(value, name);
}

static void syscall_semWait(TSem sem){
    waitSemaphore(sem);
}

static void syscall_semPost(TSem sem){
    postSemaphore(sem);
}

static TSem syscall_semOpen(char * name){
    return openSem(name);
}

static uint16_t syscall_semClose(TSem sem){
    return closeSem(sem);
}

static void syscall_malloc(const size_t memoryToAllocate){
    allocMemory(memoryToAllocate);
}

static void syscall_free(void * const restrict memoryToFree){
    freeMemory(memoryToFree);
}

static void syscall_sleep(int seconds){
    uint32_t limite = syscall_seconds() + seconds;
    while(syscall_seconds() < limite){
        yieldProcess();
    }
}

static int syscall_changeFd(uint64_t pid, int16_t fileDescriptors[]){
    int resp = changeFds(pid, fileDescriptors);
    return resp;
}







