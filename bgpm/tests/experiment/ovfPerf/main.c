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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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
 *
 * Experimentation program for showing number of overflows by varying period value and 
 * plot a graph showing numberof overflows versus time taken to process the overflows
 * or number of overflows and number of cycles taken to process the overflows.
 *

*/


#include "stdlib.h"
#include "stdio.h"

#include "bgpm/include/bgpm.h"
#include <omp.h>


#define MAX_COUNTERS 24
#define NUM_THREADS 24
//int numProcs;
//int numThreads;

//Array to hold overflow count for each event in the event set
int ovfArray[NUM_THREADS][MAX_COUNTERS];

void PrintCounts(int thdId,unsigned hEvtSet)
{
    int i;

    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    //printf("%6s  %18s %18s %30s %15s %10s\n\n","THD ID","EVENT COUNT","EVENT COUNT","EVENT LABEL","PERIOD","OVERFLOWS");
    //numEvts=1;
    for (i=0; i<numEvts; i++) {
    //for (i=0; i<1; i++) {
        //Read counter for given event, and current thread.
        //Counts will be availible in "cnt" array.
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        uint64_t period;
        Bgpm_OverflowHandler_t handler;
        Bgpm_GetOverflow(hEvtSet, i, &period, &handler);
        if(period){
          printf("  0x%018lx %18ld %30s \t%10ld %5d\n", cnt,cnt, Bgpm_GetEventLabel(hEvtSet, i),period,ovfArray[thdId][i]);
          //printf("%3d  0x%018lx %18ld %30s\n", thdId,cnt,cnt,Bgpm_GetEventLabel(hEvtSet, i));
        }
        else{
          printf("%3d  0x%018lx %18ld %30s\n", thdId,cnt,cnt,Bgpm_GetEventLabel(hEvtSet, i));
        }
    }
}

// This is the overflow handler.
void OvfHandler(int hEvtSet, uint64_t address, uint64_t ovfVector, const ucontext_t *pContext)
{
    unsigned ovfIdxs[BGPM_MAX_OVERFLOW_EVENTS];
    unsigned len = BGPM_MAX_OVERFLOW_EVENTS;
    //Within an overflow event handler, convert the passed opaque ovfVector into
    //a list of indicies into the event set to the events which overflowed
    //parameters:
    //  hEvtSet    =>  handle to event set
    //  ovfVector  =>  eventset unique opaque mask needed by Bgpm_GetOverflowEventIndices() to identify which events have overflowed.
    //  *pIndicies =>  user allocated array to receive output indicies
    //  *pLen      =>  Input array length / output number of filled indicies

    Bgpm_GetOverflowEventIndices(hEvtSet, ovfVector, ovfIdxs, &len);
    unsigned thdId = Kernel_ProcessorID();

    unsigned i;
    for (i=0; i<len; i++) {
        unsigned idx = ovfIdxs[i];
        ovfArray[thdId][idx]++;
    }

}

// Calculate total number of overflow for the eventset.
int GetTotalOvfs(unsigned hEvtSet, int ovfs[NUM_THREADS][MAX_COUNTERS],int thdId)
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int idx;
    int total = 0;
    for (idx=0; idx<numEvts; idx++) {
        total += ovfs[thdId][idx];
    }
    return total;
}

// Fresh start of Array holding overlow count for each event.
void reset_ofarray(int thdId)
{
   int i;
   for(i=0;i<MAX_COUNTERS;i++){
   ovfArray[thdId][i]=0;
   }
}


// Some arbitrary loops to create some events.
void CreateEvents()
{
    int i,j;
    int loops = 2000;
    #define DSIZE   (256   *       8     *  2)           // size needed to flush cache
     volatile double target[DSIZE];
     volatile double source[DSIZE];
     for (i=0; i<DSIZE; i++) {
         source[i] = i;
         target[i] = i+1;
     }
    // Exercise something - one to cause cache misses, and the other to cause
    // a lot of instructions w/o misses, another to get FP ops.
    // L2 cache is 32 Meg
   
    for (j=0; j<loops; ++j) {
        for (i=DSIZE-1; i>=0; i-=8) {
                target[i] = source[i];  // here is where the misses should occur
                target[i] = source[i] * 5.0;  // fp multiple should occur here.
            }
        }
   // Crete some floating point instruction events
   volatile double a=9.87678;
   volatile double b=8.767876;
   volatile int x[1024],y[1024],z[2048];
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

void ExercisePunit_of(int period,int mux,int numThreads,int numProcs)
{
    //unsigned thdId;

   #pragma omp parallel default(none) num_threads(numThreads)    \
      shared(stderr,numProcs,numThreads,mux,period,ovfArray)
   {
   unsigned  thdId = Kernel_ProcessorID();

    reset_ofarray(thdId);
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    uint64_t ref_clock=0,clock=0;
    int hEvtSet = Bgpm_CreateEventSet();
#if 0
    unsigned evtList[] = {
        PEVT_CYCLES,
        PEVT_XU_TIMEBASE_TICK,
        PEVT_IU_IS1_STALL_CYC,
        PEVT_INST_ALL,
        PEVT_IU_IL1_MISS, 
        PEVT_AXU_CR_COMMIT,
        PEVT_IU_IS2_STALL_CYC,
        PEVT_L1P_BAS_LU_STALL_LIST_RD,
        PEVT_IU_IBUFF_FLUSH,
        //PEVT_L1P_STRM_WRT_INVAL,
        PEVT_XU_ANY_FLUSH
        //PEVT_AXU_INSTR_COMMIT, 
        //PEVT_AXU_IDLE
    };
#endif
     unsigned evtList[] = {
        PEVT_CYCLES,
        PEVT_CYCLES,
        PEVT_CYCLES,
        PEVT_CYCLES,
        PEVT_CYCLES,
        PEVT_CYCLES
        }; 
    if(mux){
    Bgpm_SetMultiplex2(hEvtSet, 10000,BGPM_NOTNORMAL ,BGPMMuxMinEvts); 
    }

    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    //Bgpm_SetOverflow is used to set the event counter for the current software thread to interrupt on overflow.
    // need to provide the period value to indicate the threshold value of the counter
    // to get generate the ineterrupt.
    if(period){ 
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 0), period);
//    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 1), period);
//    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 2), period);
//    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 3), period);
//    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 4), period);
//    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 5), period);
//    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 1), (period+1000));
//    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 2), (period+2000));
//    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 3), (period+3000));
//    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 4), (period+4000));
//    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_CYCLES, 5), (period+5000));
    //Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_XU_TIMEBASE_TICK, 3), period);
    //Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_XU_TIMEBASE_TICK, 4), period);
    //Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_XU_TIMEBASE_TICK, 5), period);
    //If an event has Overflow set, then the handler will be scheduled to be called as a child of an BGPM signal handl
    Bgpm_SetOverflowHandler(hEvtSet, OvfHandler);
    }
    Bgpm_Apply(hEvtSet);

    ref_clock=GetTimeBase();
    Bgpm_Start(hEvtSet);

    CreateEvents();
    Bgpm_Stop(hEvtSet);
    clock=GetTimeBase();
   #pragma omp critical(prints)
    {
    printf("thd %d Time taken for period %d   is : %ld \n",thdId,period,(clock-(ref_clock)));
    PrintCounts(thdId,hEvtSet);
    //Finally we display total number of overrlow we got for all events.
    //fprintf(stderr, "\n  TotalNumberof Overflows = %d\n",GetTotalOvfs(hEvtSet, ovfArray));
    printf("\n thdId: %d  TotalNumberof Overflows = %d\n",thdId,GetTotalOvfs(hEvtSet, ovfArray,thdId));
    }
   Bgpm_Disable();
  }

}




int main(int argc, char *argv[])
{
    printf("=================================================================\n");
    printf("Example :PUNIT UNIT OVERFLOW With More than 6 counters per thread ( Multiplexing)\n");
    printf("=================================================================\n");
    int i;
     #if 1
     int numThreads = atoi(argv[1]);
     int numProcs= Kernel_ProcessCount();
     printf("numProcs=%d numThreads: %d\n", numProcs,numThreads);
     #endif
#if 1   
    for(i=100000;i>900;i=i-1000){
    //for(i=3000;i>900;i=i-1){
    printf("Checking %d\n",i);
    ExercisePunit_of(i,BGPM_FALSE,numThreads,numProcs);  
    }
#endif
    //ExercisePunit_of(0,BGPM_FALSE);

    return 0;
}

