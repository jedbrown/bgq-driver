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

#include "bgpm/include/bgpm.h"
#include "EvtSetList.h"
#include "CoreReservations.h"
#include "ES_HWPunit.h"
#include "ES_NWunit.h"
#include "globals.h"
#include "ES_Factory.h"



using namespace bgpm;





__BEGIN_DECLS


/* Some internal usage notes about Attach/Detach:
 *
 * In order to count events for a thread, each thread needs a separate event set object
 * to configure and control the associated events (or at least they need separate objects
 * like the upci punit structures.  The thread associate with the eventset must be known
 * at the time events are added.
 * However, we desire the user to be able to reuse the same event set configuration across
 * multiple threads Also,
 * it makes "usage" sense to be able to apply the same event configuration to multiple threads
 * in a hardware perspective mode of operation.  Event in a sw mode it could make some sense.
 * This implies that at least, the fact that the thread must be known at event set creation time
 * should be hidden from the user.
 *
 * To accomplish EvtSet reuse and hide the thread dependency,
 * while maintaining a consistent interface across sw/hw/speculative operations requires
 * that new event sets be created and passed as part of the interface, or be hidden from the
 * user.  This occurs in the following fashion
 * (at least with this 1st pass of software - this comment could
 * become stale - hopefully not).
 *
 * Note:
 * - The main process thread maintains a process wide event set list.
 * - Each thread also has it's own local list, and the main thread process list and thread list are
 *   equivalent.
 *
 * In SW Perspective Modes w/o speculation for a Punit evtset
 * - each thread normally does their own evtset creation and configure with separate controls.
 * - The thread value is encoded with the punit events and conf at Bgpm_AddEvent() time
 * - Bgpm_Apply() always assumes the event set config is consistent with the current thread.
 *
 * In a HW perspective mode for a punit event set:
 * - On Attach(), the passed event set is duplicated with a target thread value, and
 *   the new event set handle is returned.
 * - The new handle must be used for subsequent accesses.
 * - The initial event set handle remains, but is never Attach()ed.
 * - An apply of the initial handle is possible, which applies it to the current thread only
 *   if they match (fails otherwise), and there is not a set already attached.
 *
 * NW Event Set
 * - On Apply(), all links are attached and no further is possible.
 * - On Attach() the event set is duplicated and only those links are attached.
 *   Subsequent attachments are possible for available links, but controlled separately.
 *   The new event set handle is returned.
 */



int Bgpm_Attach(unsigned hEvtSet, uint64_t targMask, uint64_t agentMask)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (curHwThdId != Kernel_ProcessorID()) {
        return lastErr.PrintOrExit(BGPM_ESWTHD_MOVED, BGPM_ERRLOC);
    }

    int rc = procEvtSets[hEvtSet]->Attach(targMask, agentMask);
    return rc;
}



int Bgpm_Detach(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->Detach();
}



__END_DECLS
