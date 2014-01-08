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


/**
 * \file rgetpacing.c
 *
 * \brief C File containing Comm Agent Remote Get Pacing Implementation
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <hwi/include/bqc/MU_Addressing.h>
#include <hwi/include/bqc/MU_Fifo.h>
#include <hwi/include/bqc/A2_core.h>
#include <spi/include/mu/Addressing.h>
#include <spi/include/mu/Addressing_inlines.h>
#include <spi/include/kernel/memory.h>
#include <spi/include/kernel/location.h>
#include <spi/include/l2/atomic.h>
#include <spi/include/mu/Descriptor.h>
#include <spi/include/mu/Descriptor_inlines.h>
#include <agents/include/comm/commagent.h>
#include <agents/include/comm/commagent_impl.h>
#include <agents/include/comm/rgetpacing.h>
#include "rgetpacing_internal.h"
#include "commagent_internal.h"


#ifdef TRACE
#undef TRACE
#endif
#define TRACE(x) //fprintf x

#ifdef TRACE2
#undef TRACE2
#endif
#define TRACE2(x) //fprintf x


/** 
 * \brief Work Item ID
 * 
 * A unique ID associated with an rget work item.  All work items 
 * with this same ID are processed sequentially.
 */
typedef union workItemID
{
  struct
  {
    uint16_t globalInjFifo;
    uint16_t globalRgetFifo;
    uint32_t destCoords;
  }  __attribute__((__packed__)) components;

  uint64_t ID;
} workItemID_t;


/**
 * \brief Work Item
 *
 * remainingInjectLength
 *   The remaining number of bytes to be injected by this remote get.  This is
 *   initially set to the full message length in the payload descriptor.  It is
 *   decremented by the sub-message size each time a sub-message is injected.
 * remainingCompletionLength
 *   The remaining number of bytes to be completed on the network.  This is
 *   initially set to the full message length in the payload descriptor.  It is
 *   decremented by the sub-message size each time a sub-message is completed.
 * nextPtr, prevPtr
 *   Active work items are on a circular doubly linked list so that new work
 *   items can be inserted before any given work item.  Inactive work items
 *   are kept on a last-in-first-out singly-linked free list so their storage
 *   can be reused, minimizing the number of mallocs needed.
 * nextPeerPtr, prevPeerPtr
 *   This is a list whose head is on the "active work items" list (see nextPtr, prevPtr).
 *   Work items on this list all have the same ID and must be processed sequentially.
 *   For example, the peer of the work item for the first rget payload descriptor is 
 *   the work item for the second rget payload descriptor.
 *
 * descNum
 *   This is the descriptor sequence number returned when the rget descriptor was
 *   injected.  The work item cannot be removed/reused until the rget descriptor
 *   has been injected, otherwise the payload descriptor could be reused for another
 *   work item before it is sent.
 *
 * Note: This work item should be a multiple of 32B so it is aligned 
 *       to allow optimized copying of the descriptors.
 */
typedef struct workItem
{
  MUHWI_Descriptor_t  rgetDescriptor;
  MUHWI_Descriptor_t  payloadDescriptor;
  uint64_t            remainingInjectLength;
  uint64_t            remainingCompletionLength;
  struct workItem    *nextPtr;
  struct workItem    *prevPtr;
  struct workItem    *nextPeerPtr;
  struct workItem    *prevPeerPtr;
  uint64_t            paceIt         : 1;
  uint64_t            ensureComplete : 1;
  uint64_t            padFlags       : 62;
  uint64_t            peerID;
  workItemID_t        ID;
  uint64_t            descNum;
  uint64_t            unused[2];
} __attribute__((__packed__)) workItem_t;

/**
 * \brief Global variables
 *
 */
static uint64_t *_counterAtomicOffsets;
static uint64_t _bytesWaitingToInject; /* Number of bytes waiting to be injected into the network */

static int _numInitRequests=0; /* Number of rget init requests seen. */

static workItem_t  _dummyWorkItem;
static workItem_t *_nextWorkItemToProcess       = &_dummyWorkItem;
static workItem_t *_freeWorkItemList            = NULL;
static workItem_t *_ensureWorkItemList          = NULL;
static workItem_t **_counterWorkItems; /* Array of pointers to work items that are associated with
					* the sub-message counters */

/* We need rget payload buffers, one for each active rget.  The payload (containing descriptors)
 * is copied from the work item payload into the buffer associated with the counter being used.
 * This payload buffer remains intact until the rget completes.  Whereas, the payload in the
 * work item may be updated for the next sub-message and copied into another payload buffer.
 */
static char **_counterPayload; /* Array of pointers to payload buffers associated with each
				* sub-message counter */
static uint64_t *_counterPayloadPA; /* Array of physical addresses of payload buffers associated with each
				     * sub-message counter */
static int _numNonDPutWorkItems=0;
static int _numActiveCounters=0;
static int _numActivePacedCounters=0;
static int _isDD1; /* 0 = Not DD1 hardware
		    * 1 = DD1 hardware */


/**
 * \brief Return whether to pace this work item.
 */
inline
unsigned int paceWorkItem ( workItem_t *workItem )
{
  /* If no rgets are to be paced, say no. */
  if ( _paceRgets == 0 ) return 0;

  return 1;
}


/**
 * \brief Return whether this is a direct put work item
 */
inline
unsigned int isDPutWorkItem ( workItem_t *workItem )
{
  if ( (workItem->payloadDescriptor.PacketHeader.NetworkHeader.pt2pt.Byte8.Byte8 & 0xc0) ==
       MUHWI_PACKET_TYPE_PUT )
    return 1;
  else
    return 0;
}


/**
 * \brief Return whether this is an active work item or it is waiting to become active (on peer list).
 */
inline
unsigned int active ( workItem_t *workItem )
{
  if ( workItem->nextPtr )
    return 1;
  else
    return 0;
}


/**
 * \brief Set Up the SubMessage Payload Buffers
 */
static
int setupPayloadBuffers ()
{
  int size;
  void *memory;
  int i, rc;

  /* Allocate space for the array of pointers to payload buffers, one buffer for each counter */
  size = _numCounters*sizeof(char*);
  rc = posix_memalign ( &memory, 8, size );
  if (rc) 
	{
	  fprintf(stderr,"Remote Get Pacing: Failed to allocate heap for payload buffer pointers.\n");
	  assert( rc == 0 );
	}
  _counterPayload = (char **)memory;

  /* Allocate space for the array of physical addresses of the payload buffers, one buffer for each counter */
  size = _numCounters*sizeof(uint64_t);
  rc = posix_memalign ( &memory, 8, size );
  if (rc) 
	{
	  fprintf(stderr,"Remote Get Pacing: Failed to allocate heap for payload buffer physical addresses.\n");
	  assert( rc == 0 );
	}
  _counterPayloadPA = (uint64_t *)memory;

  /* Allocate a payload buffer associated with each counter.  Each buffer is large enough to hold
   * 1 descriptor.  Also store the physical address of each payload buffer.
   */
  for ( i=0; i<_numCounters; i++ )
    {
      size = sizeof(MUHWI_Descriptor_t);
      rc = posix_memalign ( &memory, 32, size);
      if (rc) 
	{
	  fprintf(stderr,"Remote Get Pacing: Failed to allocate heap for payload buffers.\n");
	  assert( rc == 0 );
	}
      _counterPayload[i] = (char *)memory;
  
      /* Get the physical address of the payload buffer */
      Kernel_MemoryRegion_t memRegion;
      rc = Kernel_CreateMemoryRegion (&memRegion, 
				      memory,
				      sizeof(MUHWI_Descriptor_t) );
      assert ( rc == 0 );
      _counterPayloadPA[i] = ((uint64_t)memory - (uint64_t)memRegion.BaseVa) + (uint64_t)memRegion.BasePa;
    }

  return 0;
}


/**
 * \brief Set Up the SubMessage Reception Counters
 */
static
int setupRecCounters ()
{
  int size;
  void *memory;
  int i, rc;

  /* Initialize the counter values to zero */
  size = _numCounters * sizeof(uint64_t);
  memory = (void*)_agentShmPtr->receptionCounters;
  memset ( memory, 0x00, size );

  /* Initialize an array of atomic offsets corresponding to each of the counters.
   * The appropriate atomic offsets will be placed into the payload descriptor
   * before each rget is injected so the correct counter gets decremented.
   */
  rc = posix_memalign ( &memory, 8, size );
  if (rc) 
    {
      fprintf(stderr,"Remote Get Pacing: Failed to allocate heap for atomic counter offsets\n");
      assert( rc == 0 );
    }
  _counterAtomicOffsets = (uint64_t*)memory;
  
  /* Get the physical address of the first counter */
  Kernel_MemoryRegion_t memRegion;
  rc = Kernel_CreateMemoryRegion (&memRegion, 
				  (void*)_agentShmPtr->receptionCounters, 
				  size);
  assert ( rc == 0 );
  uint64_t PA;
  PA = ((uint64_t)_agentShmPtr->receptionCounters - (uint64_t)memRegion.BaseVa) + (uint64_t)memRegion.BasePa;

  for ( i=0; i<_numCounters; i++, PA+=sizeof(uint64_t) )
    {
      /* Get the physical address of each counter and convert it to
       * an atomic address with a store-add-coherence-on-zero
       * opcode so when the MU decrements it, it won't wake us up
       * until it hits zero.
       */
      _counterAtomicOffsets[i] =
	MUSPI_GetAtomicOffsetFromBaseAddress (
					      &_batSubgroup,
					      _batId,
					      PA,
					      MUHWI_ATOMIC_OPCODE_STORE_ADD_COHERENCE_ON_ZERO);
    }

  /* Initialize an array of pointers to work items associated with each submessage counter */
  size = _numCounters * sizeof(workItem_t *);
  rc = posix_memalign ( &memory, 8, size );
  if (rc) 
    {
      fprintf(stderr,"Remote Get Pacing: Failed to allocate heap for counter work item pointers\n");
      assert( rc == 0 );
    }
  memset ( memory, 0x00, size );
  _counterWorkItems = ( workItem_t **)memory;


  return 0;
}


#if 0
/**
 * \brief Add to Reception Counter
 *
 * Do an atomic add to an L2 counter.  This allows the agent
 * to decrement the app's counter.
 */
static
void add2RecCounter ( uint64_t value )
{
/*   L2_AtomicStoreAdd( &_recCounter, */
/* 		     value ); */
}
#endif


/**
 * \brief Initialize
 */
static
int init ()
{
  int rc = 0;

  rc = setupRecCounters ();
  if (rc) return rc;

  rc = setupPayloadBuffers ();
  if (rc) return rc;

  memset( &_dummyWorkItem, 0x00, sizeof(_dummyWorkItem) );
  _dummyWorkItem.nextPtr = &_dummyWorkItem;
  _dummyWorkItem.prevPtr = &_dummyWorkItem;
  _dummyWorkItem.ID.ID   = 0xFFFFFFFFFFFFFFFFULL; /* Make this so it doesn't match any valid ID */

  /* Determine if this is running on DD1 hardware */
  uint32_t pvr; /* Processor version register */
  rc = Kernel_GetPVR( &pvr );
  assert(rc==0);
  if ( pvr == SPRN_PVR_DD1 ) 
    _isDD1 = 1;
  else
    _isDD1 = 0;
  TRACE((stderr,"%s() [%s:%d]: Running on DD1 hardware = %d\n",__FUNCTION__,__FILE__,__LINE__,_isDD1));

  return rc;
}


/**
 * \brief Remove A Work Item from the Work List
 *
 * Put the removed work item on the free list.
 */
static
void removeWorkItem ( workItem_t *workItem )
{
  workItem_t *next = workItem->nextPtr;
  workItem_t *prev = workItem->prevPtr;

  /* If there is a peer work item, move it from the peer list to the active list */
  if ( workItem->nextPeerPtr != workItem )
    {
      workItem_t *peer;
      peer = workItem->nextPeerPtr;
      peer->prevPtr = prev;
      peer->nextPtr = next;
      peer->prevPeerPtr = workItem->prevPeerPtr;
      workItem->prevPeerPtr->nextPeerPtr = peer;
      prev->nextPtr = peer;
      next->prevPtr = peer;
      if ( isDPutWorkItem ( peer ) )
	_bytesWaitingToInject += peer->remainingInjectLength;
      else
	_numNonDPutWorkItems++;
    }
  else
    {
      next->prevPtr = prev;
      prev->nextPtr = next;
    }

  /* If the work item we just removed is the next work item to process, move
   * the pointer along.
   */
  if ( _nextWorkItemToProcess == workItem ) _nextWorkItemToProcess = prev->nextPtr;

  /* If the work item requires that we check to ensure it has been completed from the
   * injection fifo, check it for completion.  If it is not complete yet, put it on 
   * the "ensure" list.
   */
  if ( workItem->ensureComplete )
  {
    if ( MUSPI_CheckDescComplete ( _ififo,
                                   workItem->descNum ) == 0 )
    {
      /* Not complete yet.  Put on ensure list */
      if ( _ensureWorkItemList ) _ensureWorkItemList->prevPtr = workItem;
      workItem->nextPtr            = _ensureWorkItemList;
      workItem->prevPtr            = NULL;
      _ensureWorkItemList          = workItem;
      
      return; /* Don't put it on the free list yet */
    }
  }
  /* Put the removed work item on the free list. */
  workItem->nextPtr  = _freeWorkItemList;
  _freeWorkItemList  = workItem;
}


/**
 * \brief Add A Work Item to the Work List
 *
 * Insert a new work item in the list before the _nextWorkItemToProcess.
 * Return the pointer to the work item.
 */
static
workItem_t *addWorkItem ( workItemID_t ID,
			  uint64_t     peerID )
{
  workItem_t *next, *prev, *peer, *mainItem, *match=NULL, *matchOnPeer=NULL, *matchOnID=NULL;

  /* Obtain storage for the work item.
   * - If there is a work item on the free list, use it.
   * - Otherwise, malloc a chunk of work items and put them on the free list.
   */
  workItem_t *workItem;
  if ( _freeWorkItemList == NULL )
    {
      void *memory;
      int rc, size, i;

      size = COMM_AGENT_REMOTE_GET_PACING_NUM_WORK_ITEMS_IN_HEAP_BLOCK * sizeof(workItem_t);
      rc = posix_memalign ( &memory, 64, size );
      if (rc) 
	{
	  fprintf(stderr,"Remote Get Pacing: Failed to allocate heap for work items\n");
	  assert( rc == 0 );
	}
      _freeWorkItemList = memory;

      /* Link the items together */
      for ( i=0; i<COMM_AGENT_REMOTE_GET_PACING_NUM_WORK_ITEMS_IN_HEAP_BLOCK; i++ )
	{
	  _freeWorkItemList[i].prevPtr = NULL; /* Not used, since it is a singly-linked free list */
	  _freeWorkItemList[i].nextPtr = &_freeWorkItemList[i+1];
	}
      _freeWorkItemList[COMM_AGENT_REMOTE_GET_PACING_NUM_WORK_ITEMS_IN_HEAP_BLOCK-1].nextPtr = NULL;
    }

  workItem          = _freeWorkItemList;
  _freeWorkItemList = _freeWorkItemList->nextPtr;
  memset( workItem, 0x00, sizeof(*workItem) );

  /* Scan the work item list to see if there is another work item with the same ID or peerID.
   * The match on peerID is the strongest, so need to keep scanning until we get a match on
   * peerID or we have scanned the whole list.
   */
  mainItem = _dummyWorkItem.nextPtr; /* This is the item on the main portion of the list. */
  /* Scan the main work item list */
  while ( ( matchOnPeer == NULL ) && ( mainItem != &_dummyWorkItem ) )
    {
      peer = mainItem;
      do {
        /* Both peer IDs are not zeros (they are set) AND they match */
        if ( (peer->peerID != 0) && (peerID != 0) && (peer->peerID == peerID) )
	  {
	    matchOnPeer = mainItem; /* We save 'main' at the time of the match */
	  }
        /* Check for match on ID */
	if ( peer->ID.ID == ID.ID )
	  {
	    if ( matchOnID==NULL ) matchOnID = mainItem; /* We save 'main' at the time of the match */
	  }
	peer = peer->nextPeerPtr;
      } while ( ( matchOnPeer == NULL ) && ( peer != mainItem ) );

      mainItem = mainItem->nextPtr;
    }

  if ( matchOnPeer ) match = matchOnPeer;
  else match = matchOnID;

  /* Insert the work item into the work item queue, depending on whether we found a matching ID. */
  if ( match == NULL )
    { /* No work item with matching ID was found.  Insert the work item in the active list. */
      next = _nextWorkItemToProcess;
      prev = next->prevPtr;
      workItem->nextPtr = next;
      workItem->prevPtr = prev;
      prev->nextPtr     = workItem;
      next->prevPtr     = workItem;
      workItem->nextPeerPtr = workItem; /* Make its peer list be circular to itself */
      workItem->prevPeerPtr = workItem;
    }
  else
    { /* Work item with matching ID was found.  Insert the work item at the end of the peer list */
      prev = match->prevPeerPtr;
      workItem->nextPeerPtr = match;
      workItem->prevPeerPtr = prev;
      prev->nextPeerPtr     = workItem;
      match->prevPeerPtr    = workItem;
    }
  
  return workItem;
}


/**
 * \brief Complete Work Item
 *
 * Complete a work item.  Remove it from the work item list.
 */
static
void completeWorkItem ( workItem_t *workItem )
{
  /* If this is the last sub message to be completed for this work item,
   * remove the work item from the list.  Otherwise, decrement the
   * length remaining to complete and leave the work item active.
   */
  if ( ( workItem->paceIt == 0 ) || (workItem->remainingCompletionLength <= _subRemoteGetSize ) )
    {
      TRACE((stderr,"%s() [%s:%d]: Removing work item %p\n",__FUNCTION__,__FILE__,__LINE__,workItem));
      removeWorkItem ( workItem );
    }
  else
    {
      workItem->remainingCompletionLength -= _subRemoteGetSize;
      TRACE((stderr,"%s() [%s:%d]: Work item %p still active, remaining inject length %lu, remaining completion length %lu\n",__FUNCTION__,__FILE__,__LINE__,workItem,workItem->remainingInjectLength,workItem->remainingCompletionLength));
    }

  /* \todo Perform completion notification if necessary */
}


/**
 * \brief Check the Ensure List for Work Items Waiting For Completion
 *
 * These work items have been injected, but before they are put on the free list
 * they must wait for their descriptor to leave the injection fifo (because their
 * payload is in the work item).
 */
void checkWorkItemsWaitingForCompletion()
{
  workItem_t *workItem, *nextWorkItem, *prevWorkItem;

  /* The _ensureWorkItemList is doubly linked.
   * The _freeWorkItemList   is singly linked.
   */

  workItem = _ensureWorkItemList;

  while ( workItem )
  {
    nextWorkItem = workItem->nextPtr;
    prevWorkItem = workItem->prevPtr;

    /* If it is complete, remove it from the ensure list and add it to the free list. */
    if ( MUSPI_CheckDescComplete ( _ififo,
                                   workItem->descNum ) == 1 )
    {
      if ( prevWorkItem ) prevWorkItem->nextPtr = nextWorkItem;
      if ( nextWorkItem ) nextWorkItem->prevPtr = workItem->prevPtr;
      if ( _ensureWorkItemList == workItem ) _ensureWorkItemList = nextWorkItem;

      workItem->nextPtr  = _freeWorkItemList;
      _freeWorkItemList  = workItem;
    }

    workItem = nextWorkItem;
  }
}


/**
 * \brief Inject Non-Direct-Put Messages
 *
 * Scan through the work item list starting at the _nextWorkItemToProcess, 
 * find all non-direct-put messages, and inject them.
 */
static
void injectNonDPutMessages ( )
{
  TRACE((stderr,"%s() [%s:%d]: _numNonDPutWorkItems = %d\n",__FUNCTION__,__FILE__,__LINE__,_numNonDPutWorkItems));
  workItem_t *workItem, *nextWorkItem;
  workItem = _nextWorkItemToProcess; /* Start with the next work item */

  /* If there are no non-direct-put work items, just return */
  /* Loop through the non-direct-put work items */
  while ( _numNonDPutWorkItems > 0 )
    {
      /* Get a pointer to the injection fifo descriptor slot.
       * At the same time, check to be sure there is enough space in the injection
       * fifo.  If not, just return and we will eventually be awakened when
       * there is enough space.
       */
      uint64_t result;
      void *fifoSlotPtr;
      result = MUSPI_InjFifoNextDesc ( _ififo,
				       &fifoSlotPtr );
      if ( result == (uint64_t)-1 ) return;
      
      /* If this is the dummy work item, or this is a direct-put work item, skip over it.
       * We keep doing this until we find an eligible work item.  We are guaranteed to find
       *  an eligible work item because the _numNonDPutWorkItems is non-zero.
       */
      while ( ( workItem == &_dummyWorkItem ) ||
	      ( isDPutWorkItem(workItem) ) )
	{
	  workItem = workItem->nextPtr;
	}

      nextWorkItem = workItem->nextPtr; // Remember item to start with for next iteration.
	      
      /* We found an eligible non-dput work item.
       * Copy the remote get descriptor into the injection fifo 
       * Advance the fifo tail to inject the descriptor.
       */
      memcpy ( fifoSlotPtr,
	       &workItem->rgetDescriptor,
	       sizeof(MUHWI_Descriptor_t) );
      
      TRACE((stderr,"%s() [%s:%d]: Injecting non-DPut Message for work item %p, _numNonDPutWorkItems=%d\n",__FUNCTION__,__FILE__,__LINE__,workItem,_numNonDPutWorkItems));
#ifdef TRACE
      //      MUSPI_DescriptorDumpHex( "Remote Get Descriptor", &workItem->rgetDescriptor );
      //      MUSPI_DescriptorDumpHex( "Payload Descriptor", &workItem->payloadDescriptor );
#endif
      
      /* Inject */
      workItem->descNum = MUSPI_InjFifoAdvanceDesc ( _ififo );
      workItem->ensureComplete = 1; /* Indicate that we must ensure this descriptor
                                     * is complete (from the inj fifo) before it
                                     * can be removed from the work item list.
                                     * This check is done in removeWorkItem() */

      /* Remove the work item from the list since it is done. */
      removeWorkItem ( workItem );

      /* Decrement the number of non-direct-put work items */
      _numNonDPutWorkItems--;

      /* Move to next work item */
      workItem = nextWorkItem;
    }
}


/**
 * \brief Inject the Next DPut Sub-Message
 *
 * Scan through the work item list starting at the _nextWorkItemToProcess, 
 * find an eligible direct-put work item,  and inject one sub-message for it, 
 * using the specified counter.
 * \retval  0  Informs caller that we did not inject anything so don't call us
 *             again unless a new work item is added to the list.
 * \retval  1  Informs caller to call us again even if another work item is
 *             not added to the list.  Something else may change.
 */
static
uint32_t injectNextDPutSubMessage ( uint64_t counterNum )
{
  workItem_t *workItem;

  TRACE((stderr,"%s() [%s:%d]: _bytesWaitingToInject = %lu\n",__FUNCTION__,__FILE__,__LINE__,_bytesWaitingToInject));

  /* There are no direct put work items if there is nothing to inject. */
  if ( _bytesWaitingToInject == 0 ) return 1; /* Return 1 so caller calls us again */

  /* Get a pointer to the injection fifo descriptor slot.
   * At the same time, check to be sure there is enough space in the injection
   * fifo.  If not, just return and we will eventually be awakened when
   * there is enough space.
   */
  uint64_t result;
  void *fifoSlotPtr;
  result = MUSPI_InjFifoNextDesc ( _ififo,
				   &fifoSlotPtr );
  if ( result == (uint64_t)-1 ) return 1; /* Return 1 so caller calls us again */
  
  /* If this is the dummy work item,
   * or this is a non-direct-put message, 
   * or there is nothing left to inject for this work item,
   * or the work item is paced but there are no more sub-message counters available,
   * skip over it.  We keep doing this until we find an eligible work item,
   * or exhaust the list.
   */
  workItem = _nextWorkItemToProcess;
  while ( ( workItem == &_dummyWorkItem )   ||
	  ( isDPutWorkItem(workItem) == 0 ) ||
	  ( workItem->remainingInjectLength == 0 ) ||
	  ( (workItem->paceIt) && (_numActivePacedCounters==_numSubMessageCounters ) ) )
    {
      workItem = workItem->nextPtr;

      /* If we didn't find any work item to process, return 0 so caller does not call us again. */
      if ( workItem == _nextWorkItemToProcess ) return 0;
    }

  /* Update the next work item to be the one after the one we found. */
  _nextWorkItemToProcess = workItem->nextPtr;

  /* We found an eligible work item.
   * 1. - Set the payloadDescriptor message length.
   *      a. If paced, set it to the
   *        min ( _subRemoteGetSize, work item's remaining message length ).
   *      b. If not paced, set it to the full message length.
   *    - Decrement the bytes remaining to be sent in the work item.
   *    - Decrement the overall number of bytes waiting to be sent.
   *    - Set the counter to the message length.
   * Also, if paced, increment the number of active paced counters.
   */
  uint64_t messageLength;
  if ( workItem->paceIt )
    {
      messageLength = _subRemoteGetSize;
      if ( workItem->remainingInjectLength < messageLength ) messageLength = workItem->remainingInjectLength;
      _numActivePacedCounters++;
    }
  else
    {
      messageLength = workItem->remainingInjectLength;
    }
  workItem->payloadDescriptor.Message_Length = messageLength;
  workItem->remainingInjectLength -= messageLength;
  _bytesWaitingToInject           -= messageLength;
  _agentShmPtr->receptionCounters[counterNum] = messageLength;
  _numActiveCounters++;
  
  /* 2. - Set the reception counter atomic offset for this counter. */
  workItem->payloadDescriptor.PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Counter_Offset = _counterAtomicOffsets[counterNum];

  /* Copy the rget payload descriptor into the buffer associated with the counter.
   * Set the physical address of this payload buffer into the rget descriptor.
   */
  memcpy ( _counterPayload[counterNum],
	   &workItem->payloadDescriptor,
	   sizeof(MUHWI_Descriptor_t) );
  workItem->rgetDescriptor.Pa_Payload = _counterPayloadPA[counterNum];

  /* 3. - Copy the remote get descriptor into the injection fifo 
   */
  memcpy ( fifoSlotPtr,
	   &workItem->rgetDescriptor,
	   sizeof(MUHWI_Descriptor_t) );

  TRACE((stderr,"%s() [%s:%d]: Injecting submessage for work item %p for counter %lu, remaining inject bytes %lu, remaining completion bytes %lu, active paced counters %u, active counters %u\n",__FUNCTION__,__FILE__,__LINE__,workItem,counterNum,workItem->remainingInjectLength,workItem->remainingCompletionLength,_numActivePacedCounters,_numActiveCounters));
#ifdef TRACE
  //MUSPI_DescriptorDumpHex( "Remote Get Descriptor", &workItem->rgetDescriptor );
  //MUSPI_DescriptorDumpHex( "Payload Descriptor", &workItem->payloadDescriptor );
#endif
  
  /* 4. Inject */
  MUSPI_InjFifoAdvanceDesc ( _ififo );

  /* 5. Remember this work item as being associated with this counter. */
  _counterWorkItems[counterNum] = workItem;

  /* 6. Update the payload descriptor's payload offset and receive offset for next time */
  workItem->payloadDescriptor.Pa_Payload += _subRemoteGetSize;
  uint64_t offset, LSB, MSB;
  LSB = workItem->payloadDescriptor.PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Put_Offset_LSB;
  MSB = workItem->payloadDescriptor.PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Put_Offset_MSB;
  offset = LSB | (MSB << 32);
  offset += _subRemoteGetSize;
  workItem->payloadDescriptor.PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Put_Offset_LSB = offset;
  workItem->payloadDescriptor.PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Put_Offset_MSB = offset >> 32;
  return 1;
}


/**
 * \brief Do Work
 *
 * Make one pass through the sub-message counters, processing work items when
 * counters are free.  Return a count of any progress made.
 */
void  doRemoteGetPacingWork ()
{
  uint64_t numCounters, numCountersToBeProcessed, counterNum;
  volatile uint64_t *subMessageRecCounters;

  /* Ignore if not initialized yet */
  if ( _numInitRequests == 0 ) return;

  numCounters           = _numCounters;
  subMessageRecCounters = _agentShmPtr->receptionCounters;

  TRACE2((stderr,"%s() [%s:%d]: numCounters=%lu, _numActiveCounters=%d, _numActivePacedCounters=%d, _bytesWaitingToInject=%lu, _numNonDPutWorkItems=%u\n",__FUNCTION__,__FILE__,__LINE__,numCounters, _numActiveCounters, _numActivePacedCounters, _bytesWaitingToInject, _numNonDPutWorkItems));

  /* Keep going while new work may have appeared due to completion of existing work */
  uint32_t doWorkAgain, tryToInject;
  
  do {
    doWorkAgain = 0;
    tryToInject = 1;
    numCountersToBeProcessed = _numActiveCounters;

    for ( counterNum=0; 
	  (counterNum < numCounters) && (numCountersToBeProcessed || _bytesWaitingToInject);
	  counterNum++ )
    {
      /* If the counter is zero, then
       * 1. Complete any work item associated with the counter.
       * 2. Process the next work item using this counter.
       */
      if ( subMessageRecCounters[counterNum] == 0 )
	{
	  /* Complete any work item associated with this zero counter */
	  workItem_t *workItem;
	  workItem = _counterWorkItems[counterNum];
	  if ( workItem )
	    {
	      TRACE((stderr,"%s() [%s:%d]: Counter %lu hit zero for work item %p\n",__FUNCTION__,__FILE__,__LINE__,counterNum,workItem));

	      if ( workItem->paceIt ) _numActivePacedCounters--;
	      _numActiveCounters--;
	      numCountersToBeProcessed--;

	      completeWorkItem ( workItem );
	      _counterWorkItems[counterNum] = NULL;
	      tryToInject = 1;
	    }

	  /* At this point we know there is no work item associated with this counter.
	   * If there are any work items still waiting to inject direct-put sub-messages, 
	   * go do that.
	   * "tryToInject" comes back 0 if nothing was able to be injected (e.g.
	   * there are paced work items to inject but all the sub-message counters
	   * are already active).  We don't call it next time unless a work item completes
	   * (then, another work item could become active and we need to try to inject it).
	   */
	  if ( (tryToInject) && (_bytesWaitingToInject) )
	    {
	      tryToInject = injectNextDPutSubMessage( counterNum );
	    }
	}
    }
  
    /* If there are any non-direct-put messages to be injected, go do that.
     * There is no need to wait for an available counter, since they don't
     * use counters.  This function will inject all that fit into the
     * injection fifo.
     */
    if ( _numNonDPutWorkItems > 0 )
      {
	injectNonDPutMessages();
	doWorkAgain = 1; /* Must do work again since new work items may become
			  * active during injectNonDPutMessages(). */
      }

    /* If there are any work items waiting for their descriptor to be
     * completed, go do that.  After checking, if there are still some waiting,
     * stay awake and keep processing work items and checking again.  If we
     * went to wakeup-wait, we may not wake up when it is completed.
     */
    if ( _ensureWorkItemList )
    {
      checkWorkItemsWaitingForCompletion();
      if ( _ensureWorkItemList ) doWorkAgain = 1;
    }

  } while ( doWorkAgain );
}
/*
 * \brief Setup a New Work Item
 *
 * \param[in]  workRequestPtr  Pointer to work request containing the rget
 *                             descriptor and the payload descriptor(s).
 */
void setupWorkItem( CommAgent_RemoteGetPacing_InternalWorkRequest_t *workRequestPtr )
{
  TRACE2((stderr,"%s() [%s:%d]:\n",__FUNCTION__,__FILE__,__LINE__));
  
  workItem_t *workItem;
  
  /* Determine the ID of the remote get.  All remote gets with this same ID must be
   * processed sequentially to maintain ordering semantics.  Rgets having the 
   * same ID are queued as peers here in the agent, such that subsequent
   * rgets are not issued until the previous rget completes.
   *
   * The ID is calculated to be the concatenation of the following, such that
   * requests having the same value for all three of these are sequentially 
   * processed...otherwise they can be processed in any order or in parallel...
   * 1.  Global injection fifo number where this rget would have been injected by 
   *     the caller.
   * 2.  Global rget fifo number on the destination node where the payload will be
   *     injected.  The destination node has a set of rget fifos.  We achieve the
   *     most parallelization by making each of these fifos have a unique ID so
   *     that rget requests targeted for different fifos are processed in parallel.
   * 3.  Destination node's coordinates.
   */
  workItemID_t ID;
  ID.components.globalInjFifo  = workRequestPtr->info.request.rget.globalInjFifo;
  ID.components.globalRgetFifo = workRequestPtr->info.request.rget.rgetDescriptor.PacketHeader.messageUnitHeader.Packet_Types.Remote_Get.Rget_Inj_FIFO_Id;
  ID.components.destCoords     = workRequestPtr->info.request.rget.rgetDescriptor.PacketHeader.NetworkHeader.pt2pt.Destination.Destination.Destination;

  /* Obtain a free work item and add it to the work list.
   * If there is already a request queued having the same ID or peerID, queue it
   * on the same peer queue as the matching request so it is processed after the
   * matching request.
   */
  workItem = addWorkItem( ID, 
			  workRequestPtr->info.request.rget.peerID );
  
  /* Initialize the work item. */
  
  /* 1. - Copy the rget descriptor.
   * \todo Optimize this
   */
  memcpy ( &workItem->rgetDescriptor,
	   &workRequestPtr->info.request.rget.rgetDescriptor,
	   sizeof(MUHWI_Descriptor_t) );

  /* 2. - Copy the payload descriptor. */
  memcpy ( &workItem->payloadDescriptor,
	   &workRequestPtr->info.request.rget.payloadDescriptor,
	   sizeof(MUHWI_Descriptor_t) );

  /* 3. If this is a non-dput payload descriptor, convert the payload address in the work item 
   *    to a physical address.  Put that address in the rget descriptor in the work item.
   *    Otherwise, for a dput payload descriptor, this is delayed until the moment the descriptor 
   *    is injected because the payload must be copied into a buffer associated with the
   *    counter so it remains unique (unchanged) for the duration of that dput.
   */
  if ( isDPutWorkItem(workItem) == 0 )
    {
      Kernel_MemoryRegion_t memRegion;
      uint32_t rc;
      rc = Kernel_CreateMemoryRegion (&memRegion, 
				      &workItem->payloadDescriptor, 
				      sizeof(MUHWI_Descriptor_t));
      assert ( rc == 0 );
      
      workItem->rgetDescriptor.Pa_Payload = 
	(uint64_t)memRegion.BasePa +
	((uint64_t)&workItem->payloadDescriptor - 
	 (uint64_t)memRegion.BaseVa);
    }

  /* 4. Set other fields */
  workItem->remainingInjectLength     = workItem->payloadDescriptor.Message_Length;
  workItem->remainingCompletionLength = workItem->payloadDescriptor.Message_Length;
  workItem->ID.ID  = ID.ID;
  workItem->peerID = workRequestPtr->info.request.rget.peerID;

  /* 5. Determine if we really want to pace this.  Set appropriate flag. */
  workItem->paceIt = paceWorkItem(workItem);

  /* 6. If it is a direct put, 
   *    - Increment the bytes waiting to inject. 
   *    - Set the agent's BAT ID for the counters.
   *    Otherwise, if it is on the active list, increment the number of non-dput work items.
   */
  if ( isDPutWorkItem(workItem) )
    {
      if ( active( workItem ) )
	_bytesWaitingToInject  += workItem->remainingInjectLength;

      MUSPI_SetRecCounterBaseAddressInfo( &workItem->payloadDescriptor,
					  _globalBatId,
					  0 ); /* Use 0 offset since this will be set later */
    }
  else
    {
      if ( active( workItem ) ) _numNonDPutWorkItems++;
    }

  TRACE((stderr,"%s() [%s:%d]: Added work item %p, PaceIt=%u, DPut=%u, Len=%lu, peerID=%lu, ID=0x%lx, _bytesWaitingToInject=%lu, _numNonDPutWorkItems=%u\n",__FUNCTION__,__FILE__,__LINE__,workItem,workItem->paceIt,isDPutWorkItem(workItem),workItem->remainingInjectLength,workItem->peerID,workItem->ID.ID,_bytesWaitingToInject,_numNonDPutWorkItems));
}


/**
 * \brief Process a Remote Get Pacing Work Request
 *
 * This function copies the work request from the request queue into a
 * work item and queues it onto the remote get pacing work item list.
 *
 * \param[in]  workRequestPtr  Pointer to the work request in the request queue.
 */
void processRgetpacingWorkRequest( 
       CommAgent_RemoteGetPacing_InternalWorkRequest_t *workRequestPtr )
{
  TRACE2((stderr,"%s() [%s:%d]: \n",__FUNCTION__,__FILE__,__LINE__));

  /* Create a work item */
  setupWorkItem ( workRequestPtr ); 
}


/**
 * \brief Process a Remote Get Pacing Init Work Request
 *
 * This function initializes the comm agent for handling remote get
 * work requests.
 *
 * \param[in]  workRequestPtr  Pointer to the work request in the request queue.
 */
void processRgetpacingInitWorkRequest( 
       CommAgent_RemoteGetPacingInit_WorkRequest_t *workRequestPtr )
{
  TRACE((stderr,"%s() [%s:%d]: \n",__FUNCTION__,__FILE__,__LINE__));

  /* Ignore subsequent init requests. */
  if ( _numInitRequests++ > 0 ) return;

  init ();
}


#undef TRACE
#undef TRACE2
