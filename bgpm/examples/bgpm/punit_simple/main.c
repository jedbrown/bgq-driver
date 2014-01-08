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
 * \example bgpm/punit_simple/main.c
 *
 * Simple use of the BGPM API to count punit events.
 * Plus, add events using event list.
 */

#include "stdlib.h"
#include "stdio.h"

#include "bgpm/include/bgpm.h"



// Some arbitrary loops to create some events.
void CreateEvents()
{
    // create some events.
    #define DSIZE 256
    volatile double target[DSIZE];
    volatile double source[DSIZE];
    short i,j;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }
    for (i=0; i<DSIZE; i++) {
        for (j=0; j<6; j++) {
            target[i] *= source[i];
            source[i] = j;
        }
    }
}



int main(int argc, char *argv[])
{
    printf("Simple example of punit event counting\n");

    // By default, the Bgpm interface prints a message and exits the
    // user's program if a usage error occurs - we'll let that happen
    // so no error checking needed.

    // initialize sw thread to use BGPM in sw thread distributed mode
    // Allows up to 12 simultaineous punit counter per thread when using only
    // 2 threads/core
    Bgpm_Init(BGPM_MODE_SWDISTRIB);

    // Create event set and add events needed to calculate CPI,
    int hEvtSet = Bgpm_CreateEventSet();
    unsigned evtList[] =
        { PEVT_IU_IS1_STALL_CYC,
          PEVT_IU_IS2_STALL_CYC,
          PEVT_CYCLES,
          PEVT_INST_ALL,
        };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned) );

    Bgpm_Apply(hEvtSet);
    Bgpm_Start(hEvtSet);

    CreateEvents();

    Bgpm_Stop(hEvtSet);


    printf("Counter Results:\n");
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        printf("    0x%016lx <= %s\n", cnt, Bgpm_GetEventLabel(hEvtSet, i));
    }

    return 0;
}

