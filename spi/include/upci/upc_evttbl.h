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

#ifndef _UPC_EVTTBL_H_
#define _UPC_EVTTBL_H_


//! \addtogroup upci_spi
//@{
/**
 *
 * \file
 *
 * \brief UPCI/UPC Event Table structures and enums.
 *
 *
 */
//@}



#include "events.h"
#include "upc_p.h"
#include "cnk/include/kcounters.h"

__BEGIN_DECLS


//! Reservation Grades
//! The reservation grade values tags events or groups of counters to indicate how they
//! share signal and counter resources with other events on the node.  This
//! grade is tracked in the event tables, event lists, the globalReserveMap, localReserveMap, and processReserveMap
//! to flag how a group of counters resources can, or have been reserved, to be used.
//! The reservation algorithms use the
//! grade to insure that two event sets can't exist which may conflict if they should
//! ever try to run simultaneously on the same core.
typedef enum ReservationGrades {
    RG_UNASSIGNED = 0,  //!< grade has not been assigned - free for any use.
    RG_THREADED   = 1,  //!< Only events which may managed independently by each thread may be assigned.
    RG_CORETHREAD = 2,  //!< Only core-wide events which require a specific counter in a group of 4
                        //!< (for a2 events with the hwthread selection overloaded to select a different core event)
    RG_THREADCOMB = 3,  //!< Thread combined to create a core-wide event
                        //!< (in punit, all 4 thread counters assigned same event, but thread 0 counter combines).
    RG_INACTIVE   = 4,  //!< indicates an event entry which is just a place holder, no counting should be done
                        //!< this event (??? not sure if this is needed, but anticipating perhaps passing
                        //!< this in a binary profile
} UPC_RsvGrade_t;



//! Source Types.
//! Include file upc_evtsrcs.h exists to allow the names, mnemonic and description
//! to be maintained together, though same header to build separate tables as needed.
//! The actual enum field is built by defining the macro to select the interesting fields and
//! and including the header.
//! Unfortunately doxygen cannot handle this game the list of values doesn't get populated in the doc.
//! You will need to lookat the header file instead.
typedef enum eUPC_EventSources {
#define UPC_EVTSRC(name, mnemonic, description) name,
#include "upc_evtsrcs.h"
    UPC_ES_NUMSRCS
} UPC_EventSources_t;



//! Event Scope
typedef enum UPC_EventScope {
    UPC_SCOPE_UNDEF = 0,
    UPC_SCOPE_THREAD = 1,   //!<  events attributable to current sw/hw thread
    UPC_SCOPE_CORE,         //!<  events are core-wide and events can't directly be attributed to single thread.
    UPC_SCOPE_CORESHARED,   //!<  events are core-wide, but also require shared counter resources (see /ref shared_counters)
    UPC_SCOPE_NODESHARED,   //!<  events are node-wide, but counter control is shared between threads
    UPC_SCOPE_NODE,         //!<  events are node-wide, but each sw thread controls independent view of counter
    UPC_SCOPE_EXCLUSIVE,    //!<  events are node-wide but with counter exclusive to a single sw thread.
    UPC_SCOPE_SAMPLED,      //!<  an event which gives an absolute sample value and cannot be started or stopped.
} UPC_EventScope_t;



//! Event Metric Type
//! Keep consistent with bgpm/events/Events-Func.xsl evt:numAbility  return values.

typedef enum UPC_EventMetric {
    UPC_METRIC_UNDEF = 0,
    UPC_METRIC_BEVENTS = 1,  //!<  counts # of events, but src capable of both events and cycles
    UPC_METRIC_BCYCLES,      //!<  counts # of cycles, but src capable of both events and cycles
    UPC_METRIC_EVENTS,       //!<  suitable only for counting # of events
    UPC_METRIC_CYCLES,       //!<  suitable only for counting # of cycles
    UPC_METRIC_SINGLE,       //!<  single cycle event - events and cycles are synonymous
    UPC_METRIC_CONSEQ,       //!<  single cycle event, but may occur on consecutive cycles - events and cycles are synonymous
} UPC_EventMetric_t;



//! Upci_GetMetricText
//! @param[in]  m   metric enum value
//! @returns    text representing event metric value
const char *Upci_GetMetricText(UPC_EventMetric_t m);



//! Event Domain
//! values defined equivalent to mask to be stored in A2 CESR "countmodes" field.
typedef enum UPC_EventDomain {
    UPC_Domain_NotDef = 0x0, //!<  Event domain is not set.
    UPC_Domain_Kernel = 0x3, //!<  Event domain is kernel (or supervisor), BG doesn't have distinction)
    UPC_Domain_User   = 0x4, //!<  Event domain is user level (problem mode)
    UPC_Domain_Any    = 0x7, //!<  Count events in any domain.
} UPC_EventDomain_t;



//! PunitEventTable Structure
struct sUPC_PunitEvtTableRec
{
    unsigned            evtId;          //!< Temporary Check #, make sure index matches looked up event id.
    const char *        label;          //!< Event id label string.
    UPC_EventSources_t  evtSrc;         //!< Event Source type - chooses allocation/select methods
    unsigned            selVal   : 32;  //!< Selection value for target selection register.

    unsigned            scope    : 4;   //!< Event Scope (hw thread or core)
    unsigned            x1Dmn    : 1;   //!< Event is an x1 domain event - sets odd/event consolidation in Mode 1
    unsigned            cycle    : 1;   //!< Cycle event rather than edge
    unsigned            invert   : 1;   //!< Invert the polarity of cycle event.
    unsigned            reserv1  : 1;
    unsigned            metric   : 4;   //!< Metric Type
    unsigned            reserv2  : 4;

    unsigned            ctrMask  : 24;   //!< mask of possible target mode 0 counters

    //! \todo Detailed Mode1 values are not currently used, and the configured values are inconsistent and incorrect.
    //! \todo Fix l1p mode1 event values - need to be masks rather than numbers.
    unsigned            t0Grp    : 12;   //!< Mode 1 thread 0 or core selection group
    unsigned            t0Ctr    : 8;    //!< Mode 1 thread 0 or core counter number
    unsigned            t1Grp    : 12;   //!< Mode 1 thread 1 selection group
    unsigned            t1Ctr    : 8;    //!< Mode 1 thread 1 counter number
    unsigned            t2Grp    : 12;   //!< Mode 1 thread 2 selection group
    unsigned            t2Ctr    : 8;    //!< Mode 1 thread 2 counter number
    unsigned            t3Grp    : 12;   //!< Mode 1 thread 3 selection group
    unsigned            t3Ctr    : 8;    //!< Mode 1 thread 3 counter number

    const char *        desc;            //!< Short Description string

};
typedef const struct sUPC_PunitEvtTableRec UPC_PunitEvtTableRec_t;



//! L2unitEventTable Structure
struct sUPC_L2unitEvtTableRec
{
    unsigned            evtId;          //!< Temporary Check #, make sure index matches looked up event id.
    const char *        label;          //!< Event id label string.
    UPC_EventSources_t  evtSrc;         //!< Event Source type - chooses allocation/select methods
    unsigned            selVal;         //!< Selection value for target selection register.
    const char *        desc;           //!< Short Description string
};
typedef const struct sUPC_L2unitEvtTableRec UPC_L2unitEvtTableRec_t;



//! IOunitEventTable Structure
struct sIOunitEvtTableRec
{
    unsigned            evtId;          //!< Temporary Check #, make sure index matches looked up event id.
    const char *        label;          //!< Event id label string.
    UPC_EventSources_t  evtSrc;         //!< Event Source type - chooses allocation/select methods
    unsigned            selVal;         //!< Selection value for target selection register.
    const char *        desc;           //!< Short Description string
};
typedef const struct sIOunitEvtTableRec UPC_IOunitEvtTableRec_t;



//! NWunitEventTable Structure
struct sNWunitEvtTableRec
{
    unsigned            evtId;          //!< Temporary Check #, make sure index matches looked up event id.
    const char *        label;          //!< Event id label string.
    UPC_EventSources_t  evtSrc;         //!< Event Source type - chooses allocation/select methods
    unsigned            selVal;         //!< Selection value for target selection register.
    unsigned            unitCtr;        //!< The nw ctr to use (1=0-3, 4=4, 5=5, 6=cycle time)
    const char *        desc;           //!< Short Description string
};
typedef const struct sNWunitEvtTableRec UPC_NWunitEvtTableRec_t;



//! CNKunitEventTable Structure
struct sCNKunitEvtTableRec
{
    unsigned            evtId;          //!< Temporary Check #, make sure index matches looked up event id.
    const char *        label;          //!< Event id label string.
    UPC_EventSources_t  evtSrc;         //!< Event Source type - chooses allocation/select methods
    unsigned            selVal;         //!< Selection value for target selection register.
    const char *        desc;           //!< Short Description string
};
typedef const struct sCNKunitEvtTableRec UPC_CNKunitEvtTableRec_t;



#if 0
//! ENVunitEventTable Structure
struct sENVunitEvtTableRec
{
    unsigned            evtId;          //!< Temporary Check #, make sure index matches looked up event id.
    const char *        label;          //!< Event id label string.
    UPC_EventSources_t  evtSrc;         //!< Event Source type - chooses allocation/select methods
    unsigned            selVal;         //!< Selection value for target selection register.
    const char *        desc;           //!< Short Description string
};
typedef const struct sENVunitEvtTableRec UPC_ENVunitEvtTableRec_t;



//! MCunitEventTable Structure
struct sMCunitEvtTableRec
{
    unsigned            evtId;          //!< Temporary Check #, make sure index matches looked up event id.
    const char *        label;          //!< Event id label string.
    UPC_EventSources_t  evtSrc;         //!< Event Source type - chooses allocation/select methods
    unsigned            selVal;         //!< Selection value for target selection register.
    const char *        desc;           //!< Short Description string
};
typedef const struct sMCunitEvtTableRec UPC_MCunitEvtTableRec_t;



//! BGPMunitEventTable Structure
struct sBGPMunitEvtTableRec
{
    unsigned            evtId;          //!< Temporary Check #, make sure index matches looked up event id.
    const char *        label;          //!< Event id label string.
    UPC_EventSources_t  evtSrc;         //!< Event Source type - chooses allocation/select methods
    unsigned            selVal;         //!< Selection value for target selection register.
    const char *        desc;           //!< Short Description string
};
typedef const struct sBGPMunitEvtTableRec UPC_BGPMunitEvtTableRec_t;
#endif


//#ifndef UPCI_EVENTS_C
extern const UPC_PunitEvtTableRec_t  punitEvtTbl[];
extern const UPC_L2unitEvtTableRec_t l2unitEvtTbl[];
extern const UPC_IOunitEvtTableRec_t IOunitEvtTbl[];
extern const UPC_NWunitEvtTableRec_t NWunitEvtTbl[];
extern const UPC_CNKunitEvtTableRec_t CNKunitEvtTbl[];
//#endif




//! Global L2 Shared Events being collected.
//! Maintained as a mask for each



/**
 * \brief Upci_GetPunitEventRec
 * Return ptr to generic event record for event id
 *
 * @param[in]  evtId
 * @return     pEvtRec
 * - null if not punit event
 */
__INLINE__ const UPC_PunitEvtTableRec_t * Upci_GetPunitEventRec(unsigned evtId)
{
    const UPC_PunitEvtTableRec_t * pTbl = NULL;
    if (evtId <= PEVT_PUNIT_LAST_EVENT) pTbl = &punitEvtTbl[evtId];
    return pTbl;
}
__INLINE__ const UPC_L2unitEvtTableRec_t * Upci_GetL2EventRec(unsigned evtId)
{
    const UPC_L2unitEvtTableRec_t * pTbl = NULL;
    if (evtId <= PEVT_PUNIT_LAST_EVENT) {}
    else if (evtId <= PEVT_L2UNIT_LAST_EVENT) pTbl = &l2unitEvtTbl[evtId-PEVT_PUNIT_LAST_EVENT];
    return pTbl;
}
__INLINE__ const UPC_IOunitEvtTableRec_t * Upci_GetIOEventRec(unsigned evtId)
{
    const UPC_IOunitEvtTableRec_t * pTbl = NULL;
    if (evtId <= PEVT_L2UNIT_LAST_EVENT) {}
    else if (evtId <= PEVT_IOUNIT_LAST_EVENT) pTbl = &IOunitEvtTbl[evtId-PEVT_L2UNIT_LAST_EVENT];
    return pTbl;
}
__INLINE__ const UPC_NWunitEvtTableRec_t * Upci_GetNWEventRec(unsigned evtId)
{
    const UPC_NWunitEvtTableRec_t * pTbl = NULL;
    if (evtId <= PEVT_IOUNIT_LAST_EVENT) {}
    else if (evtId <= PEVT_NWUNIT_LAST_EVENT) pTbl = &NWunitEvtTbl[evtId-PEVT_IOUNIT_LAST_EVENT];
    return pTbl;
}
__INLINE__ const UPC_CNKunitEvtTableRec_t * Upci_GetCNKEventRec(unsigned evtId)
{
    const UPC_CNKunitEvtTableRec_t * pTbl = NULL;
    if (evtId <= PEVT_NWUNIT_LAST_EVENT) {}
    else if (evtId <= PEVT_CNKUNIT_LAST_EVENT) pTbl = &CNKunitEvtTbl[evtId-PEVT_NWUNIT_LAST_EVENT];
    return pTbl;
}



/**
 * \brief Upci_GetEventLabel
 * Return the name attribute for an event id.
 *
 * @param[in]  evtId
 * @return  char *
 * - null or assert if not event
 */
const char * Upci_GetEventLabel(unsigned evtId);



/**
 * \brief Upci_GetEventDesc
 * Return the description attribute for an event id.
 *
 * @param[in]  evtId
 * @return  char *
 * - null or assert if not event
 */
const char * Upci_GetEventDesc(unsigned evtId);



/**
 * \brief Upci_GetEventSrc
 * Return the description attribute for an event id.
 *
 * @param[in]  evtId
 * @return     evtSrc for event
 *             0 if event id not defined (or assert)
 */
UPC_EventSources_t Upci_GetEventSrc(unsigned evtId);




//! Upci_GetEventSrcName
//! @param[in]  evtId
//! @returns    Unit Name String
//!             0 if event id not defined (or assert)
const char * Upci_GetEventSrcName(unsigned evtId);


//! Upci_IsA2Event
//! @param[in]  evtId
//! @returns    UpciTrue or UpciFalse
//!             0 if event id not defined (or assert)
__INLINE__ UpciBool_t Upci_IsA2Event(unsigned evtId) {
    UPC_EventSources_t src = Upci_GetEventSrc(evtId);
    if ((src >= UPC_ES_A2_MMU) && (src <= UPC_ES_A2_XU)) return UpciTrue;
    return UpciFalse;
}







__END_DECLS

#endif


