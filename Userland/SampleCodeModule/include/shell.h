#ifndef _SHELL_H
#define _SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <syscalls.h>
#include <libasm.h>
#include <processes.h>
#include "tests.h"

#define QTY_COMMANDS 11   //26
/**
 * @brief Corre la terminal
 * 
 */
void run_shell();
#endif