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

#include "globals.h"
#include "ES_Factory.h"
#include "EvtSet.h"
#include "ES_SWPunit.h"
#include "ES_HWPunit.h"
#include "ES_L2unit.h"
#include "ES_IOunit.h"
#include "ES_NWunit.h"
#include "ES_CNKunit.h"


using namespace bgpm;


int bgpm::GenerateEventSet(Bgpm_Modes mode, int evtId, EvtSet **pNewEvtSet)
{
    BGPM_TRACE_L2;
    assert(evtId > 0);
    assert(pShMem != NULL);
    assert(pNewEvtSet);

    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " hwtid=%02d evtId=%d, mode=%d\n", IND_STRG, Kernel_ProcessorID(), evtId, mode));


    // Punit events
    if (evtId <= PEVT_PUNIT_LAST_EVENT) {
        if (pShMem->globalMode == BGPM_MODE_LLDISTRIB) {
            *pNewEvtSet = new ES_LLPunit();
        }
        else if (pShMem->globalMode == BGPM_MODE_SWDISTRIB) {
            *pNewEvtSet = new ES_SWPunit();
            //fprintf(stderr, "GetCrtRC=%d\n", (*pNewEvtSet)->GetCrtRC());
        }
        else if (pShMem->globalMode == BGPM_MODE_HWDISTRIB) {
            *pNewEvtSet = new ES_HWPunit();
        }
#if 0
        else if (pShMem->globalMode == BGPM_MODE_HWDETAIL) {
            *pNewEvtSet = new ES_HWDetailPunit();
        }
#endif
        else {
            lastErr.PrintMsg("Mode %d not yet supported\n", pShMem->globalMode);
            return lastErr.PrintOrExit(BGPM_ENOTIMPL, BGPM_ERRLOC);
        }
    }


    else if (evtId <= PEVT_L2UNIT_LAST_EVENT) {
        ShMemLock guard(&(pShMem->l2ModeLock));
        if (pShMem->l2ModeRefCount == 0) {
            // combined L2 events - must verify combined mode before event set creation.
            pShMem->combineL2 = (evtId <= PEVT_L2_STORE_PARTIAL_LINE) ? true : false;
            UPC_L2_EnableUPC((pShMem->perspect!=BGPM_HW_PERSPECTIVE), pShMem->combineL2);
        }
        if ((evtId <= PEVT_L2_STORE_PARTIAL_LINE) != pShMem->combineL2) {
            return lastErr.PrintOrExit(BGPM_ECONF_L2_COMBINE, BGPM_ERRLOC);
        }
        *pNewEvtSet = new ES_L2unit(pShMem->combineL2);
        pShMem->l2ModeRefCount++;
    }


    else if (evtId <= PEVT_IOUNIT_LAST_EVENT) {
        *pNewEvtSet = new ES_IOunit();
    }
    else if (evtId <= PEVT_NWUNIT_LAST_EVENT) {
        *pNewEvtSet = new ES_NWunit();
    }
    else if (evtId <= PEVT_CNKUNIT_LAST_EVENT) {
        *pNewEvtSet = new ES_CNKunit();
    }
#if 0
    else if (evtId <= PEVT_ENVUNIT_LAST_EVENT) {
        return lastErr.PrintOrExit(BGPM_ENOTIMPL, BGPM_ERRLOC);
    }
    else if (evtId <= PEVT_MCUNIT_LAST_EVENT) {
        return lastErr.PrintOrExit(BGPM_ENOTIMPL, BGPM_ERRLOC);
    }
    else if (evtId <= PEVT_BGPMUNIT_LAST_EVENT) {
        return lastErr.PrintOrExit(BGPM_ENOTIMPL, BGPM_ERRLOC);
    }
#endif
    else {
        return lastErr.PrintOrExit(BGPM_EUNREC_EVTID, BGPM_ERRLOC);
    }

    if (*pNewEvtSet == NULL) {
        return lastErr.PrintOrExit(BGPM_ENOMEM, BGPM_ERRLOC);
    }
    else if ((*pNewEvtSet)->GetCrtRC() < 0) {
        int rc = (*pNewEvtSet)->GetCrtRC();
        lastErr.PrintOrExit(rc, BGPM_ERRLOC);
        delete *pNewEvtSet;
        *pNewEvtSet = NULL;
        return rc;
    }

    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " hwtid=%02d evtId=%d, generated=%s\n", IND_STRG, Kernel_ProcessorID(), evtId, (*pNewEvtSet)->ESTypeLabel()));

    return 0;
}



