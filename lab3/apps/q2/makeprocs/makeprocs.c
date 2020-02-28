#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  int n_inj_S2 = 0;
  int n_inj_CO = 0;
  int i; // Loop index variable
  mol_boxes molecules;
  sem_t s_procs_completed;
  char h_mbox_str[10];
  char s_procs_completed_str[10];

  // ARGUMENTS
  if (argc != 3) {
    Printf("Usage: %s <number of processes to create\n", argv[0]);
    Exit();
  }
  n_inj_S2 = dstrtol(argv[1], NULL, 10);
  n_inj_CO = dstrtol(argv[2], NULL, 10);
  Printf("makeprocs (%d): Creating %d processes\n", getpid(), n_inj_S2+n_inj_CO);

  // MBOX CREATION
  if((molecules.mbox_S2 = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_S2!", getpid());
    Exit();
  }
  if((molecules.mbox_S = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_S!", getpid());
    Exit();
  }
  if((molecules.mbox_CO = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_CO!", getpid());
    Exit();
  }
  if((molecules.mbox_O2 = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_O2!", getpid());
    Exit();
  }
  if((molecules.mbox_C2 = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_C2!", getpid());
    Exit();
  }
  if((molecules.mbox_SO4 = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_SO4!", getpid());
    Exit();
  }

  // Open mailbox to prevent deallocation
  if(mbox_open(mbox_S2) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), mbox_S2);
    Exit();
  }
  if(mbox_open(mbox_S) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), mbox_S);
    Exit();
  }
  if(mbox_open(mbox_CO) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), mbox_CO);
    Exit();
  }
  if(mbox_open(mbox_O2) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), mbox_O2);
    Exit();
  }
  if(mbox_open(mbox_C2) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), mbox_C2);
    Exit();
  }
  if(mbox_open(mbox_SO4) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), mbox_SO4);
    Exit();
  }

  // SEM
  if ((s_procs_completed = sem_create(-(numprocs-1))) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }

  // HANDLE STRINGS
  ditoa(h_mbox, h_mbox_str);
  ditoa(s_procs_completed, s_procs_completed_str);

  // REACTION CALCULATIONS

  // INJECTIONS
  for(i=0; i<n_inj_S2; i++) {
    process_create(INJ_S2, 0, 0, , s_procs_completed_str, NULL);
    Printf("makeprocs (%d): S2 Injection %d created\n", getpid(), i);
  }

  // REACTIONS

  // And finally, wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());
}
