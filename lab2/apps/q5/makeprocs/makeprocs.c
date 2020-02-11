#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed;
  char s_procs_completed_str[10]; 
  sem_t sem_H2O;
  char sem_H2O_str[10];
  sem_t sem_H2;
  char sem_H2_str[10];
  sem_t sem_O2;
  char sem_O2_str[10];
  sem_t sem_SO2;
  char sem_SO2_str[10];
  sem_t sem_H2SO4;
  char sem_H2SO4_str[10];
  int numprocs = 5;
  int inject_H2O_count;
  char inject_H2O_count_str[10];
  int inject_SO4_count;
  char inject_SO4_count_str[10];
  int react_2H2O_count;
  char react_2H2O_count_str[10];
  int react_SO4_count;
  char react_SO4_count_str[10];
  int react_H2_O2_SO2_count;
  char react_H2_O2_SO2_count_str[10];

 // print inputs
 //
 // H20 injection
 // SO4 injection
 // 2H20 reaction
 // SO4 reaction
 // H2 O2 SO2 reaction
 //
 // print remains/results

  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" requires 3 args\n");
    Exit();
  }

  inject_H2O_count = dstrtol(argv[1], NULL, 10);
  inject_SO4_count = dstrtol(argv[2], NULL, 10);
  react_2H2O_count = inject_H2O_count/2;
  react_SO4_count = inject_SO4_count;
  react_H2_O2_SO2_count = (react_2H2O_count*2)<react_SO4_count ? 2*react_2H2O_count : react_SO4_count;

  // sem to track process completion

  if ((s_procs_completed = sem_create(-(numprocs-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sem_H2O = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sem_H2 = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sem_O2 = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sem_SO2 = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((sem_H2SO4 = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  Printf("Creating %d H2Os and %d SO4s.\n", inject_H2O_count, inject_SO4_count);

  // Setup the command-line arguments
  ditoa(s_procs_completed, s_procs_completed_str);
  ditoa(sem_H2O, sem_H2O_str);
  ditoa(sem_H2, sem_H2_str);
  ditoa(sem_O2, sem_O2_str);
  ditoa(sem_SO2, sem_SO2_str);
  ditoa(sem_H2SO4, sem_H2SO4_str);
  
  ditoa(inject_H2O_count, inject_H20_count_str);
  ditoa(inject_SO4_count, inject_SO4_count_str);
  ditoa(react_2H2O_count, react_2H20_count_str);
  ditoa(react_SO4_count, react_SO4_count_str);
  ditoa(react_H2_O2_SO2_count, react_H2_O2_SO2_count_str);

  // create processes
  process_create(INJECT_H2O, s_procs_completed_str, inject_H2O_count_str, sem_H2O_str, NULL);
  process_create(INJECT_SO4, s_procs_completed_str, inject_SO4_count_str, sem_SO4_str, NULL);
  process_create(REACT_2H2O, s_procs_completed_str, react_2H2O_count_str, sem_H20_str, sem_H2_str, sem_O2_str, NULL);
  process_create(REACT_SO4, s_procs_completed_str, react_SO4_count_str, sem_SO4_str, sem_SO2_str, sem_O2_str, NULL);
  process_create(REACT_H2_O2_SO2, s_procs_completed_str, react_H2_O2_SO2_count_str, sem_H2_str, sem_O2_str, sem_SO2_str, sem_H2SO4_str, NULL);

  // wait till processes complete
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  Printf("%d, H2O\'s left over. %d H2\'s left over. %d O2\'s left over. %d SO2\'s left over. %d H2SO4's created.\n") // NEED
}
