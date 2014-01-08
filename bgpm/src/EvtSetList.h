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

#ifndef _BGPM_EVTSETLIST_H_  // Prevent multiple inclusion
#define _BGPM_EVTSETLIST_H_

#include "spi/include/upci/upc_atomic.h"
#include "EvtSet.h"

namespace bgpm {


// Use to self document ValidSetIndex use.
enum {
    MustBeControlThread = true,
    AnyThreadOk = false
};


// Since use of the Event Set Index (handle) is so error prone, and so common,
// the following two macros exist to combine the EvtSetHazardGuard creation with the
// Handle validation operation.
// When a reader thread is used, it also requires the EvtSetHazardGuard to be created
// to protect against event set changes by the controlling thread while it's being read
// by the reader thread.   We'll use these macros to combine the operations so we don't
// make a mistake.
#define CONTROLTHREAD_VALIDATE_SET_OR_RETURN(_handle) \
   int vrc = procEvtSets.ValidSetIdx((_handle), MustBeControlThread); \
   if (UNLIKELY(vrc)) return vrc;

#define READERTHREAD_VALIDATE_SET_OR_RETURN(_handle) \
   int vrc = procEvtSets.ValidSetIdx((_handle), AnyThreadOk); \
   if (UNLIKELY(vrc)) return vrc;  \
   EvtSetHazardGuard hazardGuard((_handle));






//! EvtSetList
/*!
  Event Set List collection class
  There may be multiple instances of these, but
  at most 1 process wide instance, and 1 instance per software thread.
*/
class EvtSetList
{
    friend class EvtSetHazardGuard;
public:

    //! Record to track status of handle and maintain thread safety.
    struct ListRec {
        int       ctlThdId;     //!< Owning/controller thread for this event set.
        EvtSet *  pEvtSet;      //!< pointer to active event set.
    };


    EvtSetList();
    ~EvtSetList() {}

    //! Validate that index (event set handle) is a valid event list index.
    int ValidSetIdx(int idx, bool controlThreadOnly);

    //! Atomically add event set to list.
    //! Assumes this is a new event set - there are no other references to it - the list will gain ownership.
    //! returns error code on failure.
    //! Returns list index where added, or a negative error code.
    //! if pEvtSet == NULL, then a default "unassigned" event set will be added in prep for the real one later.
    int AddSet(EvtSet * pEvtSet);

    //! Atomically replace event set in list entry
    //! Assumes this is a new event set - there are no other references to it and the list will gain ownership.
    //! The original event set will be tossed.
    //! Only the controlling thread must call - an assert will cause termination otherwise.
    void ReplaceSet(int idx, EvtSet * pEvtSet);

    //! Remove the event set record at the given index
    void RmvSet(int idx);

    //! Remove all sets with the given controlling thread id
    void RmvAllSets(unsigned hwThdId);

    //! Return the event set pointer at the given index.
    //! WARNING:
    //! If the returned eventset can be referenced by a thread other than the controlling thread
    //! (the one who created and added it), then the caller must maintain a
    //! EvtSetHazardGuard object around use of the returned pointer to prevent
    //! the "controller" thread from deleting or changing the object before you are done using it.
    //! For example:
    //!    int rc = procEvtSets.ValidSetIdx(idx, AnyThreadOk); // if using MustBeControlThread then no need to guard
    //!    if (rc < 0) return rc;
    //!    EvtSetHazardGuard guard(hEvtSet);  // will incr the record hazardCount until it goes out of scope.
    //!    EvtSet *pEvtSet = procEvtSets[hEvtSet];
    //!    ...
    EvtSet * operator[](int idx) { return pSets[idx].pEvtSet; }

    //! Return if the current thread is the process controlling thread
    bool IsControlThread(int idx) { return pSets[idx].ctlThdId == (int)Kernel_ProcessorID(); }

    unsigned MaxSize() { return numRecs; }
    unsigned ReaderCount() { return readerCount; }
    void Dump(unsigned indent);  // debug dump of contents
    void DumpCounts();           // debug dump of counters

private:

    ListRec   *pSets;
    unsigned   numRecs;
    UPC_Lock_t updateLock;       //! serialize changes to handle list records.
    UPC_Lock_t readerCountLock;  //!< paired with readerCount to allow multiple readers with only one updater
    unsigned   readerCount;

    // Debug counts
    int activeSets;
    int maxActiveSets;
    int addCount;
    int rmvCount;
    int replCount;


    // hide these
    EvtSetList(const EvtSetList &);
    EvtSetList & operator=(const EvtSetList &);

};



//! EvtSetHazardGuard
/*!
  Upon Construction - if the caller is not the owner of the indexed record,
  it establishes a multiple reader lock which holds off further updates to the
  event set list until all readers have completed.
  The multiple reader lock is reduced when the object is destroyed.
*/
class EvtSetHazardGuard
{
public:
    EvtSetHazardGuard(int idx);
    ~EvtSetHazardGuard();
private:
    bool locked;
};



}

#endif
