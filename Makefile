LD ?= ld
NASM ?= nasm
OBJCOPY ?= objcopy

LDFLAGS += -m elf_i386 -Ttext=0x7c00
NASMFLAGS += -f elf -g3 -F dwarf
OBJCOPYFLAGS += -O binary

run: fat
	dd if=boot16.bin of=floppy.img bs=1 skip=62 seek=62
	qemu-system-i386 -drive index=0,file=floppy.img,format=raw

fat: boot16.bin

boot16.elf: boot16.o
	$(LD) $^ $(LDFLAGS) -o $@

boot16.o: boot/bootsect.asm
	$(NASM) $^ $(NASMFLAGS) -o $@

boot16.bin: boot16.elf
	$(OBJCOPY) $^ $(OBJCOPYFLAGS) $@

clean:
	rm -rf *.bin *.img *.o *.elf *.img
	rm -rf */*.bin */*.img */*.o */*.elf */*.img