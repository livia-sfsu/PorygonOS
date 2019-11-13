; Just prints 'H'
_start:
    mov ah, 0x0e
    mov al, 'H'
    int 0x10
    jmp $

times 512 - ($-$$) db 0