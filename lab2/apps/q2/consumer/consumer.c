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
  
  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  lock = dstrol(argv[3],NULL,10);
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);

  // Map shared memory page into this process's memory space
  // if ((mc = (missile_code *)shmat(h_mem)) == NULL) {
if((circ = (buffer *)shmat(h_mem))==NULL)
  { Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
   Exit();
  }
char string[] = "Hello world";
  int len = dstrlen(string);
  char consumed_item;
for(int i = 0; i  < length; i++)
  {
if (lock_acquire(lock) != SYNC_SUCCESS) {
			Printf("Could not acquire lock in "); Printf(argv[0]); Printf("\n");
			Exit();
		}
    if (circ_buff->head != circ_buff->tail) { // if not empty
      consumed_item = circ->item[circ->tail];
      circ->tail = (circ->tail + 1) % BUFFER_SIZE;
}
Printf("Consumer %d removed: %c\n", getpid(), val_read);
		if (lock_release(lock_buf) != SYNC_SUCCESS) {
			Printf("Could not release lock in "); Printf(argv[0]); Printf("\n");
			Exit();
		}

  }
														 
 
  // Signal the semaphore to tell the ilurffffeforiginal process that we're done
  
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
   }
}
