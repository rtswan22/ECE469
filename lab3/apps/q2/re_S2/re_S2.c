#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[]) {
  sem_t procs;  
  mol_boxes* mol;
  char msg[100];
  char msg_S[] = "S";
  unsigned int h_mem;
  int i = 0;
 
  procs = dstrtol(argv[1],NULL,10);
  h_mem = dstrtol(argv[2],NULL,10);
	
  if (argc != 3) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_semaphore> <handle_to_shared_mem_mol_boxes> \n"); 
    Exit();
  }

  if ((mol = (mol_boxes*)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  //Open S2
  if(mbox_open(mol->mbox_S2) == MBOX_FAIL) {
    Printf("Reaction S2 (%d): could not open mbox S\n", getpid());
    Exit();
  }

  // RECV S2
  if(mbox_recv(mol->mbox_S2, 2, (void*)msg) == MBOX_FAIL) {
    Printf("Reaction S2 (%d): could not recieve S2\n", getpid());
    Exit();
  }

  //Close Mailbox
  if(mbox_close(mol->mbox_S2) == MBOX_FAIL) {
    Printf("Reaction S2 (%d): could not close mbox S2\n", getpid());
    Exit();
  }

  //Open Mbox S
  if(mbox_open(mol->mbox_S) == MBOX_FAIL) {
    Printf("Reaction S2 (%d): could not open mbox\n", getpid());
    Exit();
  }

  // SEND S's
  for(i = 0; i < 2; i++) {
    if(mbox_send(mol->mbox_S, 1, (void *)msg_S) == MBOX_FAIL) {
      Printf("Reaction S2 (%d): could not send S \n", getpid());
      Exit();
    }
  }
  // Close S
  if(mbox_close(mol->mbox_S) == MBOX_FAIL) {
    Printf("Reaction S2 (%d): could not close mbox S\n", getpid());
    Exit();
  } 

  Printf("S2 -> S + S reacted, PID: %d\n", getpid());

  if(sem_signal(procs) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", procs); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
