#include "../std/stdint.h"
// #define GRAPHICS_H

// Represents the current cursor position
typedef struct {
    int x;
    int y;
} cursorPos;

// Now we need to define the colours
#define BLACK 0x0
#define BLUE 0x1
#define GREEN 0x2
#define CYAN 0x3
#define RED 0x4
#define MAGENTA 0x5
#define BROWN 0x6
#define GRAY 0x7
#define DARK_GRAY 0x8
#define BRIGHT_BLUE 0x9
#define BRIGHT_GREEN 0xA
#define BRIGHT_CYAN 0xB
#define BRIGHT_RED 0xC
#define BRIGHT_MAGENTA 0xD
#define BRIGHT_YELLOW 0xE
#define WHITE 0xF

// We need to store the current information about the screen
struct {
    uint8_t backgroundColor;
    uint8_t foregroundColor;
    cursorPos pos;
} screen_info;

void clear();
void set_text_color(uint8_t color);
void set_background_color(uint8_t color);
void set_cursor_pos(int x, int y);
cursorPos get_cursor_pos();
void print_text(const char *text);
void print_char(const char chr);
void print_text_at(const char *text, int x, int y);
void print_char_at(const char chr, int x, int y);
void print_text_center(const char *text, int y);
void print_char_center(const char chr, int y);
void enable_blinking();
void disable_blinking();