#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[]) {
  sem_t procs;  
  mol_boxes* mol;
  char msg[100];
  char msg_SO4[] = "SO4";
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

  //Open Mbox S
  if(mbox_open(mol->mbox_S) == MBOX_FAIL) {
    Printf("Reaction SO4 (%d): could not open mbox S\n", getpid());
    Exit();
  }

  // RECV S
  if(mbox_recv(mol->mbox_S, 1, (void*)msg) == MBOX_FAIL) {
    Printf("Reaction SO4 (%d): could not recieve S\n", getpid());
    Exit();
  }

  //Close Mailbox
  if(mbox_close(mol->mbox_S) == MBOX_FAIL) {
    Printf("Reaction SO4 (%d): could not close mbox S\n", getpid());
    Exit();
  }
	
  //Open Mbox O2
  if(mbox_open(mol->mbox_O2) == MBOX_FAIL) {
    Printf("Reaction SO4 (%d): could not open mbox\n", getpid());
    Exit();
  }

  // SEND O2's
  for(i = 0; i < 2; i++) {
    if(mbox_recv(mol->mbox_O2, 2, (void *)msg) == MBOX_FAIL) {
      Printf("Reaction SO4 (%d): could not recieve O2 \n", getpid());
      Exit();
    }
  }
  // Close O2
  if(mbox_close(mol->mbox_O2) == MBOX_FAIL) {
    Printf("Reaction SO4 (%d): could not close mbox O2\n", getpid());
    Exit();
  }

  //Open Mbox SO4
  if(mbox_open(mol->mbox_SO4) == MBOX_FAIL) {
    Printf("Reaction SO4 (%d): could not open mbox\n", getpid());
    Exit();
  }

  // SEND SO4
  if(mbox_send(mol->mbox_SO4, 3, (void *)msg_SO4) == MBOX_FAIL) {
    Printf("Reaction SO4 (%d): could not send SO4 \n", getpid());
    Exit();
  }

  // Close SO4
  if(mbox_close(mol->mbox_SO4) == MBOX_FAIL) {
    Printf("Reaction SO4 (%d): could not close mbox SO4\n", getpid());
    Exit();
  } 

  Printf("S + 2 O2 -> SO4 reacted, PID: %d\n", getpid());

  if(sem_signal(procs) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", procs); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
