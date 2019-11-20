C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c graphics/*.c disk/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h graphics/*.h disk/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o} 

# Change this if your cross-compiler is somewhere else
CC = gcc
GDB = i386 gdb
# -m32 to target i386
# -g: Use debugging symbols in gcc
# We add -fno-pie
CFLAGS = -fno-pie -g -ffreestanding -Wall -Wextra -fno-exceptions -m32

boot16.bin: boot/bootsect.asm
	nasm $^ -f bin -o $@

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: boot/kernel_entry.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
kernel.elf: boot/kernel_entry.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ 

run: img
	qemu-system-i386 -fda porygon.img

# pad the binary to 512 bytes
img: boot/secondstage.bin kernel.bin boot16.bin
	# Make sure the mounting dir exists
	mkdir -p bootdiskmnt

	dd if=/dev/zero of=porygon.img bs=512 count=2880
	mkfs -t fat porygon.img
	echo "sudo is needed to mount disk"
	sudo mount porygon.img bootdiskmnt
	sudo cp boot/secondstage.bin bootdiskmnt/POS.SYS
	sudo cp kernel.bin bootdiskmnt/KRN.SYS
	sleep 1
	sudo umount bootdiskmnt
	dd if=boot16.bin of=porygon.img bs=512 count=1 seek=0 conv=notrunc
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
	rm -rf kernel/*.o boot/*.bin boot/*.img drivers/*.o boot/*.o cpu/*.o libc/*.o graphics/*.o
