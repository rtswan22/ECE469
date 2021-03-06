//
//	memory.c
//
//	Routines for dealing with memory management.

//static char rcsid[] = "$Id: memory.c,v 1.1 2000/09/20 01:50:19 elm Exp elm $";

#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "memory.h"
#include "queue.h"
#include "memory_constants.h"

// num_pages = size_of_memory / size_of_one_page
static uint32 freemap[MEM_FREEMAP_SIZE];
static uint32 pagestart;
static int nfreepages;
static int freemapmax;
static int maxHeapOrder; // Q5:

//----------------------------------------------------------------------
//
//	This silliness is required because the compiler believes that
//	it can invert a number by subtracting it from zero and subtracting
//	an additional 1.  This works unless you try to negate 0x80000000,
//	which causes an overflow when subtracted from 0.  Simply
//	trying to do an XOR with 0xffffffff results in the same code
//	being emitted.
//
//----------------------------------------------------------------------
static int negativeone = 0xFFFFFFFF;
static inline uint32 invert (uint32 n) {
  return (n ^ negativeone);
}

//----------------------------------------------------------------------
//
//	MemoryGetSize
//
//	Return the total size of memory in the simulator.  This is
//	available by reading a special location.
//
//----------------------------------------------------------------------
int MemoryGetSize() {
  return (*((int *)DLX_MEMSIZE_ADDRESS));
}

//----------------------------------------------------------------------
//
//	MemoryModuleInit
//
//	Initialize the memory module of the operating system.
//      Basically just need to setup the freemap for pages, and mark
//      the ones in use by the operating system as "VALID", and mark
//      all the rest as not in use.
//
//----------------------------------------------------------------------
void MemoryModuleInit() {

  int	i;
  int maxpage = MEM_MAX_PHYS_MEM / MEM_PAGESIZE;
  int	curpage;
  int heap_order0_count = MEM_HEAP_ORDER0_COUNT;
  
  dbprintf('m', "MemoryModuleInit:  begin");

  pagestart = (lastosaddress + MEM_PAGESIZE - 4) / MEM_PAGESIZE;
  freemapmax = MEM_MAX_PHYS_MEM / MEM_PAGESIZE;
  //printf("pagestart: %d. freemapmax: %d\n", pagestart, freemapmax); // NOT
  dbprintf ('m', "Map has %d entries, memory size is 0x%x.\n", freemapmax, maxpage);
  dbprintf ('m', "Free pages start with page # 0x%x.\n", pagestart);
  for (i = 0; i < MEM_FREEMAP_SIZE; i++) {
    // Initially, all pages are considered in use.  This is done to make
    // sure we don't have any partially initialized freemap entries.
    freemap[i] = 0;
  }
  nfreepages = 0;
  //printf("pagestart: %d. freemapmax: %d\n", pagestart, freemapmax); // NOT
  for (curpage = pagestart; curpage < maxpage; curpage++) {
    //printf("curpage: %d\n", curpage); // NOT
    nfreepages += 1;
    MemorySetFreemap(curpage, 1);
  }
  maxHeapOrder = -1; // Q5:
  while(heap_order0_count) { // Q5: // CHECK: do we need to make sure that one order zero can fit?
    heap_order0_count = heap_order0_count >> 1;
    maxHeapOrder++;
  }
 
  //printf("nfreepages: %d\n", nfreepages); // NOT
  dbprintf ('m', "Initialized %d free pages.\n", nfreepages);
}



//----------------------------------------------------------------------
//
// MemoryTranslateUserToSystem
//
//	Translate a user address (in the process referenced by pcb)
//	into an OS (physical) address.  Return the physical address.
//
//----------------------------------------------------------------------
uint32 MemoryTranslateUserToSystem (PCB *pcb, uint32 addr) {
  int	page = addr / MEM_PAGESIZE;
  int offset = addr % MEM_PAGESIZE;
  
  if(addr > MEM_MAX_VIRTUAL_ADDRESS) { // if address over max
    printf("MemoryTranslateUserToSystem: Addr exceeded max, PCB %d\n", GetPidFromAddress(currentPCB)); // CHECK: dbprintf instead?
    ProcessKill();
    return 0;
  }
  //printf("(%d): Translating vaddr %05x\n", GetPidFromAddress(pcb), addr); // NOT:
  if(!(pcb->pagetable[page] & MEM_PTE_VALID)) { // if invalid page
    pcb->currentSavedFrame[PROCESS_STACK_FAULT] = addr;
    if(MemoryPageFaultHandler(pcb) == MEM_FAIL) {
      printf("MemoryTranslateUserToSystem: Failed to handle pagefault, PCB %d\n", GetPidFromAddress(currentPCB)); // CHECK: dbprintf instead?
      return 0;
    }
  }
  return ((pcb->pagetable[page] & MEM_PTE_MASK) + offset);
}




//----------------------------------------------------------------------
//
//	MemoryMoveBetweenSpaces
//
//	Copy data between user and system spaces.  This is done page by
//	page by:
//	* Translating the user address into system space.
//	* Copying all of the data in that page
//	* Repeating until all of the data is copied.
//	A positive direction means the copy goes from system to user
//	space; negative direction means the copy goes from user to system
//	space.
//
//	This routine returns the number of bytes copied.  Note that this
//	may be less than the number requested if there were unmapped pages
//	in the user range.  If this happens, the copy stops at the
//	first unmapped address.
//
//----------------------------------------------------------------------
int MemoryMoveBetweenSpaces (PCB *pcb, unsigned char *system, unsigned char *user, int n, int dir) {
  unsigned char *curUser;         // Holds current physical address representing user-space virtual address
  int		bytesCopied = 0;  // Running counter
  int		bytesToCopy;      // Used to compute number of bytes left in page to be copied

  while (n > 0) {
    // Translate current user page to system address.  If this fails, return
    // the number of bytes copied so far.
    curUser = (unsigned char *)MemoryTranslateUserToSystem (pcb, (uint32)user);

    // If we could not translate address, exit now
    if (curUser == (unsigned char *)0) break;

    // Calculate the number of bytes to copy this time.  If we have more bytes
    // to copy than there are left in the current page, we'll have to just copy to the
    // end of the page and then go through the loop again with the next page.
    // In other words, "bytesToCopy" is the minimum of the bytes left on this page 
    // and the total number of bytes left to copy ("n").

    // First, compute number of bytes left in this page.  This is just
    // the total size of a page minus the current offset part of the physical
    // address.  MEM_PAGESIZE should be the size (in bytes) of 1 page of memory.
    // MEM_ADDRESS_OFFSET_MASK should be the bit mask required to get just the
    // "offset" portion of an address.
    bytesToCopy = MEM_PAGESIZE - ((uint32)curUser & MEM_ADDRESS_OFFSET_MASK);
    
    // Now find minimum of bytes in this page vs. total bytes left to copy
    if (bytesToCopy > n) {
      bytesToCopy = n;
    }

    // Perform the copy.
    if (dir >= 0) {
      bcopy (system, curUser, bytesToCopy);
    } else {
      bcopy (curUser, system, bytesToCopy);
    }

    // Keep track of bytes copied and adjust addresses appropriately.
    n -= bytesToCopy;           // Total number of bytes left to copy
    bytesCopied += bytesToCopy; // Total number of bytes copied thus far
    system += bytesToCopy;      // Current address in system space to copy next bytes from/into
    user += bytesToCopy;        // Current virtual address in user space to copy next bytes from/into
  }
  return (bytesCopied);
}

//----------------------------------------------------------------------
//
//	These two routines copy data between user and system spaces.
//	They call a common routine to do the copying; the only difference
//	between the calls is the actual call to do the copying.  Everything
//	else is identical.
//
//----------------------------------------------------------------------
int MemoryCopySystemToUser (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, from, to, n, 1));
}

int MemoryCopyUserToSystem (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, to, from, n, -1));
}

//---------------------------------------------------------------------
// MemoryPageFaultHandler is called in traps.c whenever a page fault 
// (better known as a "seg fault" occurs.  If the address that was
// being accessed is on the stack, we need to allocate a new page 
// for the stack.  If it is not on the stack, then this is a legitimate
// seg fault and we should kill the process.  Returns MEM_SUCCESS
// on success, and kills the current process on failure.  Note that
// fault_address is the beginning of the page of the virtual address that 
// caused the page fault, i.e. it is the vaddr with the offset zero-ed
// out.
//
// Note: The existing code is incomplete and only for reference. 
// Feel free to edit.
//---------------------------------------------------------------------
int MemoryPageFaultHandler(PCB *pcb) {
  // faddr: fault address
  // usaddr: user stack addr
  uint32 faddr = pcb->currentSavedFrame[PROCESS_STACK_FAULT]; // CHECK: is this correct 
  uint32 usaddr = pcb->currentSavedFrame[PROCESS_STACK_USER_STACKPOINTER]; // CHECK: is this correct 
  uint32 fpage = faddr / MEM_PAGESIZE;
  uint32 uspage = usaddr / MEM_PAGESIZE;
  printf("MemoryPageFaultHandler (%d): faddr %08x, usaddr %08x\n", GetPidFromAddress(pcb), faddr, usaddr); // NOT:
  if(fpage < uspage) { // NEED: anything else needed inside? is the logic backwards or something?
    printf("SEGFAULT: PCB %d, vaddr 0x%08x\n", GetPidFromAddress(pcb), faddr);
    ProcessKill();  
    return MEM_FAIL;
  }
  else { // CHECK: should stack pointer change?
    // allocate page, setup pte, insert to table
    //printf("(%d): allocating page %d, for vaddr 0x%08x\n", GetPidFromAddress(pcb), fpage, faddr); // NOT:
    pcb->pagetable[fpage] = MemorySetupPte(MemoryAllocPage());
    pcb->npages += 1;
    return MEM_SUCCESS;
  }
}


//---------------------------------------------------------------------
// You may need to implement the following functions and access them from process.c
// Feel free to edit/remove them
//---------------------------------------------------------------------
int MemoryAllocPage() { // CHECK: do we need to check that we don't go over the maximum of 32 pages per proccess here?
  static int pagenum = 0; // CHECK: should this still be static?
  uint32 bitnum = 0;
  uint32 pageint;

  //printf("MemoryAllocPage: enter function\n");

  if (nfreepages == 0) { // CHECK: should it fail or reallocate a page or kill? print no available pages and return 0
    printf("MemoryAllocPage: no free pages available\n");
    return 0;
  }
  dbprintf ('m', "Allocating memory, starting with page %d\n", pagenum);
  while(freemap[pagenum] == 0) {
    pagenum += 1;
  }
  pageint = freemap[pagenum];
  while((pageint & (1 << bitnum)) == 0) { bitnum += 1; }
  pageint = (pagenum * 32) + bitnum;
  MemorySetFreemap(pageint, 0);
  dbprintf ('m', "Allocated memory, from map %d, page %d, map=0x%x.\n", pagenum, pageint, freemap[pagenum]);
  nfreepages -= 1;
  //printf("(%d): allocated page %d.\n", GetCurrentPid(), pageint); // NOT:
  return (pageint);
}


inline void MemorySetFreemap(int p, int b) {
  uint32	wd = p / 32;
  uint32	bitnum = p % 32;

  freemap[wd] = (freemap[wd] & invert(1 << bitnum)) | (b << bitnum);
  dbprintf ('m', "Set freemap entry %d to 0x%x.\n", wd, freemap[wd]);
}

void MemoryFreePage(uint32 page) {
  MemorySetFreemap(page, 1);
  nfreepages += 1;
  dbprintf ('m',"Freed page 0x%x, %d remaining.\n", page, nfreepages);
}

void MemoryFreePte(uint32 pte) {
  MemoryFreePage((pte & MEM_PTE_MASK) / MEM_PAGESIZE);
}

uint32 MemorySetupPte (uint32 page) {
 return ((page * MEM_PAGESIZE) | MEM_PTE_VALID);
}

void* malloc(int memsize) { // Q5: CHECK: make sure to allocate in word multiples
  int i = 0;
  int j = 0;
  int min_order = 0;
  int min_order0_count = 0;
  int min_order_possible = 0;
  int alloc_index = -1;
  int i_inc = 1;
  
  // SIZE FITS
  if((memsize > (MEM_HEAP_ORDER0 << maxHeapOrder)) || (memsize <= 0)) {
    return NULL;
  }
  // MINIMUM ORDER AND SIZE
  while((MEM_HEAP_ORDER0 << min_order) < memsize) { min_order += 1; }
  min_order0_count = (1 << min_order);
  // FIND AVAILABLE INDEX
  min_order_possible = 1 << (maxHeapOrder - min_order);
  i = 0;
  j = 0;
  while(i < min_order_possible) { // for each block of order 'min_order'
    i_inc = 1;
    for(j = 0; j < min_order0_count; j++) { // for each element in the block
      if(currentPCB->heapAlloc[j + i*min_order0_count] != 0) {
        i_inc = currentPCB->heapAlloc[j + i*min_order0_count] / min_order0_count;
        alloc_index = -1;
        break;
      }
      if(j == min_order0_count - 1) { alloc_index = i*min_order0_count; }
    }
    if(alloc_index != -1) { break; }
    i += i_inc ? i_inc : 1; // to make sure it increments by at least 1
  }
  if(alloc_index == -1) { return NULL; }
  
  // PRINT
  PrintMalloc(min_order, alloc_index, memsize);
  PrintMallocCreate(min_order, alloc_index);
  // ALLOCATE AVAILABLE INDEX
  currentPCB->heapAlloc[alloc_index] = min_order0_count;
  // RETURN VADDR
  //PrintHeapAlloc(currentPCB); // NOT:
  return (void*)((MEM_HEAP_PTE_PAGE * MEM_PAGESIZE) + alloc_index*MEM_HEAP_ORDER0);
}
int mfree(void* ptr) { // Q5:
  int alloc_index = (((uint32)ptr) & MEM_ADDRESS_OFFSET_MASK) / MEM_HEAP_ORDER0;
  int order0_free_count = currentPCB->heapAlloc[alloc_index];
  int memsize = order0_free_count * MEM_HEAP_ORDER0;
  int order = 0;
  while((order0_free_count >> 1) >> order) {
    order += 1;
  }
  // VALID PTR
  if(ptr == NULL || (((uint32)ptr) & MEM_PTE_MASK) / MEM_PAGESIZE != MEM_HEAP_PTE_PAGE) { return MEM_FAIL; }
  // FREE
  currentPCB->heapAlloc[alloc_index] = 0;
  // PRINT
  PrintMfree(order, alloc_index);
  PrintMfreeCoalesce(order, alloc_index);
  return memsize;
}
void PrintMalloc(int order, int alloc_index, int memsize) {
  int block_size = MEM_HEAP_ORDER0 << order;
  int addr = alloc_index * MEM_HEAP_ORDER0; // note that this is relative to heap
  printf("Allocated the block: order = %d, addr = %4d, requested mem size = %4d, block size = %4d\n", order, addr, memsize, block_size);
}
void PrintMfree(int order, int alloc_index) {
  int block_size = MEM_HEAP_ORDER0 << order;
  int addr = alloc_index * MEM_HEAP_ORDER0; // note that this is relative to heap
  printf("    Freed the block: order = %d, addr = %4d, size = %4d\n", order, addr, block_size);
}
void PrintMallocCreate(int order, int alloc_index) {
  int block_size = MEM_HEAP_ORDER0 << order;
  int addr = alloc_index * MEM_HEAP_ORDER0; // note that this is relative to heap
  int right_buddy = (addr / block_size) % 2; // a flag for if it is a right buddy
  int order_i_left_index;
  int order_i_right_index;
  int i;
  int j;
  int new_flag = 1;
  if(maxHeapOrder == order || right_buddy) {
    return;
  }
  else {
    for(i = maxHeapOrder - 1; i >= order; i--) { // for each order above 'order'
      if((alloc_index / (1 << i)) % 2) { // if parent is odd
        order_i_right_index = (alloc_index / (1 << i)) * (1 << i);
        order_i_left_index = order_i_right_index - (1 << i);
      } else { // if parent is even
        order_i_left_index = (alloc_index / (1 << i)) * (1 << i);
        order_i_right_index = order_i_left_index + (1 << i);
      }
      new_flag = 1;
      for(j = 0; j < (1 << i); j++) { // has order 'i' parent been used
        if(currentPCB->heapAlloc[order_i_left_index + j]) {
          new_flag = -1;
          break;
        }
      }
      if(new_flag == 1 && i != (maxHeapOrder)) { // has order 'i' parent buddy been used
        for(j = 0; j < (1 << i); j++) {
          if(currentPCB->heapAlloc[order_i_right_index + j]) {
            new_flag = -1;
            break;
          }
        }
      }
      if(new_flag == 1) {
        // LEFT
        printf("Created a  left child node (order = %d, addr = %4d, size = %4d)", i, alloc_index*MEM_HEAP_ORDER0, MEM_HEAP_ORDER0 << (i));
        printf(" of parent (order = %d, addr = %4d, size = %4d)\n", i+1, alloc_index*MEM_HEAP_ORDER0, MEM_HEAP_ORDER0 << (i + 1));
        // RIGHT
        printf("Created a right child node (order = %d, addr = %4d, size = %4d)", i, (alloc_index + (1 << i))*MEM_HEAP_ORDER0, MEM_HEAP_ORDER0 << i);
        printf(" of parent (order = %d, addr = %4d, size = %4d)\n", (i + 1), alloc_index*MEM_HEAP_ORDER0, MEM_HEAP_ORDER0 << (i + 1));
      }
    }
  }
  
}
void PrintMfreeCoalesce(int order, int alloc_index) {
  //int block_size = MEM_HEAP_ORDER0 << order;
  //int addr = alloc_index * MEM_HEAP_ORDER0; // note that this is relative to heap
  //int right_buddy = (addr / block_size) % 2; // a flag for if it is a right buddy
  int order_i_child_index;
  int order_i_child_buddy_index;
  int i;
  int j;
  int new_flag = 1;
  int left_buddy;
  int right_buddy;
  if(maxHeapOrder == order) return;
  else {
    for(i = order; i < maxHeapOrder; i++) { // for each order above 'order'
      order_i_child_index = (alloc_index / (1 << i)) * (1 << i);
      //printf("child index %d\n", order_i_child_index); // NOT:
      //new_flag = 1;
      if(i != order) {
        for(j = 0; j < (1 << i); j++) { // has order 'i' parent been used
          if(currentPCB->heapAlloc[order_i_child_index + j]) {
            //printf("failed first index %d\n", order_i_child_index + j); // NOT:
            new_flag = -1;
            break;
          }
        }
      }
      if(new_flag == 1) { // has order 'i' parent buddy been used
        order_i_child_buddy_index = order_i_child_index + (((order_i_child_index / (1 << i)) % 2) ? -1 : 1) * (1 << i);
        //printf("child %d, buddy %d\n", order_i_child_index, order_i_child_buddy_index); // NOT:
        for(j = 0; j < (1 << i); j++) {
          if(currentPCB->heapAlloc[order_i_child_buddy_index + j]) {
            new_flag = -1;
            break;
          }
        }
      }
      if(new_flag == 1) {
        if(((order_i_child_index / (1 << i)) % 2)) { // if child right
          left_buddy = order_i_child_buddy_index;
          right_buddy = order_i_child_index;
        }
        else { // if child left
          left_buddy = order_i_child_index;
          right_buddy = order_i_child_buddy_index;
        }
        printf("Coalesced buddy nodes (order = %d, addr = %4d, size = %4d) & (order = %d, addr = %4d, size = %4d)\n",
              i, left_buddy*MEM_HEAP_ORDER0, MEM_HEAP_ORDER0 << i, i, right_buddy*MEM_HEAP_ORDER0, MEM_HEAP_ORDER0 << i);
        printf("into the parent (order = %d, addr = %4d, size = %4d)\n", i + 1, left_buddy*MEM_HEAP_ORDER0, MEM_HEAP_ORDER0 << (i + 1));
      }
      else { return; }
    }
  }
}
void PrintHeapAlloc(PCB* pcb) {
  int i;
  for(i = 0; i < MEM_HEAP_ORDER0_COUNT; i++) {
    if(pcb->heapAlloc[i]) printf("Heap[%4d]: %4d\n", i*MEM_HEAP_ORDER0, pcb->heapAlloc[i]*MEM_HEAP_ORDER0);
  }
}
/*void PrintHeapInfo(int memsize, int alloc_index) {
  printf("heap block of size %d bytes: virtual address %08x, physical address %08x\n", memsize, (MEM_HEAP_PTE_PAGE * MEM_PAGESIZE) + alloc_index*MEM_HEAP_ORDER0,
                                                                                       (currentPCB->pagetable[MEM_HEAP_PTE_PAGE] & MEM_PTE_MASK) + alloc_index*MEM_HEAP_ORDER0);
}*/
