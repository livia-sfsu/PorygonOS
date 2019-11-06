#include "keyboard.h"
#include "../cpu/ports.h"
#include "../cpu/interrupts.h"
#include "cscreen.h"
#include "../libc/string.h"
#include "../libc/function.h"
#include "../kernel/kernel.h"
#include "keyboard/keys.h" // flags for keyboard
#include <stdint.h>

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define L_SHIFT 0x2A
#define R_SHIFT 0x36
#define CAPS_LOCK 0x3A

#define RELEASE_KEY 0x80

static char key_buffer[256];

#define SC_MAX 57
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y', 
        'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g', 
        'h', 'j', 'k', 'l', ';', '\'', '#', '?', '\\', 'z', 'x', 'c', 'v', 
        'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

//Giving me an error
// const char sc_uppercase[] = { '?', '?', '!', '"', '£', '$', '%', '^',     
//     '&', '*', '(', ')', '_', '+', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
//         'U', 'I', 'O', 'P', '{', '}', '?', '?', 'A', 'S', 'D', 'F', 'G', 
//         'H', 'J', 'K', 'L', ':', '@', '~', '?', '|', 'Z', 'X', 'C', 'V', 
//         'B', 'N', 'M', '<', '>', '?', '?', '?', '?', ' '};

static void keyboard_callback(interrupt_data_s* r) {
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = port_byte_in(0x60);

    // Check to see if we've released a key or not!
    uint8_t released = scancode & RELEASE_KEY;
    if (released) {
        released = scancode ^ RELEASE_KEY; // reuse released to rep key released

        switch(released) {
            case L_SHIFT:
                kflags.lshift = 0;
                break;
            case R_SHIFT:
                kflags.rshift = 0;
                break;
        }
    } else {
        switch(scancode) {
            case L_SHIFT:
                kflags.lshift = 1;
                break;
            case R_SHIFT:
                kflags.rshift = 1;
                break;
            case CAPS_LOCK:
                kflags.caps = kflags.caps ? 0 : 1;
                break;
            default:
                processKeypress(scancode);
        }
    }

   // UNUSED(regs); Just being lazy error=comment out
    return;
}

void processKeypress(uint8_t scancode) {
    /* Does not get executed! */
    if (scancode > SC_MAX) return;
    if (scancode == BACKSPACE) {
        int len = backspace(key_buffer);
        if (len > 0)
            _printbks();
    } else if (scancode == ENTER) {
        _prints("\n");
        //user_input(key_buffer); /* kernel-controlled function */
        key_buffer[0] = '\0';
    } else {
        char letter;
		//I don't feel like debugging at the moment.
        if (/*kflags.caps || */kflags.lshift || kflags.rshift) {
            //letter = sc_uppercase[(int)scancode];
        } else
            letter = sc_ascii[(int)scancode];
        /* Remember that kprint only accepts char[] */
        char str[2] = {letter, '\0'};
        append(key_buffer, letter);
        _prints(str);
    }
}


//Updating changes to interrupt table
void keyboard_init(){
	interrupts_register_callback(INT_KEYBOARD, &keyboard_callback);
}
