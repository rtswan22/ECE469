#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  int vaddr_over = 0xFFFFF + 1;
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 
  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("test2_2 (%d): Access Memory Beyond Max Virtual Address!\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("test2_2 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }
  //Set memory address greater than the MEM_MAX_VIRTUAL_ADDRESS
  Printf("test2_2 (%d): Attempting to print value at vaddr %s\n", getpid(), "0x100000");
  Printf("Value: %d\n", (*((int*)(vaddr_over))));

  Printf("test2_2 (%d): Done!\n", getpid());
}
