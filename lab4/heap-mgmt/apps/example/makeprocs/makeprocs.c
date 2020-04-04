#include "usertraps.h"
#include "misc.h"

#define TEST2_1 "test2_1.dlx.obj"
#define TEST2_2 "test2_2.dlx.obj"
#define TEST2_3 "test2_3.dlx.obj"
#define TEST2_4 "test2_4.dlx.obj"
#define TEST2_6 "test2_6.dlx.obj"

sem_t sem;
char sem_str[10];
sem_t proc_sem;

void semCREATE(int count) {
  if ((sem = sem_create(1 - count)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }
}
void semWAIT(int test_num) {
  if (sem_wait(sem) != SYNC_SUCCESS) {
    Printf("makeprocs (%d): Bad sem (%d) for test2_%d\n", getpid(), sem, test_num);
    Exit();
  }
}
void run_test2_1() {
  semCREATE(1);
  ditoa(sem, sem_str);
  process_create(TEST2_1, sem_str, NULL);
  semWAIT(1);
}
void run_test2_2() {
  semCREATE(1);
  ditoa(sem, sem_str);
  process_create(TEST2_2, sem_str, NULL);
  semWAIT(2);
}
void run_test2_3() {
  semCREATE(1);
  ditoa(sem, sem_str);
  process_create(TEST2_3, sem_str, NULL);
  semWAIT(3);
}
void run_test2_4() {
  semCREATE(1);
  ditoa(sem, sem_str);
  process_create(TEST2_4, sem_str, NULL);
  semWAIT(4);
}
void run_test2_5() {
  int i = 0;
  semCREATE(1);
  ditoa(sem, sem_str);
  for(i = 0; i < 100; i++) {
    process_create(TEST2_1, sem_str, NULL);
    // CHECK: should this wait for each process individually?
    semWAIT(5);
  }
}
void run_test2_6() {
  int i = 0;
  semCREATE(30);
  ditoa(sem, sem_str);
  for(i = 0; i < 30; i++) { 
    process_create(TEST2_6, sem_str, NULL);
  }
  semWAIT(6);
}

// CHECK: should this just run up to number 6 of the test programs based on what the given number is or what?
void main (int argc, char *argv[])
{
  int test_count = 0;             // Used to store number of processes to create
  int i;                               // Loop index variable
  if (argc != 2) {
    Printf("Usage: %s <number of hello world processes to create>\n", argv[0]);
    Exit();
  }

  // TEST COUNT
  test_count = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  if(test_count < 1 || test_count > 6) {
    Printf("makeprocs (%d): Exiting, argv[1] must be >= 1 and <= 6, argv[1] == %d\n", getpid(), test_count);
    Exit();
  }

  // PROCESSES
  for(i=1; i<=test_count; i++) {
    Printf("-------------------------------------------------------------------------------------\n");
    Printf("makeprocs (%d): test2_%d CREATE\n", getpid(), i);
    switch(i)
    {
      case 1: run_test2_1();
              break;
      case 2: run_test2_2();
              break;
      case 3: run_test2_3();
              break;
      case 4: run_test2_4(); // NEED: not freeing pages correctly?
              break;
      case 5: run_test2_5();
              break;
      case 6: run_test2_6();
              break;
      default: Printf("makeprocs (%d): invalid test number %d\n", getpid(), i);
    }
    Printf("makeprocs (%d): test2_%d DONE\n", getpid(), i);
  }
  
  Printf("-------------------------------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}
