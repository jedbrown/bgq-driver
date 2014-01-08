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


#include <stdlib.h>
#include <stdio.h>

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"
#define TEST_CASE_VERBOSE
#include "../../test_utils.h"
TEST_INIT();


unsigned evtList[] = {      // List of performance events to collect

    PEVT_LSU_COMMIT_LD_MISSES,
    PEVT_LSU_COMMIT_CACHEABLE_LDS, 
    PEVT_L1P_BAS_MISS,
    PEVT_INST_XU_ALL, 
    PEVT_INST_QFPU_ALL, 
    PEVT_INST_QFPU_FPGRP1, 
};
int numEvts = sizeof(evtList) / sizeof(int);
int Dummy=-1;

void Fast_Control()
{
    TEST_FUNCTION(Bgpm_Punit_GetHandles() PARAMETER TEST);
    TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);

    int hEvtSet = Bgpm_CreateEventSet();   // get container for events.
    TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, evtList, numEvts),0);

    TEST_CHECK_EQUAL(Bgpm_Apply(hEvtSet),0);        // apply to hardware

    Bgpm_Punit_Handles_t hCtrs;
    TEST_FUNCTION_PARM(Invalid eventset handle);
    TEST_CHECK_EQUAL(Bgpm_Punit_GetHandles(Dummy, &hCtrs),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_Punit_GetHandles(hEvtSet, &hCtrs),0);

    TEST_CHECK_EQUAL(Bgpm_Start(hEvtSet),0);   // start counting.

    TEST_CHECK_EQUAL(Bgpm_Stop(hEvtSet),0);   // stop counting - once per thread.
    Bgpm_Disable();
}

void Low_Latency()
{
    TEST_FUNCTION(Bgpm_Punit_GetLLHandles() PARAMETER TEST);
    TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_LLDISTRIB),0);

    int hEvtSet = Bgpm_CreateEventSet();   // get container for events.
    TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, evtList, numEvts),0);

    TEST_CHECK_EQUAL(Bgpm_Apply(hEvtSet),0);        // apply to hardware

    Bgpm_Punit_LLHandles_t hLL;
    TEST_FUNCTION_PARM(Invalid eventset handle);
    TEST_CHECK_EQUAL(Bgpm_Punit_GetLLHandles(Dummy, &hLL),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_Punit_GetLLHandles(hEvtSet, &hLL),0);

    //TEST_CHECK_EQUAL(Bgpm_Punit_LLResetStart(hLL.allCtrMask),0);   // start counting.
    Bgpm_Punit_LLResetStart(hLL.allCtrMask);   // start counting.
    Bgpm_Punit_LLStop(hLL.allCtrMask);
    //TEST_CHECK_EQUAL(Bgpm_Punit_LLStop(hLL.allCtrMask),0);   // stop counting - once per thread.
    
    TEST_CHECK_EQUAL(Bgpm_PrintOnError(Dummy),0);  // Dummy can be interpreted as False value, Cann't test
    Bgpm_Disable();

}

int main()
{
    Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error

    Fast_Control();
    Low_Latency();

    return(0);
}


