[org 0x0700]

[bits 16]
_start:
    mov bx, entry_msg
    call print_nl
    call print
    call print_nl
    call switch_to_pm

    jmp $

%include "boot/load_kernel.asm"
%include "boot/print.asm"
%include "boot/print_hex.asm"
%include "boot/gdt.asm"
%include "boot/switch_pm.asm"
%include "boot/32bit_print.asm"

[bits 32]
BEGIN_PM:
    mov bx, protmode_msg
    call print_string_pm
    jmp $

entry_msg db "Reached second-stage bootloader. Entering protected mode...", 0
protmode_msg db "Reached protected mode. Handing over to the kernel...", 0