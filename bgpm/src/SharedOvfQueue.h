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

#ifndef _SHARED_OVF_QUEUE_H_  // Prevent multiple inclusion
#define _SHARED_OVF_QUEUE_H_

#include <assert.h>
#include "spi/include/upci/upc_atomic.h"

//! \addtogroup bgpm_api_impl
//@{
/**
 * \file
 *
 * \brief queue of overflow statuses and data when interrupts are broadcast
 *
 */
//@}

namespace bgpm {



//////////////////////////////////////////////////////////////////
/*!
 *
 * SharedOvfQueue to maintain overflow interrupt status which
 * PM signals need to process.   When a interrupt is broadast,
 * then the 1st signal logs the interrupt and data into this queue
 * and then resets cause of the interrupts.
 * Subsequent threads will get the status to process from this queue.
 *
 * The interrupt status and signal handler logic work together to
 * maintain the interrupt active for each individual thread till it
 * has seen the overflow in this queue, even though the original cause
 * has been cleared.
 */
#define NUM_OVFQUEUE_ENTRIES 32
struct OvfQueEntry
{
    uint64_t recId;
    uint64_t intStatus;
};


struct SharedOvfQueue
{
    UPC_Barrier_t      leaderLock;
    uint64_t           nextQueRecId;  // next record ID and also mod(%) to get position to update in queue.
    OvfQueEntry        intStatusQue[NUM_OVFQUEUE_ENTRIES];
    uint64_t           thdNextRecId[CONFIG_HWTHREADS_PER_CORE];
                                        // next record ID that a core's thread hasn't processed yet.
                                        // Must be initialized to nextQueRecID whenever
                                        // a thread starts counting.
                                        // (there can only be one counting sw thread per hw thread
                                        //  so that's ok).
    unsigned           sharedOvfDropped;

    void Init() {
        leaderLock = 0;
        sharedOvfDropped = 0;
        nextQueRecId = 1;
        for (unsigned i=0; i<CONFIG_HWTHREADS_PER_CORE; ++i) thdNextRecId[i] = 1;
    }

    // assumes thread lock already in place
    void Enque(uint64_t intStatus) {
        unsigned pos = nextQueRecId % NUM_OVFQUEUE_ENTRIES;
        intStatusQue[pos].recId = nextQueRecId;
        intStatusQue[pos].intStatus = intStatus;
        intStatusQue[++nextQueRecId % NUM_OVFQUEUE_ENTRIES].recId = 0;  // recId == 0 indicate no more entries
    }

    // doesn't assume any locking in place - the only thing updated is data unique to the passed thd number (=current thread)
    OvfQueEntry Deque(unsigned thd) {
        unsigned pos = thdNextRecId[thd]%NUM_OVFQUEUE_ENTRIES;
        mbar();
        OvfQueEntry qe = intStatusQue[pos];
        if (qe.recId == thdNextRecId[thd]) {
            thdNextRecId[thd]++;
        }
        else if (qe.recId != 0) {
            sharedOvfDropped++;  // log that an overflow was dropped and reset the recId
            thdNextRecId[thd] = qe.recId + 1;  // reset to next entry next time.
        }
        return qe;
    }
};



}

#endif
