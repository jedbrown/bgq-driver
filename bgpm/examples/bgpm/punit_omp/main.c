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
 * \example bgpm/punit_omp/main.c
 *
 * Example of using multiple threads in BGPM
 *
 * This example shows how we can have multiple threads and 
 * gather all punit events.
 * It uses OMP to dispatch the various software threads.
 *
 *
 * The output from this test looks similar to the following:
\verbatim
Gather counts from multiple threads

The event counts for thread id 8 are:
-----------------------------------
Punit events:
8    0x00000000000518d4 <= PEVT_CYCLES
8    0x0000000000016a72 <= PEVT_INST_ALL
8    0x0000000000000001 <= PEVT_AXU_FP_EXCEPT
8    0x0000000000002ee0 <= PEVT_INST_QFPU_FMUL
8    0x0000000000000000 <= PEVT_AXU_DENORM_FLUSH

The event counts for thread id 12 are:
-----------------------------------
Punit events:
12    0x00000000000518d4 <= PEVT_CYCLES
12    0x0000000000016a72 <= PEVT_INST_ALL
12    0x0000000000000001 <= PEVT_AXU_FP_EXCEPT
12    0x0000000000002ee0 <= PEVT_INST_QFPU_FMUL
12    0x0000000000000000 <= PEVT_AXU_DENORM_FLUSH

The event counts for thread id 0 are:
-----------------------------------
Punit events:
0    0x00000000000518d4 <= PEVT_CYCLES
0    0x0000000000016a72 <= PEVT_INST_ALL
0    0x0000000000000001 <= PEVT_AXU_FP_EXCEPT
0    0x0000000000002ee0 <= PEVT_INST_QFPU_FMUL
0    0x0000000000000000 <= PEVT_AXU_DENORM_FLUSH
L2 unit events:
0    0x00000000000035e7 <= PEVT_L2_HITS
0    0x0000000000000118 <= PEVT_L2_MISSES
0    0x0000000000000084 <= PEVT_L2_PREFETCH
0    0x0000000000000135 <= PEVT_L2_FETCH_LINE

\endverbatim

 */


#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "bgpm/include/bgpm.h"

/* This is sample test to create some events */
void create_events()
{
   volatile double a=9.87678;
   volatile double b=8.767876;
   volatile int x[1024],y[1024],z[2048];
   int i;
   for(i=0;i<6000;i++)
   {
     a=a*5;
     a=a+b;
     b=b*5;
   }
   for(i=0;i<1024;i++)
   {
    x[i]=i*4;
    y[i]=i*8;
    z[i]=x[i]+y[i];
   }
}
int main(int argc, char *argv[])
{
    printf("Gather counts from multiple threads\n");

    int numThreads = 4;                         // num of threads
    int hL2EvtSet;                              // L2 event set will be shared between threads
    unsigned evtPuList[] = { PEVT_CYCLES, PEVT_INST_ALL,  PEVT_AXU_FP_EXCEPT,PEVT_INST_QFPU_FMUL,PEVT_AXU_DENORM_FLUSH };
    unsigned evtL2List[] = {PEVT_L2_HITS,PEVT_L2_MISSES,PEVT_L2_PREFETCH,PEVT_L2_FETCH_LINE };

    // Using OMP to have each thread do some activity
    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,evtPuList,hL2EvtSet,evtL2List)
    {
        unsigned thdId = Kernel_ProcessorID();

        //create master event list.
        Bgpm_Init(BGPM_MODE_SWDISTRIB);  // sw mode - each sw thread controls unique punit counters

        int hPuEvtSet = Bgpm_CreateEventSet();
        Bgpm_AddEventList(hPuEvtSet, evtPuList, sizeof(evtPuList)/sizeof(unsigned));
        Bgpm_Apply(hPuEvtSet);

        // We'll use one thread to control and report L2 counters since they are node wide.
        // Note that if multiple instances of an L2 or I/O event set, the 1st thread
        // to do the start - starts L2/IO counting, and 1st to stop, stops the counting.
        if(thdId == 0) {
            hL2EvtSet = Bgpm_CreateEventSet();
            Bgpm_AddEventList(hL2EvtSet, evtL2List, sizeof(evtL2List)/sizeof(unsigned));

            // Apply eventset
            Bgpm_Apply(hL2EvtSet);
        }

        // get threads in sync (to wait for L2 config above).
        #pragma omp barrier

        // Only controlling thread(thread 0 here) can start/stop counters.
        // You may wish some thread or process synchronization: MPI Barrier perhaps?
        if(thdId==0) Bgpm_Start(hL2EvtSet);
        Bgpm_Start(hPuEvtSet);   // punit counts are a little more sensitive than L2, we'll start/stop these within other units.

        //... Run work load;
        create_events();

        Bgpm_Stop(hPuEvtSet);
        if(thdId==0) Bgpm_Stop(hL2EvtSet);

        // final barrier so we don't start printing before everyone is done (just in case).
        #pragma omp barrier

        #pragma omp critical   // Use critical section so print output doesn't interleave
        {
            printf("\nThe event counts for thread id %d are:\n", thdId);
            printf("-----------------------------------\n");
            int i;
            int numPuEvts = Bgpm_NumEvents(hPuEvtSet);
            uint64_t cntPu;
            printf("Punit events:\n");
            for (i=0; i<numPuEvts; i++) {
                Bgpm_ReadEvent(hPuEvtSet, i, &cntPu);
                printf("%d    0x%016lx <= %s\n", thdId, cntPu, Bgpm_GetEventLabel(hPuEvtSet, i));
            }
            if (thdId==0) {
                int numL2Evts = Bgpm_NumEvents(hL2EvtSet);
                uint64_t cntL2;
                printf("L2 unit events:\n");
                for (i=0; i<numL2Evts; i++) {
                    Bgpm_ReadEvent(hL2EvtSet, i, &cntL2);
                    printf("%d    0x%016lx <= %s\n",thdId, cntL2, Bgpm_GetEventLabel(hL2EvtSet, i));
                }
            }
        }
    }

    Bgpm_Disable();

    return 0;
}

