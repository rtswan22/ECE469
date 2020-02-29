#include "usertraps.h"
#include "misc.h"
#include "queue.h"

#include "spawn.h"

void main (int argc, char *argv[]) 
{
        sem_t procs;  
	mbox_t mbox;
	mbox = dstrtol(argv[1], NULL, 10);
	procs = dstrtol(argv[2],NULL,10);
        
	if (argc != 3) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_page_mapped_semaphore> <CO_mbox> \n"); 
    Exit();
  } 

	//Open Mailbox
	if(mbox_open(mbox) != MBOX_SUCCESS) {
		Printf("Injection CO (%d): could not open mbox\n", getpid());
		Exit();
	}

	// Inject 1 CO
	if(mbox_send(mbox, 2, (void *)"CO") != MBOX_SUCCESS) {
		Printf("Injection CO (%d): could not send\n", getpid());
		Exit();
	}

	Printf("PID: %d Created a CO molecule.\n", getpid());
	//Close Mbox
	if(mbox_close(mbox) != MBOX_SUCCESS) {
		Printf("Injection CO (%d): could not close mbox\n", getpid());
		Exit();
	}

	if(sem_signal(procs) != SYNC_SUCCESS) {
	  Printf("Bad semaphore procs completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
	  Exit();
	}
       
}
