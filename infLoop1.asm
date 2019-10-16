; this is how you wrtie comments WOwee
loop:
	jmp loop

; a bunch of zeros
times 510-($-$$) db 0
; magic number Wowee
dw 0xaa55