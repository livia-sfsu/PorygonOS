// This contains non fs-specific functions and data for interacting with floppies

#ifndef FS_H
#define FS_H 1

#include "../../cpu/ports.h"

static int disk_interrupted = 0;

// Now some useful values we need in order to write to floppies

// standard base address of primary floppy controller
static const int floppy_base = 0x03f0;
// standard irq num for floppy controllers
static const int floppy_irq = 6;

// registers
enum floppy_registers {
    FLOPPY_DOR = 2, // digital output register
    FLOPPY_MSR = 4, // master status register, read only
    FLOPPY_FIFO = 5, // data fifo, in dma op for commands
    FLOPPY_CCR = 7 // config control register, write only
};

// these are commands of interest
enum floppy_commands {
    CMD_SPECIFY = 3,
    CMD_WRITE_DATA = 5,
    CMD_READ_DATA = 6,
    CMD_RECALIBRATE = 7,
    CMD_SENSE_INTERRUPT = 8,
    CMD_SEEK = 15
};

struct floppy_drive {
    int drive_type;
};

static struct floppy_drive floppies[2];

void detect_floppy_drives();
char *get_drive_type(int drive);
void write_floppy_cmd(int base, char cmd);
unsigned char read_floppy_data(int base);

#endif