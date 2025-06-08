#ifndef _SHELL_H
#define _SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <syscalls.h>
#include <libasm.h>
#include "builtins.h"
#include <processes.h>
#include "tests.h"
#include "philosophers.h"

/**
 * @brief Corre la terminal
 * 
 */
void run_shell();
#endif