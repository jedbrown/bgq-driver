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

#ifndef _BGPM_ES_IOUNIT_H_  // Prevent multiple inclusion
#define _BGPM_ES_IOUNIT_H_

#include "EvtSet.h"
#include "EvtInfo.h"

namespace bgpm {


//! ES_IOunit
/*!
  IOunit Event Set class
*/
class ES_IOunit : public ES_Active
{
public:
    ES_IOunit()
    : evtRecs(), pOvfHandler(NULL), ovfInUse(false), frozen(false)
    {
        assert(pShMem!=NULL);

        UPC_IO_EnableUPC((pShMem->perspect!=BGPM_HW_PERSPECTIVE), nodeConfig);
    }

    virtual ~ES_IOunit() {
        if (ovfInUse) {
            pShMem->controlIO.DecrRef();
        }
    }

    virtual ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    //virtual EvtSet *Spawn();           // create a empty sibling (another of same type)
    virtual bool IsEventIdCompatible(int evtId);
    //virtual EvtSet *Clone(uint64_t target, unsigned *addEvents, int addNum);
    virtual void Dump(unsigned indent);

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

    class IOunitEvtRec : public EvtInfo
    {
    public:
        IOunitEvtRec()
        : pEvent(NULL), ctrIdx(0),
          evtAttrMask(EvtAttr_None),
          user1(0), user2(0) {}

        IOunitEvtRec(int evtId)
        : pEvent(NULL), ctrIdx(0),
          evtAttrMask(EvtAttr_None),
          user1(0), user2(0) {
            pEvent = &IOunitEvtTbl[evtId-PEVT_MU_PKT_INJ];
            assert(pEvent->evtId == (unsigned)evtId);
            ctrIdx = pEvent->selVal;
        }

        IOunitEvtRec(const IOunitEvtRec & s)
        : pEvent(s.pEvent), ctrIdx(s.ctrIdx),
          evtAttrMask(s.evtAttrMask),
          user1(s.user1), user2(s.user2) {}

        IOunitEvtRec & operator=(const IOunitEvtRec & s) {
            pEvent = s.pEvent; ctrIdx = s.ctrIdx;
            evtAttrMask = s.evtAttrMask;
            user1 = s.user1; user2 = s.user2;
            return(*this);
        }
        void Dump(unsigned indent, unsigned idx);

        UPC_IOunitEvtTableRec_t * pEvent;
        unsigned                  ctrIdx;      //!< IO counter index
        EvtAttribs                evtAttrMask; //!< mask of local attrib settings for this event.
        uint64_t                  user1;       //!< User1 data attribute
        uint64_t                  user2;       //!< User2 data attribute
    };
    typedef std::vector<IOunitEvtRec> IOunitEvtRecs;

protected:
    IOunitEvtRecs  evtRecs;
    Bgpm_OverflowHandler_t  pOvfHandler; //!< callers overflow handler function
    bool           ovfInUse;     //!< An overflow event exists for this evt set.
    bool           frozen;       //!< has the evtset been applied to hardware or started (no more changes allowed)

    ES_IOunit(const ES_IOunit &s)
    : evtRecs(s.evtRecs),
      pOvfHandler(s.pOvfHandler), ovfInUse(s.ovfInUse), frozen(s.frozen) {
        if (ovfInUse) {
            pShMem->controlIO.IncrRef();
        }
    }

private:

    // hide these
    ES_IOunit & operator=(const ES_IOunit &);
};





}

#endif
