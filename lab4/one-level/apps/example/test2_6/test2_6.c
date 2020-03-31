#include "usertraps.h"
#include "misc.h"
#include "os/memory_constants.h"

void main (int argc, char *argv[])
{
  int i;
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

   Printf("test2_6 (%d): Before count\n", getpid());
	for (i = 0; i < 20000; i++);
  
  // Now print a message to show that everything worked
  Printf("test2_6 (%d): After count\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("test2_6 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("test2_6 (%d): Done!\n", getpid());
}
