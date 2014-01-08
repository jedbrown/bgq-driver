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
 * \example bgpm/l2_overflow/main.c
 *
 * Example program describing How to set overflow on L2 events and how to handle the overflow in BGPM
 *
 * Describes setting overflow on some events in the eventset, register overflow handler that will get called 
 * when overflow occurs. 
 *
 * The output will show how many overflows each event has got and finally how many total 
 * overflows the eventset has got. 
 *
 *
 * The output from this test looks similar to the following:
\verbatim
=================================================
Example :L2 UNIT OVERFLOW
=================================================
         EVENT COUNT          EVENT LABEL     PERIOD  OVERFLOWS

  0x000000000000355c56         PEVT_L2_HITS     5000   692
  0x00000000000000067e       PEVT_L2_MISSES     1000     1
  0x00000000000000001d     PEVT_L2_PREFETCH
  0x0000000000000002d8   PEVT_L2_FETCH_LINE
  0x0000000000000002d8   PEVT_L2_STORE_LINE     1000     0

  Total Number of Overflows=693

\endverbatim

 */


#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "bgpm/include/bgpm.h"

#define MAX_COUNTERS 24
//Array to hold overflow count for each event in the event set
int ovfArray[MAX_COUNTERS];


void PrintCounts(unsigned hEvtSet)
{
    int i;

    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    printf("  %18s %20s %10s %10s\n\n","EVENT COUNT","EVENT LABEL","PERIOD","OVERFLOWS");
    for (i=0; i<numEvts; i++) {
        //Read counter for given event, and current thread. 
        //Counts will be availible in "cnt" array.
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        uint64_t period;
        Bgpm_OverflowHandler_t handler;
        Bgpm_GetOverflow(hEvtSet, i, &period, &handler);
        if(period){
        printf("  0x%018lx %30s \t%10ld %5d\n", cnt, Bgpm_GetEventLabel(hEvtSet, i),period,ovfArray[i]);
        }
        else{
          printf("  0x%018lx %30s\n", cnt, Bgpm_GetEventLabel(hEvtSet, i));
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

    unsigned i;
    for (i=0; i<len; i++) {
        unsigned idx = ovfIdxs[i];
        ovfArray[idx]++;
    }

}
    
// Calculate total number of overflow for the eventset.
int GetTotalOvfs(unsigned hEvtSet, int ovfs[MAX_COUNTERS])
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int idx;
    int total = 0;
    for (idx=0; idx<numEvts; idx++) {
        total += ovfs[idx];
    }
    return total;
}

// Fresh start of Array holding overlow count for each event.
void reset_ofarray()
{
   int i;
   for(i=0;i<MAX_COUNTERS;i++){
   ovfArray[i]=0;
   }
}

//Create some L2 Events
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
}

void ExerciseL2unit_of()
{
    reset_ofarray(); 
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    int hEvtSet = Bgpm_CreateEventSet();
    unsigned evtList[] = {
        PEVT_L2_HITS,
        PEVT_L2_MISSES,
        PEVT_L2_PREFETCH,
        PEVT_L2_FETCH_LINE,
        PEVT_L2_STORE_LINE
    };

    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));
    //Bgpm_SetOverflow is used to set the event counter for the current software thread to interrupt on overflow.
    // need to provide the period value to indicate the threshold value of the counter
    // to get generate the ineterrupt.
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_L2_MISSES, 0), 1000);
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_L2_HITS, 0), 5000);
    Bgpm_SetOverflow(hEvtSet, Bgpm_GetEventIndex(hEvtSet, PEVT_L2_STORE_LINE, 0), 1000);
    //If an event has Overflow set, then the handler will be scheduled to be called as a child of an BGPM signal handler.
    Bgpm_SetOverflowHandler(hEvtSet, OvfHandler); 

    Bgpm_Apply(hEvtSet);
    Bgpm_Start(hEvtSet);

    CreateEvents();

    Bgpm_Stop(hEvtSet); 
    PrintCounts(hEvtSet);
    // Finally we display total number of overrlow we got for all events.
    fprintf(stderr, "\n  Total Number of Overflows=%d\n",GetTotalOvfs(hEvtSet, ovfArray));


}

int main(int argc, char *argv[])
{
    printf("=================================================\n");
    printf("Example :L2 UNIT OVERFLOW\n");
    printf("=================================================\n");

    ExerciseL2unit_of();

   return 0;

}


