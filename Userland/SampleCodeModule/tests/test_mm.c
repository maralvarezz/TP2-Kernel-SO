#include "syscalls.h"
#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loader.h"

#define MAX_BLOCKS 128

typedef struct MM_rq {
    void *address;
    uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {
    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory;

    if (argc != 1) {
        printf("Error: cantidad de argumentos incorrecta\n");
        return -1; 
    }

    max_memory = satoi(argv[0]);

    if (max_memory <= 0) {
        printf("Error: memoria maxima invalida\n");
        return -1; 
    }

    printf("Iniciando test de memoria con maximo %d bytes\n", max_memory);
    rq = 0;
    total = 0;

    printf("Solicitando bloques de memoria...\n");
    while (rq < MAX_BLOCKS && total < max_memory) {
        uint32_t size_to_alloc = GetUniform(max_memory - total - 1) + 1;
        mm_rqs[rq].address = allocMem(size_to_alloc);

        if (mm_rqs[rq].address != NULL) {
            mm_rqs[rq].size = size_to_alloc;
            total += mm_rqs[rq].size; 
            rq++;
        }
    }

    printf("Inicializando bloques con memset...\n");
    uint32_t i;
    for (i = 0; i < rq; i++) {
        if (mm_rqs[i].address) {
            memset(mm_rqs[i].address, i % 256, mm_rqs[i].size); 
        }
    }

    printf("Verificando bloques con memcheck...\n");
    for (i = 0; i < rq; i++) {
        if (mm_rqs[i].address) {
            if (!memcheck(mm_rqs[i].address, i , mm_rqs[i].size)) { 
                return -1; 
            }
        }
    }

    printf("Liberando bloques de memoria...\n");
    for (i = 0; i < rq; i++) {
        if (mm_rqs[i].address) {
            freeMem(mm_rqs[i].address); 
        }
    }
    return 0;
}