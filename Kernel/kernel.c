#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <interrupts.h>
#include <video.h>
#include <memoryManager.h>
#include "scheduler.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const memoryManagerModuleAddress = (void*)0x600000;

MemoryManagerADT memoryManagerPtr; 

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}




void initializeKernelBinary()
{
	void * moduleAddresses[] = { sampleCodeModuleAddress, sampleDataModuleAddress };
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	memoryManagerPtr = createMemoryManager( memoryManagerModuleAddress, HEAPSIZE);
}

void proceso1(){
	int aux= 50;
	while(aux--) {
		printf("Proceso 1 \n");
	}
	killProcess(getActualPid());
}



void proceso2(){
	while(1) {
		printf("Proceso 2\n");
	}
}

void proceso3(){
	while(1) {
		print("Proceso 3\n");
	}
}

void foo(){
	while(1){
		printf("Shell\n");
	}
}

int main()
{   
	_cli();
    
    createScheduler();
	//char * argv[] = { "shell" };
	char * argv1[] = { "P1" };
	char * argv2[] = { "P2" };
	//int16_t fileDescriptors[CANT_FILE_DESCRIPTORS] = { -1, 1, -1 };
	//createProcess((uint64_t)foo, argv, 1, SHELL_PRIORITY, fileDescriptors, 0);
	//createProcess((uint64_t)proceso1, argv1, 1, 5, fileDescriptors, 0);
	//createProcess((uint64_t)proceso2, argv2, 1, 5, fileDescriptors, 0);
	//createProcess((uint64_t)proceso3, argv2, 1, 5, fileDescriptors, 0);
    char * argv[] = { "shell" };
    int16_t fileDescriptors[CANT_FILE_DESCRIPTORS] = { 0, 1, 2 };
    createProcess((uint64_t)sampleCodeModuleAddress, argv, 1, SHELL_PRIORITY, fileDescriptors, 0);
	load_idt();
	_sti();
    while(1) {
		_hlt();
	}
    return 0;
}

