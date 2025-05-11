#include <stdio.h>
#include <string.h>
#include "memoryManager.h"
#include "tests.h"

int main() {
	static uint8_t memory_area[HEAPSIZE + STRUCTSIZE];
	createMemoryManager((void *) memory_area, HEAPSIZE + STRUCTSIZE);
	char *argvmm[] = {"266240"}; // 266240 bytes = HEAP_SIZE - STRUCT_SIZE
	test_mm(1, argvmm);
	return 0;
}