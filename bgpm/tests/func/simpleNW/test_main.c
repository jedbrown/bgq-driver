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



void PrintCounts(unsigned hEvtSet)
{
    printf("Counter Results:\n");
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("    0x%016lx <= %s\n", cnt, label);
    }
}






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
    printf("Simple sanity check of Bgpm NW Operations.  Counters will return 0 - as there are no real NW operations.\n");

    Bgpm_Init(BGPM_MODE_SWDISTRIB);

    int hNWSet;

    // Network Events
    hNWSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_RECV);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_RECV_FIFO);
    Bgpm_AddEvent(hNWSet, PEVT_NW_CYCLES);
    Bgpm_Attach(hNWSet, UPC_NW_ALL_LINKS, 0);

    fprintf(stderr, "numTargets=%d\n", Bgpm_NumTargets(hNWSet));


    // Start counting for each event set.
    Bgpm_ResetStart(hNWSet);

    CreateEvents();  // doesn't really do any nw operations.

    Bgpm_Stop(hNWSet);

    // Print results.  The network counts will be all zero though (except for cycles)
    PrintCounts(hNWSet);

    // Free up all resources and current mode.
    Bgpm_Disable();

    return 0;
}

