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

#ifndef _BGPM_EVTSET_H_  // Prevent multiple inclusion
#define _BGPM_EVTSET_H_

#include "bgpm/include/bgpm.h"
#include "Lock.h"


namespace bgpm {


class ES_SWPunit;



//! EvtSet
/*!
  Base Event Set class

  All possible methods are assigned to base class.
  The base class is not abstract, but should be assigned to replace
  a previously existing event set in the list.
*/
class EvtSet
{
public:
    EvtSet() : crtRC(0), inSigHandler(false), user1(0), user2(0) {};
    virtual ~EvtSet() {};

    // type indicator for each of the event set types.  The 1st few types just to exist to produce
    // error messages if the event set is not valid for a handle.
    enum ESTypes {
        ESType_Base = 0,      // Methods return "stale" handle error
        ESType_Unassigned,    // Methods return "not yet assigned type" error
        ESType_Active,        // never instantiated - just a base type for all the "real" event set types.
        ESType_LLPunit,
        ESType_SWPunit,
        ESType_MXPunit,
        ESType_HWPunit,
        ESType_HWDetailPunit,
        ESType_L2unit,
        ESType_IOunit,
        ESType_NWunit,
        ESType_CNKunit,
    };


    /**
     * Local Event Record Attribute Settings.
     * Indicates which non-default local (event specific) attributes
     * are set for this event.  The actual values may be stored elsewhere
     * (like in a upci punit object).
     * \note  These bits are similar to the coreRsvMask, in that they indicate
     * non-default attributes of an event.  However, the coreRsvMask indicates
     * a value which is shared between threads on the core, while the local event
     * attribute is only used by a particlar event instance (see ES_SWPunit.h)
     */
    typedef enum {
        EvtAttr_None         = 0x000,  //!< No attribute overrides active for this event.
        EvtAttr_Ovf          = 0x001,  //!< Overflow is active for this event
        EvtAttr_QfpuGrpMask  = 0x002,  //!< Non-predefined event Qfpu Group mask is set
        EvtAttr_XuGrpMask    = 0x004,  //!< Non-predefined event Xu group mask is set
        EvtAttr_FP           = 0x008,  //!< Override FP counting default on qfpu opcode event
        EvtAttr_Edge         = 0x010,  //!< Override default edge/cycle for event to count edges
        EvtAttr_Cycle        = 0x020,  //!< Override default edge/cycle for event to count cycles
        EvtAttr_Invert       = 0x040,  //!< Invert default event polarity
    } EvtAttribs;
    inline EvtAttribs Or_EvtAttribs(EvtAttribs l, EvtAttribs r) {
        return (EvtAttribs)( (unsigned)l | (unsigned)r );
    }
    inline bool IsAttribSet(EvtAttribs l, EvtAttribs r) {
        return ((unsigned)l & (unsigned)r);
    }



    // Some functions which just return common error messages.
    static int InvalidEvtSet();
    static int UnassignedEvtSet();
    static int UnsupportedOperation();

    inline int GetCrtRC() const { return crtRC; }  // return value indicating if object constructed successfully.
    virtual EvtSet::ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    //virtual EvtSet *Spawn();           // create a empty sibling (another of same type)
    //virtual EvtSet *Clone(uint64_t target = 0);  // create clone adjusted by target value (meaning differs by event set)

    inline void SignalHandlerIn() { inSigHandler = true; }
    inline void SignalHandlerOut() { inSigHandler = false; }

    virtual void Dump(unsigned indent);
    virtual bool IsEventIdCompatible(int evtId);
    bool IsEventIdValid(int evtId) { return ((evtId > 0) && (evtId <= PEVT_LAST_EVENT)); }
    virtual int DefaultError();

    // AddEvent and AddEventList can return a new event set if significant changes
    // to the existing event set are required.
    // pNewEvtSet returns a pointer if a new event set was created, and the caller is
    // required to delete the existing event set and replace it with the new one.
    // If pNewEvtSet returns NULL, then the existing event set is still valid.
    virtual int AddEvent(unsigned evtId, EvtSet* & pNewEvtSet);
    virtual int AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet);
    virtual int DumpEventsInError(unsigned failedEvtId); // Print punit event resources along with last one which failed.
    virtual int SetPreReserved();     // For debug - prereserve current src punit counters and signals for use in subsequent event set on same core.
    virtual int NumEvents() const;
    virtual int NumTargets() const;
    virtual int Apply(int hEvtSet);   // include the evtSet handle for evtset type depended global var updates
    virtual int Attach(uint64_t target, uint64_t agentTarg);
    virtual int Detach();
    virtual int Start();
    virtual int Stop();
    virtual int ResetStart();
    virtual int Reset();
    virtual int ReadEvent(unsigned idx, uint64_t *pVal);
    virtual int WriteEvent(unsigned idx, uint64_t val);
    virtual int L2_ReadEvent(unsigned slice, unsigned idx, uint64_t *pVal);
    virtual int L2_WriteEvent(unsigned slice, unsigned idx, uint64_t val);
    virtual int ReadThreadEvent(unsigned idx, uint64_t appMask, uint64_t agentMask, uint64_t *pVal);
    virtual int ReadLinkEvent(unsigned idx, uint16_t linkMask, uint64_t *pVal);
    virtual int Punit_GetHandles(Bgpm_Punit_Handles_t *pH);
    virtual int Punit_GetHandles4Thread(int hwThd, Bgpm_Punit_Handles_t *pH);
    virtual int Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH);
    virtual uint64_t GetActiveCtrMask();
    virtual const char * GetEventLabel(unsigned idx);
    virtual int GetEventId(unsigned idx);
    virtual int GetEventIndex(unsigned evtId, unsigned startIdx);
    virtual int SetEvtUser1(unsigned idx, uint64_t val);
    virtual int GetEvtUser1(unsigned idx, uint64_t *pVal);
    virtual int SetEvtUser2(unsigned idx, uint64_t val);
    virtual int GetEvtUser2(unsigned idx, uint64_t *pVal);
    virtual int SampleCNKEvent(unsigned idx, uint64_t *pVal);
    virtual int SetOverflowHandler(Bgpm_OverflowHandler_t handler);
    virtual int SetOverflow(unsigned idx, uint64_t threshold);
    virtual int GetOverflow(unsigned idx, uint64_t *pThreshold);
    virtual Bgpm_OverflowHandler_t GetOverflowHandler();
    virtual bool AccumResetThresholds(uint64_t intMask);
    virtual int GetOverflowIndices(uint64_t ovfVector, unsigned *pIndicies, unsigned *pLen);
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
    virtual int SwitchMuxGrp();
    virtual int GetMux(Bgpm_MuxStats_t* pMuxStats);
    virtual int GetMuxGrps();
    virtual uint64_t GetMuxCycles(unsigned muxGrp);
    virtual uint64_t GetMuxEventCycles(unsigned evtIdx, ushort normalize);
    virtual uint64_t GetMuxIntStatus(uint64_t intMask);  // return status bit when intMask includes the cycle event from the active Muxgroup
    virtual int SetVirtChannels(unsigned idx, UPC_NW_Vchannels vchan);
    virtual int GetVirtChannels(unsigned idx, UPC_NW_Vchannels * pVchan);

    virtual Upci_Punit_t *DebugGetPunit(unsigned idx=0) { return NULL; }
    virtual Upci_Punit_t *DebugGetTargPunit(unsigned thdId=0) { return NULL; }

protected:
    int  crtRC;           //!< construct return code (no using exceptions so caller needs to check).
    bool inSigHandler;    //!< This is a bit kludgy, but signal handler will set to indicate
                          //!< when event set is running in the signal handler.  Some operations
                          //!< are callable by user either in signal handler or base user state, so
                          //!< this allows the functions to operate slightly different as needed.

    EvtSet(const EvtSet &s) : crtRC(0), inSigHandler(false), user1(s.user1), user2(s.user2) {}

public:
    uint64_t user1;     //!< User1 data attribute
    uint64_t user2;     //!< User2 data attribute

private:

    // hide these
    EvtSet & operator=(const EvtSet &);
};



//! ES_Unassigned
/*!
  Unassigned event set will be used when an event set is created,
  but the type has not yet been determined.
*/
class ES_Unassigned : public EvtSet
{
public:
    ES_Unassigned() {};
    ~ES_Unassigned() {};

    virtual int DefaultError();
    virtual EvtSet::ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    //virtual EvtSet *Spawn();           // create a empty sibling (another of same type)
    //virtual EvtSet *Clone(uint64_t target = 0);  // create clone adjusted by target value (meaning differs by event set)

protected:
    ES_Unassigned(const ES_Unassigned &s) {}

private:

    // hide these
    ES_Unassigned & operator=(const ES_Unassigned &);
};



//! ES_Active
/*!
  Base Active Event Set
  Other event set types should subclass the ES_Active class
*/
class ES_Active : public EvtSet
{
public:
    ES_Active() {};
    ~ES_Active() {};

    virtual int DefaultError();
    virtual EvtSet::ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    //virtual EvtSet *Spawn();           // create a empty sibling (another of same type)
    //virtual EvtSet *Clone(uint64_t target = 0);  // create clone adjusted by target value (meaning differs by event set)

protected:
    ES_Active(const ES_Active &s) {}

private:

    // hide these
    ES_Active & operator=(const ES_Active &);
};



}

#endif
