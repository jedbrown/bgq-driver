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
#include <omp.h>
#include "string.h"

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"
#include "spi/include/l1p/sprefetch.h"


//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


int hEvtSets[64];
Bgpm_Punit_LLHandles_t hLL[64];
Bgpm_Punit_Handles_t hPu[64];
int num_events=0;
#define RESET_OVF_COUNTS(ovfarray) memset((void*)ovfarray, 0, sizeof(ovfarray))
#define RESET_ALL_OVF_COUNTS  \
        RESET_OVF_COUNTS(numOvfs1x); \
        RESET_OVF_COUNTS(numOvfs3x); \
        RESET_OVF_COUNTS(numOvfs6x); \
        RESET_OVF_COUNTS(numOvfs9x); \
        RESET_OVF_COUNTS(numOvfs12x);


#define MAX_COUNTERS 24
#define NUM_TOP_ADDRS  2
//thds counter
uint64_t  counts1x[64][MAX_COUNTERS]; // count values from 1x pass
int numOvfs1x[64][MAX_COUNTERS];

uint64_t  counts3x[64][MAX_COUNTERS]; // count values from 3x pass
int numOvfs3x[64][MAX_COUNTERS];

uint64_t  counts6x[64][MAX_COUNTERS];
int numOvfs6x[64][MAX_COUNTERS];

uint64_t  counts9x[64][MAX_COUNTERS]; // count values from 9x pass
int numOvfs9x[64][MAX_COUNTERS];

uint64_t  counts12x[64][MAX_COUNTERS];
int numOvfs12x[64][MAX_COUNTERS];



void GetCounts(unsigned hEvtSet, uint64_t counts[][MAX_COUNTERS])
{
    unsigned numEvts = Bgpm_NumEvents(hEvtSet);
    int thd = Kernel_ProcessorID();

    Bgpm_ReadEventList(hEvtSet, &(counts[thd][0]), &numEvts);
}




int GetTotalOvfs(unsigned hEvtSet, int ovfs[MAX_COUNTERS])
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int idx;
    int total = 0;
    for (idx=0; idx<numEvts; idx++) {
        total += ovfs[idx];
    }
    return total;
}




void CompareCounts(int hwThd, int num, uint64_t countsA[MAX_COUNTERS], uint64_t countsB[MAX_COUNTERS], double low, double hi, double scale)
{
    char strg[100];
    int idx;
    for (idx=0; idx<num; ++idx) {
        sprintf(strg, "thd=%02d,idx=%02d", hwThd, idx);
        TEST_CHECK_RANGEDBL_wLABEL(countsB[idx], countsA[idx]*scale*low, countsA[idx]*scale*hi, strg);
    }
}



void CheckOverflows(int hwThd, unsigned hEvtSet, uint64_t counts[MAX_COUNTERS], int ovfs[MAX_COUNTERS], double low, double hi)
{
    // verify that the number of overflows roughly corresponds to the expected values.
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int ovfsAreActive = 0;
    unsigned idx;
    unsigned expNumOvfs = 0;
    for (idx=0; idx<numEvts; ++idx) {
        uint64_t period;
        Bgpm_OverflowHandler_t handler;
        Bgpm_GetOverflow(hEvtSet, idx, &period, &handler);
        if (period > 0) {
            ovfsAreActive = 1;
            expNumOvfs += counts[idx] / period;
        }
    }

    char strg[100];

    if (ovfsAreActive) {
        int totalOvfs = GetTotalOvfs(hEvtSet, ovfs);
        TEST_CHECK_NONZERO(totalOvfs);
        sprintf(strg, "thd=%02d", hwThd);
        TEST_CHECK_RANGEDBL_wLABEL(totalOvfs, (expNumOvfs*0.85), (expNumOvfs*1.15), strg);
    }

}



void PrintCounts(unsigned hEvtSet)
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int i;
    // calc label width
    int lblWidth = 0;
    for (i=0; i<numEvts; ++i) {
        lblWidth = MAX(lblWidth, strlen(Bgpm_GetEventLabel(hEvtSet, i)));
    }
    char format1[100];
    char format2[100];
    sprintf(format1, "   %%12ld <- %%-%ds\n", lblWidth);
    sprintf(format2, "   %%12ld <- %%-%ds ovfPeriod=%%d\n", lblWidth);


    fprintf(stderr, "NumEvents=%d:\n", numEvts);
    for (i=0; i<numEvts; ++i) {
        uint64_t val;
        Bgpm_ReadEvent(hEvtSet, i, &val);

        uint64_t period;
        Bgpm_OverflowHandler_t handler;
        Bgpm_GetOverflow(hEvtSet, i, &period, &handler);

        if (period) {
            fprintf(stderr, format2, val, Bgpm_GetEventLabel(hEvtSet, i), period);
        }
        else {
            fprintf(stderr, format1, val, Bgpm_GetEventLabel(hEvtSet, i));
        }
    }
}



int (*pCurOvfs)[][MAX_COUNTERS];  // pointer to current overflow count array
void OvfHandler(int hEvtSet, uint64_t address, uint64_t ovfVector, const ucontext_t *pContext)
{
    unsigned thd = Kernel_ProcessorID();
    //fprintf(stderr, _AT_ " hEvtSet=%d, address=0x%016lx, ovfVector=0x%016lx, pContext=%p\n",
    //        hEvtSet, address, ovfVector, pContext);
    //PrintCounts(hEvtSets[thd]);

    // Get the indices of all events which have overflowed.
    unsigned ovfIdxs[BGPM_MAX_OVERFLOW_EVENTS];
    unsigned len = BGPM_MAX_OVERFLOW_EVENTS;
    Bgpm_GetOverflowEventIndices(hEvtSet, ovfVector, ovfIdxs, &len);

    // count hits for this event index
    unsigned i;
    for (i=0; i<len; i++) {
        unsigned idx = ovfIdxs[i];
        (*pCurOvfs)[thd][idx]++;
    }

}



int numThreads;
int numProcs;


void Init6EventsNoOvf(int *phEvtSet)
{
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    int hEvtSet = Bgpm_CreateEventSet();

    unsigned evtList[] = {
        PEVT_CYCLES,  // will overflow
        PEVT_CYCLES,  // will not overflow
        PEVT_LSU_COMMIT_LD_MISSES, // will overflow
        PEVT_INST_ALL,
        PEVT_INST_QFPU_FMUL,  // will overflow
        PEVT_INST_QFPU_FMUL,  // 2nd one will not
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    Bgpm_Apply(hEvtSet);

    Bgpm_Punit_GetHandles(hEvtSet, &hPu[Kernel_ProcessorID()]);
    Bgpm_Punit_GetLLHandles(hEvtSet, &hLL[Kernel_ProcessorID()]);
    //Bgpm_DumpEvtSet(hEvtSet, 0);

    *phEvtSet = hEvtSet;
}




void Init6Events(int *phEvtSet)
{
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    int hEvtSet = Bgpm_CreateEventSet();

    unsigned evtList[] = {
        PEVT_CYCLES,  // will overflow
        PEVT_CYCLES,  // will not overflow
        PEVT_LSU_COMMIT_LD_MISSES, // will overflow
        PEVT_INST_ALL,
        PEVT_INST_QFPU_FMUL,  // will overflow
        PEVT_INST_QFPU_FMUL,  // 2nd one will not
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), 500000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_FMUL, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_FMUL, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflowHandler(hEvtSet, OvfHandler);
    Bgpm_Apply(hEvtSet);

    Bgpm_Punit_GetHandles(hEvtSet, &hPu[Kernel_ProcessorID()]);
    Bgpm_Punit_GetLLHandles(hEvtSet, &hLL[Kernel_ProcessorID()]);
    //Bgpm_DumpEvtSet(hEvtSet, 0);

    *phEvtSet = hEvtSet;
}




void Init8Events(int *phEvtSet)
{
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    int hEvtSet = Bgpm_CreateEventSet();

    unsigned evtList[] = {
        PEVT_CYCLES,  // will overflow
        PEVT_CYCLES,  // will not overflow
        PEVT_LSU_COMMIT_LD_MISSES, // will overflow
        PEVT_INST_ALL,
        PEVT_INST_QFPU_FMUL,  // will overflow
        PEVT_INST_QFPU_FMUL,  // 2nd one will not
        PEVT_L1P_BAS_LD,  // Extra Events
        PEVT_L1P_BAS_MISS, // Extra Events - should cause shared overflow.
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), 500000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_FMUL, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_FMUL, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflowHandler(hEvtSet, OvfHandler);
    Bgpm_Apply(hEvtSet);

    Bgpm_Punit_GetHandles(hEvtSet, &hPu[Kernel_ProcessorID()]);
    Bgpm_Punit_GetLLHandles(hEvtSet, &hLL[Kernel_ProcessorID()]);
    //Bgpm_DumpEvtSet(hEvtSet, 0);

    *phEvtSet = hEvtSet;
}



void Init9Events(int *phEvtSet) 
{
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    int hEvtSet = Bgpm_CreateEventSet();

    unsigned evtList[] = {
        PEVT_CYCLES,  // will overflow
        PEVT_CYCLES,  // will not overflow
        PEVT_LSU_COMMIT_LD_MISSES, // will overflow
        PEVT_INST_ALL,
        PEVT_INST_ALL,
        PEVT_INST_QFPU_FMUL,  // will overflow
        PEVT_INST_QFPU_FMUL,  // 2nd one will not
        PEVT_L1P_BAS_LD,  // Extra Events
        PEVT_L1P_BAS_MISS, // Extra Events - should cause shared overflow.
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), 500000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_FMUL, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_FMUL, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflowHandler(hEvtSet, OvfHandler);
    Bgpm_Apply(hEvtSet);

    Bgpm_Punit_GetHandles(hEvtSet, &hPu[Kernel_ProcessorID()]);
    Bgpm_Punit_GetLLHandles(hEvtSet, &hLL[Kernel_ProcessorID()]);
    //Bgpm_DumpEvtSet(hEvtSet, 0);

    *phEvtSet = hEvtSet;
 }

void Init12Events(int *phEvtSet)
{
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    int hEvtSet = Bgpm_CreateEventSet();

    unsigned evtList[] = {
    PEVT_LSU_COMMIT_LD_MISSES,
    PEVT_AXU_FP_EXCEPT,
    PEVT_INST_QFPU_FPGRP2,
    PEVT_WAKE_HIT_10,
    PEVT_INST_XU_FST,
    PEVT_INST_QFPU_FMUL,
    PEVT_INST_QFPU_QCVT,
    PEVT_INST_QFPU_FPGRP1,
    PEVT_AXU_FP_EXCEPT,
    PEVT_XU_RUN_INSTR,
    PEVT_INST_XU_FLD,
    PEVT_CYCLES
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), 500000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_FMUL, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_FMUL, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_AXU_FP_EXCEPT, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet,PEVT_AXU_FP_EXCEPT , 0), GetNewProfileMapHandle());

    Bgpm_SetOverflowHandler(hEvtSet, OvfHandler);
    Bgpm_Apply(hEvtSet);

    Bgpm_Punit_GetHandles(hEvtSet, &hPu[Kernel_ProcessorID()]);
    Bgpm_Punit_GetLLHandles(hEvtSet, &hLL[Kernel_ProcessorID()]);
    //Bgpm_DumpEvtSet(hEvtSet, 0);
    *phEvtSet = hEvtSet;
}



void SampleAll6Events(int *phEvtSet)
{
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    int hEvtSet = Bgpm_CreateEventSet();

    // events to collect are rather arbitrary, except for picking events
    // which can be collected simultaneously and should actually produce significant numbers.
    unsigned evtList[] = {
        PEVT_CYCLES,  // will overflow
        PEVT_LSU_COMMIT_LD_MISSES, // will overflow
        PEVT_INST_ALL,
        PEVT_INST_QFPU_FMUL,  // will overflow
        PEVT_INST_QFPU_FMUL,
        PEVT_IU_IS1_STALL_CYC,
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    int i;
    for (i=0; i<Bgpm_NumEvents(hEvtSet); i++) {
        Bgpm_SetOverflow(hEvtSet, i, 50000+i*1000);
        Bgpm_SetEventUser1(hEvtSet, i, GetNewProfileMapHandle());
    }
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), 500000);

    Bgpm_SetOverflowHandler(hEvtSet, OvfHandler);
    Bgpm_Apply(hEvtSet);

    Bgpm_Punit_GetHandles(hEvtSet, &hPu[Kernel_ProcessorID()]);
    Bgpm_Punit_GetLLHandles(hEvtSet, &hLL[Kernel_ProcessorID()]);
    //Bgpm_DumpEvtSet(hEvtSet, 0);

    *phEvtSet = hEvtSet;
}


void SampleManyEvents(int *phEvtSet)
{
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    int hEvtSet = Bgpm_CreateEventSet();

    unsigned evtList[] = {
         PEVT_CYCLES,  // will overflow
         PEVT_LSU_COMMIT_LD_MISSES, // will overflow
         PEVT_INST_ALL,
         PEVT_INST_QFPU_FMUL,  // will overflow
         PEVT_INST_QFPU_FMUL,
         PEVT_IU_IS1_STALL_CYC,
         PEVT_IU_INSTR_FETCHED,
         PEVT_IU_FXU_ISSUE_COUNT,
         PEVT_IU_TOT_ISSUE_COUNT,
         PEVT_LSU_COMMIT_STS,
         PEVT_LSU_COMMIT_CACHEABLE_LDS,
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    int i;
    for (i=0; i<Bgpm_NumEvents(hEvtSet); i++) {
        Bgpm_SetOverflow(hEvtSet, i, 50000+i*1000);
        Bgpm_SetEventUser1(hEvtSet, i, GetNewProfileMapHandle());
    }
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), 500000);

    Bgpm_SetOverflowHandler(hEvtSet, OvfHandler);
    Bgpm_Apply(hEvtSet);

    Bgpm_Punit_GetHandles(hEvtSet, &hPu[Kernel_ProcessorID()]);
    Bgpm_Punit_GetLLHandles(hEvtSet, &hLL[Kernel_ProcessorID()]);
    //Bgpm_DumpEvtSet(hEvtSet, 0);

    *phEvtSet = hEvtSet;
}




void SampleMuxEvents(int *phEvtSet)
{
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    int hEvtSet = Bgpm_CreateEventSet();

    SHMC_ThreadBarrier();
    if (Kernel_ProcessorID() == 0) {
        //Bgpm_DebugDumpShrMem();
    }
    SHMC_ThreadBarrier();

    Bgpm_SetMultiplex(hEvtSet, 20000, 0);

    unsigned evtList[] = {
         PEVT_CYCLES,  // will overflow
         PEVT_LSU_COMMIT_LD_MISSES, // will overflow
         PEVT_INST_ALL,
         PEVT_INST_QFPU_FMUL,  // will overflow
         PEVT_INST_QFPU_FMUL,
         PEVT_IU_IS1_STALL_CYC,
         PEVT_IU_INSTR_FETCHED,
         PEVT_IU_FXU_ISSUE_COUNT,
         PEVT_IU_TOT_ISSUE_COUNT,
         PEVT_LSU_COMMIT_STS,
         PEVT_LSU_COMMIT_CACHEABLE_LDS,
         PEVT_XU_BR_MISPRED_COMMIT,
         PEVT_INST_XU_FLD,
         PEVT_INST_XU_IADD,
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    int i;
    for (i=0; i<Bgpm_NumEvents(hEvtSet); i++) {
        Bgpm_SetOverflow(hEvtSet, i, 50000+i*1000);
        Bgpm_SetEventUser1(hEvtSet, i, GetNewProfileMapHandle());
    }
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), 500000);

    Bgpm_SetOverflowHandler(hEvtSet, OvfHandler);
    Bgpm_Apply(hEvtSet);

    //Bgpm_Punit_GetHandles(hEvtSet, &hPu[Kernel_ProcessorID()]);
   // Bgpm_Punit_GetLLHandles(hEvtSet, &hLL[Kernel_ProcessorID()]);
    //Bgpm_DumpEvtSet(hEvtSet, 0);

    *phEvtSet = hEvtSet;
}







void Exercise(unsigned hEvtSet, int loops, uint64_t counts[][MAX_COUNTERS], int (*numOvfs)[][MAX_COUNTERS])
{

        L1P_SetStreamPolicy(L1P_stream_disable);

        // make sure counts start clear
        memset(counts,0,sizeof(counts12x));
        memset((void*)Bgpm_DebugGetThreadVars(),0,sizeof(BgpmDebugThreadVars_t));
        pCurOvfs = numOvfs;

        // Create some data space to create cache misses
        int i, j;
        //long long j;
        // Array Sizes  cachelines    #dbls   factor to
        //              per l1 cache  /line   flush cache
        #define DSIZE   (256   *       8     *  2)           // size needed to flush cache
        volatile double target[DSIZE];
        volatile double source[DSIZE];
        for (i=0; i<DSIZE; i++) {
            source[i] = i;
            target[i] = i+1;
        }

        // Exercise something - one to cause cache misses, and the other to cause
        // a lot of instructions w/o misses, another to get FP ops.
        #pragma omp barrier

        for (j=0; j<loops+2; ++j) {
            // stride by cache line size.
            for (i=DSIZE-1; i>=0; i-=8) {

                asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");

                target[i] = source[i];  // here is where the misses should occur

                asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");

                target[i] = source[i] * 5.0;  // fp multiple should occur here.

                asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
                asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
            }
            if (j == 0) {  // do a warmup loop 1st before starting counting.
                Bgpm_ResetStart(hEvtSet);
            }
            else if (j == loops) {  // do a warm down loop after counting (so multiple threads not switching activies)
                Bgpm_Stop(hEvtSet);
            }
        }


        GetCounts(hEvtSet, counts);
        //GetAddrs(hEvtSet, addrs);

        #pragma omp barrier

        // verify that counters are truely stopped
        if ((Kernel_ProcessorCoreID() % 4) == 0) {
            TEST_CHECK_EQUAL_HEX(upc_p_local->control, 0);
        }


        #if 0 // TEST_CASE_VERBOSE
        // Print Results each event.
        #pragma omp critical (PrintEvts)
        {
                PrintCounts(hEvtSet);
                fprintf(stderr, "NumOvfs[%d]=%d\n", hwThd, numOvfs[hwThd]);
        //        BgpmDebugPrintThreadVars();
                fprintf(stderr, "\n");
        }
        #endif


        #pragma omp barrier

}



#define LINELEN 200

void BuildFormattedCountLabels(char line[][LINELEN], int pos[], unsigned hEvtSet)
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int idx = 0;
    pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "%40s ", "Labels");
    pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "%9s ", "Period");

    for (idx=1; idx<numEvts+1; idx++) {
         pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "%40s ", Bgpm_GetEventLabel(hEvtSet, idx-1));

         uint64_t period;
         Bgpm_OverflowHandler_t handler;
         Bgpm_GetOverflow(hEvtSet, idx-1, &period, &handler);

         pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "%9ld ", period ? period : 0UL);
    }

    idx = numEvts+1;
    pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "%40s %9s ", "Overflows", "n/a");

}



void AppendFormattedCounts(char line[][LINELEN], int pos[], unsigned hEvtSet, uint64_t counts[], unsigned ovfs, char *scaleLabel )
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int idx = 0;
    pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "%12s ", scaleLabel);

    for (idx=1; idx<numEvts+1; idx++) {
         pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "%12ld ", counts[idx-1]);
    }

    idx = numEvts+1;
    pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "%12d ", ovfs);
}




#define TOVF(_buff) GetTotalOvfs(hEvtSet, _buff[hwThd])

void AppendFormattedOverflow(char line[][LINELEN], int pos[], unsigned hEvtSet, int ovfs[], char *label )
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int idx = 0;
    pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "[%6s] ", label);

    for (idx=1; idx<numEvts+1; idx++) {
         pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "[%6d] ", ovfs[idx-1]);
    }

    idx = numEvts+1;
    pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "[%6d] ", GetTotalOvfs(hEvtSet, ovfs));
}



void AppendFormattedPercent(char line[][LINELEN], int pos[], unsigned hEvtSet, uint64_t countsa[],  uint64_t countsb[], unsigned ovfa, unsigned ovfb )
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int idx = 0;
    pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "(%4s) ", "1x%");

    for (idx=1; idx<numEvts+1; idx++) {
         pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "(%4ld) ", (countsb[idx-1]*100)/countsa[idx-1]);
    }

    idx = numEvts+1;
    pos[idx] += snprintf(line[idx]+pos[idx], LINELEN-pos[idx], "(%4d) ", (ovfb*100)/ovfa);
}



void PrintFormattedLines(char line[][LINELEN], unsigned hEvtSet )
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int idx = 0;
    for (idx=0; idx<numEvts+2; idx++) {
         fprintf(stdout, "%s\n", line[idx]);
    }
    fprintf(stdout, "\n");
}



#define TOVF(_buff) GetTotalOvfs(hEvtSet, _buff[hwThd])
void PrintResults(unsigned hEvtSet, int hwThd)
{
    SHMC_SERIAL_Start(numProcs,numThreads);
    char line[MAX_COUNTERS+2][LINELEN]; memset((void*)line, 0, sizeof(line));
    int  pos[MAX_COUNTERS+2]; memset((void*)pos, 0, sizeof(pos));
    BuildFormattedCountLabels(line, pos, hEvtSet);

    AppendFormattedCounts(line, pos, hEvtSet, counts1x[hwThd],  TOVF(numOvfs1x),  "1x");
    AppendFormattedOverflow(line, pos, hEvtSet, numOvfs1x[hwThd], "ovfs" );

    AppendFormattedCounts(line, pos, hEvtSet, counts3x[hwThd],  TOVF(numOvfs3x),  "3x");
    AppendFormattedPercent(line, pos, hEvtSet, counts1x[hwThd],  counts3x[hwThd], TOVF(numOvfs1x), TOVF(numOvfs3x) );
    AppendFormattedOverflow(line, pos, hEvtSet, numOvfs3x[hwThd], "ovfs" );

    AppendFormattedCounts(line, pos, hEvtSet, counts6x[hwThd],  TOVF(numOvfs6x),  "6x");
    AppendFormattedPercent(line, pos, hEvtSet, counts1x[hwThd],  counts6x[hwThd], TOVF(numOvfs1x), TOVF(numOvfs6x) );
    AppendFormattedOverflow(line, pos, hEvtSet, numOvfs6x[hwThd], "ovfs" );

    AppendFormattedCounts(line, pos, hEvtSet, counts9x[hwThd],  TOVF(numOvfs9x),  "9x");
    AppendFormattedPercent(line, pos, hEvtSet, counts1x[hwThd],  counts9x[hwThd], TOVF(numOvfs1x), TOVF(numOvfs9x) );
    AppendFormattedOverflow(line, pos, hEvtSet, numOvfs9x[hwThd], "ovfs" );

    AppendFormattedCounts(line, pos, hEvtSet, counts12x[hwThd], TOVF(numOvfs12x), "12x");
    AppendFormattedPercent(line, pos, hEvtSet, counts1x[hwThd],  counts12x[hwThd], TOVF(numOvfs1x), TOVF(numOvfs12x) );
    AppendFormattedOverflow(line, pos, hEvtSet, numOvfs12x[hwThd], "ovfs" );

    // Let's only print stuff on core 0 even though running multiples - just to make it comprehendable.
    // But, want to see comparison of results on same core.
    //if (hwThd < 4) {
        printf("\nhwThd=%d\n", hwThd);
        PrintFormattedLines(line, hEvtSet);
    //}
    SHMC_SERIAL_End();

}



void CompareAllCounts(unsigned hEvtSet, int hwThd)
{
    SHMC_SERIAL_Start(numProcs,numThreads);
    int numEvts = Bgpm_NumEvents(hEvtSet);
    CompareCounts(hwThd, numEvts, counts1x[hwThd], counts3x[hwThd], 0.90, 1.10, 3.0);
    CompareCounts(hwThd, numEvts, counts1x[hwThd], counts6x[hwThd], 0.90, 1.10, 6.0);
    CompareCounts(hwThd, numEvts, counts1x[hwThd], counts9x[hwThd], 0.90, 1.10, 9.0);
    CompareCounts(hwThd, numEvts, counts1x[hwThd], counts12x[hwThd], 0.90, 1.10, 12.0);
    SHMC_SERIAL_End();
}



void CheckAllOverflows(unsigned hEvtSet, int hwThd)
{
    SHMC_SERIAL_Start(numProcs,numThreads);
    //fprintf(stderr, _AT_ " hEvtSet=%d, numEvents=%d\n", hEvtSet, Bgpm_NumEvents(hEvtSet));

    CheckOverflows(hwThd, hEvtSet, counts1x[hwThd],  numOvfs1x[hwThd], 0.90, 1.10);
    CheckOverflows(hwThd, hEvtSet, counts3x[hwThd],  numOvfs3x[hwThd], 0.90, 1.10);
    CheckOverflows(hwThd, hEvtSet, counts6x[hwThd],  numOvfs6x[hwThd], 0.90, 1.10);
    CheckOverflows(hwThd, hEvtSet, counts9x[hwThd],  numOvfs9x[hwThd], 0.90, 1.10);
    CheckOverflows(hwThd, hEvtSet, counts12x[hwThd], numOvfs12x[hwThd], 0.90, 1.10);
    SHMC_SERIAL_End();
}



int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Fast Threaded Overflow Scaling);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);
    //PrintWhoAmI();
    //Upci_DumpNodeParms(0);


    SHMC_Init_Barriers(numProcs, numThreads);

    const int loops = 10000;

    #pragma omp parallel default(none) num_threads(numThreads)                           \
        shared(stderr,numThreads,numProcs,hEvtSets,                                      \
               pCurOvfs, numOvfs1x, numOvfs3x, numOvfs6x, numOvfs9x, numOvfs12x,         \
               counts1x,counts3x,counts6x,counts9x,counts12x,                            \
               numFailures)
    {
        int hwThd = Kernel_ProcessorID();
        int hEvtSet;

    #if 1
        RESET_ALL_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE_PRINT0(Run events w/o overflow to get counts);
        Init6EventsNoOvf(&hEvtSet); hEvtSets[hwThd] = hEvtSet;
        #pragma omp barrier
        Exercise(hEvtSet, loops,     counts1x,  &numOvfs1x);
        Exercise(hEvtSet, loops*3,   counts3x,  &numOvfs3x);
        Exercise(hEvtSet, loops*6,   counts6x,  &numOvfs6x);
        Exercise(hEvtSet, loops*9,   counts9x,  &numOvfs9x);
        Exercise(hEvtSet, loops*12,  counts12x, &numOvfs12x);
        PrintResults(hEvtSet, hwThd);
        CompareAllCounts(hEvtSet, hwThd);
        CheckAllOverflows(hEvtSet, hwThd);
        Bgpm_Disable();

        //Compare3XCounts(6, counts1x, counts3x, 0.85, 1.15);
        //UPC_C_Dump_Counters(0, 16*2+1);
    #endif

    #if 1
        RESET_ALL_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE_PRINT0(Fast Punit Overflows < 6 ctrs per thread);
        Init6Events(&hEvtSet); hEvtSets[hwThd] = hEvtSet;
        #pragma omp barrier
        Exercise(hEvtSet, loops,     counts1x,  &numOvfs1x);
        Exercise(hEvtSet, loops*3,   counts3x,  &numOvfs3x);
        Exercise(hEvtSet, loops*6,   counts6x,  &numOvfs6x);
        Exercise(hEvtSet, loops*9,   counts9x,  &numOvfs9x);
        Exercise(hEvtSet, loops*12,  counts12x, &numOvfs12x);
        PrintResults(hEvtSet, hwThd);
        CompareAllCounts(hEvtSet, hwThd);
        CheckAllOverflows(hEvtSet, hwThd);
        Bgpm_Disable();
    #endif

    #if 1
        RESET_ALL_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE_PRINT0(Fast Shared Punit Overflows > 6 ctrs per thread);
        Init8Events(&hEvtSet); hEvtSets[hwThd] = hEvtSet;
        #pragma omp barrier
        Exercise(hEvtSet, loops,     counts1x,  &numOvfs1x);
        Exercise(hEvtSet, loops*3,   counts3x,  &numOvfs3x);
        Exercise(hEvtSet, loops*6,   counts6x,  &numOvfs6x);
        Exercise(hEvtSet, loops*9,   counts9x,  &numOvfs9x);
        Exercise(hEvtSet, loops*12,  counts12x, &numOvfs12x);
        PrintResults(hEvtSet, hwThd);
        CompareAllCounts(hEvtSet, hwThd);
        CheckAllOverflows(hEvtSet, hwThd);
        Bgpm_Disable();
    #endif

    #if 1
        RESET_ALL_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE_PRINT0(Fast Shared Punit Overflows 12 ctrs per thread);
        Init12Events(&hEvtSet); hEvtSets[hwThd] = hEvtSet;
        #pragma omp barrier
        Exercise(hEvtSet, loops,     counts1x,  &numOvfs1x);
        Exercise(hEvtSet, loops*3,   counts3x,  &numOvfs3x);
        Exercise(hEvtSet, loops*6,   counts6x,  &numOvfs6x);
        Exercise(hEvtSet, loops*9,   counts9x,  &numOvfs9x);
        Exercise(hEvtSet, loops*12,  counts12x, &numOvfs12x);
        PrintResults(hEvtSet, hwThd);
        CompareAllCounts(hEvtSet, hwThd);
        CheckAllOverflows(hEvtSet, hwThd);
        Bgpm_Disable();
    #endif

    #if 1
        RESET_ALL_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE_PRINT0(Sample all Fast Shared Punit Overflows = ctrs per thread);
        SampleAll6Events(&hEvtSet); hEvtSets[hwThd] = hEvtSet;
        #pragma omp barrier
        Exercise(hEvtSet, loops,     counts1x,  &numOvfs1x);
        Exercise(hEvtSet, loops*3,   counts3x,  &numOvfs3x);
        Exercise(hEvtSet, loops*6,   counts6x,  &numOvfs6x);
        Exercise(hEvtSet, loops*9,   counts9x,  &numOvfs9x);
        Exercise(hEvtSet, loops*12,  counts12x, &numOvfs12x);
        PrintResults(hEvtSet, hwThd);
        CompareAllCounts(hEvtSet, hwThd);
        CheckAllOverflows(hEvtSet, hwThd);
        Bgpm_Disable();
    #endif

    #if 1
        RESET_ALL_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE_PRINT0(Sample all Fast Shared Punit Overflows > 6 ctrs per thread);
        SampleManyEvents(&hEvtSet); hEvtSets[hwThd] = hEvtSet;
        #pragma omp barrier
        Exercise(hEvtSet, loops,     counts1x,  &numOvfs1x);
        Exercise(hEvtSet, loops*3,   counts3x,  &numOvfs3x);
        Exercise(hEvtSet, loops*6,   counts6x,  &numOvfs6x);
        Exercise(hEvtSet, loops*9,   counts9x,  &numOvfs9x);
        Exercise(hEvtSet, loops*12,  counts12x, &numOvfs12x);
        PrintResults(hEvtSet, hwThd);
        CompareAllCounts(hEvtSet, hwThd);
        CheckAllOverflows(hEvtSet, hwThd);
        Bgpm_Disable();


        RESET_ALL_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE_PRINT0(Sample all events with multiplexing active);
        SampleMuxEvents(&hEvtSet); hEvtSets[hwThd] = hEvtSet;
        #pragma omp barrier
        Exercise(hEvtSet, loops,     counts1x,  &numOvfs1x);
        Exercise(hEvtSet, loops*3,   counts3x,  &numOvfs3x);
        Exercise(hEvtSet, loops*6,   counts6x,  &numOvfs6x);
        Exercise(hEvtSet, loops*9,   counts9x,  &numOvfs9x);
        Exercise(hEvtSet, loops*12,  counts12x, &numOvfs12x);
        PrintResults(hEvtSet, hwThd);
        CompareAllCounts(hEvtSet, hwThd);
        CheckAllOverflows(hEvtSet, hwThd);
        Bgpm_Disable();
    #endif

    

   } 
   TEST_RETURN();

}




