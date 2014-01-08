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


#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <omp.h>

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();



// Array Sizes  cachelines    #dbls   factor to
//              per l1 cache  /line   flush cache
#define DSIZE   (256   *       8     *  2)           // size needed to flush cache
#define INITDATA \
   volatile double target[DSIZE]; \
   volatile double source[DSIZE]; \
   int di; \
   for (di=0; di<DSIZE; di++) { \
       source[di] = di; \
       target[di] = di+1; \
   }



int numThreads;
int numProcs;

UpciBool_t useCounting[CONFIG_MAX_APP_THREADS];

unsigned evtList[] = {
    PEVT_CYCLES,
    PEVT_XU_TIMEBASE_TICK,
    PEVT_XU_THRD_RUNNING,
};

#define STRIDE 8



void CollectCycleCounts(int loops)
{
    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,numProcs,evtList,numFailures,loops)
    {
        Bgpm_Init(BGPM_MODE_SWDISTRIB);
        int hEvtSet = Bgpm_CreateEventSet();
        Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
        Bgpm_Apply(hEvtSet);

        INITDATA;

        Bgpm_Punit_Handles_t hCtrs;
        Bgpm_Punit_GetHandles(hEvtSet, &hCtrs);


        //Bgpm_Start(hEvtSet);
        Bgpm_Punit_Start(hCtrs.allCtrMask);

        uint64_t startTB = GetTimeBase();

        // Exercise something - one to cause cache misses, and the other to cause
        // a lot of instructions w/o misses, another to get FP ops.
        int i, j;
        for (j=0; j<loops; ++j) {
            // stride by cache line size.
            for (i=DSIZE-1; i>=0; i-=STRIDE) {

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

            // sched_yield();
        }

        uint64_t endTB = GetTimeBase();
        Bgpm_Punit_Stop(hCtrs.allCtrMask);
        Delay(800);
        //Bgpm_Stop(hEvtSet);


        // Print Results each event.
        #pragma omp critical (PrintEvts)
        {
            PrintCountsInclOvf(hEvtSet);
            uint64_t cycles;
            Bgpm_ReadEvent(hEvtSet, 0, &cycles);
            fprintf(stderr, "%15ld <- Elapsed TimeBase,   upc cycles=%ld\n", endTB - startTB, cycles);
            long int diff = ((endTB-startTB) - cycles) < 0 ? cycles - endTB-startTB : endTB-startTB - cycles;
            double TBPC100 = (diff)*100.0 / cycles;
            fprintf(stderr, "%15ld <- Delta TB-cycles,  %%TB/cycles =%.2f %%\n", diff, TBPC100);
        }

        Bgpm_Disable();
    }
}



int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Compare Cycle Count Collections);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);

    TEST_CASE(Compare Cycle Count Collection - 1 loops);
    CollectCycleCounts(   1 );

    TEST_CASE(Compare Cycle Count Collection - 1k loops);
    CollectCycleCounts(   1E3 );

    TEST_CASE(Compare Cycle Count Collection - 10k loops);
    CollectCycleCounts(  10E3 );

    TEST_CASE(Compare Cycle Count Collection - 100k loops);
    CollectCycleCounts( 100E3 );


    TEST_RETURN();

}

