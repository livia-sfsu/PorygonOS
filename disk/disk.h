#ifndef STDINT_H
#include "../std/stdint.h"
#endif

char *read_disk(uint16_t sector, uint16_t n);
void write_disk(uint16_t sector, char *content);