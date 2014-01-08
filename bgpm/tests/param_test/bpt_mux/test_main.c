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


int main(int argc, char *argv[])
{
    Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error

    TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);

    int hEvtSet = Bgpm_CreateEventSet();

    uint64_t period = 0;
    period = 60000000;
    TEST_FUNCTION(Bgpm_SetMultiplex() PARAMETER TEST);

    TEST_FUNCTION_PARM(Invalid Event Set Handle);
    TEST_CHECK_EQUAL(Bgpm_SetMultiplex(Dummy, period, BGPM_NORMAL),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid parameter for normalization );
    TEST_CHECK_EQUAL(Bgpm_SetMultiplex(hEvtSet, period, Dummy),BGPM_EINV_PARM);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_SetMultiplex(hEvtSet, period, BGPM_NORMAL),0);

    TEST_FUNCTION(GetMultiplex() PARAMETER TEST);
    Bgpm_MuxStats_t mStats;
    TEST_FUNCTION_PARM(Invalid event Handle);
    TEST_CHECK_EQUAL(Bgpm_GetMultiplex(Dummy,&mStats),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid event Handle);
    TEST_CHECK_EQUAL(Bgpm_GetMultiplex(hEvtSet,&mStats),0);

    TEST_FUNCTION(SwitchMux() PARAMETER TEST);
    TEST_FUNCTION_PARM(Invalid Event set handle);
    TEST_CHECK_EQUAL(Bgpm_SwitchMux(Dummy),BGPM_EINV_SET_HANDLE);


    Bgpm_AddEvent(hEvtSet, PEVT_L1P_BAS_LD);
    Bgpm_AddEvent(hEvtSet, PEVT_L1P_BAS_ST_WC);
    Bgpm_AddEvent(hEvtSet, PEVT_L1P_STRM_WRT_INVAL);
    Bgpm_AddEvent(hEvtSet, PEVT_XU_BR_COMMIT);
    Bgpm_AddEvent(hEvtSet, PEVT_CYCLES);

    unsigned evtList[] = {
            PEVT_IU_IL1_MISS,
            PEVT_IU_IL1_MISS_CYC,
            PEVT_IU_INSTR_FETCHED,
            PEVT_IU_INSTR_FETCHED_CYC,
            PEVT_AXU_INSTR_COMMIT,
            PEVT_IU_IS1_STALL_CYC,
            PEVT_IU_IS2_STALL_CYC,
            PEVT_XU_PPC_COMMIT,
            PEVT_XU_COMMIT,
            PEVT_L1P_BAS_LU_DRAIN,
            PEVT_L1P_BAS_LU_DRAIN_CYC,
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));

    printf("NumEvents = %d\n", Bgpm_NumEvents(hEvtSet));

    Bgpm_Apply(hEvtSet);

    TEST_FUNCTION(GetMuxGroups() PARAMETER TEST);
    TEST_FUNCTION_PARM(Invalid Event set handle);
    TEST_CHECK_EQUAL(Bgpm_GetMuxGroups(Dummy),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Success case);
    TEST_CHECK_EQUAL(Bgpm_GetMuxGroups(hEvtSet),4); // event list results in 4 groups

    TEST_FUNCTION(GetMuxElapsedCycles() PARAMETER TEST);
    TEST_FUNCTION_PARM(Invalid event set handle);
    TEST_CHECK_EQUAL(Bgpm_GetMuxElapsedCycles(Dummy,0),BGPM_EINV_SET_HANDLE);

    TEST_FUNCTION(GetMuxEventElapsedCycles() PARAMETER TEST);
    TEST_FUNCTION_PARM(Invalid Event handle);    
    TEST_CHECK_EQUAL(Bgpm_GetMuxEventElapsedCycles(Dummy,1),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index);    
    TEST_CHECK_EQUAL(Bgpm_GetMuxEventElapsedCycles(hEvtSet,-1),BGPM_EINV_EVT_IDX);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_GetMuxEventElapsedCycles(hEvtSet,1),0);
 
    TEST_RETURN();
}

