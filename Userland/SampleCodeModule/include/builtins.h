#ifndef BUILTINS_H
#define BUILTINS_H

#include "syscalls.h"
#include "stdlib.h"
#include "tests.h"

void memInfo(int argc, char *argv[]);

void block(int argc, char *argv[]);

void unblock(int argc, char *argv[]);

void ps(int argc, char *argv[]);

void kill(int argc, char *argv[]);

void nice(int argc, char *argv[]);

#endif