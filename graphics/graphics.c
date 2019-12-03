#include <stdint.h>
#include "graphics.h"
#ifndef _SCREEN_H
#include "../drivers/cscreen.h"
#endif
#ifndef STRING_H
#include "../libc/string.h"
#endif

// Clears the screen
void clear() {
    uint8_t format = (screen_info.backgroundColor << 4) | screen_info.foregroundColor;
    _ccscreen(format);
}

// Sets the current text colour
void set_text_color(uint8_t color) {
    screen_info.foregroundColor = color;
}

// Sets the current background colour
void set_background_color(uint8_t color) {
    screen_info.backgroundColor = color;
}

// Sets the current cursor position
void set_cursor_pos(int x, int y) {
    screen_info.pos.x = x;
    screen_info.pos.y = y;

    // Now we set the offset
    _setcposx(x, y);
}

// Gets the current cursor position
cursorPos get_cursor_pos() {
    return screen_info.pos;
}

// We need to print at the current position with the current bg and fg colours
void print_text(const char *text) {
    int format = (screen_info.backgroundColor << 4) | screen_info.foregroundColor;
    _printcs(text, format);
}

// Same as print, but for a char
void print_char(const char chr) {
    int format = (screen_info.backgroundColor << 4) | screen_info.foregroundColor;
    _printcc(chr, format);
}

// Prints a string at a specified location
void print_text_at(const char *text, int x, int y) {
    set_cursor_pos(x, y);
    print_text(text);
}

// Prints a character at a specified position
void print_char_at(const char chr, int x, int y) {
    set_cursor_pos(x, y);
    print_char(chr);
}

// Prints a string in the centre of the screen
void print_text_center(const char *text, int y) {
    // We'll assume the width of the screen is 80 for now
    int length = strlen(text);
    int screen_width = 80;
    int pos = ((screen_width / 2) - (length / 2)) + 1;
    if (pos < 0) // Make sure it's a valid value
        pos = 0;
    set_cursor_pos(pos, y);
    print_text(text);
}

// Prints a character in the centre of the screen
void print_char_center(const char chr, int y) {
    // assume screen is 80x24
    int screen_width = 80;
    int pos = ((screen_width / 2) - (1 / 2)) + 1;
    set_cursor_pos(pos, y);
    print_char(chr);
}

// Enables the blinking cursor
void enable_blinking() {
    _onc();
}

// Disables the blinking cursor
void disable_blinking() {
    _offc();
}