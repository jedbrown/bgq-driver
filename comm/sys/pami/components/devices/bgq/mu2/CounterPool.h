/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file components/devices/bgq/mu2/CounterPool.h
 * \brief Defines a context-scoped pool of MU counters.
 */

#ifndef __components_devices_bgq_mu2_CounterPool_h__
#define __components_devices_bgq_mu2_CounterPool_h__


#include <hwi/include/bqc/A2_inlines.h>
#include <spi/include/kernel/memory.h>
#include <spi/include/mu/Addressing_inlines.h>
#include "util/trace.h"
#include "components/devices/generic/Device.h"
#include "components/devices/bgq/mu2/Context.h"

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      /// \brief Counter Pool Class
      ///
      /// This class manages 64 MU reception counters.
      /// The MU::Context allocates _numCounterPools classes based on env var
      /// PAMI_NUMDYNAMICROUTING.
      /// 
      /// For sending a message, the user of this class
      /// 1. Calls allocate() to allocate a counter from this pool.
      /// 2. Calls setCounter() to set the counter to the message length and
      ///    register a completion function and cookie to be called when the
      ///    counter hits zero.
      /// 3. Calls getCounterAtomicOffset() to get the MU atomic offset
      ///    to be placed in the DPut descriptor.  These offsets are calculated
      ///    when the CounterPool is constructed and stored in an array.
      ///    The offsets are relative to a base address table entry whose value
      ///    is zero.
      ///
      /// The _freeMask contains 1 status bit per counter.  When the bit is
      /// set to 1, the counter is free, otherwise the counter is allocated.
      /// Upon the transition from all of the counters being free to 1 counter
      /// being allocated, a work item is posted to the generic device work
      /// queue.  During advance, the generic device processes this work item
      /// by calling _advanceWorkFn(), which advances this counter pool.
      /// This occurs once per advance while _advanceWorkFn() still finds there
      /// to be active counters and returns PAMI_EAGAIN.  When _advanceWorkFn()
      /// finds there to be no more active counters, it returns PAMI_SUCCESS
      /// which causes the generic device to remove the work item.
      /// This technique is used so the "normal" advance codepath is not
      /// disturbed unless there are active counters.
      ///
      /// CounterPool::advance() (called from the _advanceWorkFn() ) examines
      /// the _freeMask and checks each allocated counter to see if it hit zero.
      /// For each counter that hit zero, the registered completion function
      /// is called.
      ///
      /// To support fence operations, a registration / callback mechanism
      /// is provided as follows:
      /// 1. To start the fence, the user calls addFenceOperation().  This
      ///    registers a callback function and cookie to be invoked when the
      ///    fence is complete.  The user provides a pointer to a fenceInfo_t
      ///    structure that is queued to this CounterPool until the fence
      ///    operation is complete.  This structure is filled-in by
      ///    addFenceOperation() with the callback information and a 
      ///    snapshot of the _freeMask so it knows which counters to
      ///    monitor for completion.  The fence is complete when all of the
      ///    counters in the snapshot have hit zero.
      /// 2. During advance(), if any counters have hit zero, a mask of those
      ///    counters is passed to advanceFence().  advanceFence() goes
      ///    through the list of fenceInfo_t structures queued to this
      ///    CounterPool, updating the snapshot in each structure with the
      ///    counters that have just hit zero.  When all active counters in
      ///    the snapshot have hit zero, the fenceInfo_t structure is removed
      ///    from the list and the callback function is called.
      ///
      class CounterPool
      {
        public:

#define COUNTERPOOL_NUMCOUNTERS 64

	  typedef struct completionInfo
	  {
	    pami_event_function  fn;
	    void                *cookie;
	  } __attribute__((__packed__)) completionInfo_t;

	  typedef struct fenceInfo
	  {
	    struct fenceInfo    *nextPtr;
            struct fenceInfo    *prevPtr;
	    uint64_t             freeMaskSnapshot;
	    pami_event_function  fn;
	    void                *cookie;	    
	  } __attribute__((__packed__)) fenceInfo_t;

          ///
          /// \brief Constructor
          ///
          /// \param[in] context  Context associated with this counter pool.
          ///
          inline CounterPool ( pami_context_t   context,
                               Generic::Device *progress )
          {
            TRACE_FN_ENTER();

	    _freeMask  = 0xFFFFFFFFFFFFFFFFUL;
	    _context   = context;
            _fenceInfo = NULL;
            _progress  = progress;

	    // Initialize an array of atomic offsets corresponding to each of
	    // the counters.  The appropriate atomic offsets will be placed
	    // into the payload descriptor before each rget is injected so
	    // the correct counter gets decremented.

	    // Get the physical address of the first counter
	    Kernel_MemoryRegion_t memRegion;
	    int rc, i;
	    rc = Kernel_CreateMemoryRegion (&memRegion, 
					    (void*)_counters,
					    sizeof(_counters) );
	    assert ( rc == 0 );
	    uint64_t PA;
	    PA = ((uint64_t)_counters - (uint64_t)memRegion.BaseVa) + (uint64_t)memRegion.BasePa;

	    for ( i=0; i<COUNTERPOOL_NUMCOUNTERS; i++, PA+=sizeof(uint64_t) )
	      {
		/* Get the physical address of each counter and convert it to
		 * an atomic address with a store-add-coherence-on-zero
		 * opcode so when the MU decrements it, it won't invalidate
                 * our L1 cache until it hits zero.
		 */
		_counterOffsets[i] = (uint64_t)MUSPI_GetAtomicAddress (
					       PA,
					       MUHWI_ATOMIC_OPCODE_STORE_ADD_COHERENCE_ON_ZERO);
	      }

            TRACE_FN_EXIT();
          }

          /// \brief Add fence operation to be monitored
          ///
          /// Add info about a fence operation that will complete only after
          /// all currently active counters have hit zero.
          ///
          /// \param[in] fenceInfoPtr Pointer to an empty fenceInfo_t structure
          ///                         that will be monitored until the
          ///                         currently active counters hit zero.
          ///                         The storage for this structure must
          ///                         remain intact until the callback fn
          ///                         is invoked.
          /// \param[in] fn  Callback function to be invoked when the fence
          ///                fence is complete.
          /// \param[in] cookie The cookie to pass to the callback function.
          ///
          /// \retval 0 There are active counters to be monitored.  The 
          ///           callback fn will be called when they all hit zero.
          /// \retval -1 There are no active counters to be monitored.
          ///            The callback fn was not invoked...the caller can do that.
          ///
          inline int addFenceOperation ( fenceInfo_t         *fenceInfoPtr,
                                         pami_event_function  fn,
                                         void                *cookie )
          {
            // If there are active counters, set it up to wait for them.
            if ( _freeMask != 0xFFFFFFFFFFFFFFFFUL )
            {
              // Add the fenceInfo structure to the front of the fenceInfo list.
              fenceInfo_t *headPtr  = _fenceInfo;
              fenceInfoPtr->nextPtr = headPtr;
              fenceInfoPtr->prevPtr = NULL;
              if ( headPtr ) headPtr->prevPtr = fenceInfoPtr;
              _fenceInfo            = fenceInfoPtr;
              
              // Snapshot the active counters.
              fenceInfoPtr->freeMaskSnapshot = _freeMask;
              
              // Save the fn and cookie.
              fenceInfoPtr->fn     = fn;
              fenceInfoPtr->cookie = cookie;
              return 0;
            }
            else // There are no active counters.  Return indicator of that.
            {
              return -1;
            }
          }

          /// \brief Advance fence operations
          ///
          /// Fence operations that are waiting for counters to hit zero
          /// have a fenceInfo_t structure on the _info->fenceInfo list.
          /// This function runs that list.  For any fence operation waiting
          /// for the specified counters, it marks those counters complete for
          /// that fence operation, and when all counters are complete for that
          /// fence operation, it removes the fenceInfo_t structure and calls the
          /// fence completion operation.
          ///
          /// \param[in] counterMask A bit mask indicating which counters hit
          ///                        zero.
          ///
          inline void advanceFence ( uint64_t counterMask )
          {
            // Go through the fenceInfo list, looking for a fence operation that
            // is waiting for any of the counters specified in counterMask.
            fenceInfo_t *fenceInfo = _fenceInfo;
            while ( fenceInfo )
            {
              fenceInfo_t *nextPtr = fenceInfo->nextPtr;
              
              // If this fence operation is waiting for any of the specified
              // counters to hit zero, process this fence operation.
              if ( ~(fenceInfo->freeMaskSnapshot) & counterMask )
              {
                // Turn on (mark free) the specified counter bits in the snapshot.
                fenceInfo->freeMaskSnapshot |= counterMask;

                // If the fence operation is no longer waiting for any counters
                // to hit zero, remove the fence operation from the list
                // and invoke the fence callback function.
                if ( fenceInfo->freeMaskSnapshot == 0xFFFFFFFFFFFFFFFFUL )
                {
                  // Remove the fenceInfo_t from the list.
                  if ( fenceInfo->nextPtr )
                    fenceInfo->nextPtr->prevPtr = fenceInfo->prevPtr;
                  if ( fenceInfo->prevPtr )
                    fenceInfo->prevPtr->nextPtr = fenceInfo->nextPtr;
                  else
                    _fenceInfo = fenceInfo->nextPtr;
                  
                  // Invoke the callback function, if any.
                  if ( fenceInfo->fn )
                  {
                    fenceInfo->fn ( _context,
                                    fenceInfo->cookie,
                                    PAMI_SUCCESS );
                  }
                }
              }
              // Move to the next fenceInfo_t on the list.
              fenceInfo = nextPtr;
            }
          }

	  /// \brief Advance the counters in the pool
	  ///
	  /// \retval  0  No counters remain to be advanced
	  /// \retval  non-zero  At least 1 counter still needs to be advanced
	  ///
	  inline int advance()
	  {
	    // If all counters are free, return 0.  This is a quick exit for the common
	    // case when there is nothing to do.
	    if ( _freeMask == 0xFFFFFFFFFFFFFFFFUL )
	      return 0;

	    uint64_t counterNum   = 0;
	    uint64_t testMask     = 0x8000000000000000UL;
            uint64_t fenceMask    = 0;
	    
	    // Find an allocated counter
	    while ( ( _freeMask != 0xFFFFFFFFFFFFFFFFUL ) && // There are allocated counters &
		    ( testMask ) )                           // Haven't examined them all
	      {
		if ( ( (_freeMask & testMask) == 0) && // Counter is allocated
		     ( _counters[counterNum] == 0 ) )  // Counter hit zero
		  { // Process a completed counter by invoking the completion function.
		    pami_event_function fn = _completionInfo[counterNum].fn;
		    if ( fn )  fn ( _context,
				    _completionInfo[counterNum].cookie,
				    PAMI_SUCCESS );
		    _freeMask |= testMask;  // Mark counter free.

		    // Add this counter to the fence mask for later.
                    fenceMask |= testMask;
		  }
		testMask = testMask >> 1;
		counterNum++;
	      }
	    
            // If there are any fence operations waiting for counters to hit zero and
            // any counters have hit zero, scan the fenceInfo list for the fence
            // operations waiting for any of these counters and update the fence status,
            // eventually calling the fence callback function.
            if ( _fenceInfo && fenceMask )
            {
              advanceFence ( fenceMask );
            }

	    // If all counters are free, return 0.
	    if ( _freeMask == 0xFFFFFFFFFFFFFFFFUL )
	      return 0;
	    else
	      return 1;
	  }

          /// \brief Advance Work Function
          ///
          /// This function is called by the generic device during advance
          /// (when this function has been queued to the generic device work
          /// queue...it is only on the queue when there are counters from
          /// this CounterPool to be advanced).
          ///
          /// \param[in] ctx    This context
          /// \param[in] cookie Pointer to this CounterPool object.
          ///
          /// \retval PAMI_EAGAIN There are still counters to be advanced.
          ///                     This tells the generic device to keep this
          ///                     function queued.
          /// \retval PAMI_SUCCESS There are no more counters to be advanced.
          ///                      The generic device will dequeue this function.
          ///
          static pami_result_t _advanceWorkFn ( pami_context_t  ctx, 
                                                void           *cookie)
          {
            int rc;
            CounterPool *thus = (CounterPool *)cookie;

            rc = thus->advance();

            if ( rc == 0 )
            {
              return PAMI_SUCCESS; // No more counters need advancing.
            }
            else
            {
              return PAMI_EAGAIN;  // More counters need advancing.
            }
          }

	  /// \brief Allocate a counter
	  ///
	  /// \retval  counterNumber  The counter number allocated (0 or greater).
	  /// \retval  -1             No counters are available
	  ///
	  inline int64_t allocate ()
	  {
#if 1
	    uint64_t counterNum   = 0;
	    uint64_t testMask     = 0x8000000000000000UL;
	    uint64_t freeMask     = _freeMask;

	    // Find a free counter
	    while ( (testMask) && ( (freeMask & testMask) == 0 ) )
	      {
		testMask = testMask >> 1;
		counterNum++;
	      }

	    if ( counterNum < COUNTERPOOL_NUMCOUNTERS ) 
	      {
		// A free counter was found...allocate it.
		_freeMask &= ~(testMask); // Mark counter allocated (turn bit off).

                // If the generic device is not advancing this counter pool,
                // enqueue the advance function on the queue.  Do this by creating
                // a GenericWork object, specifying the work function to be called
                // along with the cookie (which is a pointer to this CounterPool),
                // and posting that object to the generic device.
                // It will stay on the generic device queue and get called once per
                // advance until the function returns PAMI_SUCCESS (when there are
                // no more counters to advance).
                //
                // freeMask is the value of the freeMask upon entry to this function.
                // If freeMask indicates that all counters were free, then we
                // just transitioned from all-free to needing to advance.
                if ( freeMask == 0xFFFFFFFFFFFFFFFFUL )
                {
                  PAMI::Device::Generic::GenericThread *work = new ( &_advanceWork )
                    PAMI::Device::Generic::GenericThread( _advanceWorkFn, this );
                  _progress->postThread ( work );
                }
		return counterNum;
	      }
#endif
	    return -1; // No counters are available
	  }

	  /// \brief Set the value of a counter and the callback info
	  ///
	  /// \param[in]  counterNum  Number of the counter to be set
	  /// \param[in]  value       Value to be set into the counter
	  /// \param[in]  fn          Callback function
	  /// \param[in]  cookie      Callback cookie
	  ///
	  inline void setCounter ( int64_t              counterNum,
				   uint64_t             value,
				   pami_event_function  fn,
				   void                *cookie )
	  {
	    _counters[counterNum]              = value;
	    _completionInfo[counterNum].fn     = fn;
	    _completionInfo[counterNum].cookie = cookie;
	  }

	  /// \brief Get Counter Atomic Offset
	  ///
	  /// \param[in]  counterNum  Number of the counter
	  ///
	  /// \retval  counterAtomicOffset  The MU atomic offset of the
	  ///                               counter, assuming a zero base.
	  ///
	  inline uint64_t getCounterAtomicOffset ( int64_t counterNum )
	  {
	    return _counterOffsets[counterNum];
	  }

        protected:

          uint64_t         _freeMask;      // 1 bit per counter.
                                           // - 0 means counter is allocated (not free)
                                           // - 1 means counter is free.
          pami_context_t   _context;       // Context this pool is associated with.
          fenceInfo_t     *_fenceInfo;     // List of pending fence operations.
          Generic::Device *_progress;      // Pointer to generic device.
          uint64_t         _counters[COUNTERPOOL_NUMCOUNTERS];
          completionInfo_t _completionInfo[COUNTERPOOL_NUMCOUNTERS]; // Completion info for each counter.
          uint64_t         _counterOffsets[COUNTERPOOL_NUMCOUNTERS]; // MU Atomic Physical Address for each counter.
          PAMI::Device::Generic::GenericThread _advanceWork; // Used to advance.

      }; // class     PAMI::Device::MU::CounterPool
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI


#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_CounterPool_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
