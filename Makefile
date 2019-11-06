C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c memory/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h memory/*.h libc/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupts.o} 

# Change this if your cross-compiler is somewhere else
CC = gcc
GDB = i386 gdb
# -m32 to target i386
# -g: Use debugging symbols in gcc
# We add -fno-pie
CFLAGS = -fno-pie -g -ffreestanding -Wall -Wextra -fno-exceptions -m32

# First rule is run by default
os-image.bin: boot/bootsect.bin kernel.bin
	cat $^ > os-image.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: kernel_entry.o interrupts.o paging_util.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

kernel_entry.o: boot/kernel_entry.asm
	#compile the kernel entry point but dont link, we need the contents of kernel.c
	nasm boot/kernel_entry.asm -f elf -o kernel_entry.o

interrupts.o: cpu/interrupts.asm
	nasm cpu/interrupts.asm -f elf -o interrupts.o	

paging_util.o: memory/paging_util.asm
	nasm memory/paging_util.asm -f elf -o paging_util.o

# Used for debugging purposes
kernel.elf: boot/kernel_entry.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ 

run: os-image.bin
	qemu-system-i386 -fda os-image.bin

# pad the binary to 512 bytes
img: os-image.bin
	dd if=/dev/zero bs=1024 count=128 of=porygon.img
	dd if=os-image.bin of=porygon.img conv=notrunc
	echo "porygon.img created, you may now run bochs"
	
bochs: img
	bochs

# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf
	qemu-system-i386 -s -fda os-image.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf *.img
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o memory/*.o
