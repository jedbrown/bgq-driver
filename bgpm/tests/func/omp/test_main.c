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
#include "spi/include/upci/testlib/upc_p_debug.h"

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


// Test Data
#define N 100000
static double   a[N],
                b[N],
                c[N];

static double sum;


// Shared User data used to aggregate event counts from each thread.
uint64_t evtSums[24];  // Use to sum events from all thread

unsigned evtList[] = {      // List of performance events to collect
    PEVT_IU_IS1_STALL_CYC,
    PEVT_CYCLES,
    PEVT_INST_ALL
};
int numEvts = sizeof(evtList) / sizeof(int);



int main(int argc, char *argv[])
{
    printf("OMP For Loop of Bgpm API\n");

    if (argc < 2) {
        printf("ERROR: Pass in number of threads\n");
        exit(-1);
    }
    int numThreads = atoi(argv[1]);
    int numProcs = Kernel_ProcessCount();

    printf("numProcesses=%d, numThreads=%d, numEvts=%d\n", numProcs, numThreads, numEvts);

    int i,j;
    sum = 0;
    memset(evtSums,0,sizeof(evtSums));  // init


    //#define ADDINORDER
    #ifdef ADDINORDER
    InitCriticalLock();
    InitShmBarrier1(numProcs);
    ShmBarrier1();
    InitShmBarrier2(numProcs*numThreads);
    #endif

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(sum,a,b,c,evtSums,evtList,numEvts,stderr,numThreads)    \
        private(i,j)
    {
        //atexit(printAtExit);
        #ifdef ADDINORDER
        int waitTarget = Kernel_ProcessorID()/(CONFIG_MAX_HWTHREADS/numThreads);
        #endif

        // Configure and start hw counters.
        // Bgpm_Init just inits this thread use the hardware.
        // This happens once per thread.

        Bgpm_Init(BGPM_MODE_SWDISTRIB);

        //#pragma omp barrier

        int hEvtSet = Bgpm_CreateEventSet();   // get container for events.

        #ifdef ADDINORDER
        InitShmCount(0);
        ShmBarrier2();
        TEST_AT();
        ShmAtomicWait(waitTarget);
        #endif

        Bgpm_AddEventList(hEvtSet, evtList, numEvts);

        #ifdef ADDINORDER
        TEST_AT();
        ShmAtomicIncr();
        ShmBarrier2();
        TEST_AT();
        #endif

        Bgpm_Apply(hEvtSet);        // apply to hardware
        Bgpm_ResetStart(hEvtSet);   // start counting.

        // Target code to measure
        // Loops are distributed across all parallel threads.
        #pragma omp for
        for (j=0; j<N; j++) {
            a[j] = 1.0;
            b[j] = 2.0;
            c[j] = 0.0;
            sum++;
        }

        Bgpm_Stop(hEvtSet);   // stop counting - once per thread.


        // Gather sum for each event.
        #pragma omp critical (SumEvts)
        {
            for (i=0; i<numEvts; i++) {
                uint64_t val;
                Bgpm_ReadEvent(hEvtSet, i, &val);
                evtSums[i] += val;
            }
        }
    }

    printf("After OMP for, sum is: %lf\n", sum);
    printf("The event counts for %d threads are:\n", numThreads);
    for (i=0; i<numEvts; ++i) {
        printf("   %s     = %ld\n", Bgpm_GetEventIdLabel(evtList[i]), evtSums[i]);
        printf("   %s_avg = %ld\n", Bgpm_GetEventIdLabel(evtList[i]), evtSums[i]/numThreads);
    }


#if 0
    Upci_A2PC_Val_t a2qry;
    Kernel_Upci_A2PC_GetRegs(&a2qry);
    Upci_A2PC_DumpRegs(&a2qry);

    UPC_P_Dump_State(Kernel_ProcessorCoreID());
#endif

    return(0);
}


