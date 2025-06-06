#ifndef TESTS_H
#define TESTS_H

#include <stdint.h>
#include "test_util.h"


uint64_t test_mm(uint64_t argc, char *argv[]);
uint64_t test_sync(uint64_t argc, char *argv[]);
int64_t test_processes(uint64_t argc, char *argv[]);
void test_prio();


#endif