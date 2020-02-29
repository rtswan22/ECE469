#include "usertraps.h"
#include "misc.h"
#include "queue.h"

#include "spawn.h"

void main (int argc, char *argv[]) 
{
	sem_t procs;  
  mol_boxes* molecules;
	uint32 h_mem;
	procs = dstrtol(argv[1],NULL,10);
	h_mem = dstrtol(argv[2],NULL,10);
        
	if (argc != 3) { 
		Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_semaphore> <handle_to_molboxes> \n");
		Exit();
  } 


	if ((molecules = (mol_boxes*)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
	
//Open Mailbox
	if(mbox_open(molecules.mbox_CO) != MBOX_SUCCESS) {
		Printf("Injection CO (%d): could not open mbox\n", getpid());
		Exit();
	}


	// Inject 1 CO
	if(mbox_send(molecules.mbox_CO, 2, (void *)"CO") != MBOX_SUCCESS) {
		Printf("Injection CO (%d): could not send\n", getpid());
		Exit();
	}

	//Close Mbox
	if(mbox_close(molecules.mbox_CO) != MBOX_SUCCESS) {
		Printf("Injection CO (%d): could not close mbox\n", getpid());
		Exit();
	}


	Printf("PID: %d Created a CO molecule.\n", getpid());
	
	if(sem_signal(procs) != SYNC_SUCCESS) {
	  Printf("Bad semaphore procs completed (%d) in ", procs); Printf(argv[0]); Printf(", exiting...\n");
	  Exit();
	}
       
}
