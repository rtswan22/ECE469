#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  
  
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int memory;
  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 
  //Set memory address greater than the MEM_MAX_VIRTUAL_ADDRESS
  memory = (*((int*)(0xFFFFFFFF))); // CHECK: are we allowed to include the memory constants file
  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("test2_2 (%d): Access Memory Beyond Max Virtual Address! NOT NOT NOT NOT NOT NOT NOT\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("test2_2 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("test2_2 (%d): Done!\n", getpid());
}
