// FAT 12 driver

#ifndef FAT12_H
#define FAT12_H 1

#include "../cscreen.h"

char* read_sector(int sector);
char* read_sectors(int sector, int n);
void write_sector(int sector, const char *contents);
void write_sectors(int sector, int n, const char *contents);

void interrupt_handler();
void init_disk();


#endif