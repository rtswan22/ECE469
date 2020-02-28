#include "usertraps.h"
#include "misc.h"
#include "queue.h"

#include "spawn.h"
void main (int argc, char *argv[]) 
{

	mbox_t mbox_s2;

	mbox_s2 = dstrtol(argv[1], NULL, 10);

	if(mbox_open(mbox_s2) != MBOX_SUCCESS) {
		Printf("Injection S2 (%d): could not open mbox\n", getpid());
		Exit();
	}

	// Inject 1 S2
	if(mbox_send(mbox_s2, 2, (void *)"S2") != MBOX_SUCCESS) {
		Printf("Injection S2 (%d): could not send\n", getpid());
		Exit();
	}

	Printf("PID: %d Created a S2 molecule.\n", getpid());

	if(mbox_close(mbox_s2) != MBOX_SUCCESS) {
		Printf("Injection S2 (%d): could not close mbox\n", getpid());
		Exit();
	}

	
}
