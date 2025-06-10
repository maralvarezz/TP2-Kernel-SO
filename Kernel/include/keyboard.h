#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include <stdint.h>
#include "semaphore.h"
#include "process.h"
#include "scheduler.h"

void keyboardHandler();

int8_t getAscii();

char getScancode();
#endif
