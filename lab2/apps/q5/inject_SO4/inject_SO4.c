#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  sems_and_counts* sc;
  uint32 h_mem;            // Handle to the shared memory page
  int i = 0;

  if (argc != 2) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10

  // Map shared memory page into this process's memory space
  if ((sc = (sems_and_counts*)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  for(i = 0; i < sc->inject_count[SO4]; i++) {
    if(sem_signal(sc->molecules[SO4]) != SYNC_SUCCESS) {
      Printf("Bad semaphore molecules[SO4] (%d) in ", sc->molecules[SO4]); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }
    Printf("SO4 injected into Radeon atmosphere, PID: %d\n", getpid());
  }

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(sc->s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", sc->s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}

