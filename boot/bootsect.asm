; This is the bootsector
; It is FAT16

; [org 0x7c00]

; Skip past BPB, EBPB
_start:
    jmp short boot
    nop
   
bsOemName       db "POS     " ; OEM label
bsBytesPerSec   dw 0x200      ; bytes per sec
bsNumSecs       db 1          ; 1 sector per cluster - simplicity
bsNumResSecs    dw 1          ; num of reserved sectors
bsNumFAT        db 2          ; num of fat copies
bsNumDirEntries dw 16        ; size of root dir
bsTotalSectors  dw 250       ; total num secs
bsMediaDesc     db 0xF0       ; media descriptor
bsSectorsFAT    dw 4          ; num of secs per fat
bsSectorsTrack  dw 1          ; sectors per track
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
   mov [bsDriveNum], dl ; obtain drive number
   ; Initialise registers
   xor eax, eax
   mov esi, eax
   mov edi, eax

   mov ds, ax
   mov es, ax
   mov bp, 0x7C00

ff:
   mov ax, [bsSectorsFAT]
   shl ax, 1 ; mult by 2
   add ax, [bsNumResSecs]
   mov [rootstart], ax

   mov bx, [bsNumDirEntries]
   shr bx, 4 ; bx = (bx * 32) / 512
   add bx, ax ; bx is now datastart sector num
   mov [datastart], bx

ff_next_sector:
   mov bx, 0x8000
   mov si, bx
   mov di, bx
   call readsector

; search for filename, start cluster
ff_next_entry:
   mov cx, 11
   mov si, filename
   repe cmpsb
   jz ff_done ; di is now dirent+11

   add di, byte 0x20
   and di, byte -0x20
   cmp di, [bsBytesPerSec]
   jnz ff_next_entry
   ; load next sector
   dec dx ; next sec in cluster
   jnz ff_next_sector

ff_done:
   add di, 15
   mov ax, [di] ; ax now holds starting cluster num

   ; we have now found the file we want and we know the cluster where it starts
   mov bx, 0x8000
   ; want to load 0x0000:0x8000

loadfile:
   call readcluster
   cmp ax, 0xFFF8 ; have we reached the end cluster marker
   jg loadfile ; if not, read another

   mov ah, 0x0e
   mov al, 'Y'
   int 0x10
   jmp $
   jmp 0x0000:0x8000

; read a sector from the disk
readsector:
   push dx
   push si
   push di

read_it:
   push eax ; save sec num
   mov di, sp ; remember param block end

   push byte 0 ; other half of 32 bits at [c]
   push byte 0 ; [c] sec num high 32 bit

   push eax ; [8] sec num low 32bit
   push es ; [6] buff seg
   push bx ; [4] buff offset
   push byte 1 ; [2] 1 sector (word)
   push byte 16 ; [0] size of param block (word)

   mov si, sp
   mov dl, [bsDriveNum]
   mov ah, 42h ; extended read
   int 0x13

   mov sp, di ; remove param block from stack
   pop eax

   jnc read_ok ; jmp if no err

   mov dx, ax
   push ax
   xor ah, ah ; otherwise reset & retry
   int 0x13
   pop ax
   jmp read_it

read_ok:
   inc eax ; next sec
   add bx, 512 ; add bytes per sec
   jnc no_incr_es ; if overflow

incr_es:
   mov dx, es
   add dh, 0x10 ; add 1000h to es
   mov es, dx

no_incr_es:
   pop di
   pop si
   pop dx
   ret

; read a cluster from disk, using lba
readcluster:
   push cx
   mov [tcluster], ax ; save cluster val

   ; cluster x starting sec
   ; starting sec = (bssecsperclust * (cluster# - 2)) + datastart
   xor cx, cx
   sub ax, 2
   mov cl, byte [bsNumSecs]
   imul cx ; eax now holds start sec
   add ax, word [datastart] ; add datastart offset

   xor cx, cx
   mov cl, byte[bsNumSecs]
readcluster_nextsector:
   call readsector
   dec cx
   cmp cx, 0
   jne readcluster_nextsector

; we just read a cluster
; find out where the next one is
   push bx ; save mem ptr
   mov bx, 0x7E00 ; load sec from root cluster
   push bx
   mov ax, [bsNumResSecs]
   call readsector
   pop bx ; bx points to 0x7e00
   mov ax, [tcluster] ; ax holds cluster # we just read
   shl ax, 1 ; mult by 2
   add bx, ax
   mov ax, [bx]

   pop bx ; restore mem ptr
   pop cx
   
   ret

filename db "PURE64  SYS", 0
boot_msg: db 'Booting POS...', 0
booted_msg: db "Successfully loaded BPB, EBPB", 0

datastart dw 0x0000
rootstart dw 0x0000
tcluster dw 0x0000

; End boot code, pad with zeroes, begin boot sig
times 510-($-$$) db 0
dw 0xAA55