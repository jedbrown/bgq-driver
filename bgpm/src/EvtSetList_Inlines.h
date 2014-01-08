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

#ifndef _BGPM_EVTSETLIST_INLINES_H_  // Prevent multiple inclusion
#define _BGPM_EVTSETLIST_INLINES_H_


namespace bgpm {


//! Include this after global vars have been defined so inline routines can find vars

//! Validate that index (event set handle) is a valid event list index.
inline int EvtSetList::ValidSetIdx(int idx, bool controlThreadOnly) {
    if (UNLIKELY(!thdInited)) { return lastErr.PrintOrExit(BGPM_ENOT_INIT, BGPM_ERRLOC); }
    if (UNLIKELY((unsigned)idx >= numRecs) || (pSets[idx].ctlThdId < 0))  {
        return lastErr.PrintOrExit(BGPM_EINV_SET_HANDLE, BGPM_ERRLOC);
    }
    unsigned ownerHwThd = (unsigned)pSets[idx].ctlThdId;
    if (UNLIKELY(controlThreadOnly && (ownerHwThd != curHwThdId))) {
        lastErr.PrintMsg("EvtSet handle=(%d), owning thread=%d, but current thread is %d\n",
                idx, ownerHwThd, curHwThdId);
        return lastErr.PrintOrExit(BGPM_ENOT_CONTROL_THREAD, BGPM_ERRLOC);
    }
    return 0;
}



inline EvtSetHazardGuard::EvtSetHazardGuard(int idx) : locked(false) {
    if ((unsigned)procEvtSets.pSets[idx].ctlThdId != curHwThdId) {
        Lock countGuard(&procEvtSets.readerCountLock);
        locked = true;
        procEvtSets.readerCount++;
        if (procEvtSets.readerCount == 1) {
            UPC_Lock(&procEvtSets.updateLock);
        }
    }
}
inline EvtSetHazardGuard::~EvtSetHazardGuard() {
    if (locked) {
        Lock countGuard(&procEvtSets.readerCountLock);
        procEvtSets.readerCount--;
        if (procEvtSets.readerCount == 0) {
            UPC_Unlock_Any(&procEvtSets.updateLock);
        }
    }
}


}

#endif
