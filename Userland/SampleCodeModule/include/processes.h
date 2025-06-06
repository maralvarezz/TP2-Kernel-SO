#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdio.h>
#include <syscalls.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PROCESSES 100


void loop(int argc, char *argv[]);

void cat(int argc, char *argv[]);

void wc(int argc, char *argv[]);

void filter(int argc, char *argv[]);

void mainPhilo(int argc, char *argv[]);

void testMemManager(int argc, char *argv[]);

void testPriority(int argc, char *argv[]);

void testProcesses(int argc, char *argv[]);

void testSync(int argc, char *argv[]);

#endif