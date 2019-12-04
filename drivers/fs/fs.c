#include "fs.h"

// This gets the type of drive for 0 and 1
void detect_floppy_drives()
{
    port_byte_out(0x70, 0x10);
    unsigned drives = port_byte_in(0x71);

    floppies[0].drive_type = drives >> 4;
    floppies[1].drive_type = drives & 0xf;
}

// This returns the type of the specified drive as a string
char *get_drive_type(int drive)
{
    const char *drive_types[8] = {
        "none",
        "360KB 5.25\"",
        "1.2MB 5.25\"",
        "720KB 3.5\"",
        "1.44MB 3.5\"",
        "2.88MB 3.5\"",
        "Unknown Type",
        "Unknown Type"
    };

    return drive_types[floppies[drive].drive_type];
}

// Writes a command
void write_floppy_cmd(int base, char cmd)
{
    int i;
    // Now we wait and timeout if longer than 60s
    for (i = 0; i < 600; i++) {
        // TODO: sleep here 10ms
        if (0x80 & port_byte_in(base + FLOPPY_MSR)) {
            return (void) port_byte_out(base + FLOPPY_FIFO, cmd);
        }
    }

    // we could panic here
}

// Reads data from the floppy
unsigned char read_floppy_data(int base)
{
    int i;
    // timeout for 60s again
    for (i = 0; i < 600; i++) {
        // sleep for 10ms
        if (0x80 & port_byte_in(base + FLOPPY_MSR)) {
            return port_byte_in(base + FLOPPY_FIFO);
        }
    }

    // could panic here
    return 0;
}

