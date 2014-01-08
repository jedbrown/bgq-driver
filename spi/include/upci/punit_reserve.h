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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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

#ifndef _UPCI_PUNIT_RESERVE_H_  // Prevent multiple inclusion
#define _UPCI_PUNIT_RESERVE_H_

#include <assert.h>
#include <unistd.h>

//#define _UPC_TRACE_PUNIT_RESERVATIONS_

#include "cnk/include/Config.h"
#include "err.h"
#include "upc_evttbl.h"
#include "punit_eventrecs.h"
#include "mode.h"

//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief objects to track the hardware resources available and reserved by a thread
 *
 */
//@}


__BEGIN_DECLS


#if 0
//! Upci_RsvCtr_t tracks the event/signal reserved for a particular upc_p counter.
typedef struct sUpci_RsvCtr {
    Upci_EventIds   evtId;   // keep as signed value because -1 means skip.
    short           a2Sig;
} Upci_RsvCtr_t;



//! Upci_RsvCtr_Dump
//! Dump object for debug purposes.
void Upci_RsvCtr_Dump(unsigned indent, unsigned idx, Upci_RsvCtr_t * o);
#endif


//! Return the thread to counter affinity sibling thread for the given thread
//! that is - 0->1  1->0, 2->3, 3->2
__INLINE__ unsigned Upci_Rsv_SiblingThread(unsigned thd)
{
    return ((thd % 2) ? thd - 1 : thd + 1);
}





//////////////////////////////////////////////////////////////////
/*!
 *
 * Upci_Punit_Reserve_t tracks the resources which are reserved by a
 * requested events.
 *
*/
typedef struct sUpci_Punit_Reserve
{
    Upci_Mode_t        mode;       //!< copy of global mode values to help with reserve decisions
    UPC_EventSources_t l1pMode;    //!< active mode for l1p.  Value can only be of the l1p source types or undefined.
    uint8_t            affGrpMask; //!< mask of threaded upc_p counter groups in use by this punit (left bit aligned).

    //! we are assuming that all counters and signals are unique to to a thread.
    //! That is, we are not taking into account the core-shared events which can use
    //! counters belonging to another thread.  In the case of core-shared events, we
    //! assume that the caller will assign an 0xF affinity group mask to
    //! so that only one thread can use the available counters and signals.
    //! In this case, we only need to track whether a counter has been allocated,
    //! not check the event number.
    uint32_t upcpRsvCtrMask;   //!< ctr mask == UPC_P__CONTROL32_bit()

    //! a2SigEvent tracks which signals have been allocated
    //! We don't need to track events for the same reason as above.
    uint64_t a2RsvSigMask;     //!< sig mask == MASK64_bit(a2sig);
    //short a2SigEvent[UPC_P_NUM_A2_UNITS * UPC_P_NUM_A2_UNIT_SIGS ];  // keep as signed value -1 means skip

} Upci_Punit_Rsv_t;



//! Upci_Punit_Rsv_Dump
//! Dump object for debug purposes.
void Upci_Punit_Rsv_Dump(unsigned indent, Upci_Punit_Rsv_t * o);




/*!
 * init a Reserve object
 * @param[in]   pRsv       ptr to reserve object to init
 * @param[in]   pMode      ptr to local mode object
 */
__INLINE__ void Upci_Punit_Reserve_Init(Upci_Punit_Rsv_t *pRsv, Upci_Mode_t *pMode)
{
    assert(pRsv);
    memset(pRsv, 0, sizeof(Upci_Punit_Rsv_t));
    memcpy(&(pRsv->mode), pMode, sizeof(Upci_Mode_t));
    pRsv->l1pMode = UPC_ES_Undef;
    pRsv->affGrpMask = 0xF0;  // Default to attempting to assign events to all upc_p counter groups (all 24 counters)
}




/**
 * \brief Upci_Punit_Reserve_SkipCtrs
 * For debug purposes only, pre-reserve the ctrs and signals passed
 * to cause further event additions to choose a unique set of ctrs and signals.
 * @param[in,out]  pRsv          target object
 * @param[in]      a2Signals     signals retrieved from GetUsedSignalMask
 * @param[in]      upcPCtrs      counters retrieved from GetUsedCtrMask
 *
 * @note: only usable before any events added to punit
 */
void Upci_Punit_Reserve_SkipCtrs(Upci_Punit_Rsv_t *pRsv, uint64_t a2Signals, uint32_t upcPCtrs);



/**
 * \brief Upci_Punit_Reserve_GetUsedSignalMask
 * For debug purposes only
 * @param[in]  pRsv          target object
 */
uint64_t Upci_Punit_Reserve_GetUsedSignalMask(Upci_Punit_Rsv_t *pRsv);

/**
 * \brief Upci_Punit_Reserve_GetUsedCtrMask
 * For debug purposes only
 * @param[in]  pRsv          target object
 */
uint32_t Upci_Punit_Reserve_GetUsedCtrMask(Upci_Punit_Rsv_t *pRsv);



/*! Upci_Punit_ReserveEvent()
 * Validate and reserve resources for the given event.
 * populate the evtRec record
 * @param[in,out]  pRsv     ptr to reserve object
 * @param[in]      evtId    event Id number
 * @param[in,out]  pEvtRec  record to populate with assigned event/signal/counter properties
 * @param[in]      unitId   Punit number to assign (0-16)
 * @param[in]      hwThdId  Hardware thread id to assign (0-3).
 * @param[in]      affGrpMask mask of threaded counter groups available to use (equiv to
 *                         a thread mask - left aligned).  If no bits are set, then all counter
 *                         groups are considered available.
 *
 * @return       0 if reservation successful, an UPCI_SERR_* error otherwise.
 *
 */
int Upci_Punit_ReserveEvent(Upci_Punit_Rsv_t *pRsv, Upci_EventIds evtId,
                            Upci_PunitEventRec_t *pEvtRec, unsigned unitId, unsigned hwThdId,
                            uint8_t affGrpMask);



// Get masks representing all possible counters or A2 Signals given
// current affGrpMask (indicating which threaded counters reserved) and
// the given event.  Later, A2 signals to be used will be mapped from the chosen counter.
// Also used to create debug info on event addition failure.
uint32_t Upci_Punit_Reserve_PossibleCtrMask(Upci_Punit_Rsv_t *pRsv, unsigned hwThd, UPC_PunitEvtTableRec_t *pEvent);
uint8_t  Upci_Punit_Reserve_PossibleA2UnitSignalMask(uint32_t ctrMask);

// Generate thread local static strings representing possible and assigned counters or signals.  Use the sigMask and
// ctrMask to represent possible signals or counters, and a2Sig or ctr for actual signal or counter.
// If a2sig or ctr is negative, then no signal or counter is assigned.
const char *Upci_Punit_Reserve_SigMaskStrg(UPC_EventSources_t evtSrc, int a2Sig, uint8_t sigMask);
const char *Upci_Punit_Reserve_SigMaskStrgAll(UPC_EventSources_t evtSrc, uint32_t pcBusSigMask);
const char *Upci_Punit_Reserve_CtrMaskStrg(int ctr, uint32_t ctrMask);



__END_DECLS
#endif
