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

#ifndef _BGPM_ES_MXPUNIT_H_  // Prevent multiple inclusion
#define _BGPM_ES_MXPUNIT_H_

#include "ES_SWPunit.h"

namespace bgpm {

//! ES_MXPunit
/*!
  Multiplexable Software Punit Event Set class
*/
class ES_MXPunit : public ES_SWPunit
{
public:

    ES_MXPunit(unsigned targHwThdId = Kernel_ProcessorID(), uint64_t period=0,  ushort iNormalize=0, int iMaxEvents=-1);
    virtual ~ES_MXPunit();

    virtual ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    virtual EvtSet *Clone(uint64_t target, unsigned *addEvents, unsigned addNum);
    virtual void Dump(unsigned indent);

    virtual int NumEvents() const;
    virtual int Apply(int hEvtSet);
    virtual int Stop();
    virtual int AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet);
    virtual int Punit_GetHandles(Bgpm_Punit_Handles_t *pH);
    virtual int Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH);
    virtual int ResetStart();
    virtual int Reset();
    virtual int ReadEvent(unsigned idx, uint64_t *pVal);
    virtual int WriteEvent(unsigned idx, uint64_t val);
    virtual int PrintCurrentReservations();
    virtual int SwitchMuxGrp();
    virtual int GetMux(Bgpm_MuxStats_t* pMuxStats);
    virtual int GetMuxGrps();
    virtual uint64_t GetMuxCycles(unsigned muxGrp);
    virtual uint64_t GetMuxEventCycles(unsigned evtIdx, ushort normalize);
    virtual uint64_t GetMuxIntStatus(uint64_t intMask);  // return status bit when intMask includes the cycle event from the active Muxgroup


protected:

    virtual int ReadAnyEvent(unsigned idx, uint64_t *pVal);  // allow read of any event - incl mux cycle events.

    std::vector<unsigned>  muxCycleEvtIdx;  //!< index into evtRecs for the muxing cycle event (tracking how many cycles this punit has been counting)
    std::vector<uint64_t>  muxIntStat;      //!< int status associated with the muxing cycle events (to identify an mux interrupt)
    std::vector<unsigned>  muxApplies;     //!< keep track of number of times each punit switches in.
    unsigned maxMuxEvtsPerPunit;            //!< keep track of the maximum number of events which can be assigned
                                            //!< to a single punit (-1 for the standard cycle count)

    ushort normalizeEvents;                 //!< normalize read results to max cycles in groups
    uint64_t numMuxSwitches;                //!< Keep track of the number of times we switch (reset with counts)
    uint64_t muxPeriod;                     //!< configured period between switches

    uint64_t muxSwitchTransCount;           //!< Keep a transition count of mux switches.  See SwitchMuxGrp code for more info.



private:

    uint64_t GetMaxMuxCycles() {
        uint64_t max = 0;
        for (unsigned i=0; i<puList.size(); ++i) {
            max = UPCI_MAX(max, GetMuxCycles(i));
        }
        return max;
    }

    // hide these
    ES_MXPunit(const ES_MXPunit &);
    ES_MXPunit & operator=(const ES_MXPunit &);
};





}

#endif
