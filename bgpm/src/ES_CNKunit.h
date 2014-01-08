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

#ifndef _BGPM_ES_CNKUNIT_H_  // Prevent multiple inclusion
#define _BGPM_ES_CNKUNIT_H_

#include "EvtSet.h"
#include "EvtInfo.h"
#include "cnk/include/kcounters.h"

namespace bgpm {


//! ES_CNKunit
/*!
  CNKunit Event Set class
*/
class ES_CNKunit : public ES_Active
{
public:
    ES_CNKunit() : evtRecs(), running(false) { }
    virtual ~ES_CNKunit() {};

    virtual ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    virtual bool IsEventIdCompatible(int evtId);
    virtual void Dump(unsigned indent);

    virtual int AddEvent(unsigned evtId, EvtSet* & pNewEvtSet);
    virtual int AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet);
    virtual int NumEvents() const;
    virtual int Apply(int hEvtSet);   // include the evtSet handle for evtset type depended global var updates
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
    virtual int SampleCNKEvent(unsigned idx, uint64_t *pVal);



    class CNKEvtRec : public EvtInfo
    {
    public:
        CNKEvtRec() : pEvent(NULL), ctrType(CNKCtrNode), buffIdx(0), accumVal(0), startVal(0), user1(0), user2(0) {}

        CNKEvtRec(UPC_CNKunitEvtTableRec_t *ipEvent, Upci_CNKCtrType_t ictrType, unsigned ibuffIdx)
        : pEvent(ipEvent), ctrType(ictrType), buffIdx(ibuffIdx), accumVal(0),
          startVal(0), user1(0), user2(0) { }

        CNKEvtRec(const CNKEvtRec & s)
        : pEvent(s.pEvent), ctrType(s.ctrType), buffIdx(s.buffIdx), accumVal(s.accumVal),
          startVal(s.startVal), user1(s.user1), user2(s.user2) {}

        CNKEvtRec & operator=(const CNKEvtRec & s) {
            pEvent=s.pEvent; ctrType=s.ctrType; buffIdx=s.buffIdx; accumVal=s.accumVal;
            startVal=s.startVal; user1=s.user1; user2=s.user2;
            return(*this);
        }
        void Dump(unsigned indent, unsigned idx);

        UPC_CNKunitEvtTableRec_t * pEvent;
        Upci_CNKCtrType_t         ctrType;  //!< ctr type generated from source type
        unsigned                  buffIdx;     //!< index into counter type buffer for this event.
        uint64_t                  accumVal;    //!< accumulated counter value
        uint64_t                  startVal;    //!< current start value since last accumulation.
        uint64_t                  user1;       //!< User1 data attribute
        uint64_t                  user2;       //!< User2 data attribute
    };
    typedef std::vector<CNKEvtRec> CNKEvtRecs;
    typedef std::vector<PerfCountItem_t> CNKCntBuff;

protected:
    CNKEvtRecs  evtRecs;                // bgpm records
    CNKCntBuff  ctrBuffs[CNKCtrHWT+1];  // cnk request buffers
    bool        running;

    ES_CNKunit(const ES_CNKunit &s)
    : evtRecs(s.evtRecs), running(s.running)
    {
        for (int i=0; i<=(int)CNKCtrHWT; ++i) {
            ctrBuffs[i] = s.ctrBuffs[i];
        }
    }

private:

    // hide these
    ES_CNKunit & operator=(const ES_CNKunit &);

    int UpdateCountBuffers();
    int UpdateIndivCount(unsigned evtIdx);


};





}

#endif
