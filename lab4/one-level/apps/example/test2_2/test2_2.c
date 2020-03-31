#include "usertraps.h"
#include "misc.h"
#include "os/memory_constants.h"

void main (int argc, char *argv[])
{
  
  
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int *memory;
  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 
  //Set memory address greater than the MEM_MAX_VIRTUAL_ADDRESS
  memory = (MEM_MAX_VIRTUAL_ADDRESS) + 1;
  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("Part2 Test2 (%d): Access Memory Beyond Max Virtual Address!\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Part2 Test2 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("Part 2 Test2 (%d): Done!\n", getpid());
}
