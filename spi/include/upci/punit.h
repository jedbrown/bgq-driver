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

#ifndef _UPCI_PUNIT_H_  // Prevent multiple inclusion
#define _UPCI_PUNIT_H_


//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief UPCI Punit Object
 *
 */
//@}

#include "cnk/include/Config.h"
#include "hwi/include/bqc/upc_p_hw.h"
#include "mode.h"
#include "punit_eventrecs.h"
#include "punit_reserve.h"
#include "punit_config.h"


__BEGIN_DECLS


/**
 * Structure to maintain Punit Configuration data
 */
typedef struct sUpci_Punit {
    Upci_Mode_t          mode;
    Upci_UnitStatus_t    status;
    Upci_Punit_Rsv_t     rsv;
    unsigned             unit;

    //! A single Punit only supports a maximum of 24 mode0 event counters.
    Upci_PunitEventRec_t evts[UPC_P_NUM_COUNTERS];
    unsigned             numEvts;   // total assigned events
    int                  maxEvtIdx; // max event assigned (there can be holes in evts list) (starts out -1)

    Upci_Punit_Cfg_t     cfg;

} Upci_Punit_t;



//! Upci_Punit_Dump
//! Dump object for debug purposes.
void Upci_Punit_Dump(unsigned indent, Upci_Punit_t * o);



/**
 * \brief Upci_Punit_Init
 * Maintain configuration for a thread or cores Punit (with indicate of available counter groups)
 * @param[in,out]  pPunit  object to initialize
 * @param[in]      pMode   local mode object
 * @param[in]      unitId  unit (core) identifier to configure (ignored except for mode1 / mode2)
 */
__INLINE__ void Upci_Punit_Init(Upci_Punit_t *pPunit, Upci_Mode_t *pMode, unsigned unitId)
{
    assert(pPunit);
    assert(pMode);
    assert(unitId < CONFIG_MAX_CORES);
    memset(pPunit, 0, sizeof(Upci_Punit_t));
    memcpy(&(pPunit->mode), pMode, sizeof(Upci_Mode_t));
    Upci_Punit_Reserve_Init(&(pPunit->rsv), pMode);
    pPunit->unit = unitId;
    pPunit->maxEvtIdx = -1;
    Upci_Punit_Cfg_Init(&(pPunit->cfg), pMode);
    // Upci_Punit_Dump(0, pPunit);
}



/** Upci_Punit_EventH_t
 * Event Handles are returned by the Upci_Punit_AddEvent() operation
 * (or by the Punit_GetEventHandle() operation)
 * It is used in context with the Punit to reference a configured event.
 */
typedef struct sPunit_EventH
{
    uint8_t valid;    //!< non-zero if handle is valid
    uint8_t rsv[4];   //!< reserved.
    uint8_t evtIdx;   //!< Index into Punit event list (for attribute and status lookup)
    uint8_t ctrGrp;   //!< upc_c counter group
    uint8_t ctr;      //!< upc_p and upc_c counter index

} _PACK64_ Upci_Punit_EventH_t;

#ifdef _UPCI_PUNIT_C_
const Upci_Punit_EventH_t Ucpi_InvEventHandle = {0,{0,0,0,0},0,0,0};
#else
extern const Upci_Punit_EventH_t Ucpi_InvEventHandle;
#endif

#define UPCI_VALID_HANDLE_VALUE 1



//! Upci_Punit_EventH_Dump
//! Dump object for debug purposes.
void Upci_Punit_EventH_Dump(unsigned indent, unsigned idx, Upci_Punit_EventH_t * o);



/**
 * \brief Upci_Punit_SkipCtrs
 * For debug purposes only, when the 1st Punit is empty (before adding events)
 * mask off the counters and signals already used by pSrcPunit, so that pPunit will
 * only use a unique set of different set of counters and signals.
 * This will allow two punits to be used simultaneously by the same thread,
 * to start and stop counters independently.
 * \note: results undefined if using multiplexing or overflow, and does not work
 * if counter sync is active.
 * @param[in,out]  pPunit  Punit object
 * @param[in]      a2Signals     signals retrieved from GetUsedSignalMask
 * @param[in]      upcPCtrs      counters retrieved from GetUsedCtrMask
 */
__INLINE__ void Upci_Punit_SkipCtrs(Upci_Punit_t *pPunit, uint64_t a2Signals, uint32_t upcPCtrs) {
    assert(pPunit);
    assert(pPunit->numEvts == 0);
    assert(pPunit->mode.ctrMode != UPC_CM_SYNC);
    Upci_Punit_Reserve_SkipCtrs(&(pPunit->rsv), a2Signals, upcPCtrs);
}

__INLINE__ uint64_t Upci_Punit_GetUsedSignalMask(Upci_Punit_t *pPunit) {
    assert(pPunit);
    return Upci_Punit_Reserve_GetUsedSignalMask(&(pPunit->rsv));
}
__INLINE__ uint32_t Upci_Punit_GetUsedCtrMask(Upci_Punit_t *pPunit) {
    assert(pPunit);
    return Upci_Punit_Reserve_GetUsedCtrMask(&(pPunit->rsv));
}



/**
 * \brief Upci_Punit_AddEventAtIdx
 *
 * Add (reserve) an event for a particular Punit but at fixed array index within Punit.
 * This method is used to keep the array index consistent with event arrays maintained by
 * the caller (e.g Bgpm) so the caller can add events in the an efficient order different than
 * requested by the user.
 * @note - do not mix calls to Upci_Punit_AddEvent and Upci_Punit_AddEventAtIdx, or evtIdx=-1 with
 * specific indices - you will likely see unexpected results.
 * @param[in,out]  pPunit  Punit object
 * @param[in]      evtId   event Id number
 * @param[in]      thdId   thread identifier [0-3] (use 0 if thread neutral)
 * @param[in]      evtIdx  incoming array index. [-1] or [0-23] allowed, but typically shouldn't
 *                         exceed 6 or 12.  -1 means use next available index.
 * @param[in]      bkRefIdx optionally caller's provides an index value into it's own array for the event.
 *                         this will be used to for the given event.  Caller gives value into it's own array,
 *                         to use as a back reference for a given event. Used to point back to overflowing
 *                         events.
 * @param[in]      affGrpMask mask of threaded counter groups available to use (equiv to
 *                         a thread mask - left aligned).  If no bits are set, then all counter
 *                         groups are considered available.
 * @param[out]     phEvt   returned Handle to event
 * @return         0 if reservation successful, an UPCI_SERR_* error otherwise.
 *                 assert if attempt to assign index which is already in use.
 */
int Upci_Punit_AddEventAtIdx(Upci_Punit_t *pPunit, Upci_EventIds evtId, unsigned thdId, int evtIdx, int bkRefIdx,
                             uint8_t affGrpMask, Upci_Punit_EventH_t *phEvt);



/**
 * \brief Upci_Punit_AddEvent
 * Add (reserve) an event for a particular Punit.
 *
 * Punit events should be added in the following order to achieve the most efficient simultaneous
 * allocation of counters.  Subunits (l1p, xu, etc) have different counter restrictions, so
 * we want to assign the most restrictive types before assigning the most flexible.
 * - L1p/Wakeup events
 * - A2 XU events
 * - Other A2 events
 * - Opcode events (incl cycle count)
 *
 * You can assign a different number of simultaneous events from the various subunit up to the
 * total simultaneous counters possible.  It depends on the number of hw threads being counted:
 * - If 4 threads per core:
 *   - 6 simultaneous counters available per thread
 *   - 6 possible L1p/wakeup events.
 *   - 2 A2 events per subunit
 *   - 6 possible opcode counting events.
 * - If 2 threads per core:
 *   - 12 simultaneous counters available per thread
 *   - 6 L1p/wakeup events
 *   - 4 A2 events per subunit.
 *   - 12 opcode events
 * - If 1 thread per core
 *   - 24 simultaneous counters available
 *   - 6 L1p/wakeup events
 *   - 4 A2 events per subunit
 *   - 24 opcode events.
 *
 * @note No arbitration exists for "core-shared" events (\ref bgpm_events).
 *
 * @note do not mix calls to Upci_Punit_AddEvent and Upci_Punit_AddEventAtIdx,
 * you will likely see unexpected results.
 *
 * @note if using overflows, you will need to use Upci_AddEventAtIdx instead to track bkRefIdx
 *
 * @param[in,out]  pPunit  Punit object
 * @param[in]      evtId   event Id number
 * @param[in]      thdId   thread identifier [0-3]
 * @param[out]     phEvt   returned Handle to event
 *
 * @return         0 if reservation successful, an UPCI_SERR_* error otherwise.
 */
__INLINE__ int Upci_Punit_AddEvent(Upci_Punit_t *pPunit, Upci_EventIds evtId, unsigned thdId,
                                   Upci_Punit_EventH_t *phEvt)
{
    int rc = Upci_Punit_AddEventAtIdx(pPunit, evtId, thdId, -1, -1, 0, phEvt);
    //printf(_AT_ " hEvt=0x%016lx\n", *((uint64_t*)phEvt));
    return rc;
}



/**
 * \brief Upci_Punit_GetEventH
 * return handle for a configured event.
 * Event Handles along with a Punit object are used to identify the event to operate upon.
 * If the value returned by Upci_Punit_AddEvent is not retained, this method will retrieve a new handle.
 * @param[in,out]  pPunit  Punit object
 * @param[in]      evtIdx  event Id number
 * @param[out]     phEvt   returned Handle to event
 * @return         0 if reservation successful, an UPCI_SERR_* error otherwise.
 */
__INLINE__ int Upci_Punit_GetEventH(Upci_Punit_t *pPunit, unsigned evtIdx, Upci_Punit_EventH_t *phEvt)
{
    assert(pPunit);
    assert(phEvt);
    int rc = 0;
    if (evtIdx < pPunit->numEvts) {
        Upci_Punit_EventH_t hEvt = Ucpi_InvEventHandle;
        Upci_PunitEventRec_t *curEvtRec = &(pPunit->evts[evtIdx]);
        hEvt.valid = UPCI_VALID_HANDLE_VALUE;
        hEvt.evtIdx = pPunit->numEvts;
        hEvt.ctr = curEvtRec->p_Ctr;
        hEvt.ctrGrp = curEvtRec->c_Cgrp;
        *phEvt = hEvt;
    }
    else {
        *phEvt = Ucpi_InvEventHandle;
        rc = UPCI_EINV_EVT_IDX;
    }
    return rc;
}



/**
 * \brief Upci_Punit_GetBkRefIdx
 *
 * Return the back reference index recorded when the event was added.
 *
 * @param[in,out]  pPunit  Punit object
 * @param[in]      evtIdx  event list index
 * @return         bkRefIdx0
 */
__INLINE__ int Upci_Punit_GetCkRefIdx(Upci_Punit_t *pPunit, int evtIdx) {
    return pPunit->cfg.ctrBkRefIdx[pPunit->evts[evtIdx].p_Ctr];
}





/**
 * \brief Upci_Punit_EnablePMInt
 * Set to enable PM Interrupts
 * Must occur prior to Upci_Punit_Apply operation.
 * Only enables big, upc_p and upc_p shared interrupts as necessary.
 * UPC_C interrupts are not managed by the punit.
 * @param[in]  pPunit  Punit object
 */
__INLINE__ void Upci_Punit_EnablePMInt(Upci_Punit_t *pPunit)
{
    assert(pPunit);
    pPunit->cfg.enableInts = UpciTrue;
}



/**
 * \brief Upci_Punit_DisablePMInt
 * Set to enable PM Interrupts
 * Must occur prior to Upci_Punit_Apply operation.
 * Only disables big, upc_p and upc_p shared interrupts as necessary.
 * UPC_C interrupts are not managed by the punit.
 * @param[in]  pPunit  Punit object
 */
__INLINE__ void Upci_Punit_DisablePMInt(Upci_Punit_t *pPunit)
{
    assert(pPunit);
    pPunit->cfg.enableInts = UpciFalse;
}






/**
 * \brief Upci_Punit_Apply
 * Apply the UPC config to the hardware.
 * Note: Apply implicitely starts counter values off with the values captured by the last Upci_Punit_Snapshot()
 *       values.   Thus, multiple applies without resetting counters may cause counts to be inaccurate.
 *       If you never do a Upci_Punit_Snapshot(), than counts will be reset on each apply.
 * @param[in]  pPunit  Punit object
 * @return     UPCI_SERR_* error
 */
int Upci_Punit_Apply(Upci_Punit_t *pPunit);



/**
 * \brief Upci_Punit_Attach
 * Attach the UPC config to the hardware without int or context switch support
 * @param[in]  pPunit  Punit object
 * @return         UPCI_SERR_* error
 */
int Upci_Punit_Attach(Upci_Punit_t *pPunit);



/**
 * \brief Upci_Punit_UnApply
 * Make sure punit counters are stopped and detatched from kernel
 * @param[in]  pPunit  Punit object
 * @return         UPCI_SERR_* error
 */
int Upci_Punit_UnApply(Upci_Punit_t *pPunit);



/**
 * \brief Upci_Punit_UnAttach
 * Make sure punit counters are stopped and detatched from kernel
 * @param[in]  pPunit  Punit object
 * @return         UPCI_SERR_* error
 */
__INLINE__ int Upci_Punit_UnAttach(Upci_Punit_t *pPunit) { return Upci_Punit_UnApply(pPunit); }


/**
 * \brief Counter Start Options
 */
typedef enum eUpci_ControlOpts {
    UPCI_CTL_DEF    = 0x00, //!< Default Threaded counters (always affected)
    UPCI_CTL_RESET  = 0x01, //!< Reset Counters before start (slow)
    //UPCI_CTL_SHARED = 0x02, //!< Start/Stop Shared Counters along with the threaded
    UPCI_CTL_OVF    = 0x04, //!< Start/Stop Ovf counters along with threaded counters
    UPCI_CTL_DELAY  = 0x08, //!< Don't return from the operation till counters state is stable
                            //!<   after sync start, sync stop, or normal stop
} Upci_Control_Opts_t;



/**
 * \brief Upci_Punit_Event_SetThreshold
 * Set Ovf threshold value for given event.
 * Setting the threshold also makes this an overflow counter
 * Ovf counters also require the \ref UPCI_CTL_OVF control
 * options on start, stop or reset
 *
 * @param[in]  pPunit     Punit object
 * @param[in]  evtIdx     punit event index
 * @param[in]  threshold  set from UPCI_PERIOD2THRES(#evts between interrupts)
 * @return     0 if successful,
 * - \ref UPCI_EINV_EVT_IDX Index is too large for the punit.
 */
int Upci_Punit_Event_SetThreshold(Upci_Punit_t *pPunit, unsigned evtIdx, uint64_t threshold);
#define UPCI_PERIOD2THRES(_period_) (~(((uint64_t)_period_) - 1))
#define UPCI_THRES2PERIOD(_thres_)  (~(((uint64_t)_thres_) - 1))



/**
 * \brief Upci_Punit_Event_GetThreshold
 * Return threshold value assigned to event.
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  idx     punit event index
 * @return     threshold, 0 if non assigned or problem with handle.
 */
__INLINE__ uint64_t Upci_Punit_Event_GetThreshold(Upci_Punit_t *pPunit, unsigned idx)
{
    assert(pPunit);

    unsigned ctr = pPunit->evts[idx].p_Ctr;
    uint64_t threshold = pPunit->cfg.ctrOvfThres[ctr];

    return threshold;
}



/**
 * \brief Upci_Punit_GetIntMask
 * Return Interrupt status mask which would match this event if overflowed
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  idx     punit event index
 * @return     int status mask which matches this event counter
 */
__INLINE__ uint64_t Upci_Punit_GetIntMask(Upci_Punit_t *pPunit, unsigned idx)
{
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    return (UPC_P__INT_STATUS__COUNTER_INT_bit(pPunit->evts[idx].p_Ctr));
}



/**
 * Event Attribute Get/Set Methods
 * Values set and retrieved with UPC_P register alignment.
 */
__INLINE__ void Upci_Punit_Set_OpcodeGrpMask(Upci_Punit_t *pPunit, unsigned idx, uint64_t mask) {
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    mask &= UPC_P__COUNTER_CFG__XU_OPGROUP_SEL_set(~0ULL) | UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_set(~0ULL);
    unsigned ctr = pPunit->evts[idx].p_Ctr;
    pPunit->cfg.ctrCfg[ctr] |= mask;
}
__INLINE__ uint64_t Upci_Punit_Get_OpcodeGrpMask(Upci_Punit_t *pPunit, unsigned idx) {
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    unsigned ctr = pPunit->evts[idx].p_Ctr;
    uint64_t mask = pPunit->cfg.ctrCfg[ctr];
    mask &= UPC_P__COUNTER_CFG__XU_OPGROUP_SEL_set(~0ULL) | UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_set(~0ULL);
    return mask;
}
__INLINE__ void Upci_Punit_Set_OpsNotFlops(Upci_Punit_t *pPunit, unsigned idx, UpciBool_t countOps) {
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    unsigned ctr = pPunit->evts[idx].p_Ctr;
    //fprintf(stderr, _AT_ " ctr=%d, countOps=%d\n", ctr, countOps);
    UPC_P__COUNTER_CFG__OPS_NOT_FLOPS_insert(pPunit->cfg.ctrCfg[ctr], (uint64_t)countOps);
}
__INLINE__ UpciBool_t Upci_Punit_Get_OpsNotFlops(Upci_Punit_t *pPunit, unsigned idx) {
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    unsigned ctr = pPunit->evts[idx].p_Ctr;
    if (UPC_P__COUNTER_CFG__OPS_NOT_FLOPS_get(pPunit->cfg.ctrCfg[ctr])) return UpciTrue;
    return UpciFalse;
}
// edge = UpciTrue to count edges, otherwise count cycles.
// returns UpciFalse if not settable for this event.
__INLINE__ UpciBool_t Upci_Punit_Set_Edge(Upci_Punit_t *pPunit, unsigned idx, UpciBool_t edge) {
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    Upci_PunitEventRec_t *pEvtRec = &(pPunit->evts[idx]);
    ushort sigVal = pEvtRec->a2_Sig;
    if (pEvtRec->pEvent->evtSrc == UPC_ES_L1P_BASE) sigVal = pEvtRec->hwThread;
    UpciBool_t ok = Upci_Punit_Cfg_Edge_Masks(&(pPunit->cfg), sigVal, pEvtRec->pEvent, edge);
    return ok;
}
__INLINE__ UpciBool_t Upci_Punit_Get_Edge(Upci_Punit_t *pPunit, unsigned idx) {
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    Upci_PunitEventRec_t *pEvtRec = &(pPunit->evts[idx]);
    ushort sigVal = pEvtRec->a2_Sig;
    if (pEvtRec->pEvent->evtSrc == UPC_ES_L1P_BASE) sigVal = pEvtRec->hwThread;
    UpciBool_t edge = Upci_Punit_Cfg_GetEdgeValue(&(pPunit->cfg), sigVal, pEvtRec->pEvent);
    return edge;
}
// invert = UpciTrue to invert signal
// returns UpciFalse if not settable for this event (A2 only)
__INLINE__ UpciBool_t Upci_Punit_Set_Invert(Upci_Punit_t *pPunit, unsigned idx, UpciBool_t invert) {
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    Upci_PunitEventRec_t *pEvtRec = &(pPunit->evts[idx]);
    UpciBool_t ok = Upci_Punit_Cfg_Invert_Masks(&(pPunit->cfg), pEvtRec->a2_Sig, pEvtRec->pEvent, invert);
    return ok;
}
__INLINE__ UpciBool_t Upci_Punit_Get_Invert(Upci_Punit_t *pPunit, unsigned idx) {
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    Upci_PunitEventRec_t *pEvtRec = &(pPunit->evts[idx]);
    UpciBool_t invert = Upci_Punit_Cfg_GetInvertValue(&(pPunit->cfg), pEvtRec->a2_Sig, pEvtRec->pEvent);
    return invert;
}





/**
 * \brief Upci_Punit_Event_Read
 * Read the current counter value for a configured event
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  hEvt    punit event handle
 */
__INLINE__ uint64_t Upci_Punit_Event_Read(Upci_Punit_t *pPunit, Upci_Punit_EventH_t hEvt)
{
    assert(pPunit);
    assert(hEvt.valid);

    //printf("hEvt=0x%016lx\n", *((uint64_t*)(&hEvt)));

    Upci_Punit_Cfg_t *pCfg = &(pPunit->cfg);
    uint64_t result = upc_c->data24.grp[hEvt.ctrGrp].counter[hEvt.ctr];
    uint64_t thres = pCfg->ctrOvfThres[hEvt.ctr];
    if (UNLIKELY(thres)) {
        if (result > thres) result -= thres;
        result += pCfg->ctrOvfAccum[hEvt.ctr];
    }

    return result;
}




/**
 * \brief Upci_Punit_Event_ReadIdx
 * Read the counter at the given index
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  idx     punit event index
 */
__INLINE__ uint64_t Upci_Punit_Event_ReadIdx(Upci_Punit_t *pPunit, unsigned idx)
{
    assert(pPunit);
    assert(idx < pPunit->numEvts);

    Upci_PunitEventRec_t *pEvtRec = &(pPunit->evts[idx]);
    Upci_Punit_Cfg_t *pCfg = &(pPunit->cfg);
    uint64_t thres = pCfg->ctrOvfThres[pEvtRec->p_Ctr];
    uint64_t result = upc_c->data24.grp[pEvtRec->c_Cgrp].counter[pEvtRec->c_Ctr];
    if (UNLIKELY(thres)) {
        if (result >= thres) result -= thres;
        result += pCfg->ctrOvfAccum[pEvtRec->p_Ctr];
    }

    return result;
}




/**
 * \brief Upci_Punit_Event_ReadSnapShot
 * Read the counter snapshot at the given index
 * Use for punits which are not currently applied and for which a prior snapshot was taken.
 * Caller needs to know the status of the punit - the "applied" indication is the punit is unreliable
 * when multiplexing and would induce performance penalties on all reads.
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  idx     punit event index
 */
__INLINE__ uint64_t Upci_Punit_Event_ReadSnapShot(Upci_Punit_t *pPunit, unsigned idx)
{
    assert(pPunit);
    assert(idx < pPunit->numEvts);

    Upci_PunitEventRec_t *pEvtRec = &(pPunit->evts[idx]);
    Upci_Punit_Cfg_t *pCfg = &(pPunit->cfg);
    unsigned ctr = pEvtRec->p_Ctr;
    uint64_t thres = pCfg->ctrOvfThres[ctr];
    uint64_t result = pCfg->ctrSS[ctr];
    if (UNLIKELY(thres)) {
        if (result >= thres) result -= thres;
        result += pCfg->ctrOvfAccum[ctr];
    }

    return result;
}




/**
 * \brief Upci_Punit_Event_Write
 * Write the current counter value for a configured event
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  hEvt    punit event handle
 * @param[in]  value   value to write.
 */
void Upci_Punit_Event_Write(Upci_Punit_t *pPunit, Upci_Punit_EventH_t hEvt, uint64_t value);



/**
 * \brief Upci_Punit_Event_WriteIdx
 * Write the current counter value for a configured event
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  idx     event index within punit
 * @param[in]  value   value to write.
 */
void Upci_Punit_Event_WriteIdx(Upci_Punit_t *pPunit, unsigned idx, uint64_t value);



/**
 * \brief Upci_Punit_Event_WriteSnapShot
 * Write the current counter snapshot value for a configured event
 * Use for punits which are not currently applied.
 * The snapshot value will be loaded when the punit is applied.
 * Caller needs to know the status of the punit - the "applied" indication is the punit is unreliable
 * when multiplexing and would induce performance penalties on all operations.
  *
 * @param[in]  pPunit  Punit object
 * @param[in]  idx     event index within punit
 * @param[in]  value   value to write.
 */
void Upci_Punit_Event_WriteSnapShot(Upci_Punit_t *pPunit, unsigned idx, uint64_t value);




/**
 * \brief Upci_Punit_AccumResetThresholds
 * Accumulate the value for counters represented by intMask which are part if this punit
 * and reset the threshold value back to the start
 * No stop or delays - this always assumes upc_p counters are frozen.
 * @param[in]  pPunit  Punit object
 * @param[in]  intMask upcp counter int mask (may have counters on core not part of this punit)
 * @return  1 if intMask contains overflow status counters this punit
 *          0 if none of the bits in intMask pertain to this punit.
 */
int Upci_Punit_AccumResetThresholds(Upci_Punit_t *pPunit, uint64_t intMask);



/**
 * \brief Upci_Punit_GetOvfBackRefs
 * Return the event backref values which match the interrupt status bits for this punit
 * @param[in]  pPunit  Punit object
 * @param[in]  intMask upcp counter int mask (may include counters on core not part of this punit, which will be ignored)
 * @param[in,out]  *pIndicies  user allocated array to receive output indicies
 * @param[in,out]  *pLen       Input array length / output number of filled indicies
 * @return 0 if ok, -1 if pLen is not long enough for all the overflowing events.
 */
int Upci_Punit_GetOvfBackRefs(Upci_Punit_t *pPunit, uint64_t intMask, unsigned *pIndicies, unsigned *pLen);



/**
 * \brief Upci_Punit_StopN_Snapshot_Counts
 * Stop counting and Snapshot the current counter values.
 * This exists to allow capturing the values prior to applying a different
 * punit to the same counters.  Then when reapplying the counters, the
 * snapshotted values would be restored.
 * The hardware counters are not reset by this method.
 *
 * @param[in]  pPunit  Punit object
 *
 */
__INLINE__ void Upci_Punit_StopN_Snapshot_Counts(Upci_Punit_t *pPunit)
{
    Upci_Punit_Cfg_StopN_Snapshot_Counts(&(pPunit->cfg));
}



/**
 * \brief Upci_Punit_Reset_Counts
 * Reset the configured events
 * @param[in]  pPunit  Punit object
 * @param[in]  opt     option flags (mask from Upci_Control_Opts_t values)
 *
 */
void Upci_Punit_Reset_Stopped_Counters(Upci_Punit_t *pPunit, uint32_t opt);



/**
 * \brief Upci_Punit_Reset_Counts
 * Reset the configured events
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  opt     option flags (mask from Upci_Control_Opts_t values)
 *
 */
void Upci_Punit_Reset_Counts(Upci_Punit_t *pPunit, uint32_t opt);



/**
 * \brief Upci_Punit_Reset_Counts
 * Reset the configured event snapshot values only.
 * Use for punits which are not actively applied to the hardware.
 *
 * @param[in]  pPunit  Punit object
 *
 */
void Upci_Punit_Reset_SnapShots(Upci_Punit_t *pPunit);


/**
 * \brief Upci_Punit_GetEventLabelIdx
 * Return the name attribute for a configured event at index
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  idx     punit event index
 * @return  char *
 * - NULL if event not configured.
 */
__INLINE__ const char * Upci_Punit_GetEventLabelIdx(Upci_Punit_t *pPunit, unsigned idx)
{
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    Upci_PunitEventRec_t *pEvtRec = &(pPunit->evts[idx]);
    assert(pEvtRec);

    if (pEvtRec->pEvent) {
        return (pEvtRec->pEvent->label);
    }
    return NULL;
}



/**
 * \brief Upci_Punit_GetEventIdIdx
 * Return the event ID attribute for a configured event at index
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  idx     punit event index
 * @return  char *
 * - NULL if event not configured.
 */
__INLINE__ int Upci_Punit_GetEventIdIdx(Upci_Punit_t *pPunit, unsigned idx)
{
    assert(pPunit);
    assert(idx < pPunit->numEvts);
    Upci_PunitEventRec_t *pEvtRec = &(pPunit->evts[idx]);
    assert(pEvtRec);

    if (pEvtRec->pEvent) {
        return (pEvtRec->pEvent->evtId);
    }
    return 0;
}



/**
 * \brief Upci_Punit_GetEventLabel
 * Return the name attribute for a configured event.
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  hEvt    punit event handle
 * @return  char *
 * - NULL if event not configured.
 */
__INLINE__ const char * Upci_Punit_GetEventLabel(Upci_Punit_t *pPunit, Upci_Punit_EventH_t hEvt)
{
    assert(pPunit);
    assert(hEvt.valid);
    if (pPunit->evts[hEvt.evtIdx].pEvent) {
        return (pPunit->evts[hEvt.evtIdx].pEvent->label);
    }
    return NULL;
}




/**
 * \brief Upci_Punit_Start
 * Start Punit counters running
 * This is a heaver weight start with various options.
 * \note
 * - Readable counter values lag about 800 cycles from the real counter.
 * - when upc_c units are set to sync counter starts/stops,
 *   it will take another 800 cycles before they actually start counting.
 * - Synchronized start/stop effects all Punits along with L2 and I/O Units.
 *   Thus, if the counter mode (UPC_Ctr_Mode_t) is set for synchronized counting,
 *   start/stop of any punit will start/stop all, and combined reset is not available.
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  opt     option flags (mask from Upci_Control_Opts_t values)
 */
__INLINE__ void Upci_Punit_Start(Upci_Punit_t *pPunit, uint32_t opt)
{
    //assert(pPunit);

    if (UNLIKELY(opt & UPCI_CTL_RESET)) {
        Upci_Punit_Reset_Counts(pPunit, opt);
    }

    if (UNLIKELY(pPunit->mode.ctrMode == UPC_CM_SYNC)) {
        UPC_C_Start_Sync_Counting();
        if (opt & UPCI_CTL_DELAY) { Upci_Delay(800); }
    }
    else {
        //uint64_t ctlMask = 0;
        Upci_Punit_Cfg_t *cfg = &(pPunit->cfg);
        //if (opt & UPCI_CTL_SHARED) { ctlMask |= cfg->sharedCtrMask; }
        uint64_t ctlMask = cfg->thdCtrMask;
        if (UNLIKELY(opt & UPCI_CTL_OVF)) { ctlMask |= cfg->ovfCtrMask; }
        upc_p_mmio_t *const upc_p = UPC_P_Addr(pPunit->unit);
        upc_p->control_w1s = ctlMask;
        //printf("start ctlMask=0x%016lx\n", ctlMask);
        mbar();
    }
    pPunit->status |= UPCI_UnitStatus_Running;
}



/**
 * \brief Upci_Punit_Start_Sync
 * Start Punit counters running hw sync mode
 * - Does not support Reset.
 * - Does not insure that we are actually running in hw sync counter mode
 * - Synchronized start/stop effects all Punits along with L2 and I/O Units.
 *   Thus, if the counter mode (UPC_Ctr_Mode_t) is set for synchronized counting,
 *   start/stop of any punit will start/stop all, and combined reset is not available.
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  opt     option flags (mask from Upci_Control_Opts_t values)
 */
__INLINE__ void Upci_Punit_Start_Sync(Upci_Punit_t *pPunit, uint32_t opt)
{
    //assert(pPunit);
    //assert((opt & UPCI_CTL_RESET) == 0);

    pPunit->status |= UPCI_UnitStatus_Running;
    // place decision outside of start to slightly reduce counted event overhead
    if (opt & UPCI_CTL_DELAY) {
        UPC_C_Start_Sync_Counting();
        Upci_Delay(800);
    }
    else {
        UPC_C_Start_Sync_Counting();
    }
}



/**
 * \brief Upci_Punit_Start_Indep
 * Start Punit counters independently
 * - Does not support Reset.
 * - Does not insure that we are actually running in independent control mode
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  opt     option flags (mask from Upci_Control_Opts_t values)
 */
__INLINE__ void Upci_Punit_Start_Indep(Upci_Punit_t *pPunit, uint32_t opt)
{
    //assert(pPunit);
    //assert((opt & UPCI_CTL_RESET) == 0);

    Upci_Punit_Cfg_t *cfg = &(pPunit->cfg);
    uint64_t ctlMask = cfg->thdCtrMask;
    //if (opt & UPCI_CTL_SHARED) { ctlMask |= cfg->sharedCtrMask; }
    if (UNLIKELY(opt & UPCI_CTL_OVF))    { ctlMask |= cfg->ovfCtrMask; }

    upc_p_mmio_t *const upc_p = UPC_P_Addr(pPunit->unit);
    pPunit->status |= UPCI_UnitStatus_Running;
    upc_p->control_w1s = ctlMask;
    mbar();
}







/**
 * \brief Upci_Punit_Stop
 * Stop Punit counters
 * \note
 * - Readable counter values lag about 800 cycles from the real counter.
 * - when upc_c units are set to sync counter starts/stops,
 *   it will take 1600 cycles before they are ready to read
 * - Synchronized start/stop effects all Punits along with L2 and I/O Units.
 *   Thus, if the counter mode (UPC_Ctr_Mode_t) is set for synchronized counting,
 *   start/stop of any punit will start/stop all, and a combined reset is not available.
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  opt     option flags (mask from Upci_Control_Opts_t values, reset is ignored)
 */
__INLINE__ void Upci_Punit_Stop(Upci_Punit_t *pPunit, uint32_t opt)
{
    //assert(pPunit);
    if (UNLIKELY(pPunit->mode.ctrMode == UPC_CM_SYNC)) {
        UPC_C_Stop_Sync_Counting();
        if (opt & UPCI_CTL_DELAY) { Upci_Delay(1600); }
    }
    else {
        //uint64_t ctlMask = 0;
        Upci_Punit_Cfg_t *cfg = &(pPunit->cfg);
        //if (opt & UPCI_CTL_SHARED) { ctlMask |= cfg->sharedCtrMask; }
        uint64_t ctlMask = cfg->thdCtrMask;
        if (UNLIKELY(opt & UPCI_CTL_OVF)) { ctlMask |= cfg->ovfCtrMask; }
        upc_p_mmio_t *const upc_p = UPC_P_Addr(pPunit->unit);
        upc_p->control_w1c = ctlMask;
        mbar();
        if (opt & UPCI_CTL_DELAY) { Upci_Delay(800); }
    }
    pPunit->status &= ~UPCI_UnitStatus_Running;

}



/**
 * \brief Upci_Punit_Stop_Sync
 * Stop Punit counters running hw sync mode
 * - Does not insure that we are actually running in hw sync counter mode
 * - Synchronized start/stop effects all Punits along with L2 and I/O Units.
 *   Thus, if the counter mode (UPC_Ctr_Mode_t) is set for synchronized counting,
 *   start/stop of any punit will start/stop all, and combined reset is not available.
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  opt     option flags (mask from Upci_Control_Opts_t values)
 */
__INLINE__ void Upci_Punit_Stop_Sync(Upci_Punit_t *pPunit, uint32_t opt)
{
    //assert(pPunit);

    UPC_C_Stop_Sync_Counting();
    if (opt & UPCI_CTL_DELAY) { Upci_Delay(1600); }
    pPunit->status &= ~UPCI_UnitStatus_Running;
}



/**
 * \brief Upci_Punit_Stop_Indep
 * Stop Punit counters independently
 * \note
 * - Readable counter values lag about 800 cycles from the real counter.
 *
 * @param[in]  pPunit  Punit object
 * @param[in]  opt     option flags (mask from Upci_Control_Opts_t values, reset is ignored)
 */
__INLINE__ void Upci_Punit_Stop_Indep(Upci_Punit_t *pPunit, uint32_t opt)
{
    //assert(pPunit);

    Upci_Punit_Cfg_t *cfg = &(pPunit->cfg);
    uint64_t ctlMask = cfg->thdCtrMask;
    //if (opt & UPCI_CTL_SHARED) { ctlMask |= cfg->sharedCtrMask; }
    if (UNLIKELY(opt & UPCI_CTL_OVF)) { ctlMask |= cfg->ovfCtrMask; }

    upc_p_mmio_t *const upc_p = UPC_P_Addr(pPunit->unit);
    upc_p->control_w1c = ctlMask;
    mbar();
    if (opt & UPCI_CTL_DELAY) { Upci_Delay(800); }
    pPunit->status &= ~UPCI_UnitStatus_Running;
}



/**
 * \brief Upci_Punit_GetLLCtlMask
 * Return counter control mask
 * @param[in]  pPunit  Punit object
 * @param[in]  opt     option flags (mask from Upci_Control_Opts_t values, reset is ignored)
 * @return     punits ctr control mask.  Only works if punit is applied and for current thread.
 */
typedef uint64_t Upci_Ctr_CtlMask_t;
Upci_Ctr_CtlMask_t Upci_Punit_GetLLCtlMask(Upci_Punit_t *pPunit, uint32_t opt);



/**
 * \brief Upci_Punit_LLResetStart
 * Reset and start upc_p counter.
 * - Only works when ctr mode is \ref UPC_CM_DISCON.
 * - Only works when mask is for current hw thread
 * @param[in]  ctlMask
 */
__INLINE__ void Upci_Punit_LLResetStart(Upci_Ctr_CtlMask_t ctlMask)
{
    upc_p_local->control_w1s = (ctlMask << 32) | ctlMask;
    mbar();
}



/**
 * \brief Upci_Punit_LLStart
 * start upc_p counters
 * - Only works in ctr modes \ref UPC_CM_DISCON and \ref UPC_CM_INDEP
 * - Only works when mask is for current hw thread
 * @param[in]  ctlMask
 */
__INLINE__ void Upci_Punit_LLStart(Upci_Ctr_CtlMask_t ctlMask)
{
    upc_p_local->control_w1s = ctlMask;
    mbar();
}



/**
 * \brief Upci_Punit_LLStop
 * stop upc_p counters
 * - Only works in ctr modes \ref UPC_CM_DISCON and \ref UPC_CM_INDEP
 * - Only works when mask is for current hw thread
  * @param[in]  ctlMask
 */
__INLINE__ void Upci_Punit_LLStop(Upci_Ctr_CtlMask_t ctlMask)
{
    upc_p_local->control_w1c = ctlMask;
    mbar();
}



/**
 * \brief Upci_Punit_LLCCtr_Read
 * Low Level Read of 64bit counter indicated in event handle
 *
 * @param[in]  hEvt    punit event handle
 * @return     64 bit counter value. Only valid when punit is for current thread and in ctr mode \ref UPC_CM_INDEP
 */
__INLINE__ uint64_t Upci_Punit_LLCCtr_Read(Upci_Punit_EventH_t hEvt)
{
    assert(hEvt.valid);
    return upc_c->data24.grp[hEvt.ctrGrp].counter[hEvt.ctr];
}



/**
 * \brief Upci_Punit_LLPCtr_Read
 * Return Low Level 14 bit upc_p  from event handle
 *
 * @param[in]  hEvt    punit event handle
 * @return     14 bit counter value. Only reliable when punit is for current thread and in ctr mode \ref UPC_CM_DISCON
 *             Use Upci_Punit_LLPCtr_Ovf() to determine if counter value has overflowed.
 */
__INLINE__ uint64_t Upci_Punit_LLPCtr_Read(Upci_Punit_EventH_t hEvt)
{
    assert(hEvt.valid);
    return upc_p_local->counter[hEvt.ctr];
}



/**
 * \brief Upci_Punit_LLPCtr_Write
 * Write Low Level 14 bit upc_p from event handle
 *
 * @param[in]  hEvt    punit event handle
 * @param[in]  val     value to write (only least significant 14 bits are written)
 * @return     14 bit counter value. Only reliable when punit is for current thread and in ctr mode \ref UPC_CM_DISCON
 */
__INLINE__ void Upci_Punit_LLPCtr_Write(Upci_Punit_EventH_t hEvt, uint64_t val)
{
    assert(hEvt.valid);
    upc_p_local->counter[hEvt.ctr] = UPC_P__COUNTER__COUNT_set(val);
    mbar();
}




/**
 * \brief Upci_Punit_LLPCtr_Ovf
 * Test if passed upc_p 14 bit counter value has overflowed
 *
 * @param[in]  val    counter value returned from Upci_Punit_LLCtr_Read()
 * @return     nonzero if overflow occurred
 */
__INLINE__ uint64_t Upci_Punit_LLPCtr_Ovf(uint64_t val)
{
    return (val & UPC_P__COUNTER__BIG_OVF_set(1UL));
}







__END_DECLS

#endif
