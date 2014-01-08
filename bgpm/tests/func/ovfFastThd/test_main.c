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

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


int hEvtSets[64];
Bgpm_Punit_LLHandles_t hLL[64];
Bgpm_Punit_Handles_t hPu[64];
int numOvfs[64];
#define RESET_OVF_COUNTS memset((void*)numOvfs, 0, sizeof(numOvfs))


#define MAX_COUNTERS 24
#define NUM_TOP_ADDRS  2
//              thds counter
uint64_t  counts1x[64][MAX_COUNTERS]; // count values from 1x pass
QuikHit_t addrs1x[64][MAX_COUNTERS];  // Top address hits from 1x pass.
uint64_t  counts3x[64][MAX_COUNTERS]; // count values from 3x pass
QuikHit_t addrs3x[64][MAX_COUNTERS];  // Top address hits from 3x pass.
uint64_t  counts9x[64][MAX_COUNTERS]; // count values from 9x pass
QuikHit_t addrs9x[64][MAX_COUNTERS];  // Top address hits from 9x pass.


uint64_t lastTB[64]; // keep track of # cycles between overflows and keep histogram;
uint64_t *pHist[64];
uint64_t lastCount[64];
uint64_t *pHist2[64];


void PrintOverflowHits(hEvtSet)
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int i;
    int totalOvfs = 0;
    for (i=0; i<numEvts; ++i) {

        // check to see if overflow is active for this event
        uint64_t period;
        Bgpm_OverflowHandler_t handler;
        Bgpm_GetOverflow(hEvtSet, i, &period, &handler);
        if (period > 0) {

            // look to see if profiling this event.
            uint64_t hProf;
            Bgpm_GetEventUser1(hEvtSet, i, &hProf);
            if (hProf) {

                fprintf(stderr, "Overflow Address Hits for Event = %s:\n", Bgpm_GetEventLabel(hEvtSet, i));
                totalOvfs += PrintTopHits(hProf, 5);
            }
        }
    }
    fprintf(stderr, "Total Hits = %d", totalOvfs);
}



void GetCounts(unsigned hEvtSet, uint64_t counts[][MAX_COUNTERS])
{
    unsigned numEvts = Bgpm_NumEvents(hEvtSet);
    int thd = Kernel_ProcessorID();

    Bgpm_ReadEventList(hEvtSet, &(counts[thd][0]), &numEvts);
}



void GetAddrs(unsigned hEvtSet, QuikHit_t addrs[][MAX_COUNTERS])
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int i;
    int thd = Kernel_ProcessorID();

    for (i=0; i<numEvts; ++i) {

        // check to see if overflow is active for this event
        uint64_t period;
        Bgpm_OverflowHandler_t handler;
        Bgpm_GetOverflow(hEvtSet, i, &period, &handler);
        if (period > 0) {

            // look to see if profiling this event.
            uint64_t hProf;
            Bgpm_GetEventUser1(hEvtSet, i, &hProf);
            if (hProf) {
                GetTopHits(hProf, 5, &(addrs[thd][i]));
            }
        }
    }
}



void CompareCounts(int hwThd, int num, uint64_t countsA[MAX_COUNTERS], uint64_t countsB[MAX_COUNTERS], double low, double hi, double scale)
{
    char strg[100];
    int thd = Kernel_ProcessorID();
    int idx;
    for (idx=0; idx<num; ++idx) {
        sprintf(strg, "thd=%02d,idx=%02d,scale=%lf", thd, idx, scale);
        TEST_CHECK_RANGEDBL_wLABEL(countsB[idx], countsA[idx]*scale*low, countsA[idx]*scale*hi, strg);
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

    // count hits for this address value
    unsigned i;
    for (i=0; i<len; i++) {
        uint64_t hProf;
        //fprintf(stderr, _AT_ " ovfIdx[%d]=%d\n", i, ovfIdxs[i]);
        Bgpm_GetEventUser1(hEvtSet, ovfIdxs[i], &hProf);
        if (hProf) {
            //RecordAddressHit(hProf, address);
        }
        numOvfs[thd]++;
        if (ovfIdxs[i] == 2) {
            uint64_t tb = GetTimeBase();
            uint64_t count;
            Bgpm_ReadEvent(hEvtSet, ovfIdxs[i], &count);

            uint64_t deltaTb = tb - lastTB[thd];
            Add2DecHistogram(pHist[thd], deltaTb);
            lastTB[thd] = tb;

            uint64_t deltaC = count-lastCount[thd];
            Add2DecHistogram(pHist2[thd], deltaC);
            lastCount[thd] = count;
            //fprintf(stderr, _AT_ " deltaC=%ld\n", deltaC);

        }
    }

    //Bgpm_DumpEvtSet(hEvtSet, 0);
    //exit(-1);

    #if 0
    int idx = 2;
    fprintf(stderr, _AT_ " evtIdx=%d, llCtr[%d]=0x%016lx, fastCtr[%d]=0x%016lx\n",
            idx, thd, Bgpm_Punit_LLRead(hLL[thd].hCtr[idx]), thd, Bgpm_Punit_Read(hPu[thd].hCtr[idx]));
    idx = 3;
    fprintf(stderr, _AT_ " evtIdx=%d, llCtr[%d]=0x%016lx, fastCtr[%d]=0x%016lx\n",
            idx, thd, Bgpm_Punit_LLRead(hLL[thd].hCtr[idx]), thd, Bgpm_Punit_Read(hPu[thd].hCtr[idx]));
    idx = 2;
    fprintf(stderr, _AT_ " evtIdx=%d, llCtr[%d]=0x%016lx, fastCtr[%d]=0x%016lx\n",
            idx, thd, Bgpm_Punit_LLRead(hLL[thd].hCtr[idx]), thd, Bgpm_Punit_Read(hPu[thd].hCtr[idx]));
    idx = 3;
    fprintf(stderr, _AT_ " evtIdx=%d, llCtr[%d]=0x%016lx, fastCtr[%d]=0x%016lx\n",
            idx, thd, Bgpm_Punit_LLRead(hLL[thd].hCtr[idx]), thd, Bgpm_Punit_Read(hPu[thd].hCtr[idx]));
    //UPC_P_Dump_State(-1);
    #endif
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
        PEVT_INST_ALL,
        PEVT_LSU_COMMIT_LD_MISSES, // will overflow
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
        PEVT_INST_ALL,
        PEVT_LSU_COMMIT_LD_MISSES, // will overflow
        PEVT_INST_QFPU_FMUL,  // will overflow
        PEVT_INST_QFPU_FMUL,  // 2nd one will not
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_ALL, 0), 52000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_ALL, 0), GetNewProfileMapHandle());

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
        PEVT_INST_ALL,
        PEVT_LSU_COMMIT_LD_MISSES, // will overflow
        PEVT_INST_QFPU_FMUL,  // will overflow
        PEVT_INST_QFPU_FMUL,  // 2nd one will not
        PEVT_L1P_BAS_LD,  // Extra Events
        PEVT_L1P_BAS_MISS, // Extra Events - should cause shared overflow.
    };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), 5000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES, 0), GetNewProfileMapHandle());

    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_ALL, 0), 52000);
    Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_INST_ALL, 0), GetNewProfileMapHandle());

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



void Exercise(void (*initEvtFunc)(int *phEvtSet), int loops, uint64_t counts[][MAX_COUNTERS], QuikHit_t addrs[][MAX_COUNTERS])
{
       unsigned hwThd = Kernel_ProcessorID();

        // make sure counts start clear
        memset(counts,0,sizeof(counts3x));
        memset(addrs,0,sizeof(addrs3x));
        memset((void*)Bgpm_DebugGetThreadVars(),0,sizeof(BgpmDebugThreadVars_t));
        numOvfs[hwThd] = 0;

        lastTB[hwThd] = GetTimeBase();
        pHist[hwThd] = InitDecHistogram(10, 2000, 250000);
        lastCount[hwThd] = 0;
        pHist2[hwThd] = InitDecHistogram(10, 500, 100000);


        int hEvtSet;
        (*initEvtFunc)(&hEvtSet);
        hEvtSets[hwThd] = hEvtSet;

        unsigned numEvts = Bgpm_NumEvents(hEvtSet);

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

        #if 0
        #pragma omp barrier
        #pragma omp barrier
        if (hwThd == 0) UPC_C_Dump_Counters(0, 16*2+1);
        #pragma omp barrier
        PrintCounts(hEvtSet);
        fprintf(stderr, "NumOvfs[%d]=%d\n", hwThd, numOvfs[hwThd]);
        PrintOverflowHits(hEvtSet);
        BgpmDebugPrintThreadVars();
        Bgpm_DumpEvtSet(hEvtSet, 0);
        Bgpm_DumpUPCPState();
        fprintf(stderr, "\n");
        #pragma omp barrier
        exit(1);
        #endif

        #if 0
        #pragma omp critical (PrintEvts2)
        {
            if ((hwThd == 15*4) || (hwThd== 1*4)) {
            //if ((hwThd%32==0) || (hwThd%32==2)) {
                PrintCounts(hEvtSet);
                fprintf(stderr, "NumOvfs[%d]=%d\n", hwThd, numOvfs[hwThd]);
                PrintOverflowHits(hEvtSet);
                BgpmDebugPrintThreadVars();
                Bgpm_DumpUPCPState();
                UPC_C_Dump_Counters(14*2, 16*2+1);
                fprintf(stderr, "\n");
            }
        }
        #pragma omp barrier
        #endif

        #if 0
        Bgpm_Punit_Handles_t hPu;
        Bgpm_Punit_GetHandles(hEvtSet, &hPu);
        Bgpm_WriteEvent(hEvtSet, 0, hwThd/4);

        upc_p_local->counter[hwThd % 4] = (hwThd/4 << 4) | (hwThd % 4);
        upc_p_local->control_w1s = hPu.allCtrMask & ~UPC_P__CONTROL32_bit(hwThd % 4);
        fprintf(stderr, _AT_ " upc_p control=0x%08lx, cfg=0x%08lx\n", upc_p_local->control, upc_p_local->p_config);
        #endif


        // Exercise something - one to cause cache misses, and the other to cause
        // a lot of instructions w/o misses, another to get FP ops.
        #pragma omp barrier

        //if (hwThd < 2) {

        for (j=0; j<loops*2+2; ++j) {
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
                Bgpm_Start(hEvtSet);
            }
            else if (j == loops) {  // do a warm down loop after counting (so multiple threads not switching activies)
                Bgpm_Stop(hEvtSet);
            }
        }

        //}
        #if 0
        #pragma omp barrier
        #pragma omp critical (PrintEvts3)
        {
            //if ((hwThd == 15*4) || (hwThd== 1*4)) {
            //if ((hwThd%32==0) || (hwThd%32==2)) {
                PrintCounts(hEvtSet);
                fprintf(stderr, "NumOvfs[%d]=%d\n", hwThd, numOvfs[hwThd]);
                //PrintOverflowHits(hEvtSet);
                BgpmDebugPrintThreadVars();
                Bgpm_DumpEvtSet(hEvtSet, 0);
                Bgpm_DumpUPCPState();
                UPC_C_Dump_Counters(0, 16*2+1);
                fprintf(stderr, "\n");
           // }
        }
        #pragma omp barrier
        #endif


        GetCounts(hEvtSet, counts);
        //GetAddrs(hEvtSet, addrs);

        #pragma omp barrier

        // verify that counters are truely stopped
        if ((Kernel_ProcessorCoreID() % 4) == 0) {
            TEST_CHECK_EQUAL(upc_p_local->control, 0);
        }



        // verify that the number of overflows roughly corresponds to the expected values.
        int ovfsAreActive = 0;
        unsigned idx;
        unsigned expNumOvfs = 0;
        char strg1[512];

        //if (hwThd < 2) {

        int pos = 0;
        pos += snprintf(strg1+pos, 512-pos, "thd=%02d, ", hwThd);

        for (idx=0; idx<numEvts; ++idx) {
            uint64_t period;
            uint64_t count;
            Bgpm_OverflowHandler_t handler;
            Bgpm_GetOverflow(hEvtSet, idx, &period, &handler);
            if (period > 0) {
                ovfsAreActive = 1;
                Bgpm_ReadEvent(hEvtSet, idx, &count);
                expNumOvfs += count / period;
                pos += snprintf(strg1+pos, 512-pos, "cp[%02d]=%ld:%ld:%ld, ", idx, count, period, count/period);
            }
        }
        if (ovfsAreActive) {
            fprintf(stderr, "%s\n", strg1);

            TEST_CHECK_NONZERO(numOvfs[hwThd]);
            char strg[100];
            sprintf(strg, "thd=%02d, loops=%d", hwThd, loops);
            TEST_CHECK_RANGEDBL_wLABEL(numOvfs[hwThd], (expNumOvfs*0.98), (expNumOvfs*1.02), strg);
        }

        //}
        //#if TEST_CASE_VERBOSE
        // Print Results each event.
        #pragma omp critical (PrintEvts)
        {
            //if (hwThd < 2) {

            //if ((hwThd == 15*4) || (hwThd== 1*4)) {
            //if ((hwThd%32==0) || (hwThd%32==2)) {
                PrintCounts(hEvtSet);
                fprintf(stderr, "NumOvfs[%d]=%d\n", hwThd, numOvfs[hwThd]);
                //PrintOverflowHits(hEvtSet);
                BgpmDebugPrintThreadVars();
                //Bgpm_DumpEvtSet(hEvtSet, 0);
                //Bgpm_DumpUPCPState();
                //UPC_C_Dump_Counters(0, 16*2+1);
                PrintDecHistogram("Inst Overflow Delta Cycles", pHist[hwThd]);
                PrintDecHistogram("Inst Overflow Delta Count", pHist2[hwThd]);

                fprintf(stderr, "\n");
            // }
            //}
        }
        //#endif

        //fprintf(stderr, _AT_ "\n");
        // free overflow map structures.
        for (i=0; i<numEvts; i++) {
            uint64_t hProf;
            Bgpm_GetEventUser1(hEvtSet, i, &hProf);
            if (hProf) FreeProfileMapHandle(hProf);
        }
        Bgpm_Disable();
        DeleteDecHistogram(pHist[hwThd]);
        DeleteDecHistogram(pHist2[hwThd]);


        #pragma omp barrier

}


int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Fast Threaded Overflows);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);
    PrintWhoAmI();
    Upci_DumpNodeParms(0);


    SHMC_Init_Barriers(numProcs, numThreads);

    const int loops = 10000;



    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,hEvtSets,numOvfs,counts1x,counts3x,addrs1x,addrs3x,counts9x,addrs9x,numFailures)
    {
        int hwThd;
        hwThd = Kernel_ProcessorID();


    #if 1
        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Run events w/o overflow to get counts);
        Exercise(Init6EventsNoOvf, loops,   counts1x, addrs1x);
        Exercise(Init6EventsNoOvf, loops*3, counts3x, addrs3x);
        CompareCounts(hwThd, 6, counts1x[hwThd], counts3x[hwThd], 0.98, 1.02, 3.0);
        //UPC_C_Dump_Counters(0, 16*2+1);
    #endif

    #if 1
        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Fast Punit Overflows < 6 ctrs per thread);
        Exercise(Init6Events, loops,   counts1x, addrs1x);
        Exercise(Init6Events, loops*3, counts3x, addrs3x);
        CompareCounts(hwThd, 6, counts1x[hwThd], counts3x[hwThd], 0.98, 1.02, 3.0);
    #endif

    #if 1
        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Fast Shared Punit Overflows > 6 ctrs per thread);
        Exercise(Init8Events, loops  , counts1x, addrs1x);
        Exercise(Init8Events, loops*3, counts3x, addrs3x);
        CompareCounts(hwThd, 8, counts1x[hwThd], counts3x[hwThd], 0.95, 1.05, 3.0);

        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Sample all Fast Shared Punit Overflows = ctrs per thread);
        Exercise(SampleAll6Events, loops,   counts1x, addrs1x);
        Exercise(SampleAll6Events, loops*3, counts3x, addrs3x);
        CompareCounts(hwThd, 6, counts1x[hwThd], counts3x[hwThd], 0.98, 1.02, 3.0);

        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Sample all Fast Shared Punit Overflows > 6 ctrs per thread);
        Exercise(SampleManyEvents, loops,   counts1x, addrs1x);
        Exercise(SampleManyEvents, loops*3, counts3x, addrs3x);
        Exercise(SampleManyEvents, loops*9, counts9x, addrs9x);
        CompareCounts(hwThd, 11, counts1x[hwThd], counts3x[hwThd], 0.95, 1.05, 3.0);
        CompareCounts(hwThd, 11, counts1x[hwThd], counts9x[hwThd], 0.95, 1.05, 9.0);
    #endif

    #if 1
        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Sample all events with multiplexing active);
        Exercise(SampleMuxEvents, loops,   counts1x, addrs1x);
        Exercise(SampleMuxEvents, loops*3, counts3x, addrs3x);
        CompareCounts(hwThd, 14, counts1x[hwThd], counts3x[hwThd], 0.95, 1.05, 3.0);
    #endif
    }


    TEST_RETURN();

}



