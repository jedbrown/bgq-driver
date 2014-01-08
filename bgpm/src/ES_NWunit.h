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

#ifndef _BGPM_ES_NWUNIT_H_  // Prevent multiple inclusion
#define _BGPM_ES_NWUNIT_H_

#include "EvtSet.h"
#include "EvtInfo.h"
#include "globals.h"

namespace bgpm {


//! ES_NWunit
/*!
  NWunit Event Set class
*/
class ES_NWunit : public ES_Active
{
public:
    ES_NWunit()
    : evtRecs(), enabledLinks(0), mappedCtrMask(0), attachedLinkMask(0), attachedCtlMask(0), attachedCtrCtlMask(0)
    {
        assert(pShMem!=NULL);
        enabledLinks = UPC_NW_GetEnabledLinks();
        UPC_NW_EnableUPC();
    };
    virtual ~ES_NWunit();

    virtual ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    //virtual EvtSet *Spawn();           // create a empty sibling (another of same type)
    virtual EvtSet *Clone();
    virtual void Dump(unsigned indent);
    virtual bool IsEventIdCompatible(int evtId);

    virtual int AddEvent(unsigned evtId, EvtSet* & pNewEvtSet);
    virtual int AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet);
    virtual int NumEvents() const;
    virtual int NumTargets() const;
    virtual int Attach(uint64_t target, uint64_t agentTarg);
    virtual int Detach();
    virtual int Start();
    virtual int Stop();
    virtual int ResetStart();
    virtual int Reset();
    virtual int ReadEvent(unsigned idx, uint64_t *pVal);
    virtual int ReadLinkEvent(unsigned idx, uint16_t linkMask, uint64_t *pVal);
    virtual const char * GetEventLabel(unsigned idx);
    virtual int GetEventId(unsigned idx);
    virtual int GetEventIndex(unsigned evtId, unsigned startIdx);
    virtual int SetEvtUser1(unsigned idx, uint64_t val);
    virtual int GetEvtUser1(unsigned idx, uint64_t *pVal);
    virtual int SetEvtUser2(unsigned idx, uint64_t val);
    virtual int GetEvtUser2(unsigned idx, uint64_t *pVal);
    virtual int SetVirtChannels(unsigned idx, UPC_NW_Vchannels vchan);
    virtual int GetVirtChannels(unsigned idx, UPC_NW_Vchannels * pVchan);

    int Map2AvailCtr(uint16_t & ctrMask, unsigned & ctrIdx);   //!< Adjust ctrIdx to an available counter or return failure

    class NWunitEvtRec : public EvtInfo
    {
    public:
        NWunitEvtRec() : pEvent(NULL), ctrIdx(0), vchanMask(0), user1(0), user2(0)  {}

        NWunitEvtRec(int evtId) : pEvent(NULL), ctrIdx(0), vchanMask(0), user1(0), user2(0) {
            pEvent = &NWunitEvtTbl[evtId-PEVT_NW_USER_PP_SENT];
            assert(pEvent->evtId == (unsigned)evtId);
            ctrIdx = pEvent->unitCtr;
            vchanMask = pEvent->selVal;
        }

        NWunitEvtRec(const NWunitEvtRec & s)
        : pEvent(s.pEvent), ctrIdx(s.ctrIdx), vchanMask(s.vchanMask), user1(s.user1), user2(s.user2) {}

        NWunitEvtRec & operator=(const NWunitEvtRec & s) {
            pEvent = s.pEvent; ctrIdx = s.ctrIdx; vchanMask = s.vchanMask; user1 = s.user1; user2 = s.user2;
            return(*this);
        }
      void Dump(unsigned indent, unsigned idx);

        UPC_NWunitEvtTableRec_t * pEvent;
        unsigned                  ctrIdx;      //!< NW counter index
        unsigned                  vchanMask;   //!< NW Virtual channel selection for this event.
        uint64_t                  user1;       //!< User1 data attribute
        uint64_t                  user2;       //!< User2 data attribute
    };
    typedef std::vector<NWunitEvtRec> NWunitEvtRecs;

protected:
    NWunitEvtRecs  evtRecs;
    uint16_t       enabledLinks;         //!< some links not usable when in mesh
    uint16_t       mappedCtrMask;        //!< Which counters are already mapped to events.

    uint16_t       attachedLinkMask;     //!< Which links is this event set attached to.
    uint16_t       attachedCtlMask;      //!< Which links does this event set control
    uint64_t       attachedCtrCtlMask;  //!< Start/Stop mask for controlling attached counters.

    ES_NWunit(const ES_NWunit &s);

    // Check if current hardware config matches the event config of this event set.
    bool LinkConfigMatch(unsigned linkNum);


private:

    // hide these
    ES_NWunit & operator=(const ES_NWunit &);
};





}

#endif
