[bits 32]

;This enables paging
;First tell the processor where to find our page directory by putting it's address into the CR3 register. 
;This cannot be done with C code directly,
;So we ASM away!
;we use assembly to access CR3.

;Below is Standard paging code
;I got this from
;https://iron.triazo.net/triazo/kernel/tree/master/src



;;This function takes one parameter: the address of the page directory.
; It then loads the address onto the CR3 register, where the MMU will find it.

global load_page_directory


load_page_directory:
;establishing a new stack frame within the callee, 
;while preserving the stack frame of the caller.
	push ebp ;stores the previous base pointer (ebp)
	mov ebp, esp
	mov eax, [ebp+8];; Move value of parameter 1 into EAX
	mov cr3, eax;;move eax value to control register3
	mov esp, ebp
	pop ebp
	ret

global enable_paging

;We must set the 32th bit in the CR0 register, the paging bit. 

enable_paging:
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret

;Paging should now be enabled
