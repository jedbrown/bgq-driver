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


#define PRINTERR(...) fprintf(stderr, "ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }



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
    printf("Simple sanity check 2 of Bgpm API built up using new C++ implementation \n");

    Bgpm_Init(BGPM_MODE_SWDISTRIB);


    int hEvtSet = Bgpm_CreateEventSet();

    Bgpm_AddEvent(hEvtSet, PEVT_L1P_BAS_LD);
    Bgpm_AddEvent(hEvtSet, PEVT_L1P_BAS_ST_WC);
    Bgpm_AddEvent(hEvtSet, PEVT_L1P_STRM_WRT_INVAL);
    Bgpm_AddEvent(hEvtSet, PEVT_XU_BR_COMMIT);
    Bgpm_AddEvent(hEvtSet, PEVT_CYCLES);

    printf("NumEvents = %d\n", Bgpm_NumEvents(hEvtSet));

    Bgpm_Apply(hEvtSet);

    //UPC_P_Dump_State(Kernel_ProcessorCoreID());

    Bgpm_Start(hEvtSet);
    CreateEvents();
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);

    #if 0
    Upci_Punit_Dump(0, &punit);
    UPC_P_Dump_State(Kernel_ProcessorCoreID());
    UPC_C_Dump_State();
    #endif


    // repeat
    Bgpm_ResetStart(hEvtSet);
    CreateEvents();
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);

    // try fast operations
    Bgpm_Reset(hEvtSet);
    Bgpm_Punit_Handles_t hCtrs;
    Bgpm_Punit_GetHandles(hEvtSet, &hCtrs);
    Bgpm_Punit_Start(hCtrs.allCtrMask);
    CreateEvents();
    Bgpm_Punit_Stop(hCtrs.allCtrMask);
    Upci_Delay(800);
    Print_Punit_Counts(hEvtSet, &hCtrs);

    // try low latency operations
    Bgpm_Disable();
    {
        Bgpm_Init(BGPM_MODE_LLDISTRIB);

        int hEvtSet = Bgpm_CreateEventSet();

        Bgpm_AddEvent(hEvtSet, PEVT_L1P_BAS_LD);
        Bgpm_AddEvent(hEvtSet, PEVT_L1P_BAS_ST_WC);
        Bgpm_AddEvent(hEvtSet, PEVT_L1P_STRM_WRT_INVAL);
        Bgpm_AddEvent(hEvtSet, PEVT_XU_BR_COMMIT);
        Bgpm_AddEvent(hEvtSet, PEVT_CYCLES);
        int cycleIdx = 4;

        printf("NumEvents = %d\n", Bgpm_NumEvents(hEvtSet));

        Bgpm_Apply(hEvtSet);

        //UPC_P_Dump_State(Kernel_ProcessorCoreID());


        Bgpm_Punit_LLHandles_t hCtrs;
        Bgpm_Punit_GetLLHandles(hEvtSet, &hCtrs);
        Bgpm_Punit_LLResetStart(hCtrs.allCtrMask);
        CreateEvents();
        Bgpm_Punit_LLStop(hCtrs.allCtrMask);
        Print_Punit_LLCounts(hEvtSet, &hCtrs);
        uint64_t count = Bgpm_Punit_LLRead(hCtrs.hCtr[cycleIdx]);
        printf("overfowing cycle count=0x%016lx\n", count);
        if (! BGPM_LLCOUNT_OVERFLOW(count)) {
            DIE("Cycle Count 0x%016lx didn't overflow\n", count);
        }
        else {
            printf("cycle count 0x%016lx overflowed as expected\n", count);
        }
    }




    return 0;
}

