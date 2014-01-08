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
#include "cnk/include/Config.h"
#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"

#define TEST_CASE_VERBOSE
#include "../../test_utils.h"
TEST_INIT();
int Dummy=-1;

int main(int argc, char *argv[])
{
    Bgpm_ExitOnError(BGPM_FALSE);  
    printf("Count punit events using BGPM_HWDISTRIB mode\n"
           "Use first thread to count for other hw threads in process\n");

     //SPI calls to get scope of threads for this process
     int numProcesses = Kernel_ProcessCount();    // # processes on node
     int numThreads =   Kernel_ProcessorCount();  // # num hw thread avail to process
     int parentThdId =   Kernel_ProcessorID();     // # lowest hw thd id.
     printf("numProcesses=%d, numThreads=%d, parentThdId=%d\n",
     numProcesses, numThreads, parentThdId);

     // create master event list.
     Bgpm_Init(BGPM_MODE_HWDISTRIB);
     int hEvtSet = Bgpm_CreateEventSet();
     unsigned evtList[] = { PEVT_CYCLES, PEVT_INST_ALL, PEVT_IU_IL1_MISS_CYC };
     Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));

     // Get thread mask for all hw thread associated with current process
     uint64_t thdMask = Kernel_ThreadMask(Kernel_MyTcoord());

     // Attach to each of these threads.
     Bgpm_Attach(hEvtSet, thdMask, 0);

     #pragma omp parallel default(none) num_threads(numThreads)      \
                 shared(stderr,hEvtSet,evtList,thdMask,parentThdId,Dummy,numFailures)
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

            //enable child thread's access to bgpm functions.
            if (thdId!=parentThdId) Bgpm_Init(BGPM_MODE_HWDISTRIB);

            unsigned evtIdx;
            for (evtIdx=0; evtIdx < Bgpm_NumEvents(hEvtSet); evtIdx++) {

                uint64_t thdCount, totalCount;
                //get aggregate from process' threads
                Bgpm_ReadEvent(hEvtSet, evtIdx, &totalCount);
                //get value from just this application thread (can ignore agent threads)
                uint64_t curThdMask = UPC_APP_HWTID_MASK(thdId);

                TEST_FUNCTION(ReadThreadEvent() PARAMETER TEST);  
                TEST_FUNCTION_PARM(Success Case); 
                TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(hEvtSet, evtIdx, curThdMask, 0, &thdCount),0);

                //TEST_FUNCTION_PARM(Invalid Event Handle);
                //TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(Dummy,evtIdx, curThdMask, 0, &thdCount),BGPM_EINV_SET_HANDLE);    
                TEST_FUNCTION_PARM(Invalid Event Index );
                TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(hEvtSet,Dummy, curThdMask, 0, &thdCount),BGPM_EINV_EVT_IDX);    


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
