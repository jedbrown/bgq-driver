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


//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();



void PrintCounts(unsigned hEvtSet)
{
    printf("Counter Results:\n");
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("    0x%016lx <= %s\n", cnt, label);
    }
}



void Print_Punit_Counts(unsigned hEvtSet, Bgpm_Punit_Handles_t *phCtrs)
{
    printf("Fast Counter Results:\n");
    int i;
    int numEvts = phCtrs->numCtrs;
    for (i=0; i<numEvts; i++) {
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("    0x%016lx <= %s\n",
                Bgpm_Punit_Read(phCtrs->hCtr[i]),
                label);
    }
}



void Print_Punit_LLCounts(unsigned hEvtSet, Bgpm_Punit_LLHandles_t *phCtrs)
{
    printf("Low Latency Counter Results:\n");
    int i;
    int numEvts = phCtrs->numCtrs;
    for (i=0; i<numEvts; i++) {
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("    0x%016lx <= %s\n",
                Bgpm_Punit_LLRead(phCtrs->hCtr[i]),
                label);
    }
}



void CreateEvents()
{
    // create some events.
#define DSIZE 256
    volatile double target[DSIZE];
    volatile double source[DSIZE];
    short i,j;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }
    for (i=0; i<DSIZE; i++) {
        for (j=0; j<6; j++) {
            target[i] *= source[i];
            source[i] = j;
        }
    }
}





int main(int argc, char *argv[])
{
    TEST_CASE_PRINT0(Simple sanity check of Bgpm API HWDistrib mode);

    Bgpm_Init(BGPM_MODE_HWDISTRIB);

    unsigned hEvtSet = Bgpm_CreateEventSet();

    Bgpm_AddEvent(hEvtSet, PEVT_L1P_BAS_LD);
    Bgpm_AddEvent(hEvtSet, PEVT_L1P_BAS_ST_WC);
    Bgpm_AddEvent(hEvtSet, PEVT_L1P_STRM_WRT_INVAL);
    Bgpm_AddEvent(hEvtSet, PEVT_XU_BR_COMMIT);
    Bgpm_AddEvent(hEvtSet, PEVT_CYCLES);

    printf("NumEvents = %d\n", Bgpm_NumEvents(hEvtSet));

    #if 0
    Bgpm_DumpEvtSet(hEvtSet, 0);
    #endif

    uint64_t targMask = UPC_APP_HWTID_MASK(0);
    if (Bgpm_Attach(hEvtSet, targMask, 0) != 0) {
        fprintf(stderr, _AT_ " attached failed %s\n", Bgpm_LastErrStrg());
    }

    #if 0
    Bgpm_DumpEvtSet(hEvtSet, 0);
    UPC_P_Dump_State(Kernel_ProcessorCoreID());
    #endif

    Bgpm_Start(hEvtSet);
    CreateEvents();
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);

    // repeat
    Bgpm_Reset(hEvtSet);
    Bgpm_Start(hEvtSet);
    CreateEvents();
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);


    Bgpm_ExitOnError(BGPM_FALSE);

    TEST_CASE_PRINT0(fast operations should fail);
    Bgpm_Punit_Handles_t hCtrs;
    TEST_CHECK_EQUAL(Bgpm_Punit_GetHandles(hEvtSet, &hCtrs), BGPM_EINV_OP);


    TEST_CASE_PRINT0(low latency operations are should fail);
    Bgpm_Punit_LLHandles_t hllCtrs;
    TEST_CHECK_EQUAL(Bgpm_Punit_GetLLHandles(hEvtSet, &hllCtrs), BGPM_EINV_OP);

    Bgpm_ExitOnError(BGPM_TRUE);

    //Bgpm_DebugDumpShrMem();

    Bgpm_DeleteEventSet(hEvtSet);

    //Bgpm_DebugDumpShrMem();

    Bgpm_Disable();


    TEST_RETURN();
}

