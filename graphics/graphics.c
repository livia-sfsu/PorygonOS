#include "graphics.h"
#ifndef _SCREEN_H
#include "../drivers/cscreen.h"
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

// We need to print at the current position with the current bg and fg colours
void print_text(char *text) {
    int format = (screen_info.backgroundColor << 1) & screen_info.foregroundColor;
    _printcs(text, format);
}

// Same as print, but for a char
void print_char(char chr) {
    int format = (screen_info.backgroundColor << 1) & screen_info.foregroundColor;
    _printcc(chr, format);
}