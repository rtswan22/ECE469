#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[]) 
{

	sem_t procs;  
  mol_boxes* mol;
	uint32 h_mem;
	char s[10];
	int mol;


  procs = dstrtol(argv[1],NULL,10);
	h_mem = dstrtol(argv[2],NULL,10);
	if (argc != 3) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_page_mapped_semaphore> <so4_mbox>\n"); 
    Exit();
	}


if ((mol = (mol_boxes*)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
	
	if(mbox_open(mol.mbox_S2) != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not open mbox\n", getpid());
		Exit();
	}

	// Receive 1 S2
	mol = mbox_recv(mol.mbox_S2, 2, (char *) &message);
	if(mol != 2) {
		Printf("Reaction S2 (%d): could not receive s2 mbox. mol=%d\n", getpid(), mol);
	
		Exit();
	}

	if(mbox_close(mol.mbox_S2) != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not close mbox S2\n", getpid());
		Exit();
	}

	if(mbox_open(mol.mbox_S) != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not open mbox S\n", getpid());
		Exit();
	}
	// Send 2 S's
	if(mbox_send(mol.mbox_S, 1, (void *) "S") != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not send mbox S\n", getpid());
		Exit();
	}
	Printf("PID: %d Created a S molecule.\n", getpid());
	if(mbox_send(mol.mbox_S, 1, (void *) "S") != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not send mbox S \n", getpid());
		Exit();
	}	
	Printf("PID: %d Created a S molecule.\n", getpid());

	// Close the mailboxes

	if(mbox_close(mol.mbox_S) != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not close mbox_S\n", getpid());
		Exit();
	}

if(sem_signal(procs) != SYNC_SUCCESS) {
		Printf("Bad semaphore s_procs_completed (%d) in ", procs); Printf(argv[0]); Printf(", exiting...\n");
		Exit();
	}

}
