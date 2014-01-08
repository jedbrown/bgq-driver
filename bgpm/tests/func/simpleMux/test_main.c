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
    Bgpm_MuxStats_t mStats;
    Bgpm_GetMultiplex(hEvtSet, &mStats);
    printf("\nMux Stats: numGrps=%d, numSwitches=%d, elapsedCycles=0x%016lx\n",
            mStats.numGrps, mStats.numSwitches, mStats.elapsedCycles);

    //Bgpm_DumpEvtSet(hEvtSet, 0);

    int i;
    printf("grpCycles: \n");
    for (i=0; i<mStats.numGrps; i++) {
        printf("  cycles[%02d]=0x%016lx\n", i, Bgpm_GetMuxElapsedCycles(hEvtSet, i));
    }
    printf("Counter Results:\n    ElapsedCycles       NormScaledCount     RawCount\n");
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt, muxCycles, normCycles;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        muxCycles = Bgpm_GetMuxEventElapsedCycles(hEvtSet, i);
        normCycles = 0;
        if (muxCycles > 0) {
            normCycles = (((mStats.elapsedCycles * 1000000) / muxCycles) * cnt) / 1000000;
        }
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("    0x%016lx  0x%016lx  0x%016lx <= %s\n", muxCycles, normCycles, cnt, label);
    }
}





void CreateEvents()
{
    // create some events.
#define DSIZE (1024*1024)
    volatile double target[DSIZE];
    volatile double source[DSIZE];
    unsigned i,j;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }
    for (i=0; i<DSIZE; i++) {
        for (j=0; j<6; j++) {
            target[i] *= 1.005;
            source[i] = j;
        }
    }
}



void Check(int automate, int normalize)
{
    if (automate && normalize) {
        TEST_CASE_PRINT0(Simple sanity check automated and normalized multiplexing);
    }
    else if (automate) {
        TEST_CASE_PRINT0(Simple sanity check automated and not normalized multiplexing);
    }
    else if (normalize) {
        TEST_CASE_PRINT0(Simple sanity check of manual and normalized multiplexing);
    }
    else {
        TEST_CASE_PRINT0(Simple sanity check of manual and not normalized multiplexing);
    }

    Bgpm_EnableTracePrint();

    Bgpm_Init(BGPM_MODE_SWDISTRIB);

    int hEvtSet = Bgpm_CreateEventSet();
    hEvtSet = Bgpm_CreateEventSet();
    hEvtSet = Bgpm_CreateEventSet();   // use something other than the zeroth event set.

    uint64_t period = 0;
    if (automate) period = 60000000;
    Bgpm_SetMultiplex(hEvtSet, period, normalize);

    //Bgpm_DumpEvtSet(hEvtSet, 0);


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
    Bgpm_PrintCurrentPunitReservations(hEvtSet);


    //Bgpm_DumpEvtSet(hEvtSet, 0);

    //UPC_P_Dump_State(Kernel_ProcessorCoreID());
    int i;

    printf("\napply, start:\n");
    Bgpm_Apply(hEvtSet);
    CreateEvents();  // warmup cache
    const int loops = 103;
    Bgpm_Start(hEvtSet);
    for (i=0; i<loops; i++) {
        CreateEvents();
        if (!automate) Bgpm_SwitchMux(hEvtSet);
    }
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);

    //Bgpm_DumpEvtSet(hEvtSet, 0);


    #if 0
    Upci_Punit_Dump(0, &punit);
    UPC_P_Dump_State(Kernel_ProcessorCoreID());
    UPC_C_Dump_State();
    #endif

    printf("\napply, start:\n");
    Bgpm_Apply(hEvtSet);
    Bgpm_Start(hEvtSet);
    for (i=0; i<loops; i++) {
        CreateEvents();
        if (!automate) Bgpm_SwitchMux(hEvtSet);
    }
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);

    printf("\nreset, start:\n");
    Bgpm_Reset(hEvtSet);
    Bgpm_Start(hEvtSet);
    for (i=0; i<loops; i++) {
        CreateEvents();
        if (!automate) Bgpm_SwitchMux(hEvtSet);
    }
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);

    printf("\nreset, start:\n");
    Bgpm_Reset(hEvtSet);
    Bgpm_Start(hEvtSet);
    for (i=0; i<loops; i++) {
        CreateEvents();
        if (!automate) Bgpm_SwitchMux(hEvtSet);
    }
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);

    printf("\nresetstart:\n");
    Bgpm_ResetStart(hEvtSet);
    for (i=0; i<loops; i++) {
        CreateEvents();
        if (!automate) Bgpm_SwitchMux(hEvtSet);
    }
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);

    printf("\nresetstart:\n");
    Bgpm_ResetStart(hEvtSet);
    for (i=0; i<loops; i++) {
        CreateEvents();
        if (!automate) Bgpm_SwitchMux(hEvtSet);
    }
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);


    const BgpmDebugThreadVars_t *pBgpmVars = Bgpm_DebugGetThreadVars();
    BgpmDebugPrintThreadVars(pBgpmVars);


    Bgpm_DumpEvtSet(hEvtSet, 0);
    //Bgpm_PrintCurrentPunitReservations(hEvtSet);

    Bgpm_Disable();
}






int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Simple sanity check of Bgpm Multiplexing);

    Bgpm_EnableTracePrint();

    Check(0, BGPM_NOTNORMAL);  // not automated
    Check(1, BGPM_NOTNORMAL);  // automate
    //Check(1, BGPM_NORMAL);     // automate

    TEST_RETURN();
}

