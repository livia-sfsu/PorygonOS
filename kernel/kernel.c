#include "../cpu/isr.h"
#include "kernel.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../drivers/cscreen.h"
#include <stdint.h>
#include "../shellder/shellder.h"

void kernel_main() {
    isr_install();
    irq_install();

    asm("int $2");
    asm("int $3");

    _cscreen();
    _prints(" shdr > ");
}

//*
void user_input(char *input) {
    text_buffer = input;     // text_buffer is declared in kernel.h
    shellder();
}
//*/
/*
void user_input(char *input) {
    if (strcmp(input, "END") == 0) {
        _prints("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (strcmp(input, "PAGE") == 0) {
        // Lesson 22: Code to test kmalloc, the rest is unchanged //
        uint32_t phys_addr;
        uint32_t page = kmalloc(1000, 1, &phys_addr);
        char page_str[16] = "";
        hex_to_ascii(page, page_str);
        char phys_str[16] = "";
        hex_to_ascii(phys_addr, phys_str);
        _prints("Page: ");
        _prints(page_str);
        _prints(", physical address: ");
        _prints(phys_str);
        _prints("\n");
    }
    _prints("You said: ");
    _prints(input);
    _prints("\n> ");
}
*/

