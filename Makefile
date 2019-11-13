LD ?= ld
NASM ?= nasm
OBJCOPY ?= objcopy

LDFLAGS += -m elf_i386 -Ttext=0x7c00
LDFLAGSX += -m elf_i386 -Ttext=0x7c00
NASMFLAGS += -f elf -g3 -F dwarf
OBJCOPYFLAGS += -O binary

run: fat
	# Copy the contents of the bin over
	dd if=/dev/zero of=temp.img bs=1024 count=1440
	dd if=boot16.bin of=temp.img seek=0 count=1 conv=notrunc

	# Create the ISO with the file created above
	mkdir iso
	cp temp.img iso/
	cp hello.bin iso/POS.SYS
	genisoimage -quiet -V 'POS' -input-charset iso8859-1 -o floppy.iso -b temp.img -hide temp.img iso/

	qemu-system-i386 -cdrom floppy.iso

fat: boot16.bin hello.bin

hello.bin: boot/hello.elf
	$(OBJCOPY) $^ $(OBJCOPYFLAGS) $@

boot/hello.elf: boot/hello.o
	$(LD) $^ $(LDFLAGS) -o $@

boot/hello.o: boot/hello.asm
	$(NASM) $^ $(NASMFLAGS) -o $@

boot16.bin: boot/boot16.elf
	$(OBJCOPY) $^ $(OBJCOPYFLAGS) $@
boot/boot16.elf: boot/boot16.o
	$(LD) $^ $(LDFLAGS) -o $@
boot/boot16.o: boot/bootsect.asm
	$(NASM) $^ $(NASMFLAGS) -o $@

clean:
	rm -rf *.bin *.img *.o *.elf *.img
	rm -rf */*.bin */*.img */*.o */*.elf */*.img
	rm -rf iso