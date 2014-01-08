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
 * \example bgpm/punit_rtnerrs/main.c
 *
 * Simple use of the BGPM API to count punit events with local error handling,
 * rather than exiting on error.  Also, add events individually rather
 * than in a list.
 */


#include "stdlib.h"
#include "stdio.h"

#include "bgpm/include/bgpm.h"


// some utility macros
#define PRINTERR(...) fprintf(stderr, "ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }
#define BGPMDIE(_funcname_) { DIE(_funcname_ ": %d; %s\n", Bgpm_LastErr(), Bgpm_LastErrStrg()); }


#define ADD_EVENT(hEvtSet, evtId)  \
{                                    \
    int rc = Bgpm_AddEvent(hEvtSet, evtId); \
    if (rc < 0) BGPMDIE("Bgpm_AddEvent"); \
}



// Print counter results from evt set
void PrintCounts(unsigned hEvtSet)
{
    printf("Counter Results:\n");
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        if (Bgpm_ReadEvent(hEvtSet, i, &cnt)) BGPMDIE("Bgpm_ReadEvent");
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        if (!label) BGPMDIE("Bgpm_GetEventLabel");
        printf("    0x%016lx <= %s\n", cnt, label);
    }
}



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
    printf("Simple example of punit event counting - return errors\n");

    // By default, the Bgpm interface prints a message and exits the
    // user's program if a usage error occurs, so normally no need to check
    // for function errors.  But, it might be nice to know where in "my" test code
    // the error occurs, so I'll turn off the print and exit functions and
    // detect errors my self.
    Bgpm_PrintOnError(BGPM_FALSE);
    Bgpm_ExitOnError(BGPM_FALSE);


    // initialize sw thread to use BGPM in sw thread distributed mode
    // Up to 12 simultaineous punit counter per thread for 2 threads/core
    int rc = Bgpm_Init(BGPM_MODE_SWDISTRIB);
    if (rc < 0) BGPMDIE("Bgpm_Init");

    // Create event set and add events needed to calculate CPI,
    // # of stalls, and stall cycles.
    int hEvtSet = Bgpm_CreateEventSet();
    if (hEvtSet < 0) BGPMDIE("Bgpm_CreateEventSet");

    ADD_EVENT(hEvtSet, PEVT_IU_IS1_STALL_CYC);
    ADD_EVENT(hEvtSet, PEVT_IU_IS2_STALL_CYC);
    ADD_EVENT(hEvtSet, PEVT_CYCLES);
    ADD_EVENT(hEvtSet, PEVT_INST_ALL);
    printf("NumEvents = %d\n", Bgpm_NumEvents(hEvtSet));

    if (Bgpm_Apply(hEvtSet)) BGPMDIE("Bgpm_Apply");

    if (Bgpm_Start(hEvtSet) < 0) BGPMDIE("Bgpm_Start");

    CreateEvents();

    Bgpm_Stop(hEvtSet);

    PrintCounts(hEvtSet);

    return 0;
}

