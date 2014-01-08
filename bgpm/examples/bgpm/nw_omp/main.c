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
 * \example bgpm/nw_omp/main.c
 *
 * Example of collecting network counters.
 *
 * This example shows about the Punit Thread Overflows can be used to
 * sample Network counters.
 * This test does not actually actually connect to other nodes, so the
 * NW counters end up be a zero value except for the NW Cycle counts.
 *
 * Output looks similar to the following:
\verbatim
Print some network events every n cycles or m instructions from differnet threads
numProcesses=1, numThreads=2, parentThdId=0
Torus A direction NW Counts after 1000002 cycles in thdId 0:
   PEVT_NW_USER_PP_SENT                      = 0
   PEVT_NW_USER_PP_RECV                      = 0
   PEVT_NW_USER_PP_RECV_FIFO                 = 0
   PEVT_NW_CYCLES                            = 353818
Torus B direction NW Counts after 100000 instructions in thdId 4:
   PEVT_NW_USER_PP_SENT                      = 0
   PEVT_NW_USER_PP_RECV                      = 0
   PEVT_NW_USER_PP_RECV_FIFO                 = 0
   PEVT_NW_CYCLES                            = 637700
Torus A direction NW Counts after 2000004 cycles in thdId 0:
   PEVT_NW_USER_PP_SENT                      = 0
   PEVT_NW_USER_PP_RECV                      = 0
   PEVT_NW_USER_PP_RECV_FIFO                 = 0
   PEVT_NW_CYCLES                            = 703563
Torus B direction NW Counts after 200001 instructions in thdId 4:
   PEVT_NW_USER_PP_SENT                      = 0
   PEVT_NW_USER_PP_RECV                      = 0
   PEVT_NW_USER_PP_RECV_FIFO                 = 0
   PEVT_NW_CYCLES                            = 750923
...
\endverbatim
 */


#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "bgpm/include/bgpm.h"


int hNwEvtSet;

void PrintNwCounts(int hEvtSet, uint64_t nwTargMask)
{
    unsigned evtIdx;
    for (evtIdx=0; evtIdx < Bgpm_NumEvents(hEvtSet); evtIdx++) {
        uint64_t count;
        Bgpm_NW_ReadLinkEvent(hEvtSet, evtIdx, nwTargMask, &count);
        printf("   %-40s  = %ld\n", Bgpm_GetEventLabel(hEvtSet, evtIdx), count);
    }
}


void OvfHandler(int hEvtSet, uint64_t address, uint64_t ovfVector, const ucontext_t *pContext)
{
    // Get the indices of all events which have overflowed.
    unsigned ovfIdxs[BGPM_MAX_OVERFLOW_EVENTS];
    unsigned len = BGPM_MAX_OVERFLOW_EVENTS;
    Bgpm_GetOverflowEventIndices(hEvtSet, ovfVector, ovfIdxs, &len);

    // count hits for this address value
    unsigned i;
    for (i=0; i<len; i++) {
        uint64_t count;
        if (Bgpm_GetEventId(hEvtSet, ovfIdxs[i]) == PEVT_CYCLES) {
            Bgpm_ReadEvent(hEvtSet, ovfIdxs[i], &count);
            // This printf is not strictly legal - as the operations are happening
            // in a signal handler and the printf is not supposed to be reentrant.
            printf("Torus A direction NW Counts after %ld cycles in thdId %d:\n",
                    count, Kernel_ProcessorID());
            PrintNwCounts(hNwEvtSet, UPC_NW_LINK_A);
        }
        else if (Bgpm_GetEventId(hEvtSet, ovfIdxs[i]) == PEVT_INST_ALL) {
            Bgpm_ReadEvent(hEvtSet, ovfIdxs[i], &count);
            printf("Torus B direction NW Counts after %ld instructions in thdId %d:\n",
                    count, Kernel_ProcessorID());
            PrintNwCounts(hNwEvtSet, UPC_NW_LINK_B);
        }
    }
}



int main(int argc, char *argv[])
{
    printf("Print some network events every n cycles or m instructions from different threads\n");

    // SPI calls to get scope of threads for this process
    int numProcesses = Kernel_ProcessCount();    // # processes on node
    int numThreads =   2;                        // # hw threads we'll use
    int parentThdId =  Kernel_ProcessorID();     // # main process thd id.
    printf("numProcesses=%d, numThreads=%d, parentThdId=%d\n",
            numProcesses, numThreads, parentThdId);

    // create network master event list.
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    hNwEvtSet = Bgpm_CreateEventSet();
    unsigned nwEvtList[] = {
            PEVT_NW_USER_PP_SENT, PEVT_NW_USER_PP_RECV,
            PEVT_NW_USER_PP_RECV_FIFO, PEVT_NW_CYCLES
    };
    Bgpm_AddEventList(hNwEvtSet, nwEvtList, sizeof(nwEvtList)/sizeof(unsigned));
    Bgpm_Attach(hNwEvtSet, UPC_NW_ALL_LINKS, 0);  // attach to all links (agentTarg argument is ignored)

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,hNwEvtSet,nwEvtList,parentThdId)
    {
        unsigned thdId = Kernel_ProcessorID();
        if (thdId != parentThdId) Bgpm_Init(BGPM_MODE_SWDISTRIB); // only once per thread

        int isNetworkController = Bgpm_IsController(hNwEvtSet);

        unsigned puEvtList[] = { PEVT_CYCLES, PEVT_INST_ALL };

        // Set up thread to cause overflows on 1M cycles, and the other thread
        // to overflow on 100M instructions.  We'll print network counts
        // from A and B directions at each of those points.
        int hPuEvtSet = Bgpm_CreateEventSet();
        Bgpm_AddEventList(hPuEvtSet, puEvtList, sizeof(puEvtList)/sizeof(unsigned));
        if (thdId == parentThdId) {
            Bgpm_SetOverflow(hPuEvtSet, Bgpm_GetEventIndex(hPuEvtSet, PEVT_CYCLES, 0),   1000*1000);
        }
        else {
            Bgpm_SetOverflow(hPuEvtSet, Bgpm_GetEventIndex(hPuEvtSet, PEVT_INST_ALL, 0), 100*1000);
        }
        Bgpm_SetOverflowHandler(hPuEvtSet, OvfHandler);
        Bgpm_Apply(hPuEvtSet);

        if (isNetworkController) Bgpm_Start(hNwEvtSet);
        Bgpm_Start(hPuEvtSet);

        //... Run work load;
        Upci_Delay(1000*1000*10);

        Bgpm_Stop(hPuEvtSet);
        if (isNetworkController) Bgpm_Stop(hNwEvtSet);

        if (thdId != parentThdId) Bgpm_Disable(); // only once per thread

    }


    Bgpm_Disable();

    return 0;
}

