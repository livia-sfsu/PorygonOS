#ifndef PORTS_H
#include "../cpu/ports.h"
#endif
#ifndef MEM_H
#include "../libc/mem.h"
#endif

#include "cscreen.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define VIDEO_MEMORY 0xb8000
#define STD_TEXT 0x0f
#define STD_ERR 0x4f

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

void __printc(char chr, int format);

// Clears the screen and resets cursor
void _cscreen() {
    char *vidMem = (char*) VIDEO_MEMORY;
    int sSize = SCREEN_WIDTH * SCREEN_HEIGHT;
    int i = 0;

    for (i; i < sSize; i++) {
        vidMem[i * 2] = 0;
        vidMem[i * 2 + 1] = STD_TEXT;
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
void _printbks() {
    int offset = _getcpos();
    offset -= 2;

    char *vidMem = (char*) VIDEO_MEMORY;
    vidMem[offset] = 0;
    vidMem[offset + 1] = STD_TEXT;

    _setcpos(offset);
}

// Turns the flashing cursor on
void _onc() {
    // lower cursor shape
    port_byte_out(REG_SCREEN_CTRL, 0x0a);
    // turn it on
    port_byte_out(REG_SCREEN_DATA, 14);
    // more shape stuff
    port_byte_out(REG_SCREEN_CTRL, 0x0b);
    // turn it on
    port_byte_out(REG_SCREEN_DATA, 15);
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

// Gets the current row for the cursor
int _getrowpos() {
    int offset = _getcpos();
    return (offset / (SCREEN_WIDTH * 2));
}

// Gets the current column for the cursor
int _getcolpos() {
    int offset = _getcpos();
    return (offset % (SCREEN_WIDTH * 2));
}


// internal functions

// Prints a character to the screen in a specified format
void __printc(char chr, int format) {
    char *vidMem = (char*) VIDEO_MEMORY;
    int offset = _getcpos();
    int maxOffset = SCREEN_HEIGHT * SCREEN_WIDTH * 2;
    int newOffset;
    int lineLength = SCREEN_WIDTH * 2;

    // Right, here's what we do:
    // 1) Work out the new offset
    // 2) Work out if we need to scroll a line

    if (chr == '\n') {
        int multiples = offset / lineLength + 1;
        newOffset = (multiples) * lineLength;
    } else {
        newOffset = offset + 2;
    }

    // At this point, we need to scroll the screen

    if (newOffset > maxOffset) {
        int i;
        uint32_t currOffset, prevOffset;

        // Copy over the bytes from each row to the one above
        for (i = 1; i < SCREEN_HEIGHT; i++) {
            currOffset = (uint32_t) vidMem + i * SCREEN_WIDTH * 2;
            prevOffset = (uint32_t) vidMem + (i - 1) * SCREEN_WIDTH * 2;
            memory_copy((char*) currOffset, (char*) prevOffset, SCREEN_WIDTH * 2);
        }

        // Erase the last row
        currOffset = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2;
        for (i = currOffset; i < maxOffset; i++)
            vidMem[i] = 0;

        _setcpos(currOffset);
        offset = currOffset;
    } else if (chr == '\n') {
        _setcpos(newOffset);
    }

    // Finally, put the character into place
    if (chr != '\n') {
        vidMem[offset] = chr;
        vidMem[offset + 1] = format;
        _setcpos(offset + 2);
    }
}