#include "../cpu/interrupts.h"
#include "paging.h"
#include "heap.h"
#include "memory.h"


//Please check out: https://wiki.osdev.org/Paging
//The code may look similiar :)
//If you have any questions please ask because it gets confusing with bit shifting


extern void load_page_directory(uint32_t**);
extern void enable_paging();

uint32_t** _page_directory = 0;




uint32_t* get_page_table(){
	uint32_t* page_table = heap_alloc_page_frames(1);

	for(int i = 0; i < 1024; i++){
		page_table[i] = 0 | 2; //supervisor, write enabled, not present
	}

	return page_table;
}





uint32_t** get_page_directory(){
	uint32_t** page_directory = heap_alloc_page_frames(1);

	for(int i = 0; i < 1024; i++){
		page_directory[i] = (uint32_t*) ((uint32_t) get_page_table() | 2); //attributes: supervisor, write enabled, not present
	}

	return page_directory;
}



// The CR3 value, that is, the value containing the address of the page directory,
//  is in physical form. Once, then, the computer is in paging mode,
//   only recognizing those virtual addresses mapped into the paging tables,
//    how can the tables be edited and dynamically changed?

// Many prefer to map the last PDE to itself. The page directory will look like 
// a page table to the system. To get the physical address of any virtual address
//  in the range 0x00000000-0xFFFFF000 is then just a matter of: 



void paging_map_identity_page(void* addr, int size){
	uint32_t from = (uint32_t) addr;

//Checking a few things here
	//1)Page Directory is an actual thing
	//2)IF page table entry is present
	//3)An allignment check of the page being requested


	
	if(from % 4096 != 0){
		_prints("Page-alignment error\n");
		halt();
	}

	if(_page_directory != 0){ //if not zero, we dont have a directory yet, fail silently

		//Below Becomes funky to understand
	// 	//
	// Memory management Unit(MMU) reads it to map va to pa
 //    page dir lives in RAM
 //    1024 32-bit PDEs (page directory entry)
 //    each PDE describes 4 MB of virtual address space
 //    MMU uses top 10 bits of va as index
 //     replaces top bits with top bits of PDE
 //     this is virtual to physical translation

		//// Make sure that both addresses are page-aligned.
		uint32_t page_directory_index = (uint32_t) from >> 22;
		uint32_t page_table_index = (from >> 12) & 0x03ff;


		uint32_t* page_table = _page_directory[page_directory_index];
		if(((uint32_t)page_table & 0x1) == 0){ //page table not present, make one and insert it
			_page_directory[page_directory_index] = (uint32_t*) ((uint32_t)_page_directory[page_directory_index] | 3);

			paging_map_identity_page((void*) (((uint32_t) page_table) & 0xfffff000), 4096); //we need to be able to access the table after we enable paging
		}

		page_table = (uint32_t*) (((uint32_t) page_table) & 0xfffff000); //clear the flags
		page_table += page_table_index; //start from the desired index

		
		


		//The CPU pushes an error code on the stack before firing a page fault exception.
		//The error code must be analyzed by the exception handler to determine
		//how to handle the exception. The bottom 3 bits of the exception code are the only
		// ones used, bits 3-31 are reserved.

		//Bit 0 (P) is the Present flag.
		//Bit 1 (R/W) is the Read/Write flag.
		//Bit 2 (U/S) is the User/Supervisor flag.
		from = from & 0xfffff000; // discard bits we don't want
		int i = 0;
		for(;size>0;from+=4096,size-=4096,page_table++){
			i++;
			*page_table=from|3;     // mark page as supervisor, read/write, present.
			//Todo
		//Remake make this so user proccesses.
		//Will probably make a seperate pagingUser.c for t
		}
	}
}






//NOTE FOR UNMAPPING:
// Unmapping an entry is essentially the same as above, 
// but instead of assigning the pt[ptindex] a value, you set it
//  to 0x00000000 (i.e. not present). When the entire page table is empty,
//  you may want to remove it and mark the page directory entry 'not present'.
//   Of course you don't need the 'flags' or 'physaddr' for unmapping. 


//PAGE FAULTS:
//When a process does something the memory-management unit doesn't like, a page fault interrupt is thrown.
//*Reading from or writing to an area of memory that is not mapped (page entry/table's 'present' flag is not set)
//*The process is in user-mode and tries to write to a read-only page.
//*The process is in user-mode and tries to access a kernel-only page.
//*The page table entry is corrupted - the reserved bits have been overwritt
//The page fault interrupt is number 14.
//(Jamesm tutorial)

//Todo//TO MUST
//Virtual Address stepping stone


//This function gets called because the page fault interrupt gives us the memory address 
//Of where the fault was caused.
//This is located in reigster cr2
//But because there may be more than one error
//More interrupts will be thrown and address is usually lost if not saved immediately.
void page_fault_handler(interrupt_data_s* r){
	uint32_t virtual_address;
	__asm__ __volatile__ ( "mov %%cr2, %0" : "=r"(virtual_address) ); 
	//the virtual address that was attempted is stored in the cr2 register, extract it
	
	//These are throwing errors!
	//Need to fix
	//uint32_t page_directory_index = (uint32_t) virtual_address >> 22;
	//uint32_t page_table_index = (virtual_address >> 12) & 0x03ff;

	//printf("Page fault accesing %d from esp = %x\n", virtual_address, r->esp);
	
	//This is not working as should.
	//https://github.com/berkus/jamesm-tutorials/blob/master/src/vmm.c
	int z = 0;
	int y = 3 / z; //:)
	halt();
}

void paging_init(){
	interrupts_register_callback(INT_PAGE_FAULT, &page_fault_handler);

	uint32_t** page_directory = get_page_directory();

	_page_directory = page_directory;

	paging_map_identity_page(0x00000000, 1024 * 1024); //id page the first megabyte of space
	paging_map_identity_page(page_directory, PAGE_SIZE_BYTES); //id page the directory itself
	paging_map_identity_page(heap_get_metadata_ptr(), heap_get_metadata_size());

	load_page_directory(page_directory);
	enable_paging();
}
