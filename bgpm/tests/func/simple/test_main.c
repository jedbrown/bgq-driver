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

#ifdef INCL_BTMALLOC
#include "BackTraceMalloc.h"
#define  BTMCMD(_cmd_) _cmd_
#else
#define  BTMCMD(_cmd_)
#endif

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"


#define PRINTERR(...) fprintf(stderr, "ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }
#define BGPMDIE(_funcname_) { DIE(_funcname_ ": %d; %s\n", Bgpm_LastErr(), Bgpm_LastErrStrg()); }



#define ADD_EVENT(hEvtSet, evtId)  \
{                                    \
    int rc = Bgpm_AddEvent(hEvtSet, evtId); \
    if (rc < 0) BGPMDIE("Bgpm_AddEvent"); \
}



void PrintCounts(unsigned hEvtSet)
{
    fprintf(stderr, "Counter Results:\n");
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        if (Bgpm_ReadEvent(hEvtSet, i, &cnt)) BGPMDIE("Bgpm_ReadEvent");
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        if (!label) BGPMDIE("Bgpm_GetEventLabel");
        fprintf(stderr, "    0x%016lx <= %s\n", cnt, label);
    }
}



void Print_Punit_Counts(unsigned hEvtSet, Bgpm_Punit_Handles_t *phCtrs)
{
    fprintf(stderr, "Fast Counter Results:\n");
    int i;
    int numEvts = phCtrs->numCtrs;
    for (i=0; i<numEvts; i++) {
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        if (!label) BGPMDIE("Bgpm_GetEventLabel");
        fprintf(stderr, "    0x%016lx <= %s\n",
                Bgpm_Punit_Read(phCtrs->hCtr[i]),
                label);
    }
}



void Print_Punit_LLCounts(unsigned hEvtSet, Bgpm_Punit_LLHandles_t *phCtrs)
{
    fprintf(stderr, "Low Latency Counter Results:\n");
    int i;
    int numEvts = phCtrs->numCtrs;
    for (i=0; i<numEvts; i++) {
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        if (!label) BGPMDIE("Bgpm_GetEventLabel");
        fprintf(stderr, "    0x%016lx <= %s\n",
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
    //fprintf(stderr, _AT_ "\n");
    unsigned i,j;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }
    //fprintf(stderr, _AT_ "\n");

    for (i=0; i<DSIZE; i++) {
        for (j=0; j<6; j++) {
            target[i] *= source[i];
            source[i] = j;
        }
    }
    //fprintf(stderr, _AT_ "\n");

}





int main(int argc, char *argv[])
{
    BTMCMD(BTMalloc());

    fprintf(stderr, "Simple sanity check of Bgpm API\n");

    int rc = Bgpm_Init(BGPM_MODE_SWDISTRIB);
    if (rc < 0) BGPMDIE("Bgpm_Init");

    int hEvtSet = Bgpm_CreateEventSet();
    if (hEvtSet < 0) BGPMDIE("Bgpm_CreateEventSet");


    ADD_EVENT(hEvtSet, PEVT_L1P_BAS_LD);
    ADD_EVENT(hEvtSet, PEVT_L1P_BAS_ST_WC);
    ADD_EVENT(hEvtSet, PEVT_L1P_STRM_WRT_INVAL);
    ADD_EVENT(hEvtSet, PEVT_XU_BR_COMMIT);
    ADD_EVENT(hEvtSet, PEVT_CYCLES);

    fprintf(stderr, "NumEvents = %d\n", Bgpm_NumEvents(hEvtSet));


    if (Bgpm_Apply(hEvtSet)) BGPMDIE("Bgpm_Apply");

    if (Bgpm_Start(hEvtSet) < 0) BGPMDIE("Bgpm_Start");
    CreateEvents();
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);

    #if 0
    Bgpm_DumpEvtSet(hEvtSet, 0);
    UPC_P_Dump_State(Kernel_ProcessorCoreID());
    UPC_A2_Dump_SPRs();
    UPC_C_Dump_State();
    UPC_C_Dump_Counters(0,1);
    #endif


    // repeat
    if (Bgpm_ResetStart(hEvtSet) < 0) BGPMDIE("Bgpm_ResetStart");
    CreateEvents();
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);

    // try fast operations
    if (Bgpm_Reset(hEvtSet) < 0) BGPMDIE("Bgpm_Reset");
    Bgpm_Punit_Handles_t hCtrs;
    if (Bgpm_Punit_GetHandles(hEvtSet, &hCtrs)) BGPMDIE("Bgpm_Punit_GetHandles");
    Bgpm_Punit_Start(hCtrs.allCtrMask);
    CreateEvents();
    Bgpm_Punit_Stop(hCtrs.allCtrMask);
    Upci_Delay(800);
    Print_Punit_Counts(hEvtSet, &hCtrs);

    // try low latency operations
    Bgpm_Disable();

    BTMCMD(BTMark("FastDisable"));

    {
        rc = Bgpm_Init(BGPM_MODE_LLDISTRIB);
        if (rc < 0) BGPMDIE("Bgpm_Init");

        int hEvtSet = Bgpm_CreateEventSet();
        if (hEvtSet < 0) BGPMDIE("Bgpm_CreateEventSet");

        ADD_EVENT(hEvtSet, PEVT_L1P_BAS_LD);
        ADD_EVENT(hEvtSet, PEVT_L1P_BAS_ST_WC);
        ADD_EVENT(hEvtSet, PEVT_L1P_STRM_WRT_INVAL);
        ADD_EVENT(hEvtSet, PEVT_XU_BR_COMMIT);
        ADD_EVENT(hEvtSet, PEVT_CYCLES);
        int cycleIdx = 4;

        fprintf(stderr, "NumEvents = %d\n", Bgpm_NumEvents(hEvtSet));

        if (Bgpm_Apply(hEvtSet)) BGPMDIE("Bgpm_Apply");

        //UPC_P_Dump_State(Kernel_ProcessorCoreID());
        //fprintf(stderr, _AT_ "\n");

        Bgpm_Punit_LLHandles_t hCtrs;
        if (Bgpm_Punit_GetLLHandles(hEvtSet, &hCtrs)) BGPMDIE("Bgpm_Punit_GetLLHandles");

        Bgpm_Punit_LLResetStart(hCtrs.allCtrMask);

        //fprintf(stderr, _AT_ "\n");
        CreateEvents();
        //fprintf(stderr, _AT_ "\n");

        Bgpm_Punit_LLStop(hCtrs.allCtrMask);
        Print_Punit_LLCounts(hEvtSet, &hCtrs);
        //fprintf(stderr, _AT_ "\n");


        uint64_t count = Bgpm_Punit_LLRead(hCtrs.hCtr[cycleIdx]);
        fprintf(stderr, "count=0x%016lx\n", count);
        if (! BGPM_LLCOUNT_OVERFLOW(count)) {
            DIE("Cycle Count didn't overflow\n");
        }

        Bgpm_Disable();

        BTMCMD(BTMark("LowLatDisable"));

    }




    fprintf(stderr, _AT_ "\n");

    return 0;
}

