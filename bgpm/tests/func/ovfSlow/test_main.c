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


// Create some database to create cache misses
//long long j;
// Array Sizes  size per          bytes/   #slices  factor to
//              l2 slice          double            cache
#define DSIZE   (2*1024*1024   /    8     *  16       * 2)  // size needed to flush cache
volatile double target[DSIZE];
volatile double source[DSIZE];




int hEvtSets[64];
int hCnkEvtSets[64];

uint64_t misses[64][2];  // keep track of two passes of miss counts and compare.
int numOvfs[64];

void PrintOverflowHits(hEvtSet)
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int i;
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
                PrintTopHits(hProf, 5);
            }
        }
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
    numOvfs[thd]++;

    // Get the indices of all events which have overflowed.
    unsigned ovfIdxs[BGPM_MAX_OVERFLOW_EVENTS];
    unsigned len = BGPM_MAX_OVERFLOW_EVENTS;
    Bgpm_GetOverflowEventIndices(hEvtSet, ovfVector, ovfIdxs, &len);
    //fprintf(stderr, _AT_ " hEvtSet=%d, ovfVector=0x%016lx, ovfIdx[0]=%d, len=%d\n",
    //        hEvtSet, ovfVector, ovfIdxs[0], len);

    // count hits for this address value
    unsigned i;
    for (i=0; i<len; i++) {
        uint64_t hProf;
        //fprintf(stderr, _AT_ " ovfIdx[%d]=%d\n", i, ovfIdxs[i]);
        Bgpm_GetEventUser1(hEvtSet, ovfIdxs[i], &hProf);
        if (hProf) {
            RecordAddressHit(hProf, address);
        }
    }

}



int numThreads;
int numProcs;

extern __thread int         swModeAppliedL2EvtSet;

void ExerciseL2(int passNum)
{
    memset((void*)numOvfs, 0, sizeof(numOvfs));

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,hEvtSets,hCnkEvtSets,misses,numOvfs,target,source,passNum)
    {
        Bgpm_Init(BGPM_MODE_SWDISTRIB);
        int hEvtSet = Bgpm_CreateEventSet();
        hEvtSets[Kernel_ProcessorID()] = hEvtSet;

        unsigned evtList[] = {
            PEVT_L2_HITS,
            PEVT_L2_MISSES,
            PEVT_L2_PREFETCH,
            PEVT_L2_FETCH_LINE,
            PEVT_L2_STORE_LINE,
            PEVT_L2_STORE_PARTIAL_LINE
        };
        Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));

        uint64_t mapHandle = 0;
        if (passNum > 0) {
            Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_L2_MISSES, 0), 1000);
            mapHandle = GetNewProfileMapHandle();
            Bgpm_SetEventUser1(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_L2_MISSES, 0), mapHandle);
            Bgpm_SetOverflowHandler(hEvtSet, OvfHandler);
        }

        // Setup cnk unit for double check
        int hCnkEvtSet = Bgpm_CreateEventSet();
        hCnkEvtSets[Kernel_ProcessorID()] = hCnkEvtSet;

        unsigned cnkEvtList[] = {
            PEVT_CNKHWT_PERFMON,
        };
        Bgpm_AddEventList(hCnkEvtSet, cnkEvtList, sizeof(cnkEvtList)/sizeof(unsigned));


        Bgpm_Apply(hEvtSet);
        Bgpm_Apply(hCnkEvtSet);

        //fprintf(stderr, _AT_ " hEvtSet=%d  swModeAppliedL2EvtSet=%d  mapHandle=0x%016lx\n",
        //        hEvtSet, swModeAppliedL2EvtSet, mapHandle);

#if 0
        UPC_C_DCR__CONFIG__L2_COMBINE_insert(upc_c->c_config,0);
        upc_c->overflow_state_control_low = ( UPC_C_DCR__UPC_C_OVERFLOW_STATE_CONTROL_LOW__MODE2_DONE_set( UPC_NONCRITICAL_INTERRUPT ) \
                | UPC_C_DCR__UPC_C_OVERFLOW_STATE_CONTROL_LOW__IO_COUNTER_OVF_set( UPC_NONCRITICAL_INTERRUPT ) \
                | UPC_C_DCR__UPC_C_OVERFLOW_STATE_CONTROL_LOW__PROC_COUNTER_OVF_set( UPC_NONCRITICAL_INTERRUPT ) );
#endif

#if 0
        Bgpm_DumpEvtSet(hEvtSet, 0);
        UPC_P_Dump_State(-1);
        UPC_C_Dump_State();
        UPC_C_Dump_Counters(34, 34);
#endif

        #pragma omp barrier

        Bgpm_Start(hCnkEvtSet);
        if ((Kernel_ProcessorCoreID() == 0)) {
            //Bgpm_Reset(hEvtSet);
            //Bgpm_ResetStart(hEvtSet);
            Bgpm_Start(hEvtSet);
        }


        // Exercise something - one to cause cache misses, and the other to cause
        // a lot of instructions w/o misses, another to get FP ops.
        // L2 cache is 32 Meg
        int i,j;
        int loops = 5;
        for (j=0; j<loops; ++j) {
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

#if 0
            if (j==0) {
                UPC_P_Dump_State(-1);
                UPC_C_Dump_State();
                UPC_C_Dump_Counters(34, 34);
            }
#endif
        }

        if ((Kernel_ProcessorCoreID() == 0)) Bgpm_Stop(hEvtSet);
        Bgpm_Stop(hCnkEvtSet);


        #pragma omp barrier

        // Print Results each event.
        #pragma omp critical (PrintEvts)
        {
            //unsigned coreId = Kernel_ProcessorCoreID();
            //if ((coreId==0) || (coreId==8)) {
            //if ((Kernel_ProcessorID()%32==0) || (Kernel_ProcessorID()%32==2)) {
                Bgpm_ReadEvent(hEvtSet, 1, &(misses[Kernel_ProcessorID()][passNum]));

                PrintCounts(hCnkEvtSet);
                PrintCounts(hEvtSet);
                fprintf(stderr, "NumOvfs[%d]=%d\n", Kernel_ProcessorID(), numOvfs[Kernel_ProcessorID()]);
                PrintOverflowHits(hEvtSet);
                BgpmDebugPrintThreadVars();
            //}
        }

        Bgpm_Disable();
    }

}


int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Slow L2 and I/O Overflows);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);



    // Create some data space to create cache misses
    // we won't care that there may be collisions between threads.
    int i;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }


    TEST_CASE(L2 counts - no overflows - warmup);
    ExerciseL2(0);

    TEST_CASE(L2 counts - no overflows);
    ExerciseL2(0);

    TEST_CASE(L2 misses with Overflows);
    ExerciseL2(1);

    printf("\nmiss counts with and without overflows:\n");
    for (i=0; i<64; i+=64/numThreads) {
        printf("thd=%02d  %10ld   %10ld\n", i, misses[i][0], misses[i][1]);
    }
    //TEST_CASE(Fast Shared Punit Overflows > 6 ctrs per thread);
    //Exercise(8);


    TEST_RETURN();

}



