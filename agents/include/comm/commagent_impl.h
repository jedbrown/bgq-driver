/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2011, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef  __COMM_AGENT_IMPL__
#define  __COMM_AGENT_IMPL__


/**
 * \file commagent_impl.h
 *
 * \brief C Header File containing Comm Agent Inline Implementations.
 *        This header is included by commagent.h.
 *
 * The comm agent uses its own shared memory segment to communicate with the
 * processes on the node.  CommAgent_Init() is used by a process to map this
 * shared memory and wait for a field in that shared memory to change to
 * the comm agent's version number, indicating that it is ready to accept
 * requests.
 *
 * After using the other initializer functions to initialize the fence and
 * rget pacing functions of the agent, a process is ready to submit requests
 * to the agent.  CommAgent_AllocateWorkRequest() returns a pointer to
 * storage in the shared memory where the process can build the request.
 * When the process is done building the request, it calls one of the
 * submit functions to submit the request to the agent.  This storage
 * for the request is actually a slot in an L2 atomic bounded queue.
 * The allocate function attempts to increment the L2 atomic producer
 * counter.  The process must keep calling the allocate function until the
 * counter is successfully incremented.  The value of the counter
 * (mod'd by the number of elements in the queue)
 * is the slot number in the queue, which is converted to the pointer to
 * the storage for the request.  The successful increment of the counter
 * wakes up the comm agent (since the atomic queue counters are in the
 * wakeup address compare range).  The comm agent spins until the request
 * has been submitted.  After the process builds the request in the storage,
 * and during the submit function, a "ready indicator" in the request is
 * changed from zero to non-zero.  When the comm agent sees this change, it
 * knows the request is ready to consume.  The comm agent malloc's space for a
 * work item, copies info from the request into the work item, clears the
 * "ready indicator" in the request storage, and increments the bound on the
 * queue, freeing up that slot in the queue.  Basically, the comm agent
 * moves the requests from the queue into work items, freeing up space in
 * the queue as soon as possible.  The comm agent manages its work items
 * separate from the queue.
 */


#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/MU_Addressing.h>
#include <spi/include/kernel/memory.h>
#include <spi/include/kernel/location.h>

#ifdef TRACE
#undef TRACE
#endif
#define TRACE(x) //fprintf x


/**
 * \brief Comm Agent Shared Memory File Name
 */
#define COMM_AGENT_SHM_FILE_NAME "/unique-comm-agent-shmem-file"


/**
 * \brief Size of Comm Agent Shared Memory File
 */
#define COMM_AGENT_SHM_FILE_SIZE (65536)


/**
 * \brief Number of Entries in Comm Agent's Request Queue
 */
#define COMM_AGENT_REQUEST_QUEUE_SIZE (64)


/**
 * \brief Agent L2 Atomics Bounded Queue
 */
typedef struct CommAgent_QueueHeader
{
  volatile uint64_t producerL2counter; /**< Incremented by the producer when it
					*   wants a new slot in the queue.
					*   This counter (mod'd by the queue size)
					*   is the index into the queue.
					*/
  volatile uint64_t boundL2counter;    /**< The upper bound on the producerL2counter.
					*   When producerL2counter is incremented
					*   using atomic-increment-bounded, the L2 
					*   will fail the increment if the 
					*   producerL2counter has run into the 
					*   boundL2counter.  This occurs when the
					*   queue is full.
					*/
  uint64_t headCounter;                /**< The index into the queue of the next
					*   slot to be processed by the consumer.
					*   Does not need to be an L2 counter since
					*   there is only one consumer.
					*/
} CommAgent_QueueHeader_t;


/**
 * \brief Agent Shared Memory Map
 *
 * Maps the fields in the agent's shared memory.
 * The first field (agentState) indicates whether the agent's shared memory
 * is initialized yet.
 * The queue and receptionCounters pointers point to blocks of the agent's
 * shared memory where the queue and remote get reception counters are located.
 * These need to be pointers because the queue and number of counters are a
 * configurable size and may vary in size from run to run.
 * Note that the atomic counters in the request queue header and the remote
 * get pacing counters must be in the Wakeup Address Compare (WAC) range used
 * by the agent.  
 * - When the queue counters change, the agent needs to wake up to extract
 *   elements from the queue.
 * - When the reception counters change, the agent must wake up to process the
 *   completion of the sub-message.
 * The rest of the fields don't really change.
 * This WAC range must be a 2^^N in size and the WAC range must be on that same
 * 2^^N  boundary.  The actual WAC size is computed at runtime based on the size
 * of the request queue header and the reception counters.
 */
typedef struct CommAgent_SharedMemoryMap
{
  volatile CommAgent_State_t agentState; /**< Agent state
					  * COMM_AGENT_STATE_UNINITIALIZED = 0,
					  * COMM_AGENT_STATE_INITIALIZED_VERSION_1   = 1
					  * COMM_AGENT_STATE_INITIALIZED_VERSION_2   = 2
					  * etc.
					  */
  uint32_t globalRecFifoId; /**< The global reception fifo ID used by the comm agent. */

  CommAgent_QueueHeader_t *queueHeader; /**< Pointer to Agent's request queue header. */

  CommAgent_WorkRequest_t *queue;      /**< Pointer to the Agent's request queue,
					*   which is an array of work requests
					*   having _requestQueueSize entries.
					*/
  
  volatile uint64_t *receptionCounters; /**< These are the reception counters
					 *   used by the agent for each remote get
					 *   sub-message.  The number of
					 *   sub-messages could change, so this is
					 *   an open-ended set of counters.
					 *   Nothing should come after this in shared
					 *   memory.
					 */
  /* After these pointers, in shared memory, are the actual queue, queueHeader,
   * and receptionCounters, pointed-to by the above pointers.
   * The queue header and reception counters are in the WAC range, so they must be
   * a power of 2 in size and be aligned on that same power of 2 boundary.
   */
} CommAgent_SharedMemoryMap_t;


/**
 * \brief Control Info Structure
 *
 * This is an internal structure used by the remote get pacing functions.
 * This structure is malloc'd within RemoteGetPacing_Init() and anchored
 * in the control structure passed back to the app.
 */
typedef struct CommAgent_ControlInfo
{
  int agentShmFd;               /**< File descriptor for the agent's
				 *   shared memory.                          
				 */
  CommAgent_SharedMemoryMap_t *agentShmPtr; /**< Pointer to the 
					      * agent's shared memory pointers.
					      */
  volatile uint64_t *producerCounter[4]; /**< L2 Atomic pointers to the producer
					  *   counter (one per HW thread (0..3).
					  */

} CommAgent_ControlInfo_t;


/**
 * \brief L2 Atomic Queue Full Indicator
 */
#define COMM_AGENT_L2_ATOMIC_QUEUE_FULL        0x8000000000000000UL


/**
 * \brief Internal Comm Agent Work Request
 *
 * This is an internal view of what is placed in the request queue to describe
 * a work request to the agent.  This expresses what is common to all work
 * requests.
 * \see CommAgent_RemoteGetPacing_InternalWorkRequest_t
 * \see CommAgent_Fence_InternalWorkRequest_t
 */
typedef struct CommAgent_InternalWorkRequest
{
  volatile char type; /**< This first byte of a work request indicates whether the work
		       *   request is ready for the agent to process.  It also indicates
		       *   the type of the work request.  
		       *   - COMM_AGENT_WORK_REQUEST_TYPE_NONE indicates the request
		       *     is NOT READY and has a value of binary zero.
		       *   - COMM_AGENT_WORK_REQUEST_TYPE_FENCE indicates the work
		       *     request is ready for the agent to process and indicates
		       *     the type of the work request.  It should be set AFTER the
		       *     entire work request has been initialized.  Note that this
		       *     overlays the first byte of the work request.
		       *   After allocating a slot in the request queue, the work request
		       *   should be copied into the slot.  The first byte of the work
		       *   request must be binary zero, such that as the work request is
		       *   being copied it will still indicate NOT_READY until the entire
		       *   work request has been copied to the slot.  Then, this "state"
		       *   should be set to one of the READY type values so the agent
		       *   knows it is ready to be processed and the type of request.
		       *   CommAgent_Fence_SubmitWorkRequest() sets this state
		       *   to the READY value, and the agent sets it to NOT_READY (TYPE_NONE)
		       *   after extracting the work request from the work queue.
		       */
} CommAgent_InternalWorkRequest_t;


/**
 * \brief Reception Fifo Dispatch Ids
 *
 * These are the dispatch Ids assigned to various comm agent functions.
 * They are used as indexes into the comm agents internal dispatch array.
 * COMM_AGENT_MAX_DISPATCH_ID must be the last, since it is used as
 * the dimension of the dispatch function array.
 */
typedef enum
  {
    COMM_AGENT_FENCE_DISPATCH_ID,
    COMM_AGENT_MAX_DISPATCH_ID
  } CommAgent_RecFifoDispatchIds_t;


/**
 * \brief Build an L2 Atomic Address
 *
 * An L2 atomic address must be used to reference an 8-byte L2 atomic counter.
 * The address is constructed by ORing the following:
 *
 * - Base Address, obtained from Kernel_L2AtomicsBaseAddress().
 *
 * - Virtual Address<<5, which is the virtual address of the 8-byte-aligned counter,
 *   shifted by 5 to open up 8 bits at the bottom of the address field.
 *
 * - 8-bits at the low-order end of the address:
 *
 *   - Thread ID (0..3)<<6, which is the thread ID relative to the core.  So, we need 4
 *     addresses, each with differing Thread IDs to address the counter in different
 *     hardware threads.  This Thread ID is shifted by 6, so it is the highest-order
 *     2 bits of the 8 bits at the bottom of the address field.
 *
 *   - L2 Opcode<<3, which is the L2 Atomic opcode shifted by 3.  It is the next 3 bits
 *     below the Thread ID.
 *
 *   - Unused, which is the low-order 3 bits.
 *
 * \param[in]  VA      Virtual address of the counter, 8-byte aligned.
 * \param[in]  opCode  L2 atomic op-code.  See hwi/include/bqc/MU_Addressing.h,
 *                     definitions MUHWI_ATOMIC_OPCODE_xxxxx.
 * \param[out] counterPtrs  Array of 4 pointers output from this function.
 *                          counterPtrs[i] is the L2 atomic address for thread i,
 *                          where i=[0..3].
 */
__INLINE__
void CommAgent_BuildL2AtomicAddresses ( volatile uint64_t  *VA,
					uint64_t            opCode,
					volatile uint64_t **counterPtrs )
{
  uint64_t tid = 0;
  uint64_t base = Kernel_L2AtomicsBaseAddress();

  for (tid = 0; tid < 4UL; tid ++)
    {
      counterPtrs[tid] = (volatile uint64_t *)
	((( base +
	    ((((uint64_t)VA) << 5) & ~0xFFUL)) |
	  (tid << 6)) +
	 (opCode << 3));
    }
}


/**
 * \brief Setup Access to the Agent's Shared Memory
 *
 * Access the agent's shared memory, and map it for L2 atomics.
 *
 * 1. shm_open and mmap the agent's shared memory file.
 *    Note that this open/map sequence includes an ioctl to 
 *    obtain L2 atomics addressability to the agent's shared memory
 *    so we can use an L2 atomic bounded queue within the
 *    shared memory.
 *
 * \param[out]  agentShmPtr  Pointer to the mapped shared memory.
 * \param[out]  fd           Shared memory file descriptor.
 *
 * \retval  0       Successful.
 * \retval  errno   Indicates the reason for failure.
 */
__INLINE__ 
int CommAgent_SetupAgentSharedMemory( CommAgent_SharedMemoryMap_t **agentShmPtr,
				      int *fd )
{
  int rc;
  int internalFd;
  CommAgent_SharedMemoryMap_t *internalAgentShmPtr;    

  /* Open/create the shared memory */
  internalFd = shm_open( COMM_AGENT_SHM_FILE_NAME, 
			 O_CREAT | O_RDWR, 
			 S_IRUSR | S_IWUSR );
  if ( internalFd == -1 ) 
    {
      return errno;
    }
  *fd = internalFd; /* Return the file descriptor. */

  /* Indicate that this shared memory is to be mapped for L2 atomics */
  uint64_t dummy=1;
  rc = ioctl(internalFd, FIOBGQATOMIC, &dummy);
  if ( rc )
    {
      close( internalFd );
      return errno;
    }

  /* Size the shared memory */
  if ( ftruncate( internalFd, 
		  COMM_AGENT_SHM_FILE_SIZE ) == -1)
    {
      close( internalFd );
      return errno;
    }

  /* Obtain a pointer to the shared memory */
  internalAgentShmPtr = (CommAgent_SharedMemoryMap_t*)
                mmap( NULL, 
		      COMM_AGENT_SHM_FILE_SIZE,
		      PROT_READ | PROT_WRITE, 
		      MAP_SHARED, 
		      internalFd, 
		      0 );
  if ( internalAgentShmPtr == MAP_FAILED )
    {
      close( internalFd );
      return errno;
    }
  *agentShmPtr = internalAgentShmPtr; /* Return the pointer to the shared memory */

  TRACE((stderr,"%s() [%s:%d]: Shared memory mapping address %p for length %u\n",__FUNCTION__,__FILE__,__LINE__,internalAgentShmPtr,COMM_AGENT_SHM_FILE_SIZE));

  return 0;
}


/**
 * \brief Initialize the Comm Agent
 *
 * This function must be called to initialize the comm agent control structure
 * before calling any other comm agent functions.
 * - If multiple processes on the node are sharing the control structure
 *   (e.g. in shared memory), this only needs to be called by one of those
 *   processes.
 * - If each process on the node has its own control structure (for convenience),
 *   this should be called by each of those processes.
 *
 * \param[out] controlPtr           Pointer to the control structure.  The
 *                                  structure is initialized by this function,
 *                                  and it must be passed into subsequent calls
 *                                  to CommAgent_XXXX() functions unchanged.
 *
 * \retval  0       Successful.  The agent is initialized and ready to receive
 *                  comm requests.
 * \retval  errno   Indicates the reason for failure.
 *                  ENOENT means the Comm Agent is not running.
 */
__INLINE__
int CommAgent_Init ( CommAgent_Control_t *controlPtr )
{
  int rc;
  CommAgent_SharedMemoryMap_t *agentShmPtr=NULL;

  /* Malloc storage for the internal control info structure.
   * Anchor it in the control structure passed in by the caller.
   */
  CommAgent_ControlInfo_t *infoPtr;
  infoPtr = (CommAgent_ControlInfo_t *)malloc(sizeof(CommAgent_ControlInfo_t));
  if ( infoPtr == NULL ) return ENOMEM;
  memset(infoPtr, 0x00, sizeof(*infoPtr));
  controlPtr->pOpaqueObject = (void *)infoPtr;

  /* Set up access to the agent's shared memory, including L2 atomic access to it. */
  rc = CommAgent_SetupAgentSharedMemory( &agentShmPtr,
					 &infoPtr->agentShmFd );
  if (rc)
    {
      free( infoPtr );
      return rc;
    }

  infoPtr->agentShmPtr = agentShmPtr;

  TRACE((stderr,"%s() [%s:%d]: SetupAgentSharedMemory() returned agentShmPtr=%p, infoPtr=%p.  Waiting for agent\n",__FUNCTION__,__FILE__,__LINE__,agentShmPtr,infoPtr));

  /* Wait for the agent to finish its initialization.
   * It will change its state (in its shared memory).
   * If this does not occur within 1 second, return
   * ENOENT to indicate the agent is not runnng.
   */
  uint64_t diffTime;
  uint64_t startTime = GetTimeBase();
  do {
    diffTime = GetTimeBase() - startTime;
  }
  while ( ( agentShmPtr->agentState == COMM_AGENT_STATE_UNINITIALIZED ) &&
          ( diffTime < 1600000000 ) );
  if ( agentShmPtr->agentState == COMM_AGENT_STATE_UNINITIALIZED ) return ENOENT;

  /* Set up the pointers to the agent's atomic queue counters.
   * - Producer counter is updated via bounded-increment.
   * - Bound counter is not used by the app.
   * There are 4 pointers, one for each HW thread on a core because the HW 
   * thread id (0..3) is part of the L2 atomic address.
   */
  CommAgent_BuildL2AtomicAddresses ( &agentShmPtr->queueHeader->producerL2counter,
				     MUHWI_ATOMIC_OPCODE_LOAD_INCREMENT_BOUNDED,
				     &infoPtr->producerCounter[0] );

  TRACE((stderr,"%s() [%s:%d]: Done waiting for agent at %lu.  queue=%p, producerCounters=%p,%p,%p,%p\n",__FUNCTION__,__FILE__,__LINE__,GetTimeBase(),&agentShmPtr->queue,infoPtr->producerCounter[0],infoPtr->producerCounter[1],infoPtr->producerCounter[2],infoPtr->producerCounter[3]));

  return 0;
}


/**
 * \brief Allocate Storage for a Comm Agent Work Request
 *
 * Obtain a pointer to storage where a comm agent work request can be
 * built.  Upon successful return, the returned pointer points to storage
 * large enough to contain a CommAgent_WorkRequest_t.  The app
 * should fill in the storage as described in that structure's definition
 * and call the appropriate CommAgent_XXXXX_SubmitWorkRequest() function 
 * to submit the request.
 *
 * This request to allocate storage may fail with EAGAIN.  This means the
 * queue is full, and the app must wait until the agent removes request(s)
 * from the queue to free up storage.
 *
 * \param[in]  control                The control structure initialized by
 *                                    CommAgent_Init().
 * \param[out]  workRequestPtrAddress  The address of a pointer where the pointer
 *                                     to the work request storage is returned.
 *                                     The pointer will be NULL if no storage is
 *                                     available.
 * \param[out]  uniqueID               Pointer to storage where an ID that is
 *                                     unique across the node of this particular
 *                                     work request is returned.  This can
 *                                     later be passed to the comm agent inside
 *                                     rgetpacing work requests to tie them 
 *                                     together as peers, ensuring that the
 *                                     work items are processed sequentially.
 *
 * \retval  0       Successful.  The request has been queued.
 * \retval  EAGAIN  The queue is full.  Try again later.
 */
__INLINE__
int CommAgent_AllocateWorkRequest (
                CommAgent_Control_t       control,
                CommAgent_WorkRequest_t **workRequestPtrAddress,
		uint64_t                 *uniqueID )
{
  uint32_t tid = Kernel_ProcessorThreadID();

  CommAgent_ControlInfo_t *info =
    (CommAgent_ControlInfo_t*)control.pOpaqueObject;

  TRACE((stderr,"%s() [%s:%d]: InfoPtr=%p, tid=%u\n",__FUNCTION__,__FILE__,__LINE__,info,tid));

  /* Do a Load-Increment-Bounded touch of the producerCounter.
   * This will return the current value of the counter, which
   * is the index into the queue array of the next slot in the
   * queue, and increment the counter.  If the queue is full,
   * the COMM_AGENT_L2_ATOMIC_QUEUE_FULL value is returned
   * indicating this.
   */
  uint64_t index = 0;
  if ( ( index = *(info->producerCounter[tid]) ) != 
       COMM_AGENT_L2_ATOMIC_QUEUE_FULL )
    {
      /* Return this index as the unique ID of this work request */
      *uniqueID = index;

      /* Massage the producerCounter value (index) so it is within the bounds
       * of the queue */
      index &= (COMM_AGENT_REQUEST_QUEUE_SIZE - 1);

      /* Return the address of that queue slot */
      CommAgent_WorkRequest_t *workRequestPtr;
      workRequestPtr = (CommAgent_WorkRequest_t*)&info->agentShmPtr->queue[index];

      *workRequestPtrAddress = workRequestPtr;

      /* Wait until we see that the "type" has been cleared by the agent
       * from the previous request in this same queue slot. */
      CommAgent_InternalWorkRequest_t *workRequestPtr2 = (CommAgent_InternalWorkRequest_t*)workRequestPtr;
      while ( workRequestPtr2->type != COMM_AGENT_WORK_REQUEST_TYPE_NONE );

      TRACE((stderr,"%s() [%s:%d]: At %p, returning pointer %p to request queue slot %lu (global slot %lu)\n",__FUNCTION__,__FILE__,__LINE__,workRequestPtrAddress,*workRequestPtrAddress,index,*uniqueID));
    }
  else 
    {
      TRACE((stderr,"%s() [%s:%d]: Returning EAGAIN.\n",__FUNCTION__,__FILE__,__LINE__));
      return EAGAIN; /* The queue is full */
    }

  return 0;
}


/**
 * \brief Get Comm Agent Reception Fifo Id
 *
 * This function returns the global reception FIFO Id used by the comm agent.
 * It should be called after CommAgent_Init() successfully completes.
 * The return value is undefined otherwise.
 *
 * \param[in]  control              The control structure initialized by
 *                                  CommAgent_Init().
 *
 * \retval  recFifoId  The reception fifo Id.
 */
__INLINE__
uint32_t CommAgent_GetRecFifoId ( CommAgent_Control_t control )
{
  CommAgent_ControlInfo_t *info =
    (CommAgent_ControlInfo_t*)control.pOpaqueObject;

  return ( info->agentShmPtr->globalRecFifoId );
}


/**
 * \brief Get Comm Agent's Version Number
 *
 * This function can be called after CommAgent_Init() to get the version number of
 * the comm agent.
 *
 *
 * \param[in]  control              The control structure initialized by
 *                                  CommAgent_Init().
 *
 * \retval  version  The version number of the comm agent.
 */
__INLINE__
CommAgent_State_t CommAgent_GetVersion ( CommAgent_Control_t control )
{
  CommAgent_ControlInfo_t *info =
    (CommAgent_ControlInfo_t*)control.pOpaqueObject;

  return ( info->agentShmPtr->agentState );
}


#undef TRACE

#endif
