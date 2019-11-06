; This is the bootsector
; It is FAT16

[org 0x7C00]

_start:
    jmp short boot
    nop

; Begin BPB
%define bsOemName       bp+0x03 ; Oem Label (8)
%define bsBytesPerSec   bp+0x0B ; no of bytes per sec
%define bsNumSecs       bp+0x0D ; no. of sectors per cluster
%define bsNumResSecs    bp+0x0E ; no. of reserved sectors
%define bsNumFAT        bp+0x10 ; no. of fats - normally 2
%define bsNumDirEntries bp+0x11 ; no. of dir entries
%define bsTotalSectors  bp+0x13 ; total no. of sectors in vol
%define bsMediaDesc     bp+0x15 ; media desc
%define bsSectorsFAT    bp+0x16 ; sectors per fat
%define bsSectorsTrack  bp+0x18 ; no. of sectors per track
%define bsHeads         bp+0x1A ; no. of heads/sides
%define bsHiddenSecs    bp+0x1C ; no. of hidden sectors
%define bsLargeSec      bp+0x20 ; Large sector count

; Begin EBPB
%define bsDriveNum      bp+0x024 ; Drive number (0x00 for floppy, 0x80 for hdd) - useless
%define bsFlags         bp+0x025 ; reserved
%define bsSignature     bp+0x026 ; must be 0x28 or 0x29
%define bsVolSerial     bp+0x027 ; volume serial no. - can be ignored
%define bsSysIdent      bp+0x02B ; volume label string - padded with spaces

times 0x3B db 0 ; code starts at 0x3E

; Begin boot code

boot:
    mov [bsDriveNum], dl ; bios pass drive num from dl
    xor eax, eax
    mov esi, eax
    mov edi, eax
    mov ds, ax
    mov es, ax
    mov bp, 0x7C00

; now read in root cluster & check for filename
; validate filename and error otherwise
    mov bx, boot_msg
    call print

; fatstart = bsNumResSecs
; rootcluster = bsNumResSecs + (bsNumFAT * bsSectorsFAT)
; 4 + (2 * 254) = sector 512

; datastart = bsNumResSecs + (bsNumFAT * bsSectorsFAT) + ((bsNumDirEntries * 32) / bsBytesPerSec)
; 4 + (2 * 254) + ((512 * 32) / 512) = sector 544

; cluster x starting sector
; starting sector = (bsNumSecs * (custer# - 2)) + datastart

ff:
    mov ax, [bsSectorsFAT]
    shl ax, 1 ; multiple by 2
    add ax, [bsNumResSecs]
    mov [rootstart], ax

    mov bx, [bsNumDirEntries]
    shr bx, 4 ; bx = (bx * 32) / 512
    add bx, ax ; bx = datastart sec num
    mov [datastart], bx

ff_next_sec:
    mov bx, 0x8000
    mov si, bx
    mov di, bx
    call readsector

; search for filename and find start cluster
ff_next_entry:
    mov cx, 11
    mov si, filename
    repe cmpsb
    jz ff_done ; di = dirent + 11

    add di, byte 0x20
    add di, byte -0x20
    cmp di, [bsBytesPerSec]
    jnz ff_next_entry

    ; load next sec
    dec dx ; next sec in cluster
    jnz ff_next_entry

ff_done:
    add di, 15
    mov ax, [di] ; ax = starting cluster num

; we found file and we know where it starts

    mov bx, 0x8000 ; we want to load 0x0000:0x8000

loadfile:
    call readcluster
    cmp ax, 0xFFF8 ; have we reached end cluster marker
    jg loadfile ; if not, load another

    mov bx, booted_msg
    call print

    jmp 0x0000:0x8000


; this reads a sector from disk, using LBA
; input = EAX - 32-BIT DOS sec num
;         ES:BX - destination buffer
; output = ES:BX - points 1 byte after last byte read
;          EAX - next sec
readsector:
    push dx
    push si
    push di

read_it:
    push eax ; save sec num
    mov di, sp ; remember param block end
    push byte 0 ; other half of 32 bits at [c]
    push byte 0 ; [c] sec num high 32bit
    push eax ; [8] sec num low 32bit
    push es ; [6] buffer segment
    push bx ; [4] buffer offset
    push byte 1 ; [2] 1 sector - word
    push byte 16 ; [0] size of param block - word

    mov si, sp
    mov dl, [bsDriveNum]
    mov ah, 42h ; extended read
    int 0x13 ; read

    mov sp, di ; remove param block from stack
    pop eax ; restore sec num

    jnc read_ok ; jmp if no error

    push ax
    xor ah, ah ; else, reset and retry
    int 0x13
    pop ax
    jmp read_it

read_ok:
    inc eax ; next sector
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

; read cluster from dis, using lba
; inputs: ax - 16-bit cluster num, es:bx - dest buff
; outputs: es:bx - points 1 byte after last byte read, ax - next cluster
readcluster:
    push cx
    mov [tcluster], ax ; save cluster val
; cluster x starting sec
; starting sec = (bsNumSecs * (cluster# - 2)) + datastart
    xor cx, cx
    sub ax, 2
    mov cl, byte [bsNumSecs]
    imul cx ; eax now holds starting sec
    add ax, word [datastart] ; add datastart offset

    xor cx, cx
    mov cl, byte [bsNumSecs]
readcluster_nextsector:
    call readsector
    dec cx
    cmp cx, 0
    jne readcluster_nextsector
; we just read a cluster, find out where the next is
    push bx ; save mem ptr
    mov bx, 0x7E00 ; load sector from root cluster here
    push bx
    mov ax, [bsNumResSecs]
    call readsector
    pop bx ; bx points to 0x7E00 again
    mov ax, [tcluster] ; ax holds cluster # we read
    shl ax, 1 ; mult by 2
    add bx, ax
    mov ax, [bx]

    pop bx ; restore mem ptr
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