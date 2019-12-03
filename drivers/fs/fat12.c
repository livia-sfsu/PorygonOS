#include "fat12.h"
#include <stdint.h>

#ifndef ISR_H
#include "../../cpu/isr.h"
#endif

#include "fs.h"

#define FLOPPY_SECTORS_PER_TRACK 18

void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector);
void wait_for_irq();

// Writes a sector to disk
void write_sector(int sector, const char *contents)
{
    // We're using a floppy

}

void interrupt_handler()
{
    disk_interrupted = 1;
}




// This might be useful later on
void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector)
{
    *cyl = lba / (2 * FLOPPY_SECTORS_PER_TRACK);
    *head = ((lba % (2 * FLOPPY_SECTORS_PER_TRACK)) / FLOPPY_SECTORS_PER_TRACK);
    *sector = ((lba % (2 * FLOPPY_SECTORS_PER_TRACK)) % FLOPPY_SECTORS_PER_TRACK + 1);
}

// Waits for the interrupt
void wait_for_irq()
{
    while (disk_interrupted == 0) {
        ;
    }

    disk_interrupted = 0;
}

// Initialises the disk and registers the handler
void init_disk()
{
    register_interrupt_handler(IRQ6, interrupt_handler);
    detect_floppy_drives();
}