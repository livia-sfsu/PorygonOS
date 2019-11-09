//Still Figuring this out
//this is a copy from xv6
//this is greate for understanding and organization for mind machine and soul

// MEMEORY LAYOUT

//TODO:
//SET UP

//#define EXTMEM  ?????            // Start of extended memory
//#define PHYSTOP 0xE000000           // Top physical memory
//#define DEVSPACE 0xFE000000         // Other devices are at high addresses

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0x00001000         // First kernel virtual address
//#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked

//#define V2P(a) (((uint) (a)) - KERNBASE)
//#define P2V(a) ((void *)(((char *) (a)) + KERNBASE))

//#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
//#define P2V_WO(x) ((x) + KERNBASE)    // same as P2V, but without casts