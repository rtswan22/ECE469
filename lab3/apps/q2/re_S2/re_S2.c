#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[]) 
{

	mbox_t mbox_s2, mbox_s;
	char message[5];
	int mol;


	mbox_s2 = dstrtol(argv[1], NULL, 10);
	mbox_s = dstrtol(argv[2], NULL, 10);
if (argc != 3) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_page_mapped_semaphore> <so4_mbox>\n"); 
    Exit();
	}

	if(mbox_open(mbox_s2) != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not open mbox\n", getpid());
		Exit();
	}

	// Receive 1 S2
	mol = mbox_recv(mbox_s2, 2, (char *) &message);
	if(mol != 2) {
		Printf("Reaction S2 (%d): could not mol s2 mbox. Rcv=%d\n", getpid(), mol);
		Printf(msg); Printf("\n");
		Exit();
	}

	if(mbox_close(mbox_s2) != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not close mbox\n", getpid());
		Exit();
	}

	if(mbox_open(mbox_s) != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not open mbox\n", getpid());
		Exit();
	}
	// Send 2 S's
	if(mbox_send(mbox_s, 1, (void *) "S") != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not send \n", getpid());
		Exit();
	}
	Printf("PID: %d Created a S molecule.\n", getpid());
	if(mbox_send(mbox_s, 1, (void *) "S") != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not send \n", getpid());
		Exit();
	}	
	Printf("PID: %d Created a S molecule.\n", getpid());

	// Close the mailboxes

	if(mbox_close(mbox_s) != MBOX_SUCCESS) {
		Printf("Reaction S2 (%d): could not close mbox\n", getpid());
		Exit();
	}

if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
		Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
		Exit();
	}

}
