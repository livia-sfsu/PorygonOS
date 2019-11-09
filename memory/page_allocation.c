#include "paging.h"
#include "heap.h"
#include "memory.h"
//Boiler plate code

//Reason why there is a lock system here:
//If an I/O controller (like the keyboard) is doing direct-memory access,
// it would be wrong to change pages in the middle of the I/O operation.
//Also
//There are things that the kernel may want to do, but 
//cannot tolerate having their pages swapped out. 
//What is here will be slow,
//And needs to be revised for a better system later.

//Check out http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html
//I followed him pretty literally
//Went to a few other tutorials but they just did varations of what he did.


//Todo
//All of this lock stuff needs to be abstracted out
//Can be used for context swiching later.
typedef int basic_lock;

basic_lock allocator_lock;

int liballoc_lock(){
	lock(&allocator_lock);

	return 0;
}

int liballoc_unlock(){
	unlock(&allocator_lock);
	return 0;
}

//TODO IMPLEMENT
void* liballoc_alloc(int pages){
	return heap_alloc_page_frames(pages);
}
//TODO IMPLEMENT
void liballoc_free(void* start, int pages){
	heap_free_page_frames(start, pages);
}

void lock(basic_lock* l){
	int result;


/*  __sync_bool_compare_and_swap
*is a built-in functions for atomic memory access
*
*"described in the Intel documentation to take “an optional list of variables protected by *the memory barrier”. It's not clear what is meant by that; it could mean that only the *following variables are protected, or it could mean that these variables should in *addition be protected. At present GCC ignores this list and protects all variables which *are globally accessible."

*/

	do{
		result = __sync_bool_compare_and_swap(l, 0, 1);
	} while(result == 0);
}

void unlock(basic_lock* l){
	__sync_bool_compare_and_swap(l, 1, 0);
}
