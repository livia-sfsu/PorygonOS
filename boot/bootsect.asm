; This is the bootsector
; It is FAT16

[org 0x7c00]

_start:
    jmp short boot
    nop

bsOemName       db "POS     " ; OEM label
bsBytesPerSec   dw 0x200      ; bytes per sec
bsNumSecs       db 1          ; 1 sector per cluster - simplicity
bsNumResSecs    dw 1          ; num of reserved sectors
bsNumFAT        db 2          ; num of fat copies
bsNumDirEntries dw 224        ; size of root dir
bsTotalSectors  dw 2880       ; total num secs
bsMediaDesc     db 0xF0       ; media descriptor
bsSectorsFAT    dw 9          ; num of secs per fat
bsSectorsTrack  dw 18          ; sectors per track
bsHeads         dw 2          ; num of rw heads
bsHiddenSecs    dd 0          ; num of hidden secs
bsLargeSec      dd 0          ; num of > 32mb secs

; begin ebpb
bsDriveNum      db 0          ; drive number
bsFlags         db 0          ; reserved for flags
bsSignature     db 0x29       ; extended boot sig
bsVolSerial     db "seri"     ; disk serial
bsVolLabel      db "MYVOLUME   " ; volume label
bsSysIdent      db "FAT16   " ; fs type

times 0x3B db 0 ; code starts at 0x3E

; Begin boot code

boot:
   mov [bsDriveNum], dl   ; BIOS passes drive number in DL
   xor eax, eax
   xor esi, esi
   xor edi, edi
   mov ds, ax
   mov es, ax
   mov bp, 0x7c00

; read in the root cluster
; check for the filename
; if found save the starting cluster
; if not then error

;fatstart = bsResSectors

;rootcluster = bsResSectors + (bsFATs * bsSecsPerFat)
; 4 + (2 * 254) = sector 512

;datastart = bsResSectors + (bsFATs * bsSecsPerFat) + ((bsRootDirEnts * 32) / bsBytesPerSec)
; 4 + (2 * 254) + ((512 * 32) / 512) = sector 544

;cluster X starting sector
; starting sector = (bsSecsPerClust * (cluster# - 2)) + datastart

ff:
   mov ax, [bsSectorsFAT]
   shl ax, 1   ; quick multiply by two
   add ax, [bsNumResSecs]
   mov [rootstart], ax

   mov bx, [bsNumDirEntries]
   shr bx, 4   ; bx = (bx * 32) / 512
   add bx, ax   ; BX now holds the datastart sector number
   mov [datastart], bx
   
ff_next_sector:
   mov bx, 0x8000
   mov si, bx
   mov di, bx
   call readsector

; Search for file name, and find start cluster.
ff_next_entry:
   mov cx, 11
   mov si, filename
   repe cmpsb
   jz ff_done      ; note that di now is at dirent+11

   add di, byte 0x20
   and di, byte -0x20
   cmp di, [bsBytesPerSec]
   jnz ff_next_entry
   ; load next sector
   dec dx         ; next sector in cluster
   jnz ff_next_sector

ff_done:
   add di, 15
   mov ax, [di]   ; AX now holds the starting cluster #

; At this point we have found the file we want and know the cluster where the file starts

   mov bx, 0x8000   ; We want to load to 0x0000:0x8000
loadfile:   
   call readcluster
   cmp ax, 0xFFF8   ; Have we reached the end cluster marker?
   jg loadfile   ; If not then load another
   
   jmp 0x0000:0x8000

   
   
;------------------------------------------------------------------------------
; Read a sector from disk, using LBA
; input:   EAX - 32-bit DOS sector number
;      ES:BX - destination buffer
; output:   ES:BX points one byte after the last byte read
;      EAX - next sector
readsector:
   push dx
   push si
   push di

read_it:
   push eax   ; Save the sector number
   mov di, sp   ; remember parameter block end

   push byte 0   ; other half of the 32 bits at [C]
   push byte 0   ; [C] sector number high 32bit
   push eax   ; [8] sector number low 32bit
   push es    ; [6] buffer segment
   push bx    ; [4] buffer offset
   push byte 1   ; [2] 1 sector (word)
   push byte 16   ; [0] size of parameter block (word)

   mov si, sp
   mov dl, [bsDriveNum]
   mov ah, 42h   ; EXTENDED READ
   int 0x13   ; http://hdebruijn.soo.dto.tudelft.nl/newpage/interupt/out-0700.htm#0651

   mov sp, di   ; remove parameter block from stack
   pop eax      ; Restore the sector number

   jnc read_ok    ; jump if no error

   push ax
   xor ah, ah   ; else, reset and retry
   int 0x13
   pop ax
   jmp read_it

read_ok:
   inc eax       ; next sector
   add bx, 512      ; Add bytes per sector
   jnc no_incr_es      ; if overflow...

incr_es:
   mov dx, es
   add dh, 0x10      ; ...add 1000h to ES
   mov es, dx

no_incr_es:
   pop di
   pop si
   pop dx
   ret
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Read a cluster from disk, using LBA
; input:   AX - 16-bit cluster number
;      ES:BX - destination buffer
; output:   ES:BX points one byte after the last byte read
;      AX - next cluster
readcluster:
   push cx
   mov [tcluster], ax      ; save our cluster value
;cluster X starting sector
; starting sector = (bsSecsPerClust * (cluster# - 2)) + datastart
   xor cx, cx
   sub ax, 2
   mov cl, byte [bsNumSecs]
   imul cx      ; EAX now holds starting sector
   add ax, word [datastart]   ; add the datastart offset

   xor cx, cx
   mov cl, byte [bsNumSecs]
readcluster_nextsector:
   call readsector
   dec cx
   cmp cx, 0
   jne readcluster_nextsector

; Great! We read a cluster.. now find out where the next cluster is
   push bx   ; save our memory pointer
   mov bx, 0x7E00   ; load a sector from the root cluster here
   push bx
   mov ax, [bsNumResSecs]
   call readsector
   pop bx   ; bx points to 0x7e00 again
   mov ax, [tcluster] ; ax holds the cluster # we just read
   shl ax, 1   ; multipy by 2
   add bx, ax
   mov ax, [bx]
   
   pop bx   ; restore our memory pointer
   pop cx
   
   ret

print:
    pusha
print_start:
    mov al, [bx]
    cmp al, 0 ; will be zero if at end of string
    je print_done

    mov ah, 0x0e
    int 0x10 ; print char

    add bx, 1
    jmp print_start
print_done:
    popa
    ret

; declare some data!
filename db "POS        ", 0
datastart dw 0x0000
rootstart dw 0x0000
tcluster dw 0x0000

boot_msg: db 'Booting POS...', 0
booted_msg: db 'Successfully loaded BPB, EBPB', 0

; End boot code, pad with zeroes, begin boot sig
times 510-$+$$ db 0
sign dw 0xAA55