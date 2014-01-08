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
#include "../../test_utils.h"

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upc_p_debug.h"



unsigned evtList[6] = { PEVT_IU_AXU_FXU_DEP_HIT_CYC,    //  0
                        PEVT_LSU_COMMIT_LD_MISSES,      //  1
                        PEVT_LSU_COMMIT_CACHEABLE_LDS,  //  2
                        PEVT_L1P_BAS_MISS,              //  3
                        PEVT_INST_XU_ALL,               //  4
                        PEVT_INST_QFPU_ALL};            //  5

unsigned l2EvtList[6] = {
        PEVT_L2_HITS,
        PEVT_L2_MISSES,
        PEVT_L2_PREFETCH,
        PEVT_L2_FETCH_LINE,
        PEVT_L2_STORE_LINE,
        PEVT_L2_STORE_PARTIAL_LINE};

int numEvts = sizeof(evtList) / sizeof(unsigned);
int numL2Evts = sizeof(l2EvtList) / sizeof(unsigned);


int main(int argc, char *argv[])
{
    fprintf(stderr, "Stress of punit Bgpm_Apply()\n");

    int numThreads = 64;
    int seconds = 300;
    if (argc > 1) {
       seconds = atoi(argv[1]);
    }
    fprintf(stderr, "numThreads=%d, seconds=%d\n", numThreads, seconds);

    uint64_t cpuSpeed = 1600; // Upci_GetNodeParms()->cpuSpeed
    uint64_t targCycles = cpuSpeed * 1024*1024*seconds;
    fprintf(stderr, "cpuspeed=%ld  targCycles=%ld\n", cpuSpeed, targCycles);

    uint64_t startTB = GetTimeBase();
    uint64_t totalLoops = 0;

    #pragma omp parallel default(none) num_threads(numThreads)      \
         shared(evtList,numEvts,stderr,targCycles,startTB,totalLoops,numL2Evts,l2EvtList)
    {
        uint64_t curTB = startTB;
        printf(_AT_ " Begin thread %d\n", Kernel_ProcessorID());

        while ((GetTimeBase()-startTB) < targCycles) {

            Bgpm_Init(BGPM_MODE_SWDISTRIB);

#if 0
             int hPEvtSet = Bgpm_CreateEventSet();
             int hL2EvtSet = Bgpm_CreateEventSet();
             Bgpm_AddEventList(hPEvtSet, evtList, numEvts);
             Bgpm_AddEventList(hL2EvtSet, l2EvtList, numL2Evts);
             Bgpm_Apply(hPEvtSet);        // apply to hardware
             Bgpm_ResetStart(hPEvtSet);   // start counting.
             Bgpm_Start(hL2EvtSet);
             Bgpm_Stop(hPEvtSet);
             Bgpm_Stop(hL2EvtSet);
             Bgpm_DeleteEventSet(hL2EvtSet);
             Bgpm_DeleteEventSet(hPEvtSet);
#endif
             Bgpm_Disable();

             //TEST_AT();
             if (Kernel_ProcessorID() == 0) {
                 if (totalLoops++ % 2000 == 0) {
                     uint64_t newTB = GetTimeBase();
                     uint64_t deltaTB = newTB-curTB;
                     curTB = newTB;
                     printf("TotalLoops = %ld, elapsedTB=%14ld, deltaTB=%14ld\n", totalLoops, curTB-startTB, deltaTB);
                     MemSizes_t memSizes;
                     GetMemSizes(&memSizes);
                     PrintMemSizes(_AT_, &memSizes);
                 }
             }
        }
        printf(_AT_ " End thread %d\n", Kernel_ProcessorID());
    }


    printf("After OMP parallel, totalLoops = %ld\n", totalLoops);
    return(0);
}


