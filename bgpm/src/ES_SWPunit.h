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

#ifndef _BGPM_ES_SWPUNIT_H_  // Prevent multiple inclusion
#define _BGPM_ES_SWPUNIT_H_

#include "EvtSet.h"
#include "EvtInfo.h"
#include "CoreReservations.h"

namespace bgpm {


typedef std::vector<Upci_Punit_t*> PunitList;

//! ES_SWPunit
/*!
  Single Software Punit Event Set class
*/
class ES_SWPunit : public ES_Active
{
    friend class ES_HWPunit;
public:

    ES_SWPunit(unsigned targHwThdId = Kernel_ProcessorID());
    virtual ~ES_SWPunit();

    virtual ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    //virtual EvtSet *Spawn();           // create a empty sibling (another of same type)
    virtual EvtSet *Clone(uint64_t target, unsigned *addEvents, unsigned addNum);
    // Merge existing in SWPunit events with new events and src attributes.  Assumes the existing
    // event set is empty (no attribs or event added yet).
    int MergeEventCfg(const ES_SWPunit &s, unsigned targHwThdId, unsigned *addEvents, unsigned addNum);

    virtual void Dump(unsigned indent);

    virtual bool IsEventIdCompatible(int evtId);

    // AddEvent and AddEventList can return a new event set if significant changes
    // to the existing event set are required.
    // pNewEvtSet returns a pointer if a new event set was created, and the caller is
    // required to delete the existing event set and replace it with the new one.
    // If pNewEvtSet returns NULL, then the existing event set is still valid.
    virtual int AddEvent(unsigned evtId, EvtSet* & pNewEvtSet);
    virtual int AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet);
    virtual int DumpEventsInError(Upci_Punit_t *pPunit, int rc, unsigned failedEvtId); // Print punit event resources along with last one which failed.
    virtual int SetPreReserved();
    virtual int NumEvents() const;
    virtual int Apply(int hEvtSet);   // include the evtSet handle for evtset type to make global var updates
    virtual int Start();
    virtual int Stop();
    virtual int ResetStart();
    virtual int Reset();
    virtual int ReadEvent(unsigned idx, uint64_t *pVal);
    virtual int WriteEvent(unsigned idx, uint64_t val);
    virtual int Punit_GetHandles(Bgpm_Punit_Handles_t *pH);
    virtual int Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH);
    virtual uint64_t GetActiveCtrMask();
    virtual const char * GetEventLabel(unsigned idx);
    virtual int GetEventId(unsigned idx);
    virtual int GetEventIndex(unsigned evtId, unsigned startIdx);
    virtual int SetEvtUser1(unsigned idx, uint64_t val);
    virtual int GetEvtUser1(unsigned idx, uint64_t *pVal);
    virtual int SetEvtUser2(unsigned idx, uint64_t val);
    virtual int GetEvtUser2(unsigned idx, uint64_t *pVal);
    virtual int SetOverflowHandler(Bgpm_OverflowHandler_t handler);
    virtual int SetOverflow(unsigned idx, uint64_t threshold);
    virtual int GetOverflow(unsigned idx, uint64_t *pThreshold);
    virtual Bgpm_OverflowHandler_t GetOverflowHandler();
    virtual bool AccumResetThresholds(uint64_t intMask);  // accum values and reset active overflow threshold - return true if at least one user overflow found
    virtual int  GetOverflowIndices(uint64_t ovfVector, unsigned *pIndicies, unsigned *pLen);
    virtual int SetContext(Bgpm_Context ctx);
    virtual int GetContext(Bgpm_Context *pCtx);
    virtual int AllowMixedContext();
    virtual int SetQfpuGrpMask(unsigned idx, uint64_t mask);
    virtual int GetQfpuGrpMask(unsigned idx, uint64_t *pMask);
    virtual int SetXuGrpMask(unsigned idx, uint64_t mask);
    virtual int GetXuGrpMask(unsigned idx, uint64_t *pMask);
    virtual int SetCountFP(unsigned idx, ushort countFp);
    virtual int GetCountFP(unsigned idx, ushort *pCountFp);
    virtual int SetEdge(unsigned idx, ushort edge);
    virtual int GetEdge(unsigned idx, ushort *pEdge);
    virtual int SetInvert(unsigned idx, ushort invert);
    virtual int GetInvert(unsigned idx, ushort *pInvert);
    virtual int SetXuMatch(unsigned idx, uint16_t match, uint16_t mask);
    virtual int GetXuMatch(unsigned idx, uint16_t *pMatch, uint16_t *pMask);
    virtual int SetQfpuMatch(unsigned idx, uint16_t match, uint16_t mask, ushort fpScale);
    virtual int GetQfpuMatch(unsigned idx, uint16_t *pMatch, uint16_t *pMask, ushort *pFpScale);
    virtual int SetFpSqrScale(ushort fpScale);
    virtual int GetFpSqrScale(ushort *pFpScale);
    virtual int SetFpDivScale(ushort fpScale);
    virtual int GetFpDivScale(ushort *pFpScale);
    virtual int PrintCurrentReservations();
    virtual int GetMux(Bgpm_MuxStats_t* pMuxStats);
    virtual int GetMuxGrps();
    virtual uint64_t GetMuxCycles(unsigned muxGrp);
    virtual uint64_t GetMuxEventCycles(unsigned evtIdx, ushort normalize);

    virtual bool FillFeedbackBuffer(char *buf, size_t bufSize, Upci_Punit_t *pPunit, unsigned failedEvtId, int muxGrp=-1);

    virtual Upci_Punit_t *DebugGetPunit(unsigned idx) { return puList[idx]; }


    /**
     * Event Record to (one per event in this event set).
     */
    class PunitEvtRec : public EvtInfo
    {
    public:
        PunitEvtRec()
        : puEvtIdx(0), pPunit(NULL), evtAttrMask(EvtAttr_None),
          user1(0), user2(0) {}

        PunitEvtRec(unsigned idx, Upci_Punit_t *ipPunit)
        : puEvtIdx(idx), pPunit(ipPunit), evtAttrMask(EvtAttr_None),
          user1(0), user2(0) {}

        PunitEvtRec(const PunitEvtRec & s)
        : puEvtIdx(s.puEvtIdx), pPunit(s.pPunit),
          evtAttrMask(s.evtAttrMask),
          user1(s.user1), user2(s.user2) {}

        // Only allow assignment if "this" is empty.
        // Target will assume ownership of pPunit object
        PunitEvtRec & operator=(const PunitEvtRec & s) {
            assert(pPunit == NULL);
            puEvtIdx = s.puEvtIdx; pPunit = s.pPunit;
            evtAttrMask = s.evtAttrMask;
            user1 = s.user1; user2 = s.user2;
            return(*this);
        }

        void Dump(unsigned indent, unsigned idx);

        unsigned      puEvtIdx;       //!< Index into the puint for this event.
        Upci_Punit_t *pPunit;      //!< pointer to the supporting punit for this event (in case of multiplexing)
        EvtAttribs    evtAttrMask; //!< mask of local attrib settings for this event.

        uint64_t      user1;       //!< User1 data attribute
        uint64_t      user2;       //!< User2 data attribute
    };
    typedef std::vector<PunitEvtRec> SWPunitEvtRec;



protected:
    SWPunitEvtRec     evtRecs;
    unsigned          numUserEvts;     //!< number of events user has assigned.
                                       //!< Muxing cycles events are always added last and hidden from the user list.
    PunitList         puList;
    unsigned          activePunitIdx;  //!< indicates index into puList for active punit
                                       //!< used to make much code common between ES_SWPunit
                                       //!< and ES_MPPunit event sets.

    unsigned          targCore;        //!< 0-16
    unsigned          targHwThd;       //!< 0-3
    CoreReservations &coreRsv;         //!< shared memory version of core-wide reservations.
    unsigned          coreRsvMask;     //!< enum CoreDepBits: mask of core-wide resources needed by events in this set.
    unsigned          appliedRsvMask;  //!< enum CoreDepBits: mask of core-wide resources applied to punits

    bool              ctrGrpRvd;       //!< Keep track of whether this evtset has already reserved count affinty groups or not.
                                       //!< so we only refCount the affinity group once per evtSet.

    bool              frozen;          //!< has the evtset been applied to hardware (no more changes allowed)
    bool              running;         //!< track if event set is running.

    Bgpm_OverflowHandler_t    pOvfHandler; //!< callers overflow handler function

    virtual void Dump(unsigned indent, bool limit);  // limit punit dump to only the active punit.




protected:
    virtual void SetEvtSetPunitInts(Upci_Punit_t *pPunit);  // Activate pm interrupts for this punit.

    inline unsigned GetTargThdId() { return targCore * CONFIG_HWTHREADS_PER_CORE + targHwThd; }

    // return reserved counter group mask
    inline uint8_t CtrGrpMask() { return coreRsv.ctrGrpMask[targHwThd]; }

    inline bool RsvAttachedCtrGrps(uint8_t ctrMask) { return coreRsv.RsvAttachedCtrGrp(ctrMask); }

    // Confirm that core wide l1p mode for event matches (or reserve new l1p mode).
    int ConfirmL1pCoreReservation(unsigned evtId);

    // ReserveEvent  - common sequence of operations need to add an event (couldn't find creative/descriptive name)
    int ReserveEvent(int listIdx, unsigned evtId, Upci_Punit_t *pPunit, unsigned puEvtIdx);

    #define ES_EVTRECIDX_NOTFOUND 9999
    virtual unsigned EvtRecIdx(Upci_Punit_t *pPunit, unsigned unitEvtIdx);  // find corresponding evtRec index for event assigned to punit

    // Apply coreRsvMask attributes to the punit prior to attach or apply operations.
    virtual void SetPunitCoreAttribs(Upci_Punit_t *pPunit, unsigned coreRsvMask);

    // Return the order in which punit event ids would best be added to
    // Get most efficient allocation of events to counters.
    // Most efficient order is:
    //    L1p or Wakeup Events
    //    XU Events
    //    Other A2 Events
    //    Opcode Events
    void GetBestAddOrder(unsigned *evtIds, unsigned num, std::vector<unsigned> & idxs);

    // Get a new Punit, initialized and inited with any counters to skip
    Upci_Punit_t *AllocateFreshPunit();

    // print the contents of buffer at most 1024 chars at a time terminating in '\n' char
    // This is because the print function on BGQ can't handle more than 1k buffer at a time
    // and just splits the contents otherwise
    void PrintLines(char *buf, size_t bufSize);

#if 0
    // incr Overflow Accumulated Values for interrupts in mask
    // return boolean indicating when a matching interrupt was found
    bool IncrHits(uint64_t intMask)
    {
        bool intFound = false;
        for (unsigned ovfIdx=0; ovfIdx<ovfList.size(); ++ovfIdx) {
            if (ovfList[ovfIdx].intMask & intMask) {
                unsigned idx = ovfList[ovfIdx].idx;
                uint64_t delta = (~(ovfList[ovfIdx].threshold)+1);  // add period
                delta += Upci_Punit_Event_ReadIdx(evtRecs[idx].pPunit, evtRecs[idx].puEvtIdx);  // add residual
                ovfList[ovfIdx].accumVal += delta;
                intFound = true;
            }
        }
        return intFound;
    }
#endif


private:

    // hide these
    ES_SWPunit(const ES_SWPunit &);
    ES_SWPunit & operator=(const ES_SWPunit &);
};



//! ES_LLPunit
/*!
  Low Latency Punit Event Set class
*/
class ES_LLPunit : public ES_SWPunit
{
public:
    ES_LLPunit() {}
    virtual ~ES_LLPunit() {}

    virtual ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    //virtual EvtSet *Spawn();           // create a empty sibling (another of same type)
    virtual EvtSet *Clone(uint64_t target, unsigned *addEvents, unsigned addNum);

    virtual int Start(); // disable operation
    virtual int Stop(); // disable operation
    virtual int ResetStart(); // disable operation
    virtual int Reset(); // disable operation
    virtual int ReadEvent(unsigned idx, uint64_t *pVal); // disable operation
    virtual int WriteEvent(unsigned idx, uint64_t val); // disable operation
    virtual int Punit_GetHandles(Bgpm_Punit_Handles_t *pH); // disable operation
    //virtual int Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH);

    virtual int SetOverflowHandler(Bgpm_OverflowHandler_t handler);
    virtual int SetOverflow(unsigned idx, uint64_t threshold);

protected:

    virtual void SetEvtSetPunitInts(Upci_Punit_t *pPunit);

private:

    // hide these
    ES_LLPunit(const ES_LLPunit &);
    ES_LLPunit & operator=(const ES_LLPunit &);
};



}

#endif
