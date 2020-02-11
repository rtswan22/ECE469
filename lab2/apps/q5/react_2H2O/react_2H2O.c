#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  buffer *circ;        // Used to access missile codes in shared memory page
  uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  lock_t lock;
  cond_t condp;
  cond_t condc;
  char message[] = "Hello World";
  int message_len = dstrlen(message);
  int i = 0;

  if (argc != 6) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  lock = dstrtol(argv[3], NULL, 10);
  condp = dstrtol(argv[4], NULL, 10);
  condc = dstrtol(argv[5], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((circ = (buffer *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  // Now print a message to show that everything worked
  Printf("consumer: PID %d created\n", getpid());


  for(i = 0; i < message_len; i++) {
    while(lock_acquire(lock) != SYNC_SUCCESS);
    while(circ->head == circ->tail) {
      if(cond_wait(condc) != SYNC_SUCCESS) {
        Printf("Bad cond_wait for cond (%d) in ", lock); Printf(argv[0]); Printf(", exiting...\n");
        Exit();
      }
    }

    Printf("Consumer %d removed: %c\n", getpid(), circ->item[circ->tail]);
    circ->tail = (circ->tail + 1) % BUFFER_SIZE;

    if(cond_signal(condp) != SYNC_SUCCESS) {
      Printf("Bad cond_signal for cond (%d) in ", lock); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }

    if(lock_release(lock) != SYNC_SUCCESS) {
      Printf("Bad lock_release for lock (%d) in ", lock); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }
  }

  // Signal the semaphore to tell the original process that we're done
  Printf("consumer: PID %d is complete.\n", getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
