#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int vaddr_no_alloc = 0xFFFFF + 1 - 10*4096;
  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("test2_3 (%d): Access memory inside the virtual address space, but outside of currently allocated pages!\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("test2_3 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }
  Printf("test2_3 (%d): Attempting to print vaddr %s\n", getpid(), "0xF6000);
  Printf("Value: %d\n", (*((int*)(vaddr_no_alloc)))); // CHECK: are we allowed to include the memory constants file
  Printf("test2_3 (%d): Done!\n", getpid());
}
