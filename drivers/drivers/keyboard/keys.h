#include "../../cpu/type.h"

// We need somewhere to store and access flags about the keyboard, so here we are.
// These could all ultimately be stored in a single 8-bit integer

#ifndef KEYS_H
#define KEYS_H

struct {
    uint8_t caps;
    uint8_t lshift;
    uint8_t rshift;
    uint8_t lctrl;
    uint8_t rctrl;
} kflags;

#endif