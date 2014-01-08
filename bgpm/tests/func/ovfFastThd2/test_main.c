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

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


int hEvtSets[64];
Bgpm_Punit_LLHandles_t hLL[64];
Bgpm_Punit_Handles_t hPu[64];
int numOvfs[64];
int num_events=0;
#define RESET_OVF_COUNTS memset((void*)numOvfs, 0, sizeof(numOvfs))


#define MAX_COUNTERS 24
#define NUM_TOP_ADDRS  2
//thds counter
uint64_t  counts1x[64][MAX_COUNTERS]; // count values from 1x pass
QuikHit_t addrs1x[64][MAX_COUNTERS];  // Top address hits from 1x pass.
uint64_t  counts3x[64][MAX_COUNTERS]; // count values from 3x pass
QuikHit_t addrs3x[64][MAX_COUNTERS];  // Top address hits from 3x pass.
uint64_t  counts9x[64][MAX_COUNTERS]; // count values from 9x pass
QuikHit_t addrs9x[64][MAX_COUNTERS];  // Top address hits from 9x pass.
    
uint64_t  counts6x[64][MAX_COUNTERS];
QuikHit_t addrs6x[64][MAX_COUNTERS];  // Top address hits from 3x pass.
uint64_t  counts12x[64][MAX_COUNTERS];
QuikHit_t addrs12x[64][MAX_COUNTERS];  // Top address hits from 9x pass.

void PrintFormattedCounts(int, uint64_t counts[][MAX_COUNTERS],int);

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




void Compare3XCounts(int num, uint64_t counts1x[][MAX_COUNTERS], uint64_t counts3x[][MAX_COUNTERS], double low, double hi)
{
    char strg[100];
    int thd = Kernel_ProcessorID();
    int idx;
    for (idx=0; idx<num; ++idx) {
        sprintf(strg, "thd=%02d,idx=%02d", thd, idx);
        if (Kernel_ProcessorID() == 0) {
            fprintf(stderr, _AT_ " thdidx:[%02d][%02d]:  counta=%10ld, countb=%10ld\n", thd,idx,counts1x[thd][idx], counts3x[thd][idx]);
        }
        TEST_CHECK_RANGEDBL_wLABEL(counts3x[thd][idx], counts1x[thd][idx]*3.0*low, counts1x[thd][idx]*3.0*hi, strg);
    }
}


void Compare6XCounts(int num, uint64_t counts1x[][MAX_COUNTERS], uint64_t counts6x[][MAX_COUNTERS], double low, double hi)
{
    char strg[100];
    int thd = Kernel_ProcessorID();
    int idx;
    for (idx=0; idx<num; ++idx) {
        sprintf(strg, "thd=%02d,idx=%02d", thd, idx);
        if (Kernel_ProcessorID() == 0) {
            fprintf(stderr, _AT_ " thdidx:[%02d][%02d]:  counta=%10ld, countb=%10ld\n", thd,idx,counts1x[thd][idx], counts6x[thd][idx]);
        }
        TEST_CHECK_RANGEDBL_wLABEL(counts6x[thd][idx], counts1x[thd][idx]*6.0*low, counts1x[thd][idx]*6.0*hi, strg);
    }
}

void Compare9XCounts(int num, uint64_t counts1x[][MAX_COUNTERS], uint64_t counts9x[][MAX_COUNTERS], double low, double hi)
{
    char strg[100];
    int thd = Kernel_ProcessorID();
    int idx;
    for (idx=0; idx<num; ++idx) {
        sprintf(strg, "thd=%02d,idx=%02d", thd, idx);
        if (Kernel_ProcessorID() == 0) {
            fprintf(stderr, _AT_ " thdidx:[%02d][%02d]:  counta=%10ld, countb=%10ld\n", thd,idx,counts1x[thd][idx], counts9x[thd][idx]);
        }
        TEST_CHECK_RANGEDBL_wLABEL(counts9x[thd][idx], counts1x[thd][idx]*9.0*low, counts1x[thd][idx]*9.0*hi, strg);
    }
}

void Compare12XCounts(int num, uint64_t counts1x[][MAX_COUNTERS], uint64_t counts12x[][MAX_COUNTERS], double low, double hi)
{
    char strg[100];
    int thd = Kernel_ProcessorID();
    int idx;
    for (idx=0; idx<num; ++idx) {
        sprintf(strg, "thd=%02d,idx=%02d", thd, idx);
        if (Kernel_ProcessorID() == 0) {
            fprintf(stderr, _AT_ " thdidx:[%02d][%02d]:  counta=%10ld, countb=%10ld\n", thd,idx,counts1x[thd][idx], counts12x[thd][idx]);
        }
        TEST_CHECK_RANGEDBL_wLABEL(counts12x[thd][idx], counts1x[thd][idx]*12.0*low, counts1x[thd][idx]*12.0*hi, strg);
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
        PEVT_L1P_BAS_HIT,  // Extra Events
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
        PEVT_L1P_BAS_HIT,  // Extra Events
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


void Exercise(void (*initEvtFunc)(int *phEvtSet), int loops, uint64_t counts[][MAX_COUNTERS], QuikHit_t addrs[][MAX_COUNTERS])
{
       unsigned hwThd = Kernel_ProcessorID();

        // make sure counts start clear
        memset(counts,-1,sizeof(counts1x)); 
        memset(counts,-1,sizeof(counts3x));
        memset(counts,-1,sizeof(counts6x));
        memset(counts,-1,sizeof(counts9x));
        memset(counts,-1,sizeof(counts12x));
        memset(addrs,0,sizeof(addrs3x));
        memset((void*)Bgpm_DebugGetThreadVars(),0,sizeof(BgpmDebugThreadVars_t));
        numOvfs[hwThd] = 0;

        int hEvtSet;
        (*initEvtFunc)(&hEvtSet);
        hEvtSets[hwThd] = hEvtSet;

        unsigned numEvts = Bgpm_NumEvents(hEvtSet);

        num_events=numEvts; 
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

        for (j=0; j<loops+1; ++j) {
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
        }



        Bgpm_Stop(hEvtSet);

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
        for (idx=0; idx<numEvts; ++idx) {
            uint64_t period;
            uint64_t count;
            Bgpm_OverflowHandler_t handler;
            Bgpm_GetOverflow(hEvtSet, idx, &period, &handler);
            if (period > 0) {
                ovfsAreActive = 1;
                Bgpm_ReadEvent(hEvtSet, idx, &count);
                expNumOvfs += count / period;
            }
        }
        if (ovfsAreActive) {
            TEST_CHECK_NONZERO(numOvfs[hwThd]);
            TEST_CHECK_RANGE(expNumOvfs, (int)(numOvfs[hwThd]*0.85), (int)(numOvfs[hwThd]*1.15) );
        }


        //#if TEST_CASE_VERBOSE
        // Print Results each event.
        #pragma omp critical (PrintEvts)
        {
                PrintCounts(hEvtSet);
                fprintf(stderr, "NumOvfs[%d]=%d\n", hwThd, numOvfs[hwThd]);
        //        BgpmDebugPrintThreadVars();
                fprintf(stderr, "\n");
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

        #pragma omp barrier

}


int main(int argc, char *argv[])
{
    int i;
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
    char header[100];
    char column_names[100];

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,hEvtSets,numOvfs,counts1x,counts3x,addrs1x,addrs3x,counts9x,addrs9x,counts6x,addrs6x,counts12x,addrs12x,numFailures)
    {

    #if 0
        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Run events w/o overflow to get counts);
        Exercise(Init6EventsNoOvf, loops,   counts1x, addrs1x);
        Exercise(Init6EventsNoOvf, loops*3, counts3x, addrs3x);
        Exercise(Init6EventsNoOvf, loops*6, counts6x, addrs3x);
        Exercise(Init6EventsNoOvf, loops*9, counts9x, addrs3x);
        Exercise(Init6EventsNoOvf, loops*12, counts12x, addrs3x);
        Compare3XCounts(6, counts1x, counts3x, 0.85, 1.15);
        //UPC_C_Dump_Counters(0, 16*2+1);
        #pragma omp barrier
    #endif

    #if 1
        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Fast Punit Overflows < 6 ctrs per thread);
        Exercise(Init6Events, loops,   counts1x, addrs1x);
        Exercise(Init6Events, loops*3, counts3x, addrs3x);
        Exercise(Init6Events, loops*6, counts6x, addrs6x);
        Exercise(Init6Events, loops*9, counts9x, addrs9x);
        Exercise(Init6Events, loops*12, counts12x, addrs12x);
        Compare3XCounts(6, counts1x, counts3x, 0.85, 1.15);
        printf("Comparing 6X counters\n");
        Compare6XCounts(6, counts1x, counts6x, 0.85, 1.15);
        printf("Comparing 9X counters\n");
        Compare9XCounts(6, counts1x, counts9x, 0.85, 1.15);
        printf("Comparing 12X counters\n");
        Compare12XCounts(6, counts1x, counts12x, 0.85, 1.15);
        #pragma omp barrier
    #endif
#if 0
        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Fast Shared Punit Overflows > 6 ctrs per thread);
        Exercise(Init8Events, loops  , counts1x, addrs1x);
        Exercise(Init8Events, loops*3, counts3x, addrs3x);
        Exercise(Init8Events, loops*6, counts6x, addrs6x);
        Exercise(Init8Events, loops*9, counts9x, addrs9x);
        Exercise(Init8Events, loops*12, counts12x, addrs12x);
        Compare3XCounts(8, counts1x, counts3x, 0.85, 1.15);
        printf("Comparing 6X counters\n");
        Compare6XCounts(8, counts1x, counts6x, 0.85, 1.15);
        printf("Comparing 9X counters\n");
        Compare6XCounts(8, counts1x, counts9x, 0.85, 1.15);
        printf("Comparing 12X counters\n");
        Compare6XCounts(8, counts1x, counts12x, 0.85, 1.15);
#endif
#if 0
        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Fast Shared Punit Overflows 12 ctrs per thread);
        Exercise(Init12Events, loops  , counts1x, addrs1x);
        Exercise(Init12Events, loops*3, counts3x, addrs3x);
        Exercise(Init12Events, loops*6, counts6x, addrs6x);
        Exercise(Init12Events, loops*9, counts9x, addrs9x);
        Exercise(Init12Events, loops*12, counts12x, addrs12x);
        Compare3XCounts(12, counts1x, counts3x, 0.85, 1.15);
        printf("Comparing 6X counters\n");
        Compare6XCounts(12, counts1x, counts6x, 0.85, 1.15);
        printf("Comparing 9X counters\n");
        Compare6XCounts(12, counts1x, counts9x, 0.85, 1.15);
        printf("Comparing 12X counters\n");
        Compare6XCounts(12, counts1x, counts12x, 0.85, 1.15);
#endif

    #if 0 
        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Sample all Fast Shared Punit Overflows = ctrs per thread);
        Exercise(SampleAll6Events, loops,   counts1x, addrs1x);
        Exercise(SampleAll6Events, loops*3, counts3x, addrs3x);
        Compare3XCounts(6, counts1x, counts3x, 0.85, 1.15);     // as there are more overflows, there are more misses which occur.

        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Sample all Fast Shared Punit Overflows > 6 ctrs per thread);
        Exercise(SampleManyEvents, loops,   counts1x, addrs1x);
        Exercise(SampleManyEvents, loops*3, counts3x, addrs3x);
        Exercise(SampleManyEvents, loops*9, counts9x, addrs9x);
        Compare3XCounts(11, counts1x, counts3x, 0.85, 1.15);
        Compare3XCounts(11, counts3x, counts9x, 0.85, 1.15);

        RESET_OVF_COUNTS;
        #pragma omp barrier
        TEST_CASE(Sample all events with multiplexing active);
        Exercise(SampleMuxEvents, loops,   counts1x, addrs1x);
        Exercise(SampleMuxEvents, loops*3, counts3x, addrs3x);
        Compare3XCounts(14, counts1x, counts3x, 0.85, 1.15);

    #endif
    }
    
    snprintf(header,sizeof(header),"%70s","=======================================================================================================================");
    printf("%s",header);
    snprintf(header,sizeof(header),"\n");
    printf("%s",header);
  //  char header_events[100];
    snprintf(header,sizeof(header),"%5s","ThdId"); 
    strcat(column_names,header);
    for(i=0;i<num_events;i++){
     snprintf(header,sizeof(header)," %11s%2d","Event",i);
     strcat(column_names,header);
    } 
    snprintf(header,sizeof(header),"%8s","Ovfs"); 
    strcat(column_names,header); 
    printf("%s",column_names);
    snprintf(header,sizeof(header),"\n");
    printf("%s",header);
    snprintf(header,sizeof(header),"%70s","======================================================================================================================");
    printf("%s",header);
    snprintf(header,sizeof(header),"\n");
    printf("%s",header);


   for(i=0;i<64;i++){
    PrintFormattedCounts(i,counts1x,1);
    PrintFormattedCounts(i,counts3x,3);
    PrintFormattedCounts(i,counts6x,6);
    PrintFormattedCounts(i,counts9x,6);
    PrintFormattedCounts(i,counts12x,6);
   } 
   TEST_RETURN();

}



void PrintFormattedCounts(int thdId, uint64_t counts[][MAX_COUNTERS],int scale)
{
   int j;
   char string[200],formatted[400];
   string[0]='\0';
   char temp_array[100];

   snprintf(formatted,sizeof(formatted),"%5d ",thdId);
   for(j=0;j<MAX_COUNTERS;j++){
      if(counts[thdId][j] != -1){
      snprintf(temp_array,sizeof(temp_array),"%13ld ",counts[thdId][j]);
      strcat(string,temp_array);
     }
   }
   if(string[0]!='\0'){
   snprintf(temp_array,sizeof(temp_array),"%8d",numOvfs[thdId]);    
   strcat(string,temp_array);
   strcat(formatted,string);
   printf("%s",formatted);
   printf("\n");
   }   
  strcpy(formatted," ");
}

