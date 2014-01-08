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
    PEVT_LSU_COMMIT_LD_MISSES,
    PEVT_INST_ALL,
    PEVT_INST_QFPU_FMUL,
    PEVT_L1P_BAS_HIT,
    PEVT_L1P_BAS_MISS,
};

#define LOOPS 5000
#define STRIDE 8



void Overcomitted_Exercise1(UpciBool_t doCounting)
{
    memset((void*)useCounting, 0, sizeof(useCounting));

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,numProcs,useCounting,evtList,numFailures)  \
        firstprivate(doCounting)
    {
        unsigned hwThd = Kernel_ProcessorID();
        #pragma omp critical (CheckIfCountable)
        {
            if (useCounting[hwThd] == 0) {
                useCounting[hwThd] = omp_get_thread_num() + 1;
                //fprintf(stderr, "ompThd=%02d Would Count\n", omp_get_thread_num());
            }
            else {
                doCounting = BGPM_FALSE;
                //fprintf(stderr, "ompThd=%02d Would Not Count\n", omp_get_thread_num());
            }
        }

        int hEvtSet = -1;

        if (doCounting) {
            Bgpm_Init(BGPM_MODE_SWDISTRIB);
            hEvtSet = Bgpm_CreateEventSet();
            Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
            Bgpm_Apply(hEvtSet);
        }

        INITDATA;


        if (doCounting) Bgpm_Start(hEvtSet);

        // Exercise something - one to cause cache misses, and the other to cause
        // a lot of instructions w/o misses, another to get FP ops.
        int i, j;
        int loops = LOOPS;
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

            sched_yield();
        }

        if (doCounting) Bgpm_Stop(hEvtSet);


        // Print Results each event.
        #pragma omp critical (PrintEvts)
        {
            if (doCounting && ((hwThd % 16) == 0)) {
                PrintCountsInclOvf(hEvtSet);
            }
            if (doCounting) {
                unsigned fpIdx = Bgpm_GetEventIndex(hEvtSet,PEVT_INST_QFPU_FMUL,0);
                uint64_t count;
                Bgpm_ReadEvent(hEvtSet, fpIdx, &count);
                unsigned expCount = LOOPS * DSIZE/STRIDE;
                TEST_CHECK_EQUAL(count, expCount);
            }
        }

        if (doCounting) {
            Bgpm_Disable();
        }
    }

}


void Overcomitted_Exercise2(UpciBool_t doCounting)
{
    memset((void*)useCounting, 0, sizeof(useCounting));

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,numProcs,useCounting,evtList,numFailures)  \
        firstprivate(doCounting)
    {
        unsigned hwThd = Kernel_ProcessorID();
        #pragma omp critical (CheckIfCountable)
        {
            if (useCounting[hwThd] == 0) {
                useCounting[hwThd] = omp_get_thread_num() + 1;
                //fprintf(stderr, "ompThd=%02d Would Count\n", omp_get_thread_num());
            }
            else {
                doCounting = BGPM_FALSE;
                //fprintf(stderr, "ompThd=%02d Would Not Count\n", omp_get_thread_num());
            }
        }

        int hEvtSet = -1;
        uint64_t appThdMask = UPC_APP_HWTID_MASK(hwThd);
        if (doCounting) {
            Bgpm_Init(BGPM_MODE_HWDISTRIB);
            hEvtSet = Bgpm_CreateEventSet();
            Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
            Bgpm_Attach(hEvtSet, appThdMask, 0);
        }

        INITDATA;

        #pragma omp barrier
        if (doCounting) Bgpm_Start(hEvtSet);


        // Exercise something - one to cause cache misses, and the other to cause
        // a lot of instructions w/o misses, another to get FP ops.
        int i, j;
        int loops = LOOPS;
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

            sched_yield();
        }

        if (doCounting) Bgpm_Stop(hEvtSet);


        // Print Results each event.
        #pragma omp critical (PrintEvts)
        {
            if (doCounting && ((hwThd % 16) == 0)) {
                PrintCountsInclOvf(hEvtSet);
            }
            if (doCounting) {
                unsigned fpIdx = Bgpm_GetEventIndex(hEvtSet,PEVT_INST_QFPU_FMUL,0);
                uint64_t count;
                Bgpm_ReadEvent(hEvtSet, fpIdx, &count);
                unsigned expCount = LOOPS * DSIZE/STRIDE;
                TEST_CHECK_RANGE(count, expCount+expCount/2, expCount*2);
            }
        }

        if (doCounting) {
            Bgpm_Disable();
        }
    }

}



int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Check Context Switch (sw thread swap) Counting);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);


    TEST_CASE(Overcommit Threads without counting);
    Overcomitted_Exercise1(BGPM_FALSE);

    TEST_CASE(Overcommit Threads with counting);
    Overcomitted_Exercise1(BGPM_TRUE);

    TEST_CASE(Overcommit Threads with counting w/o context sw);
    Overcomitted_Exercise2(BGPM_TRUE);



    TEST_RETURN();

}

