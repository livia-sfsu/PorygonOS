#ifndef PORTS_H
#include "../cpu/ports.h"
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
        vidMem[i * 2] = ' ';
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
    vidMem[offset] = ' ';
    vidMem[offset] = STD_TEXT;

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
    int newLineNeeded = 0;

    int finalRow = SCREEN_HEIGHT - 1;
    if (chr == '\n' && _getrowpos() >= finalRow)
        newLineNeeded = 1;
    else if (offset + 2 > maxOffset)
        newLineNeeded = 1;

    int newchrpos, prevchrpos;

    // Check to see if we need to scroll
    if (newLineNeeded) {
        // We need to scroll, so let's shift the contents up by 1
        int i, j;
        for (i = 0; i < SCREEN_HEIGHT - 1; i++) {
            for (j = 0; j < SCREEN_WIDTH; j++) {
                // Copy text, then format
                newchrpos = (i * (SCREEN_WIDTH * 2) + j * 2);
                prevchrpos = ((i + 1) * (SCREEN_WIDTH * 2) + j * 2);                

                vidMem[newchrpos] = vidMem[prevchrpos];
                vidMem[newchrpos + 1] = vidMem[prevchrpos + 1];
            }
        }

        // Now, we need to make sure that the final line is clear of text
        for (i = 0; i < SCREEN_WIDTH; i++) {
            vidMem[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + (i * 2)] = ' ';
            vidMem[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + (i * 2) + 1] = STD_TEXT;
        }

        // Finally, move the cursor to the new position
        _setcpos((SCREEN_HEIGHT - 2) * SCREEN_WIDTH * 2);
    }

    if (chr == '\n' && !newLineNeeded) {
        int noLines = offset / (SCREEN_WIDTH * 2);
        int newOffset = (noLines + 1) * (SCREEN_WIDTH * 2);
        _setcpos(newOffset);
    } else {
        vidMem[offset] = chr;
        vidMem[offset + 1] = format;
        _setcpos(offset + 2);
    }
}