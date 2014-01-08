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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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


//! \addtogroup bgpm_api_impl
//@{
/**
 * \file
 *
 * \brief include to build shared control structures
 *
 */
//@}


//////////////////////////////////////////////////////////////////
/*!
 *
 * SharedControl contains a shared implementation
 * with data and logic to track the state of
 * operations with the L2 and I/O units.
 *
 * The logic and data are almost identical, and since they must
 * be retained in shared memory, a virtual object is not applicable.
 * Thus, we'll use this header to maintain the common structure and
 * logic, and use a parent header to select the minor variations.
 *
 * The two defines needed to select the differences:
 *   SHARED_CONTROL_STRUCT_NAME
 *   SHARED_CONTROL_CTR_RANGE
 *
 * Parent must also end the structure "};" after adding any new inline methods.
*/

#ifndef _SHARED_CONTROL_C_


struct SHARED_CONTROL_STRUCT_NAME
{
    UPC_Lock_t         lock;
    int                refCount;          //!< evtSet Ref Count (num evtSets using)

    //! Track the running state of the UPC_C counters.
    //! An overflow interrupt will stop the unit from counting
    //! to mitigate pollution of the counts during processing of the overflow.
    //! This flag indicates if the counts should be running, so they can be
    //! restarted at the end of the overflow signal handler.
    bool               running;

    uint64_t           ovfMask;  //! mask of counters which have overflow active.
    uint64_t           accumVals[SHARED_CONTROL_CTR_RANGE];  // overflow accum values
    uint64_t           thresholds[SHARED_CONTROL_CTR_RANGE]; // threshold values.

    SharedOvfQueue     ovfQue;   //! Shared Queue of counter interrupt statuses.

    inline void Init() {
        memset((void*)this, 0, sizeof(SHARED_CONTROL_STRUCT_NAME));
        ovfQue.Init();
    }

    inline void IncrRef() {
        Lock guard(&lock);
        assert(refCount >= 0);
        refCount++;
    }

    inline void DecrRef() {
        Lock guard(&lock);
        assert(refCount >= 0);
        if (--refCount) {
            running = false;
            ovfMask = 0;
            ovfQue.Init();
        }
    }


    inline void SetRunning(bool run) { running = run; }
    inline bool GetRunning() { return running; }

    inline bool RsvOvfThreshold(unsigned ctr, uint64_t threshold) {
        assert(ctr < SHARED_CONTROL_CTR_RANGE);
        Lock guard(&lock);
        // check if overflow already set
        uint64_t ctrMask = MASK64_bit(ctr);
        if ((ovfMask & ctrMask) && (threshold != thresholds[ctr])) {
            return false;
        }
        thresholds[ctr] = threshold;
        ovfMask |= ctrMask;
        return true;
    }

    inline uint64_t GetOvfThreshold(unsigned ctr) {
        assert(ctr < SHARED_CONTROL_CTR_RANGE);
        return thresholds[ctr];
    }

    inline void ResetThreshold(unsigned ctr);

    inline void AccumThreshold(unsigned ctr, uint64_t curCtrVal) {
        assert(ctr < SHARED_CONTROL_CTR_RANGE);
        uint64_t delta = ~(thresholds[ctr])+1;
        delta += curCtrVal;
        accumVals[ctr] += delta;
    }


    uint64_t ResetAndGetIntMask(uint64_t intStatus);
    void     EnqueIfActiveOvf(uint64_t intStatus);
    bool     DequeAndCallOvfHandler(int hEvtSet, uint64_t ipaddr, ucontext_t *uc);

    void Dump(unsigned indent);
};


#else

// Call after LeaderLatch freed to process each entry in queue.
bool bgpm::SHARED_CONTROL_STRUCT_NAME::DequeAndCallOvfHandler(int hEvtSet, uint64_t ipaddr, ucontext_t *uc) {
    unsigned curQueLen = 0;
    unsigned intDroppedStart = ovfQue.sharedOvfDropped;
    bool ovfDetected = false;

    Bgpm_OverflowHandler_t pHandler = NULL;
    if (hEvtSet >= 0) {
        pHandler = procEvtSets[hEvtSet]->GetOverflowHandler();
    }

    unsigned thd = Kernel_ProcessorID();
    bool again;
    do {
       again = false;
       OvfQueEntry rec = ovfQue.Deque(thd);
       if (rec.recId != 0) {
           ovfDetected = true;
           curQueLen++;
           if (rec.intStatus && pHandler) {
               dbgThdVars.numUpcCOvfsDelivered++;
              (*pHandler)(hEvtSet, ipaddr, rec.intStatus, uc);
           }
           again = true;
       }
    } while (again);

    if (intDroppedStart != ovfQue.sharedOvfDropped) {
        dbgThdVars.droppedSharedUpcCOvfs++;
    }
    dbgThdVars.maxUpcCSharedQueLen = UPCI_MAX(dbgThdVars.maxUpcCSharedQueLen, curQueLen);

    return ovfDetected;
}

#endif


