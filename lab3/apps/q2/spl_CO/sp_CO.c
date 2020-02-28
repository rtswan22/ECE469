#include "usertraps.h"
#include "misc.h"

#include "radeon.h"

void main (int argc, char *argv[]) 
{

	mbox_t mbox_co, mbox_o2;
	char msg[2];
	int rv1, rv2, rv3, rv4;

        
	mbox_co = dstrtol(argv[1], NULL, 10);
	mbox_o2 = dstrtol(argv[2], NULL, 10);
	//Open Mbox Co
	if(mbox_open(mbox_co) != MBOX_SUCCESS) {
		Printf("Split CO (%d): could not open mbox\n", getpid());
		Exit();
	}

	// RECIEVE CO's
	rv1 = mbox_recv(mbox_co, 2, (void *) &msg);
	if(rv1 != 2) {
		Printf("Split CO (%d): could not RV1 mbox 1. Rcv=%d\n", getpid(), rv1);
		Exit();
	}
	rv2 = mbox_recv(mbox_co, 2, (void *) &msg);
	if(rv2 != 2) {
		Printf("Split CO (%d): could not RV2 mbox 2. Rcv=%d\n", getpid(), rv2);
		Exit();
	}
	rv3 = mbox_recv(mbox_co, 2, (void *) &msg);
	if(rv3 != 2) {
		Printf("Split CO (%d): could not RV3 mbox 3. Rcv=%d\n", getpid(), rv3);
		Printf((char *) msg); Printf("\n");
		Exit();
	}
	rv4 = mbox_recv(mbox_co, 2, (void *) &msg);
	if(rv4 != 2) {
		Printf("Split CO (%d): could not RV4 mbox 4. Rcv=%d\n", getpid(), rv4);
		Exit();
	}

	//Close Mailbox
	if(mbox_close(mbox_co) != MBOX_SUCCESS) {
		Printf("Split CO (%d): could not close mbox\n", getpid());
		Exit();
	}
	
	//Open Mbox O2
	if(mbox_open(mbox_o2) != MBOX_SUCCESS) {
		Printf("Split CO (%d): could not open mbox\n", getpid());
		Exit();
	}

	// CREATE O2's
	if(mbox_send(mbox_o2, 2, (void *) "O2") != MBOX_SUCCESS) {
		Printf("Split CO (%d): could not send \n", getpid());
		Exit();
	}
	Printf("PID: %d Created an O2 molecule.\n", getpid());
	if(mbox_send(mbox_o2, 2, (void *) "O2") != MBOX_SUCCESS) {
		Printf("Split CO (%d): could not send \n", getpid());
		Exit();
	}	
	Printf("PID: %d Created an O2 molecule.\n", getpid());

	// Close mailboxes
	if(mbox_close(mbox_o2) != MBOX_SUCCESS) {
		Printf("Split CO (%d): could not close mbox\n", getpid());
		Exit();
	}

	
}
