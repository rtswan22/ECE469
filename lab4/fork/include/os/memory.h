#ifndef	_memory_h_
#define	_memory_h_

// Put all your #define's in memory_constants.h
#include "memory_constants.h"

extern int lastosaddress; // Defined in an assembly file

//--------------------------------------------------------
// Existing function prototypes:
//--------------------------------------------------------

int MemoryGetSize();
void MemoryModuleInit();
uint32 MemoryTranslateUserToSystem (PCB *pcb, uint32 addr);
int MemoryMoveBetweenSpaces (PCB *pcb, unsigned char *system, unsigned char *user, int n, int dir);
int MemoryCopySystemToUser (PCB *pcb, unsigned char *from, unsigned char *to, int n);
int MemoryCopyUserToSystem (PCB *pcb, unsigned char *from, unsigned char *to, int n);
int MemoryPageFaultHandler(PCB *pcb);

//---------------------------------------------------------
// Put your function prototypes here
//---------------------------------------------------------
// All function prototypes including the malloc and mfree functions go here
int MemoryAllocPage();
inline void MemorySetFreemap(int p, int b);
void MemoryFreePage(uint32 page);
void MemoryFreePte (uint32 pte);
uint32 MemorySetupPte (uint32 page);

int MemoryROPAccessHandler(PCB* pcb); // Q3:
void increment_refcounters(int page);

void *malloc(); // NEED: how is this setup
int mfree(); // NEED: how is this setup

#endif	// _memory_h_
