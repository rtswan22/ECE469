#include "usertraps.h"
#include "misc.h"

#include "spawn.h"


void main (int argc, char *argv[]) 
{


  sem_t procs;  
  mol_boxes* mol;
	char s[10];
	char o[10];
	uint32 h_mem;
  

	int mol1, mol2, mol3, mol4;
	procs = dstrtol(argv[1],NULL,10);
	h_mem = dstrtol(argv[2],NULL,10);


	if (argc != 3) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle to page mapped semaphore> <handle of mailbox>\n"); 
    Exit();
  } 

if ((mol = (mol_boxes*)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
	
	

	//OPEN ALL MAILBOX
	if(mbox_open(mol.box_SO4) != MBOX_SUCCESS) {
		Printf("Re_SO4 (%d): could not open its own mbox\n", getpid());
		Exit();
	}

       if(mbox_open(mol.mbox_O2) != MBOX_SUCCESS) {
		Printf("Re_SO4 (%d): could not open mbox_O2\n", getpid());
		Exit();
	}


	if(mbox_open(mol.mbox_S) != MBOX_SUCCESS) {
		Printf("Re_SO4 (%d): could not open mbox_S\n", getpid());
		Exit();
	}

	//RECEIVE ALL MAILBOX
	if(mbox_recv(mol.mbox_S, 2, (char *) &s) != 1) {
		Printf("Re_SO4 (%d): could not receive mbox_S\n", getpid());
		Exit();
	}
	if(mbox_recv(mol.mbox_O2, 2, (char *) &o) != 2) {
		Printf("Re_SO4 (%d): could not receive mbox_o2 \n", getpid());
		Exit();
	}

	//SEND MBOX
	if(mbox_send(mol.mbox_SO4, 3, (void *) "SO4") != MBOX_SUCCESS) {
		Printf("Re_SO4 (%d): could not send \n", getpid());
		Exit();
	}
	//Print check statement
	Printf("PID: %d Created a SO4 molecule.\n", getpid());

	// Close the mailboxes
	if(mbox_close(mol.mbox_SO4) != MBOX_SUCCESS) {
		Printf("Re_SO4 (%d): could not close its own mbox\n", getpid());
		Exit();
	}

	//CLOSE ALL MBOX
		if(mbox_close(mol.mbox_O2) != MBOX_SUCCESS) {
		Printf("Re_SO4 (%d): could not close mbox_O2\n", getpid());
		Exit();
	}

	if(mbox_close(mol.mbox_S) != MBOX_SUCCESS) {
		Printf("Re_SO4 (%d): could not close mbox_S \n", getpid());
		Exit();
	}


	if(sem_signal(procs) != SYNC_SUCCESS) {
		Printf("Bad semaphore s_procs_completed (%d) in ", procs); Printf(argv[0]); Printf(", exiting...\n");
		Exit();
	}
}
