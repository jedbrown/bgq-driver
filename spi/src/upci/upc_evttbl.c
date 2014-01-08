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

#include "spi/include/upci/upc_evttbl.h"

#undef UPC_EVTSRC
static const char *upci_UnitNames[] = {
#define UPC_EVTSRC(name, mnemonic, description) mnemonic,
#include "spi/include/upci/upc_evtsrcs.h"
};


const char * Upci_GetEventLabel(unsigned evtId)
{
    const char * lbl = NULL;
    if      (evtId <= PEVT_PUNIT_LAST_EVENT)   lbl = punitEvtTbl[evtId].label;
    else if (evtId <= PEVT_L2UNIT_LAST_EVENT)  lbl = l2unitEvtTbl[evtId-PEVT_PUNIT_LAST_EVENT].label;
    else if (evtId <= PEVT_IOUNIT_LAST_EVENT)  lbl = IOunitEvtTbl[evtId-PEVT_L2UNIT_LAST_EVENT].label;
    else if (evtId <= PEVT_NWUNIT_LAST_EVENT)  lbl = NWunitEvtTbl[evtId-PEVT_IOUNIT_LAST_EVENT].label;
    else if (evtId <= PEVT_CNKUNIT_LAST_EVENT) lbl = CNKunitEvtTbl[evtId-PEVT_NWUNIT_LAST_EVENT].label;
    assert(lbl);
    return lbl;
}



const char * Upci_GetEventDesc(unsigned evtId)
{
    const char * desc = NULL;
    if      (evtId <= PEVT_PUNIT_LAST_EVENT)   desc = punitEvtTbl[evtId].desc;
    else if (evtId <= PEVT_L2UNIT_LAST_EVENT)  desc = l2unitEvtTbl[evtId-PEVT_PUNIT_LAST_EVENT].desc;
    else if (evtId <= PEVT_IOUNIT_LAST_EVENT)  desc = IOunitEvtTbl[evtId-PEVT_L2UNIT_LAST_EVENT].desc;
    else if (evtId <= PEVT_NWUNIT_LAST_EVENT)  desc = NWunitEvtTbl[evtId-PEVT_IOUNIT_LAST_EVENT].desc;
    else if (evtId <= PEVT_CNKUNIT_LAST_EVENT) desc = CNKunitEvtTbl[evtId-PEVT_NWUNIT_LAST_EVENT].desc;
    assert(desc);
    return desc;
}



UPC_EventSources_t Upci_GetEventSrc(unsigned evtId)
{
    UPC_EventSources_t evtSrc = 0;
    if      (evtId <= PEVT_PUNIT_LAST_EVENT)   evtSrc = punitEvtTbl[evtId].evtSrc;
    else if (evtId <= PEVT_L2UNIT_LAST_EVENT)  evtSrc = l2unitEvtTbl[evtId-PEVT_PUNIT_LAST_EVENT].evtSrc;
    else if (evtId <= PEVT_IOUNIT_LAST_EVENT)  evtSrc = IOunitEvtTbl[evtId-PEVT_L2UNIT_LAST_EVENT].evtSrc;
    else if (evtId <= PEVT_NWUNIT_LAST_EVENT)  evtSrc = NWunitEvtTbl[evtId-PEVT_IOUNIT_LAST_EVENT].evtSrc;
    else if (evtId <= PEVT_CNKUNIT_LAST_EVENT) evtSrc = CNKunitEvtTbl[evtId-PEVT_NWUNIT_LAST_EVENT].evtSrc;
    else {
        assert(0);
    }
    return evtSrc;
}



const char * Upci_GetEventSrcName(unsigned evtId)
{
    UPC_EventSources_t evtSrc = Upci_GetEventSrc(evtId);
    return upci_UnitNames[evtSrc];
}



const char *Upci_GetMetricText(UPC_EventMetric_t m) {
    static char *p;
    switch (m) {
        case UPC_METRIC_BEVENTS: { p = "be"; break; }
        case UPC_METRIC_BCYCLES: { p = "bc"; break; }
        case UPC_METRIC_EVENTS:  { p = "e"; break; }
        case UPC_METRIC_CYCLES: { p = "c"; break; }
        case UPC_METRIC_SINGLE: { p = "s"; break; }
        case UPC_METRIC_CONSEQ: { p = "v"; break; }
        default: { p = "undef"; break; }
    }
    return p;
}



