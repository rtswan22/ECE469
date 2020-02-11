#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  sems_and_counts* sc;
  uint32 h_mem;
  char h_mem_str[10];
  int numprocs = 5;
 // print inputs
 //
 // H20 injection
 // SO4 injection
 // 2H20 reaction
 // SO4 reaction
 // H2 O2 SO2 reaction
 //
 // print remains/results

  // ARGC
  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" requires 3 args\n");
    Exit();
  }

  // SHARED MEMORY
  if ((h_mem = shmget()) == 0) {
    Printf("ERROR: could not allocate shared memory page in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  if ((sc = (sems_and_counts*)shmat(h_mem)) == NULL) {
    Printf("Could not shmat(handle) in "); Printf(argv[0]); Printf(", exiting..\n");
    Exit();
  }

  // SEMAPHORES
  if ((sc->s_procs_completed = sem_create(-(numprocs-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sc->molecules[H2O] = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sc->molecules[SO4] = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sc->molecules[H2] = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sc->molecules[O2] = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sc->molecules[SO2] = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sc->molecules[H2SO4] = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  // COUNTS
  sc->inject_count[H2O] = dstrtol(argv[1], NULL, 10);
  sc->inject_count[SO4] = dstrtol(argv[2], NULL, 10);
  sc->react_count[R_2H2O] = sc->inject_count[H2O]/2;
  sc->react_count[R_SO4] = sc->inject_count[SO4];
  sc->react_count[R_H2_O2_SO2] = (sc->react_count[R_2H2O]*2)<sc->react_count[R_SO4] ? 2*sc->react_count[R_2H2O] : sc->react_count[R_SO4];

  Printf("Creating %d H2Os and %d SO4s.\n", sc->inject_count[H2O], sc->inject_count[SO4]);

  // Setup the command-line arguments
  ditoa(h_mem, h_mem_str);

  // create processes
  process_create(INJECT_H2O, h_mem_str, NULL);
  process_create(INJECT_SO4, h_mem_str, NULL);
  process_create(REACT_2H2O, h_mem_str, NULL);
  process_create(REACT_SO4, h_mem_str, NULL);
  process_create(REACT_H2_O2_SO2, h_mem_str, NULL);

  // wait till processes complete
  if (sem_wait(sc->s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", sc->s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  //Printf("%d, H2O\'s left over. %d H2\'s left over. %d O2\'s left over. %d SO2\'s left over. %d H2SO4's created.\n") // NEED
}
