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

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"
#include "spi/include/l1p/sprefetch.h"


#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


// Vars for dummy operations to measure
#define DSIZE 512
volatile long target[DSIZE];
volatile long source[DSIZE];



void PrintCounts(unsigned hEvtSet)
{
    printf("Counter Results:\n");
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("thd %02d:   0x%016lx <= %s\n", Kernel_ProcessorID(), cnt, label);
    }
}



void MeasureOperations(unsigned hEvtSet)
{
    // init arrays for multiple operations test
    int i;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }

    // create a few L2 events - work backward to foil prefetch?
    // Perform loads to create miss events
    Bgpm_ResetStart(hEvtSet);
    Upci_Delay(200000);
    for (i=DSIZE-1; i>=0; i-=32) {
        source[i] *= target[i];
    }
    Bgpm_Stop(hEvtSet);
}



int main(int argc, char *argv[])
{
    TEST_MODULE(L2 operation with multiple threads);

    Bgpm_Init(BGPM_MODE_SWDISTRIB);

    int hEvtSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hEvtSet, PEVT_L2_HITS);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_MISSES);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_PREFETCH);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_FETCH_LINE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_STORE_LINE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_STORE_PARTIAL_LINE);

    TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 6);
    Bgpm_Apply(hEvtSet);  // should have no effect for L2

    MeasureOperations(hEvtSet);

    PrintCounts(hEvtSet);

    Bgpm_DeleteEventSet(hEvtSet);

    TEST_RETURN();
}

