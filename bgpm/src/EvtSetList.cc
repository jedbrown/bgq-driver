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

#include "EvtSetList.h"
#include "Lock.h"
#include "globals.h"
#include "Debug.h"

using namespace bgpm;


#define SETS_PER_THREAD 10
#define MIN_SETS_PER_PROCESS 68 //!< (4+16+1+11+2)


// Create a static dummy event so all handle accesses will find something which can respond to operations.
static EvtSet invalidEvtSet;



EvtSetList::EvtSetList()
: pSets(NULL), numRecs(0),
  updateLock(0), readerCountLock(0), readerCount(0),
  activeSets(0), maxActiveSets(0), addCount(0), rmvCount(0), replCount(0)
{
    // Size of event set list is fixed and based on number of hardware threads in this process.
    // The fixed size is required to maintain thread safety without requiring all accesses to be
    // serialized.
    int setsPerThread = envVars.GetSetsPerThread(SETS_PER_THREAD);
    numRecs = UPCI_MAX(MIN_SETS_PER_PROCESS, setsPerThread * Kernel_ProcessorCount());

    // Allocate one extra record to allow safe debug prints of data when list is exceeded.
    // new terminates when allocating too much memory.
    pSets = new ListRec[numRecs+1];

    // init entries to start with no owner.
    for (unsigned i=0; i<numRecs+1; ++i) {
        pSets[i].ctlThdId = -1;   // make sure no owner to record.
        pSets[i].pEvtSet = &invalidEvtSet;
    }
}


//static unsigned nextHandle2Try = 0; // always get next handle experiment <<<<<<<<<<<<<<<<<<<<
int EvtSetList::AddSet(EvtSet * pEvtSet)
{
    BGPM_TRACE_L3;

    Lock guard(&updateLock);

    // find and replace 1st non-owned event set (ctlThdId < 0 means non-owned)
    unsigned hEvtSet = 0;
    //unsigned hEvtSet = nextHandle2Try;  // always get next handle experiment <<<<<<<<<<<<<<<<<<<<<<<<<
    while ((hEvtSet<numRecs) && (pSets[hEvtSet].ctlThdId >= 0)) hEvtSet++;

    #if 0
    // always get next handle experiment <<<<<<<<<<<<<<<
    if (hEvtSet >= numRecs) {
        hEvtSet = 0;
        while ((hEvtSet<numRecs) && (pSets[hEvtSet].ctlThdId >= 0)) hEvtSet++;
    }
    nextHandle2Try = hEvtSet+1;
    // <<<<<<<<<<<<<<
    #endif

    BGPM_TRACE_DATA_L2(
        fprintf(stderr, "%s" _AT_ " hEvtSet=%d, ctlThdId=%d, numRecs=%d, curEvtSet=%s, pEvtSet=%s\n", IND_STRG,
                               hEvtSet, pSets[hEvtSet].ctlThdId, numRecs,
                               (pSets[hEvtSet].pEvtSet?pSets[hEvtSet].pEvtSet->ESTypeLabel():"NULL"),
                               (pEvtSet?pEvtSet->ESTypeLabel():"NULL"))
    );


    if (hEvtSet<numRecs) {

        // Either use the passed evtset, or get a new unassigned evtset type to add to list.
        EvtSet * pNewEvtSet = pEvtSet;
        if (pNewEvtSet == NULL) pNewEvtSet = new ES_Unassigned();
        if (pNewEvtSet == NULL) return lastErr.PrintOrExit(BGPM_ENOMEM, BGPM_ERRLOC);

        // Get what's currently assigned to this set index to delete it carefully.
        EvtSet *pCurEvtSet = pSets[hEvtSet].pEvtSet;
        pSets[hEvtSet].pEvtSet = pNewEvtSet;  // assign new set.
      //  mbar();
        pSets[hEvtSet].ctlThdId = curHwThdId; // make this thread the owner
        addCount++; activeSets++; maxActiveSets = UPCI_MAX(maxActiveSets,activeSets);
        mbar(); // use mbar to insure another simultaneous reader thread would see either prior evt set or new evt set - not the deleted one.
        if ((pCurEvtSet) && (pCurEvtSet != &invalidEvtSet)) {
            pCurEvtSet->user1 = 0xAAAAAAAAAAAAAAAAULL;  // for test purposes - mark so can check if trying to use deleted set.
            mbar();
            delete pCurEvtSet;
        }
    }
    else {
        return lastErr.PrintOrExit(BGPM_EMAX_EVTSETS, BGPM_ERRLOC);
    }

    return hEvtSet;
}



void EvtSetList::ReplaceSet(int idx, EvtSet * pEvtSet)
{
    BGPM_TRACE_L3;

    Lock guard(&updateLock);

    assert((unsigned)idx<numRecs);
    assert((unsigned)pSets[idx].ctlThdId == curHwThdId);

    EvtSet *pCurEvtSet = pSets[idx].pEvtSet;
    pSets[idx].pEvtSet = pEvtSet;
    replCount++;
    mbar();  // use mbar to insure a reader would see prior evt set or new evt set - not the deleted one.

    if ((pCurEvtSet) && (pCurEvtSet != &invalidEvtSet)) {
        pCurEvtSet->user1 = 0xAAAAAAAAAAAAAAAAULL; // for test purposes - mark so can check if trying to use deleted set.
        mbar();
        delete pCurEvtSet;
    }
  //  mbar();
}



void EvtSetList::RmvSet(int idx)
{
    BGPM_TRACE_L3;

    Lock guard(&updateLock);

    assert((unsigned)idx < numRecs);
    assert((unsigned)pSets[idx].ctlThdId == curHwThdId);

    EvtSet *pCurEvtSet = pSets[idx].pEvtSet;
    BGPM_TRACE_DATA_L2(
        fprintf(stderr, "%s" _AT_ " idx=%d, pCurEvtSet=%s\n", IND_STRG, idx, pCurEvtSet->ESTypeLabel())
    );
    pSets[idx].pEvtSet = &invalidEvtSet;
    mbar();
    pSets[idx].ctlThdId = -1;  // mark so no owner of record
    rmvCount++; activeSets--;
 //   mbar();  // use mbar to insure a reader would see prior evt set or new evt set - not the deleted one.

    if ((pCurEvtSet) && (pCurEvtSet != &invalidEvtSet)) {
        pCurEvtSet->user1 = 0xAAAAAAAAAAAAAAAAULL; // for test purposes - mark so can check if trying to use deleted set.
        mbar();
        delete pCurEvtSet;
    }
}



void EvtSetList::RmvAllSets(unsigned hwThdId)
{
    BGPM_TRACE_L3;

    Lock guard(&updateLock);

    for (unsigned idx=0; idx<numRecs; ++idx) {
        if ((unsigned)pSets[idx].ctlThdId == hwThdId) {
            EvtSet *pCurEvtSet = pSets[idx].pEvtSet;
            pSets[idx].pEvtSet = &invalidEvtSet;
        //    mbar();
            pSets[idx].ctlThdId = -1;  // mark so no owner of record
            rmvCount++; activeSets--;
            mbar();  // use mbar to insure a reader would see prior evt set or new evt set - not the deleted one.

            if ((pCurEvtSet) && (pCurEvtSet != &invalidEvtSet)) {
                pCurEvtSet->user1 = 0xAAAAAAAAAAAAAAAAULL; // for test purposes - mark so can check if trying to use deleted set.
                mbar();
                delete pCurEvtSet;
            }
        }
    }
}



void EvtSetList::DumpCounts()
{
    fprintf(stderr, "EvtSetList: numRecs=%d\n", numRecs);
    fprintf(stderr, "  updateLock=0x%08x, readerCountLock=0x%08x, readerCount=%d\n", updateLock, readerCountLock, readerCount);
    fprintf(stderr, "  activeSets=%d, maxActiveSets=%d\n", activeSets, maxActiveSets);
    fprintf(stderr, "  addCount=%d, rmvCount=%d, replCount=%d\n", addCount, rmvCount, replCount);
}
