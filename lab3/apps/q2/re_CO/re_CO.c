#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[]) 
{

  sem_t procs;  
  mol_boxes* mol;
	char message[3];
	uint32 h_mem;
  
	int mol1, mol2, mol3, mol4;

	procs = dstrtol(argv[1],NULL,10);
	h_mem = dstrtol(argv[2],NULL,10);


	
	if (argc != 3) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_page_mapped_semaphore> <handle_to__molboxes>\n"); 
    Exit();
    }

	if ((mol = (mol_boxes*)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
	//Open Mbox Co
	if(mbox_open(mol.mbox_CO) != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not open mbox CO\n", getpid());
		Exit();
	}

	// RECEIVE CO's
	mol1 = mbox_recv(mol.mbox_CO, 2, (void *) &message);
	if(mol1 != 2) {
		Printf("Reaction CO (%d): could not receive mbox 1. mol=%d\n", getpid(), mol1);
		Exit();
	}
	mol2 = mbox_recv(mbox_co, 2, (void *) &message);
	if(mol2 != 2) {
		Printf("Reaction CO (%d): could not receive mbox 2. mol2=%d\n", getpid(), mol2);
		Exit();
	}
	mol3 = mbox_recv(mol.mbox_CO, 2, (void *) &message);
	if(mol3 != 2) {
		Printf("Reaction CO (%d): could not receive mbox 3. mol3=%d\n", getpid(), mol3);
		Exit();
	}
	mol4 = mbox_recv(mol.mbox_CO, 2, (void *) &message);
	if(mol4 != 2) {
		Printf("Reaction CO (%d): could not receive mbox 4. Rv=%d\n", getpid(), mol4);
		Exit();
	}

	//Close Mailbox
	if(mbox_close(mol.mbox_CO) != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not close mbox CO\n", getpid());
		Exit();
	}
	
	//Open Mbox O2
	if(mbox_open(mol.mbox_O2) != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not open mbox\n", getpid());
		Exit();
	}

	// CREATE O2's
	if(mbox_send(mol.mbox_O2, 2, (void *) "O2") != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not send O2 \n", getpid());
		Exit();
	}
	Printf("PID: %d Created an O2 molecule.\n", getpid());
	
if(mbox_send(mol.mbox_O2, 2, (void *) "O2") != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not send \n", getpid());
		Exit();
	}	
	Printf("PID: %d Created an O2 molecule.\n", getpid());

	// Close mailboxes
	if(mbox_close(mol.mbox_O2) != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not close mbox O2\n", getpid());
		Exit();
	}

	if(sem_signal(procs) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", procs); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
