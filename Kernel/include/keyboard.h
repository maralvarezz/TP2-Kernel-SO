#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include <stdint.h>
#include "semaphore.h"
#include "process.h"
#include "scheduler.h"

void initializeKeyboardDriver();

/* Handler de interrupciones de teclado */
void keyboardHandler();

/* Devuelve el valor ascii del ultimo caracter en el buffer de teclado */
int8_t getAscii();

/* Devuelve el scancode del ultimo caracter en el buffer de teclado */
char getScancode();
#endif
