#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "synch.h"
#include "queue.h"
#include "mbox.h"

static mbox mboxes[MBOX_NUM_MBOXES];
static mbox_message mbox_messages[MBOX_NUM_BUFFERS];

//-------------------------------------------------------
//
// void MboxModuleInit();
//
// Initialize all mailboxes.  This process does not need
// to worry about synchronization as it is called at boot
// time.  Only initialize necessary items here: you can
// initialize others in MboxCreate.  In other words, 
// don't waste system resources like locks and semaphores
// on unused mailboxes.
//
//-------------------------------------------------------

void MboxModuleInit() {
  int i;
  dbprintf('p', "MboxModuleInit: Entering MboxModuleInit\n");
  for(i=0; i<MBOX_NUM_BOXES; i++) { mboxes[i].inuse = 0; }
  for(i=0; i<MBOX_NUM_BUFFERS; i++) { mbox_messages[i].inuse = 0; }
}

//-------------------------------------------------------
//
// mbox_t MboxCreate();
//
// Allocate an available mailbox structure for use. 
//
// Returns the mailbox handle on success
// Returns MBOX_FAIL on error.
//
//-------------------------------------------------------
mbox_t MboxCreate() {
  mbox_t mbx;
  mbox* mbx_init;
  uint32 intrval;
  int i;
  
  intrval = DisableIntrs(); // CHECK
  for(mbx=0; mbx<MBOX_NUM_MBOXES; mbx++) {
    if(mboxes[mbx].inuse==0) {
      mboxes[mbx].inuse = 1;
      break;
    }
  }
  RestoreIntrs(intrval); // CHECK
  if(mbx==MBOX_NUM_MBOXES) { return MBOX_FAIL; }

  mbx_init = &mboxes[mbx];
  if((mbx_init->lock = LockCreate()) != SYNC_SUCCES) { return MBOX_FAIL; }
  if((mbx_init->notfull = CondCreate(mbx_init->lock)) != SYNC_SUCCESS) { return MBOX_FAIL; }
  if((mbx_init->notempty = CondCreate(mbx_init->lock)) != SYNC_SUCCESS) { return MBOX_FAIL; }
  if(AQueueInit(&mbx_init->msg_queue) != QUEUE_SUCCESS) {
    printf("FATAL ERROR: could not initialize mbox_message waiting queue in MboxCreate!\n");
    exitsim();
  }
  for(i=0; i<PROCESS_MAX_PROCS; i++) {
    if(mbx_init->procs[i] != 0) {
      return MBOX_FAIL;
    }
  }

  return MBOX_SUCCESS;
}

//-------------------------------------------------------
// 
// void MboxOpen(mbox_t);
//
// Open the mailbox for use by the current process.  Note
// that it is assumed that the internal lock/mutex handle 
// of the mailbox and the inuse flag will not be changed 
// during execution.  This allows us to get the a valid 
// lock handle without a need for synchronization.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxOpen(mbox_t handle) {
  //get pid
  int pid = GetCurrentPid();

  //use lock
  if(LockHandleAcquire(mboxes[handle].lock == SYNC_FAILURE)
    {
      printf("Mbox Open successfully acquire the lock\n");
      return MBOX_FAIL;
      
    }

    //add to proc list
    mboxes[handle].procs[pid] = 1;
    //release lock
     if(LockHandleRelease(mboxes[handle].lock == SYNC_FAILURE)
    {
      printf("Mbox Open successfully release the lock\n");
      return MBOX_FAIL;
    }
    
       return MBOX_SUCCESS;
  

//-------------------------------------------------------
//
// int MboxClose(mbox_t);
//
// Close the mailbox for use to the current process.
// If the number of processes using the given mailbox
// is zero, then disable the mailbox structure and
// return it to the set of available mboxes.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxClose(mbox_t handle) {
  
  //get pid
  int pid = GetCurrentPid();
  //counter variable
  int numprocs
  //use lock
  if(LockHandleAcquire(mboxes[handle].lock == SYNC_FAILURE)
    {
      printf("Mbox Close successfully acquire the lock\n");
      return MBOX_FAIL;
      
    }

    //remove from proc list
    mboxes[handle].procs[pid] = 0;

    //checking to see if anyone else using the pid
    for(pid = 0; pid < PROCESS_MAX_PROCS; pid++)
      {
	if(mboxes[handle].procs[pid] == 1)
	  numprocs++;
      }

    //no other process using it
  if(numprocs == 0)
    {   
      mboxes[handle].inuse = 0; 
      //reinitialize queue
      AQueueInit(&(mboxes[handle].msg_queue));   
  }

    //release lock
     if(LockHandleRelease(mboxes[handle].lock == SYNC_FAILURE)
    {
      printf("Mbox Close successfully release the lock\n");
      return MBOX_FAIL;
    }
    
       return MBOX_SUCCESS;

}

//-------------------------------------------------------
//
// int MboxSend(mbox_t handle,int length, void* message);
//
// Send a message (pointed to by "message") of length
// "length" bytes to the specified mailbox.  Messages of
// length 0 are allowed.  The call 
// blocks when there is not enough space in the mailbox.
// Messages cannot be longer than MBOX_MAX_MESSAGE_LENGTH.
// Note that the calling process must have opened the 
// mailbox via MboxOpen.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxSend(mbox_t handle, int length, void* message) {
  return MBOX_FAIL;
}

//-------------------------------------------------------
//
// int MboxRecv(mbox_t handle, int maxlength, void* message);
//
// Receive a message from the specified mailbox.  The call 
// blocks when there is no message in the buffer.  Maxlength
// should indicate the maximum number of bytes that can be
// copied from the buffer into the address of "message".  
// An error occurs if the message is larger than maxlength.
// Note that the calling process must have opened the mailbox 
// via MboxOpen.
//   
// Returns MBOX_FAIL on failure.
// Returns number of bytes written into message on success.
//
//-------------------------------------------------------
int MboxRecv(mbox_t handle, int maxlength, void* message) {
  return MBOX_FAIL;
}

//--------------------------------------------------------------------------------
// 
// int MboxCloseAllByPid(int pid);
//
// Scans through all mailboxes and removes this pid from their "open procs" list.
// If this was the only open process, then it makes the mailbox available.  Call
// this function in ProcessFreeResources in process.c.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//--------------------------------------------------------------------------------
int MboxCloseAllByPid(int pid) {
  return MBOX_FAIL;
}
