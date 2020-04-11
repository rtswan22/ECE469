#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  //int data_num;
  void* alloc1;
  void* alloc2;
  void* alloc3;
  void* alloc4;
  void* alloc5;
  void* alloc6;
  void* alloc7;
  void* alloc8;
  //int fork_val;
  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("tester (%d): full alloc single\n", getpid());
  alloc1 = malloc(4096);
  mfree(alloc1);
  Printf("tester (%d): full random\n", getpid());
  alloc1 = malloc(5);
  alloc2 = malloc(1023);
  alloc3 = malloc(2000);
  alloc4 = malloc(500);
  mfree(alloc4);
  mfree(alloc1);
  alloc1 = malloc(1024);
  mfree(alloc2);
  mfree(alloc3);
  mfree(alloc1);
  Printf("tester (%d): full alloc small to large\n", getpid());
  alloc1 = malloc(1);
  alloc2 = malloc(32);
  alloc3 = malloc(64);
  alloc4 = malloc(128);
  alloc5 = malloc(256);
  alloc6 = malloc(512);
  alloc7 = malloc(1024);
  alloc8 = malloc(2048);
  mfree(alloc1);
  mfree(alloc2);
  mfree(alloc3);
  mfree(alloc4);
  mfree(alloc5);
  mfree(alloc6);
  mfree(alloc7);
  mfree(alloc8);
  Printf("tester (%d): full alloc to large to small\n", getpid());
  alloc1 = malloc(2048);
  alloc2 = malloc(1024);
  alloc3 = malloc(512);
  alloc4 = malloc(256);
  alloc5 = malloc(128);
  alloc6 = malloc(64);
  alloc7 = malloc(32);
  alloc8 = malloc(1);
  mfree(alloc1);
  mfree(alloc2);
  mfree(alloc3);
  mfree(alloc4);
  mfree(alloc5);
  mfree(alloc6);
  mfree(alloc7);
  mfree(alloc8);
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("tester (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("tester (%d): Done!\n", getpid());
}
