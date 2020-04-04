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
  printf("MemoryPageFaultHandler (%d): faddr %08x, usaddr %08x\n", faddr, usaddr); // NOT:
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



