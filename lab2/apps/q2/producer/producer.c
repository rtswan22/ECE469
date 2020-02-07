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
  char message[] = "Hello World";
  int message_len = dstrlen(message);
  char produced_item;
  int insert_success = 0;
  int i = 0;

  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  lock = dstrtol(argv[3], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((circ = (buffer *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  // Now print a message to show that everything worked
  Printf("producer: PID %d created\n", getpid());


  for(i = 0; i < message_len; i++) {
    produced_item = message[i];
    insert_success = 0;
    while(!insert_success) {
      while(lock_acquire(lock) != SYNC_SUCCESS);
      if (((circ->head + 1) % BUFFER_SIZE) != circ->tail) { // is it not full
        circ->item[circ->head] = produced_item;
        circ->head = (circ->head + 1) % BUFFER_SIZE;
        insert_success = 1;
        Printf("Producer %d inserted: %c\n", getpid(), produced_item);
      }
      if(lock_release(lock) != SYNC_SUCCESS) {
        Printf("Bad lock_release for lock (%d) in ", lock); Printf(argv[0]); Printf(", exiting...\n");
        Exit();
      }
    }
  }

  // Signal the semaphore to tell the original process that we're done
  Printf("producer: PID %d is complete.\n", getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
