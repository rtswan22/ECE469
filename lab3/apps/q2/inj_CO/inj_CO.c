#include "usertraps.h"
#include "misc.h"
#include "queue.h"

#include "spawn.h"

void main (int argc, char *argv[]) 
{

	mbox_t mbox;
	mbox = dstrtol(argv[1], NULL, 10);
	


	//Open Mailbox
	if(mbox_open(mbox) != MBOX_SUCCESS) {
		Printf("Injection CO (%d): could not open mbox\n", getpid());
		Exit();
	}

	// Inject 1 CO
	if(mbox_send(mbox, 2, (void *)"CO") != MBOX_SUCCESS) {
		Printf("Injection CO (%d): could not send\n", getpid());
		Exit();
	}

	Printf("PID: %d Created a CO molecule.\n", getpid());
	//Close Mbox
	if(mbox_close(mbox) != MBOX_SUCCESS) {
		Printf("Injection CO (%d): could not close mbox\n", getpid());
		Exit();
	}

       
}
