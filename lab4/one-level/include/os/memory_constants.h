#ifndef	_memory_constants_h_
#define	_memory_constants_h_

//------------------------------------------------
// #define's that you are given:
//------------------------------------------------

// We can read this address in I/O space to figure out how much memory
// is available on the system.
#define	DLX_MEMSIZE_ADDRESS	0xffff0000

// Return values for success and failure of functions
#define MEM_SUCCESS 1
#define MEM_FAIL -1

//--------------------------------------------------------
// Put your constant definitions related to memory here.
// Be sure to prepend any constant names with "MEM_" so 
// that the grader knows they are defined in this file.

//--------------------------------------------------------

// in a virtual address.
#define MEM_L1FIELD_FIRST_BITNUM 12 // CHECK: Assuming its bytes
#define MEM_MAX_VIRTUAL_ADDRESS 0xFFFFF // 8bits for 256 pages, 12bits for page offset
#define MEM_MAX_SIZE (0x1 << 21) // Use a maximum physical memory size of 2MB

//Given to us in Lab Document under "Bitwise Tricks
// PTE codes
#define MEM_PTE_READONLY 0x4
#define MEM_PTE_DIRTY 0x2
#define MEM_PTE_VALID 0x1

#define MEM_PAGESIZE (0x1 << MEM_L1FIELD_FIRST_BITNUM)
#define MEM_PAGE_OFFSET_MASK (MEM_PAGESIZE - 1)
#define MEM_L1PAGETABLE_SIZE ((MEM_MAX_VIRTUAL_ADDRESS + 1) >> MEM_L1FIELD_FIRST_BITNUM)
#define MEM_MASK_PTE2PAGE (~(MEM_PTE_READONLY | MEM_PTE_DIRTY | MEM_PTE_VALID))
#define MEM_NUM_PAGES (MEM_MAX_SIZE / MEM_PAGESIZE)
#define MEM_FREEMAP_SIZE (MEM_NUM_PAGES/32)
#endif	// _memory_constants_h_
