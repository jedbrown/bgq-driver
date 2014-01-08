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

#ifndef _BGPM_ES_L2UNIT_H_  // Prevent multiple inclusion
#define _BGPM_ES_L2UNIT_H_

#include "EvtSet.h"
#include "EvtInfo.h"

namespace bgpm {


//! ES_L2unit
/*!
  L2unit Event Set class
*/
class ES_L2unit : public ES_Active
{
public:
    ES_L2unit(bool icombine)
    : evtRecs(), combine(icombine), pOvfHandler(NULL), ovfInUse(false), frozen(false)
    {}

    virtual ~ES_L2unit() {
        ShMemLock guard(&(pShMem->l2ModeLock));
        pShMem->l2ModeRefCount -= (pShMem->l2ModeRefCount == 0) ? 0: 1;
        if (ovfInUse) {
            pShMem->controlL2.DecrRef();
        }
    }

    virtual ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    //virtual EvtSet *Spawn();           // create a empty sibling (another of same type)
    //virtual EvtSet *Clone(uint64_t target, unsigned *addEvents, int addNum);
    virtual void Dump(unsigned indent);
    virtual bool IsEventIdCompatible(int evtId);
    virtual bool L2ModeOk(int evtId);

    virtual int AddEvent(unsigned evtId, EvtSet* & pNewEvtSet);
    virtual int AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet);
    virtual int NumEvents() const;
    virtual int Apply(int hEvtSet);
    virtual int Start();
    virtual int Stop();
    virtual int ResetStart();
    virtual int Reset();
    virtual int ReadEvent(unsigned idx, uint64_t *pVal);
    virtual int WriteEvent(unsigned idx, uint64_t val);
    virtual int L2_ReadEvent(unsigned slice, unsigned idx, uint64_t *pVal);
    virtual int L2_WriteEvent(unsigned slice, unsigned idx, uint64_t val);
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
    virtual int GetOverflowIndices(uint64_t ovfVector, unsigned *pIndicies, unsigned *pLen);

    class L2unitEvtRec : public EvtInfo
    {
    public:
        L2unitEvtRec()
        : pEvent(NULL), ctrIdx(0),
          evtAttrMask(EvtAttr_None),
          user1(0), user2(0) {}

        L2unitEvtRec(int evtId)
        : pEvent(0), ctrIdx(0),
          evtAttrMask(EvtAttr_None),
          user1(0), user2(0)
        {
            pEvent = &l2unitEvtTbl[evtId-PEVT_L2_HITS];
            assert(pEvent->evtId == (unsigned)evtId);
            ctrIdx = pEvent->selVal;
        }

        L2unitEvtRec(const L2unitEvtRec & s)
        : pEvent(s.pEvent), ctrIdx(s.ctrIdx),
          evtAttrMask(s.evtAttrMask),
          user1(s.user1), user2(s.user2) {}

        L2unitEvtRec & operator=(const L2unitEvtRec & s) {
            pEvent = s.pEvent; ctrIdx = s.ctrIdx;
            evtAttrMask = s.evtAttrMask;
            user1 = s.user1; user2 = s.user2;
            return(*this);
        }
        void Dump(unsigned indent, unsigned idx);

        UPC_L2unitEvtTableRec_t * pEvent;
        unsigned                  ctrIdx;      //!< L2 counter index
        EvtAttribs                evtAttrMask; //!< mask of local attrib settings for this event.
        uint64_t                  user1;       //!< User1 data attribute
        uint64_t                  user2;       //!< User2 data attribute
    };
    typedef std::vector<L2unitEvtRec> L2unitEvtRecs;

protected:
    L2unitEvtRecs  evtRecs;
    bool combine;
    Bgpm_OverflowHandler_t  pOvfHandler; //!< callers overflow handler function
    bool           ovfInUse;     //!< An overflow event exists for this evt set.
    bool           frozen;       //!< has the evtset been applied to hardware or started (no more changes allowed)

    ES_L2unit(const ES_L2unit &s)
    : evtRecs(s.evtRecs), combine(s.combine),
      pOvfHandler(s.pOvfHandler), ovfInUse(s.ovfInUse), frozen(s.frozen)
    {
        if (ovfInUse) {
            pShMem->controlL2.IncrRef();
        }
    }

private:

    // hide these
    ES_L2unit & operator=(const ES_L2unit &);
};





}

#endif
