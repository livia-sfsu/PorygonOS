#ifndef PORTS_H
#include "../cpu/ports.h"
#endif


#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define VIDEO_MEMORY 0xb8000
#define STD_TEXT 0xf0
#define STD_ERR 0xf4

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

// Clears the screen and resets cursor
void _cscreen() {
    char *vidMem = (char*) VIDEO_MEMORY;
    int i, j;
    for (i = 0; i < SCREEN_HEIGHT; i++) {
        for (j = 0; j < SCREEN_WIDTH; j++) {
            vidMem[i * SCREEN_HEIGHT + j * 2] = ' ';
            vidMem[i * SCREEN_HEIGHT + j * 2 + 1] = STD_TEXT;
        }
    }
    
    _setcpos(0);
}

// Prints a character to the screen in standard format
void _printc(char chr) {
    __printc(chr, STD_TEXT);
}

// Prints a character to the screen in standard error format
void _printec(char chr) {
    __printc(chr, STD_ERR);
}

// Prints a string to the screen in standard format
void _prints(char *str) {
    int i = 0;

    while (str[i]) {
        __printc(str[i++], STD_TEXT);
    }
}

// Prints a string to the screen in standard error format
void _printes(char *str) {
    int i = 0;

    while (str[i]) {
        __printc(str[i++], STD_ERR);
    }
}

// Prints a backspace character and moves the cursor back by one
void _printbackspace() {
    int offset = _getcpos();
    offset -= 2;

    char *vidMem = (char*) VIDEO_MEMORY;
    vidMem[offset] = ' ';
    vidMem[offset] = STD_TEXT;

    _setcpos(offset);
}

// Turns the flashing cursor on
void _onc() {
    int offset = _getcpos();
    // lower cursor shape
    port_byte_out(REG_SCREEN_CTRL, 0x0a);
    // turn it on
    port_byte_out(REG_SCREEN_DATA, offset);
    // more shape stuff
    port_byte_out(REG_SCREEN_CTRL, 0x0b);
    // turn it on
    port_byte_out(REG_SCREEN_DATA, offset);
}

// Turns the flashing cursor off
void _offc() {
    port_byte_out(REG_SCREEN_CTRL, 0x0a);
    port_byte_out(REG_SCREEN_DATA, 0x20);
}

// Sets the cursor position, accounting for 2-byte cells
// Offset is calculated as SCREEN_WIDTH * y + x * 2;
void _setcpos(int offset) {
    offset = offset / 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (uint8_t) (offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (uint8_t) (offset & 0xff));
}

// Gets the cursor position, accounting for 2-byte cells
int _getcpos() {
    int offset = 0;
    port_byte_out(REG_SCREEN_CTRL, 14);
    offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);

    // finally, account for 2-byte cells
    return offset * 2;
}


// internal functions

// Prints a character to the screen in a specified format
void __printc(char chr, int format) {
    int offset = _getcpos();
    char *vidMem = (char*) VIDEO_MEMORY;
    vidMem[offset] = chr;
    vidMem[offset + 1] = format;
    _setcpos(offset + 2);
}