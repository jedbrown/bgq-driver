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

#ifndef _BGPM_ES_HWPUNIT_H_  // Prevent multiple inclusion
#define _BGPM_ES_HWPUNIT_H_

#include "ES_SWPunit.h"

namespace bgpm {


//! ES_HWPunit
/*!
  Hardware perspective Punit Event Set class
*/
class ES_HWPunit : public ES_SWPunit
{
public:
    ES_HWPunit(unsigned hwThdID = Kernel_ProcessorID())
    : ES_SWPunit(hwThdID), targPuList(CONFIG_MAX_HWTHREADS),
      targAppliedRsvMask(CONFIG_MAX_HWTHREADS), targAttachedCtrGrpMask(CONFIG_MAX_HWTHREADS)
    { }
    virtual ~ES_HWPunit();

    virtual ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    //virtual EvtSet *Spawn();           // create a empty sibling (another of same type)
    virtual EvtSet *Clone(uint64_t target, unsigned *addEvents, unsigned addNum);

    // Transfer Reservations and Punit from the given SWPunit Event Set to this HWPunit event set.
    // SWPunit event set is left unusable.
    virtual void TransferSwPunit(ES_SWPunit & src);

    virtual int NumTargets() const;

    virtual int Attach(uint64_t target, uint64_t agentTarg);
    virtual int Detach();

    virtual int Start();
    virtual int Stop();
    virtual int ResetStart();
    virtual int Reset();
    virtual int ReadEvent(unsigned idx, uint64_t *pVal);
    virtual int WriteEvent(unsigned idx, uint64_t val);

    virtual int ReadThreadEvent(unsigned idx, uint64_t appMask, uint64_t agentMask, uint64_t *pVal);

    virtual int Punit_GetHandles(Bgpm_Punit_Handles_t *pH);
    virtual int Punit_GetHandles4Thread(int hwThd, Bgpm_Punit_Handles_t *pH);
    virtual int Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH);

    virtual int SetOverflowHandler(Bgpm_OverflowHandler_t handler);
    virtual int SetOverflow(unsigned idx, uint64_t threshold);

    virtual void Dump(unsigned indent);

    virtual Upci_Punit_t *DebugGetTargPunit(unsigned thdId) { return targPuList[thdId]; }


protected:

    virtual void SetEvtSetPunitInts(Upci_Punit_t *pPunit);

private:

    PunitList             targPuList;          //!<  List of attached punits
    std::vector<unsigned> targAppliedRsvMask;  //!<  Applied Rsv mask for each target thread.
    std::vector<uint8_t>  targAttachedCtrGrpMask; //!<  Indicate counter thread groups reserved by this target punit

    // hide these
    ES_HWPunit(const ES_HWPunit &);
    ES_HWPunit & operator=(const ES_HWPunit &);
};



//! ES_HWDetailPunit
/*!
  Detail Punit Event Set Class
*/
class ES_HWDetailPunit : public ES_HWPunit
{
public:
    ES_HWDetailPunit(unsigned hwThdID = Kernel_ProcessorID()) : ES_HWPunit(hwThdID) {}
    virtual ~ES_HWDetailPunit() {}

    virtual ESTypes ESType();  // use instead of C++ type identifier
    virtual const char *ESTypeLabel();
    //virtual EvtSet *Spawn();           // create a empty sibling (another of same type)
    virtual EvtSet *Clone(uint64_t target, unsigned *addEvents, unsigned addNum);

protected:


private:

    // hide these
    ES_HWDetailPunit(const ES_HWDetailPunit &);
    ES_HWDetailPunit & operator=(const ES_HWDetailPunit &);
};



}

#endif
