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
  if((mbx_init->lock = LockCreate()) != SYNC_SUCCES) { return MBOX_FAIL; } // lock
  if((mbx_init->notfull = CondCreate(mbx_init->lock)) != SYNC_SUCCESS) { return MBOX_FAIL; } // notfull
  if((mbx_init->notempty = CondCreate(mbx_init->lock)) != SYNC_SUCCESS) { return MBOX_FAIL; } // notempty
  if(AQueueInit(&mbx_init->msg_queue) != QUEUE_SUCCESS) { // message queue
    printf("FATAL ERROR: could not initialize mbox_message waiting queue in MboxCreate!\n");
    exitsim();
  }
  for(i=0; i<PROCESS_MAX_PROCS; i++) { // has it been opened
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
  int pid = GetCurrentPid(); //get pid

  if(LockHandleAcquire(mboxes[handle].lock) == SYNC_FAIL) { //use lock
    printf("MboxOpen could not acquire the lock\n");
    exitsim(); // CHECK return MBOX_FAIL;
  }

  mboxes[handle].procs[pid] = 1; //add to proc list
 
  if(LockHandleRelease(mboxes[handle].lock) == SYNC_FAIL) { //release lock
    printf("MboxOpen could not release the lock\n");
    exitsim(); // CHECK return MBOX_FAIL;
  }
  
  return MBOX_SUCCESS;
}
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
  
  int pid = GetCurrentPid(); //get pid 
  int numprocs; //counter variable

  if(LockHandleAcquire(mboxes[handle].lock) == SYNC_FAIL) { //use lock
    printf("MboxClose could not acquire the lock\n");
    return MBOX_FAIL; // CHECK exit
  }

  mboxes[handle].procs[pid] = 0; //remove from proc list

  for(pid = 0; pid < PROCESS_MAX_PROCS; pid++) { //checking to see if anyone else using the pid
    if(mboxes[handle].procs[pid] == 1) { numprocs++; }
  }

  if(numprocs == 0) { //no other process using it 
    mboxes[handle].inuse = 0; 
    AQueueInit(&(mboxes[handle].msg_queue)); //CHECK: reinitialize queue, ProcessFreeResources?
  }

  if(LockHandleRelease(mboxes[handle].lock) == SYNC_FAIL) { //release lock
    printf("MboxClose failed to release the lock\n");
    return MBOX_FAIL; // CHECK exit
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
  uint32 pid = GetCurrentPid();
  uint32 intrval;
  int mbox_msg;

  if(mboxes[handle].procs[pid] != 1) { // does the process have the mbox open
    printf("MboxSend: failed, PID %d does not have mailbox %d open\n", pid, handle);
    return MBOX_FAIL; // CHECK exit
  }

  if(LockHandleAcquire(mboxes[handle].lock) == SYNC_FAIL) { //use lock
    printf("MboxSend: could not acquire the lock\n");
    return MBOX_FAIL; // CHECK exit
  }

  while(AQueueLength(&mboxes[handle].msg_queue) >= MBOX_MAX_BUFFERS_PER_BOX) {
    if(CondHandleWait(mboxes[handle].notfull) != SYNC_SUCCESS) { // wait for notfull
      printf("MboxSend: bad CondHandleWait() for mbox %d in PID %d\n", handle, pid);
      return MBOX_FAIL; // CHECK exit
    }
  }

  intrval = DisableIntrs();
  for(mbox_msg=0; mbox_msg<MBOX_NUM_BUFFERS; mbox_msg++) {
    if(mbox_messages[mbox_msg].inuse == 0) {
      mbox_messages[mbox_msg].inuse = 1;
      break;
    }
  }
  RestoreIntrs(intrval)

  mbox_messages[mbox_msg].length = length;
  bcopy(message, (void*)mbox_messages[mbox_msg].buffer, length); // CHECK

  intrval = DisableIntrs(); // CHECK
  if ((l = AQueueAllocLink ((void *)&mbox_messages[mbox_msg])) == NULL) { // CHECK curretPCB previously
    printf("FATAL ERROR: could not allocate link for message queue in MboxSend!\n");
    return MBOX_FAIL;
  }
  if (AQueueInsertLast (&mboxes[handle].msg_queue, l) != QUEUE_SUCCESS) {
    printf("FATAL ERROR: could not insert new link into message waiting queue in MboxSend!\n");
    return MBOX_FAIL;
  }
  RestoreIntrs(intrval); // CHECK

  if(CondHandleSignal(mboxes[handle].notempty) == SYNC_FAIL) {
    printf("MboxSend: bad CondHandleSignal for mbox %d in PID %d\n", handle, pid);
    return MBOX_FAIL; // CHECK exit
  }

  return MBOX_SUCCESS;
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
  uint32 pid = GetCurrentPid();
  uint32 intrval;
  Link* l;
  mbox_message* msg_recv;
  int recv_length

  if(mboxes[handle].procs[pid] != 1) { // does the process have the mbox open
    printf("MboxRecv: failed, PID %d does not have mailbox %d open\n", pid, handle);
    return MBOX_FAIL; // CHECK exit
  }

  if(LockHandleAcquire(mboxes[handle].lock) == SYNC_FAIL) { //use lock
    printf("MboxRecv: could not acquire the lock\n");
    return MBOX_FAIL; // CHECK exit
  }

  while(AQueueEmpty(&mboxes[handle].msg_queue)) {
    if(CondHandleWait(mboxes[handle].notempty) != SYNC_SUCCESS) { // wait for notempty
      printf("MboxRecv: bad CondHandleWait() for mbox %d in PID %d\n", handle, pid);
      return MBOX_FAIL; // CHECK exit
    }
  }

  intrval = DisableIntrs(); // CHECK
  l = AQueueFirst(&mboxes[handle].msg_queue);
  msg_recv = (mbox_message*)AQueueObject(l);
  if((AQueueRemove(&l) != QUEUE_SUCCESS)) {
    printf("MboxRecv: could not AQueueRemove link\n");
    return MBOX_FAIL;
  }
  RestoreIntrs(intrval); // CHECK

  if(msg_recv->length > maxlength) {
    printf("MboxRecv: message too long\n");
    msg_recv->inuse = 0; // CHECK
    return MBOX_FAIL;
  }

  bcopy((void*)msg_recv->buffer, message, msg_recv->length);

  recv_length = msg_recv->length;
  msg_recv->inuse = 0; // CHECK

  if(CondHandleSignal(mboxes[handle].notfull) == SYNC_FAIL) {
    printf("MboxRecv: bad CondHandleSignal for mbox %d in PID %d\n", handle, pid);
    return MBOX_FAIL; // CHECK exit
  }

  return recv_length;
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
int i; 
int j;
int numprocs;

//loops through all mboxes.
//if pid opened it-> remove from list
//if not mark not in use. 
  for (i = 0; i < MBOX_NUM_MBOXES; i++) {
    //use lock
    if(LockHandleAcquire(mboxes[i].lock) == SYNC_FAIL) {
      printf("Mbox Close All By Pid did not successfully acquire the lock\n");
      return MBOX_FAIL;
    }

    numprocs = 0;
    if(mboxes[i].procs[pid] == 1) {
      mboxes[i].procs[pid] = 0;

      for(j = 0; j < PROCESS_MAX_PROCS; j++) {
        if(mboxes[i].procs[j] == 1) {
          numprocs++;
        }
      }
      if(numprocs == 0) { mboxes[i].inuse = 0 };

    }
    //release lock
    if(LockHandleRelease(mboxes[handle].lock == SYNC_FAIL) {
      printf("Mbox Close All By Pid did not successfully release the lock\n");
      return MBOX_FAIL;
    }
  }

  return MBOX_SUCCESS;
}
