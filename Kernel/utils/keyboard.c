#include <keyboard.h>
#include <stdint.h>
#include <lib.h>
#include <video.h>
#include <time.h>
#include "scheduler.h"

#define BUFFER_CAPACITY 256 
#define LCTRL 29
#define LSHIFT 42
#define C 0x2E
#define D 0x20
#define R 0x13
#define RELEASED 0x80 
#define SHIFTED 0x80
#define EOF (-1)

static uint8_t _bufferStart = 0;                
static char _bufferSize = 0;                    
static uint8_t _buffer[BUFFER_CAPACITY] = {0};   

static uint8_t _ctrl = 0;					 
static uint8_t _shift = 0;
                                                
static const char charHexMap[] =             
    {   0,  0,  '1',  '2',  '3',  '4',  '5',  '6', '7',  '8',  '9', '0', '-',  '=',  '\b',  ' ',
        'q',  'w',  'e',  'r',  't',  'y',  'u',  'i', 'o',  'p',  '[', ']',  '\n',  0, 'a', 's',
        'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  '\'',  0,  0,  '\\', 'z',  'x', 'c', 'v',
        'b',  'n',  'm',  ',',  '.',  '/',  0,  '*', 0,  ' ',  0, 0, 0,  0, 0, 0};


static const char charHexMapShift[] = 
	{0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
	 '+', '\b', ' ', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	 '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	 ';', '"', 0, 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
	 '>', '?', 0, '*', 0, ' '};

static void writeKey(int8_t key);

/**
 * @brief  Obtiene el indice del elemento en la cola dado un corrimiento 
 * @param  offset: corrimiento
 * @return Indice del elemento en la cola
 */
static int getBufferIndex(int offset) {
	return (_bufferStart + offset) % (BUFFER_CAPACITY);
}

void keyboardHandler() {
	uint8_t key = getKeyPressed();
	if (!(key & RELEASED)) {
		if (key == LCTRL)
			_ctrl = 1;
		else if (key == LSHIFT)
			_shift = 1;
		else if (_ctrl) {
			if (key == C) {
				_bufferStart = _bufferSize = 0;
				killForegroundProcess();
			}
			else if (key == R)
				saveRegisters();
			else if (key == D && _bufferSize < BUFFER_CAPACITY - 1)
				writeKey(EOF);
		}
		else if (_bufferSize < BUFFER_CAPACITY - 1) {
			if (_shift)
				key = SHIFTED | key;
			writeKey(key);
		}
	}
	else {
		if (key == (LCTRL | RELEASED))
			_ctrl = 0;
		else if (key == (LSHIFT | RELEASED))
			_shift = 0;
	}
}

static void writeKey(int8_t key) {
	if (((key & 0x7F) < sizeof(charHexMap) && charHexMap[key & 0x7F] != 0) || (int) key == EOF) {
		_buffer[getBufferIndex(_bufferSize)] = key;
		_bufferSize++;
	}
}

char getScancode() {
    if(_bufferSize > 0){
        char c = _buffer[getBufferIndex(0)];
        _bufferStart = getBufferIndex(1);
        _bufferSize--;
        return c;
    }
    return 0;
}

int8_t getAscii() {
	int scanCode = getScancode();
	if (scanCode == EOF)
		return EOF;
	if (SHIFTED & scanCode) {
		scanCode &= 0x7F;
		return charHexMapShift[scanCode];
	}
	return charHexMap[scanCode];
}
