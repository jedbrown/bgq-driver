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


/**
 * \example bgpm/punit_hwmode/main.c
 *
 * Example of use the BGPM_HWDISTRIB mode.
 *
 * This mode allows a single thread in a process (or even across processes)
 * to create an Punit event set, attach, and collect counts from other physical hardware threads,
 * regardless of what software activity is occurring on the threads.
 * Also, in the HW modes, most of the the counter collection is synchronized - a single
 * start/stop controls the counters for all Punit/L2/IO counter collections
 * (Though Network and CNK event sets are still controlled independently).
 *
 * This example shows how a single thread
 * can gather all punit events for all applications threads on the node, and
 * retrieve the aggregate, and individual thread values.
 * It uses OMP to dispatch the various software threads.
 *
 *
 * The output from this test looks similar to the following:
\verbatim
Count punit events using BGPM_HWDISTRIB mode
Use first thread to count for other hw threads in process
numProcesses=1, numThreads=64, parentThdId=0
The event counts for thread id 8 are:
   PEVT_CYCLES                =   1560, processTotal =  99840
   PEVT_INST_ALL              =    241, processTotal =   2335
   PEVT_IU_IL1_MISS_CYC       =    719, processTotal =   1947
The event counts for thread id 15 are:
   PEVT_CYCLES                =   1560, processTotal =  99840
   PEVT_INST_ALL              =     28, processTotal =   2335
   PEVT_IU_IL1_MISS_CYC       =      0, processTotal =   1947
The event counts for thread id 12 are:
   PEVT_CYCLES                =   1560, processTotal =  99840
   PEVT_INST_ALL              =      0, processTotal =   2335
   PEVT_IU_IL1_MISS_CYC       =      0, processTotal =   1947
The event counts for thread id 20 are:
   PEVT_CYCLES                =   1560, processTotal =  99840
   PEVT_INST_ALL              =      0, processTotal =   2335
   PEVT_IU_IL1_MISS_CYC       =      0, processTotal =   1947
The event counts for thread id 16 are:
   PEVT_CYCLES                =   1560, processTotal =  99840
   PEVT_INST_ALL              =      0, processTotal =   2335
   PEVT_IU_IL1_MISS_CYC       =      0, processTotal =   1947
The event counts for thread id 32 are:
   PEVT_CYCLES                =   1560, processTotal =  99840
   PEVT_INST_ALL              =      0, processTotal =   2335
   PEVT_IU_IL1_MISS_CYC       =      0, processTotal =   1947
...
The average event counts for process starting with tcoord 0:
   Avg PEVT_CYCLES                = 1560.00
   Avg PEVT_INST_ALL              =   36.48
   Avg PEVT_IU_IL1_MISS_CYC       =   30.42
\endverbatim

 */


#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "bgpm/include/bgpm.h"


int main(int argc, char *argv[])
{
    printf("Count punit events using BGPM_HWDISTRIB mode\n"
           "Use hw thread process thread count for other hw threads in process\n");

    // SPI calls to get scope of threads for this process
    int numProcesses = Kernel_ProcessCount();    // # processes on node
    int numThreads   = Kernel_ProcessorCount();  // # num hw thread avail to process
    int parentThdId  = Kernel_ProcessorID();     // # process hw thread id
    printf("numProcesses=%d, numThreads=%d, parentThdId=%d\n",
            numProcesses, numThreads, parentThdId);

    // create master event list.
    Bgpm_Init(BGPM_MODE_HWDISTRIB);
    int hEvtSet = Bgpm_CreateEventSet();
    unsigned evtList[] = { PEVT_CYCLES, PEVT_INST_ALL, PEVT_IU_IL1_MISS_CYC };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));

    // Get thread mask for all hw thread associated with current process
    uint64_t thdMask = Kernel_ThreadMask(Kernel_MyTcoord());

    // Attach to all of these threads.
    Bgpm_Attach(hEvtSet, thdMask, 0);


    // Using OMP to have each thread do some activity
    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,hEvtSet,evtList,thdMask,parentThdId)
    {
        unsigned thdId = Kernel_ProcessorID();

        // 1st start or stop controls counting of attached events across node.
        //  (except for NW and CNK units, which are always controlled independently)
        // Thus, will probabaly need some cross process synchronization to ensure all
        // Threads on node have been configured before starting counting.
        // MPI Barrier perhaps?
        if (thdId==parentThdId) Bgpm_Start(hEvtSet);

        //... Run work load;

        if (thdId==parentThdId) Bgpm_Stop(hEvtSet);

        #pragma omp critical (print_thread_results)
        {
            printf("The event counts for thread id %d are:\n", thdId);

            // enable child thread's access to bgpm functions.
            // (only needed if thread is going to use Bgpm functions, in this case to read
            // it's own counters).
            if (thdId!=parentThdId) Bgpm_Init(BGPM_MODE_HWDISTRIB);

            unsigned evtIdx;
            for (evtIdx=0; evtIdx < Bgpm_NumEvents(hEvtSet); evtIdx++) {
                uint64_t thdCount, totalCount;
                // get aggregate from all process' threads
                Bgpm_ReadEvent(hEvtSet, evtIdx, &totalCount);
                // get value from just this application thread (can ignore agent threads)
                uint64_t curThdMask = UPC_APP_HWTID_MASK(thdId);
                Bgpm_ReadThreadEvent(hEvtSet, evtIdx, curThdMask, 0, &thdCount);

                printf("   %-25s  = %6ld, processTotal = %6ld\n", Bgpm_GetEventLabel(hEvtSet, evtIdx), thdCount, totalCount);
            }
            if (thdId!=parentThdId) Bgpm_Disable();
        }
    }

    printf("The average event counts for process starting with tcoord %d:\n", Kernel_MyTcoord());
    unsigned evtIdx;
    for (evtIdx=0; evtIdx < Bgpm_NumEvents(hEvtSet); evtIdx++) {
        uint64_t count;
        Bgpm_ReadEvent(hEvtSet, evtIdx, &count);
        printf("   Avg %-25s  = %7.2f\n", Bgpm_GetEventLabel(hEvtSet, evtIdx), ((double)count)/numThreads);
    }

    Bgpm_Disable();

    return 0;
}

