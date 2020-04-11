#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  //int data_num;
  void* alloc1;
  void* alloc2;
  void* alloc3;
  //int fork_val;
  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  alloc1 = malloc(1);
  alloc2 = malloc(1024);
  alloc3 = malloc(2048);
  mfree(alloc1);
  alloc1 = malloc(1024);

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("tester (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("tester (%d): Done!\n", getpid());
}
