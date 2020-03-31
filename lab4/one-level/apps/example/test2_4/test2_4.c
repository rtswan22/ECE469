#include "usertraps.h"
#include "misc.h"

int grow(int n) {
  //int top = 0;
  if (n == 0) {
    return 0;
  }
  //Printf("Part2 Test4 (%d): Grow Stack %d times\n", getpid(), n);
  return(grow(n-1)+1);
}

void main (int argc, char *argv[])
{
  int grow_out;
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  grow_out = grow(278);
  // Now print a message to show that everything worked
  Printf("Part2 Test4 (%d): Grow Stack!\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Part2 Test4 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("Part2 Test4 (%d): Done!\n", getpid());
}
