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

#include "stdlib.h"
#include "stdio.h"

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"


#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


#define NUM_THREADS 68
#define INVALID_PARM -1
int Dummy=-1;

void OtherUnitAttrib_test(int);
void Overflow_test(int);

int main(int argc, char *argv[])
{
    Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error

    TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);

    // set to look at Punit Counters
    int hPuSet = Bgpm_CreateEventSet();
    // define eventlist to be added
    unsigned measList[] = {
    PEVT_NW_USER_PP_SENT,
    PEVT_NW_SYS_PP_SENT,
    PEVT_NW_SYS_COL_SENT,
    PEVT_NW_USER_PP_RECV
    };
    TEST_CHECK_EQUAL(Bgpm_AddEventList(hPuSet,measList,4),0);
   
    unsigned hOfSet = Bgpm_CreateEventSet();
    unsigned evtList[] = {
    PEVT_CYCLES,  // will overflow
    PEVT_CYCLES,  // will not overflow
    PEVT_LSU_COMMIT_LD_MISSES, // will overflow
    PEVT_INST_ALL,
    PEVT_INST_QFPU_FMUL,  // will overflow
    PEVT_INST_QFPU_FMUL,  // 2nd one will not
    PEVT_L1P_BAS_HIT,  // Extra Events
    PEVT_L1P_BAS_MISS, // Extra Events - should cause shared overflow.
    };
    TEST_CHECK_EQUAL(Bgpm_AddEventList(hOfSet,evtList,8),0);

    OtherUnitAttrib_test(hPuSet);
    Overflow_test(hOfSet);

    TEST_CHECK_EQUAL( Bgpm_NumEvents(hPuSet),4);
    TEST_RETURN();
}
void OtherUnitAttrib_test(int hPuSet)
{
    unsigned Idx = Bgpm_GetEventIndex(hPuSet, PEVT_NW_SYS_PP_SENT, 0);
    TEST_FUNCTION(SetEventVirtChannels() PARAMETER TEST);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL(Bgpm_SetEventVirtChannels(Dummy,Idx,UPC_NW_VC_USER_PRIORITY),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid event index);
    TEST_CHECK_EQUAL(Bgpm_SetEventVirtChannels(hPuSet,Dummy,UPC_NW_VC_USER_PRIORITY),BGPM_EINV_EVT_IDX);
    TEST_FUNCTION_PARM(Invalid Channel);
    TEST_CHECK_EQUAL(Bgpm_SetEventVirtChannels(hPuSet,Idx,-1),BGPM_EUNSUP_ATTRIB);

    UPC_NW_Vchannels *p=NULL; 
    TEST_FUNCTION(GetEventVirtChannels() PARAMETER TEST);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL(Bgpm_GetEventVirtChannels(Dummy,Idx,p),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Channel);
    TEST_CHECK_EQUAL(Bgpm_GetEventVirtChannels(hPuSet,Idx,p),BGPM_EINV_PARM);
    UPC_NW_Vchannels q; 
    TEST_FUNCTION_PARM(Invalid event index);
    TEST_CHECK_EQUAL(Bgpm_GetEventVirtChannels(hPuSet,Dummy,&q),BGPM_EINV_EVT_IDX);
}

void Overflow_test(int hOfSet)
{
    unsigned Idx = Bgpm_GetEventIndex(hOfSet, PEVT_LSU_COMMIT_LD_MISSES, 0);
    TEST_FUNCTION(SetOverflow() PARAMETER TEST);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL(Bgpm_SetOverflow(Dummy,Idx,100000),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid event index);
    TEST_CHECK_EQUAL(Bgpm_SetOverflow(hOfSet,Dummy,100000),BGPM_EINV_EVT_IDX);
    TEST_FUNCTION_PARM(Invalid Channel);
    TEST_CHECK_EQUAL(Bgpm_SetOverflow(hOfSet,Idx,-1),BGPM_EINV_OVF);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_SetOverflow(hOfSet,Idx,100000),0);

    uint64_t period;
    Bgpm_OverflowHandler_t handler;
    TEST_FUNCTION(GetOverflow() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_GetOverflow(hOfSet, Idx, &period, &handler),0);
    TEST_FUNCTION_PARM(Invalid Event Handle); 
    TEST_CHECK_EQUAL(Bgpm_GetOverflow(Dummy,Idx, &period, &handler),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid event index);
    TEST_CHECK_EQUAL(Bgpm_GetOverflow(hOfSet,Dummy,&period, &handler),BGPM_EINV_EVT_IDX);

    uint64_t ovfVector=0;
    unsigned ovfIdxs[BGPM_MAX_OVERFLOW_EVENTS];
    unsigned len = BGPM_MAX_OVERFLOW_EVENTS;
    TEST_FUNCTION(GetOverflowEventIndices() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_GetOverflowEventIndices(hOfSet,  ovfVector, ovfIdxs, &len),0);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL(Bgpm_GetOverflowEventIndices(Dummy,  ovfVector, ovfIdxs, &len),BGPM_EINV_SET_HANDLE);
}
