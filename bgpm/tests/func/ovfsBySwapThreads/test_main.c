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

#define MAX_COUNTERS 24
#define NUM_THREADS 128 
int ovfArray[NUM_THREADS][MAX_COUNTERS];

// Array Sizes  cachelines    #dbls   factor to
//              per l1 cache  /line   flush cache
//#define DSIZE   (256   *       8     *  2)           // size needed to flush cache
#define DSIZE   (2048   *       8     *  2)           // size needed to flush cache
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

//UpciBool_t useCounting[CONFIG_MAX_APP_THREADS];
UpciBool_t useCounting[128];


unsigned L2evtList[] = {
   PEVT_L2_MISSES,
   PEVT_L2_HITS,
   PEVT_L2_PREFETCH,
   PEVT_L2_FETCH_LINE 
};

#define LOOPS 5000
#define STRIDE 8

int GetTotalOvfs(unsigned hEvtSet, int ovfArray[NUM_THREADS][MAX_COUNTERS],int thdId)
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int idx;
    int total = 0;
    for (idx=0; idx<numEvts; idx++) {
        total += ovfArray[thdId][idx];
    }
    return total;
}

void PrintCounts(int thdId,unsigned hEvtSet)
{
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    //printf("%5s %18s  %18s %400s %10s %10s\n\n","THD","EVENT COUNT","EVENT COUNT","EVENT LABEL","PERIOD","OVERFLOWS");
    for (i=0; i<numEvts; i++) {
        
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        uint64_t period;
        Bgpm_OverflowHandler_t handler;
        Bgpm_GetOverflow(hEvtSet, i, &period, &handler);
        if(period){
        printf("%5d %18ld  0x%018lx %30s \t%10ld %5d\n",thdId,cnt, cnt, Bgpm_GetEventLabel(hEvtSet, i),period,ovfArray[thdId][i]);
        }
        else{
          printf("%5d %18ld  0x%018lx %30s\n", thdId,cnt,cnt, Bgpm_GetEventLabel(hEvtSet, i));
        }
    }
}


void OvfHandler(int L2EvtSet, uint64_t address, uint64_t ovfVector, const ucontext_t *pContext)
{
    unsigned ovfIdxs[BGPM_MAX_OVERFLOW_EVENTS];
    unsigned len = BGPM_MAX_OVERFLOW_EVENTS;
   
    unsigned thdId = Kernel_ProcessorID();  
    Bgpm_GetOverflowEventIndices(L2EvtSet, ovfVector, ovfIdxs, &len);

    unsigned i;
    for (i=0; i<len; i++) {
        unsigned idx = ovfIdxs[i];
        ovfArray[thdId][idx]++;
    }
}

void reset_ofarray(int thdId)
{
   int i;
   for(i=0;i<MAX_COUNTERS;i++){
   ovfArray[thdId][i]=0;
   }
}

void Overcomitted_Exercise1(UpciBool_t doCounting)
{
    memset((void*)useCounting, 0, sizeof(useCounting));

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,numProcs,L2evtList,numFailures,useCounting) \
        firstprivate(doCounting) 

    {
        unsigned hwThd = Kernel_ProcessorID();
        reset_ofarray(hwThd);
        #pragma omp critical (CheckIfCountable)
        {
            if (useCounting[hwThd] == 0) {
                useCounting[hwThd] = omp_get_thread_num() + 1;
                //fprintf(stderr, "ompThd=%02d Would Count useCounting[hwThd: %d]= %d\n", omp_get_thread_num(),hwThd, useCounting[hwThd]);
                }
           else {
                doCounting = BGPM_FALSE;
                //fprintf(stderr, "ompThd=%02d Would Not Count\n", omp_get_thread_num());
                }
        } 

        int L2EvtSet = -1;

       if(doCounting){
            Bgpm_Init(BGPM_MODE_SWDISTRIB);
            L2EvtSet = Bgpm_CreateEventSet();
            Bgpm_AddEventList(L2EvtSet, L2evtList, sizeof(L2evtList)/sizeof(unsigned));
            Bgpm_SetOverflow(L2EvtSet, Bgpm_GetEventIndex(L2EvtSet, PEVT_L2_MISSES, 0), 1500);
            Bgpm_SetOverflowHandler(L2EvtSet, OvfHandler);
            Bgpm_Apply(L2EvtSet);
        }

        INITDATA;
        if(doCounting) {
        Bgpm_Start(L2EvtSet);
        }

       //#pragma omp barrier    
        // Exercise something - one to cause cache misses, and the other to cause
        // a lot of instructions w/o misses, another to get FP ops.
        int i, j;
        int loops = LOOPS;

      
        for (j=0; j<loops; ++j) {
            // stride by cache line size.
            for (i=DSIZE-1; i>=0; i-=STRIDE) {

                target[i] = source[i];  // here is where the misses should occur
                target[i] = source[i] * 5.0;  // fp multiple should occur here.
            }

            sched_yield();
        }
       if(doCounting){ 
        Bgpm_Stop(L2EvtSet);
       }

        // Print Results each event.
        #pragma omp critical (PrintEvts)
        {
            if (doCounting) {
                printf("Printing %d : handle : %d\n",hwThd,L2EvtSet);
                PrintCounts(hwThd,L2EvtSet);
                   
            }

        }
        if(doCounting){    
        BgpmDebugPrintThreadVars();
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
    fprintf(stderr, "CONFIG_MAX_APP_THREADS=%d\n", CONFIG_MAX_APP_THREADS);

    TEST_CASE(Overcommit Threads);
    Overcomitted_Exercise1(BGPM_TRUE);

    TEST_RETURN();

}

