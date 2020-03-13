#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  int n_inj_S2 = 0;
  int n_inj_CO = 0;
  int n_re_S2 = 0;
  int n_re_CO = 0;
  int n_re_SO4 = 0;
  int i; // Loop index variable
  mol_boxes* molecules;
  sem_t s_procs_completed;
  unsigned int h_mem; //CHECK
  char h_mem_str[10];
  char s_procs_completed_str[10];
  int pinfo = 0;

  // ARGUMENTS
  if (argc != 3) {
    Printf("Usage: %s <number of S2 injects> <number of CO injects>\n", argv[0]);
    Exit();
  }
  n_inj_S2 = dstrtol(argv[1], NULL, 10);
  n_inj_CO = dstrtol(argv[2], NULL, 10);
  Printf("Creating %d S2s and %d COs.\n", n_inj_S2, n_inj_CO);

  // SHARED MEMORY
  if ((h_mem = shmget()) == 0) {
    Printf("ERROR: could not allocate shared memory page in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  if ((molecules = (mol_boxes*)shmat(h_mem)) == NULL) {
    Printf("Could not shmat(handle) in "); Printf(argv[0]); Printf(", exiting..\n");
    Exit();
  }

  // MBOX CREATION
  if((molecules->mbox_S2 = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_S2!", getpid());
    Exit();
  }
  if((molecules->mbox_S = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_S!", getpid());
    Exit();
  }
  if((molecules->mbox_CO = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_CO!", getpid());
    Exit();
  }
  if((molecules->mbox_O2 = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_O2!", getpid());
    Exit();
  }
  if((molecules->mbox_C2 = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_C2!", getpid());
    Exit();
  }
  if((molecules->mbox_SO4 = mbox_create()) == MBOX_FAIL) {
    Printf("makeprocs (%d): ERROR: could not allocate mbox_SO4!", getpid());
    Exit();
  }
  
  // NOT
  //Printf("S2:%d, S:%d, CO:%d, O2:%d, C2:%d, SO4:%d\n", molecules->mbox_S2, molecules->mbox_S, molecules->mbox_CO, molecules->mbox_O2, molecules->mbox_C2, molecules->mbox_SO4);

  // Open mailbox to prevent deallocation
  if(mbox_open(molecules->mbox_S2) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), molecules->mbox_S2);
    Exit();
  }
  if(mbox_open(molecules->mbox_S) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), molecules->mbox_S);
    Exit();
  }
  if(mbox_open(molecules->mbox_CO) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), molecules->mbox_CO);
    Exit();
  }
  if(mbox_open(molecules->mbox_O2) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), molecules->mbox_O2);
    Exit();
  }
  if(mbox_open(molecules->mbox_C2) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), molecules->mbox_C2);
    Exit();
  }
  if(mbox_open(molecules->mbox_SO4) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox %d!\n", getpid(), molecules->mbox_SO4);
    Exit();
  }

  // REACTION CALCULATIONS
  n_re_S2 = n_inj_S2;
  n_re_CO = n_inj_CO/4;
  n_re_SO4 = 2*n_re_S2<n_re_CO ? 2*n_re_S2 : n_re_CO;
  /*Printf("inj_S2: %d\n", n_inj_S2);
  Printf("re_S2: %d\n", n_re_S2);
  Printf("inj_CO: %d\n", n_inj_CO);
  Printf("re_CO: %d\n", n_re_CO);
  Printf("re_SO4: %d\n", n_re_SO4);*/

  // SEM
  if ((s_procs_completed = sem_create(-((n_inj_S2+n_inj_CO+n_re_S2+n_re_CO+n_re_SO4)-1))) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }

  // HANDLE STRINGS
  ditoa(h_mem, h_mem_str);
  ditoa(s_procs_completed, s_procs_completed_str);

  // INJ/RE
  while(n_inj_S2 + n_inj_CO + n_re_S2 + n_re_CO + n_re_SO4) {
    if(n_inj_S2) {
      process_create(INJ_S2, 0, pinfo, s_procs_completed_str, h_mem_str, NULL);
      n_inj_S2--;
    }
    if(n_re_S2) {
      process_create(RE_S2, 0, pinfo, s_procs_completed_str, h_mem_str, NULL);
      n_re_S2--;
    }
    for(i = 0; i < 4; i++) {
      if(n_inj_CO) {
        process_create(INJ_CO, 0, pinfo, s_procs_completed_str, h_mem_str, NULL);
        n_inj_CO--;
      }
    }
    if(n_re_CO) {
      process_create(RE_CO, 0, pinfo, s_procs_completed_str, h_mem_str, NULL);
      n_re_CO--;
    }
    if(n_re_SO4) {
      process_create(RE_SO4, 0, pinfo, s_procs_completed_str, h_mem_str, NULL);
      n_re_SO4--;
    }   
  }

  // And finally, wait until all spawned processes have finished.
  if(sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }

  //Printf("%d S2s left over. ", ); // CHECK

  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());
}
