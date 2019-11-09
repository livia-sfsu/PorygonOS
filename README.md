# PorygonOS

To Keep people up to speed:

1)This is my exploration branch. I will be creating a branch that is better suited for merging than the mess here.

2)Been mostly focuesed on paging. Right now setting it up better for the scheduler.

3)The interrupt table was re-worked for better .C access when needed for paging, context switching, ect....

4)Most files new added files just have header files in them. Purely for organizing of the mind and what might to expect in the future.

5)Boot files were change around with a lot.

6)Read the read.me file for paging. Paging is not understandable without some background.

TODO:
1)Set up a kernelDirectory strcut for all page table, so it has its own page table and every page table has that struct that points back to it.
2)Get a different Branch for clean code for a easy merge.
3)Get the keyboard to acccess the proper interrupt for use
4)Clearer organization of memlayout.h



