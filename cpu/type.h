#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

#endif

typedef uint32_t uint_t;
typedef uint32_t size_t;


//G: Because I come from Java
/*typedef enum{
	false=0,
	true=1
} bool; */

//Needed
//a pointer to a location in memory
typedef char* memloc_t; 





