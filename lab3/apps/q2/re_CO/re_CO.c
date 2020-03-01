#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[]) {
  sem_t procs;  
  mol_boxes* mol;
  char msg[100];
  char msg_O2[] = "O2";
  char msg_C2[] = "C2";
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

  //Open Mbox Co
  if(mbox_open(mol->mbox_CO) == MBOX_FAIL) {
    Printf("Reaction CO (%d): could not open mbox CO\n", getpid());
    Exit();
  }

  // RECV CO's
  for(i = 0; i < 4; i++) {
    if(mbox_recv(mol->mbox_CO, 2, (void*)msg) == MBOX_FAIL) {
      Printf("Reaction CO (%d): could not recieve CO\n", getpid());
      Exit();
    }
  }

  //Close Mailbox
  if(mbox_close(mol->mbox_CO) == MBOX_FAIL) {
    Printf("Reaction CO (%d): could not close mbox CO\n", getpid());
    Exit();
  }
	
  //Open Mbox O2
  if(mbox_open(mol->mbox_O2) == MBOX_FAIL) {
    Printf("Reaction CO (%d): could not open mbox\n", getpid());
    Exit();
  }

  // SEND O2's
  for(i = 0; i < 2; i++) {
    if(mbox_send(mol->mbox_O2, 2, (void *)msg_O2) == MBOX_FAIL) {
      Printf("Reaction CO (%d): could not send O2 \n", getpid());
      Exit();
    }
  }
  // Close O2
  if(mbox_close(mol->mbox_O2) == MBOX_FAIL) {
    Printf("Reaction CO (%d): could not close mbox O2\n", getpid());
    Exit();
  }

  //Open Mbox C2
  if(mbox_open(mol->mbox_C2) == MBOX_FAIL) {
    Printf("Reaction CO (%d): could not open mbox\n", getpid());
    Exit();
  }

  // SEND C2's
  for(i = 0; i < 2; i++) {
    if(mbox_send(mol->mbox_C2, 2, (void *)msg_C2) == MBOX_FAIL) {
      Printf("Reaction CO (%d): could not send C2 \n", getpid());
      Exit();
    }
  }
  // Close C2
  if(mbox_close(mol->mbox_C2) == MBOX_FAIL) {
    Printf("Reaction CO (%d): could not close mbox C2\n", getpid());
    Exit();
  } 

  Printf("4 CO -> 2 O2 + 2 C2 reacted, PID: %d\n", getpid());

  if(sem_signal(procs) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", procs); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
