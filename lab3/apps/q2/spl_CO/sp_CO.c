#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[]) 
{

	mbox_t mbox_co, mbox_o2;
	char message[2];
	int mol1, mol2, mol3, mol4;

        
	mbox_co = dstrtol(argv[1], NULL, 10);
	mbox_o2 = dstrtol(argv[2], NULL, 10);
	//Open Mbox Co
	if(mbox_open(mbox_co) != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not open mbox\n", getpid());
		Exit();
	}

	// RECEIVE CO's
	mol1 = mbox_recv(mbox_co, 2, (void *) &message);
	if(mol1 != 2) {
		Printf("Reaction CO (%d): could not mol1 mbox 1. mol=%d\n", getpid(), mol1);
		Exit();
	}
	mol2 = mbox_recv(mbox_co, 2, (void *) &message);
	if(mol2 != 2) {
		Printf("Reaction CO (%d): could not mol2 mbox 2. mol2=%d\n", getpid(), mol2);
		Exit();
	}
	mol3 = mbox_recv(mbox_co, 2, (void *) &message);
	if(mol3 != 2) {
		Printf("Reaction CO (%d): could not mol3 mbox 3. mol3=%d\n", getpid(), mol3);
		Printf((char *) message); Printf("\n");
		Exit();
	}
	mol4 = mbox_recv(mbox_co, 2, (void *) &message);
	if(mol4 != 2) {
		Printf("Reaction CO (%d): could not mol4 mbox 4. Rv=%d\n", getpid(), mol4);
		Exit();
	}

	//Close Mailbox
	if(mbox_close(mbox_co) != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not close mbox\n", getpid());
		Exit();
	}
	
	//Open Mbox O2
	if(mbox_open(mbox_o2) != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not open mbox\n", getpid());
		Exit();
	}

	// CREATE O2's
	if(mbox_send(mbox_o2, 2, (void *) "O2") != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not send \n", getpid());
		Exit();
	}
	Printf("PID: %d Created an O2 molecule.\n", getpid());
	if(mbox_send(mbox_o2, 2, (void *) "O2") != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not send \n", getpid());
		Exit();
	}	
	Printf("PID: %d Created an O2 molecule.\n", getpid());

	// Close mailboxes
	if(mbox_close(mbox_o2) != MBOX_SUCCESS) {
		Printf("Reaction CO (%d): could not close mbox\n", getpid());
		Exit();
	}

	
}
