#include "usertraps.h"
#include "misc.h"

#define TESTER "tester.dlx.obj"

sem_t sem;
char sem_str[10];
sem_t proc_sem;

void semCREATE(int count) {
  if ((sem = sem_create(1 - count)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }
}
void semWAIT() {
  if (sem_wait(sem) != SYNC_SUCCESS) {
    Printf("makeprocs (%d): Bad sem (%d) for tester\n", getpid(), sem);
    Exit();
  }
}

// CHECK: should this just run up to number 6 of the test programs based on what the given number is or what?
void main (int argc, char *argv[])
{
  if (argc != 1) {
    Printf("Usage: %s\n", argv[0]);
    Exit();
  }

  // PROCESSES
  Printf("-------------------------------------------------------------------------------------\n");
  semCREATE(2);
  ditoa(sem, sem_str);
  process_create(TESTER, sem_str, NULL);
  semWAIT();
  Printf("-------------------------------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}
