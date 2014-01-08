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
 * \example bgpm/punit_fastaccess/main.c
 *
 * Example using Bgpm fast access functions
 *
 * The fast access functions bypass some overhead when starting/stopping
 * and reading punit counters, however they may only be used
 * when overflows and multiplexing is disabled.
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
    printf("Count punit events with Fast access methods\n");

    Bgpm_Init(BGPM_MODE_SWDISTRIB);

    int hEvtSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hEvtSet, PEVT_IU_IS1_STALL_CYC);
    Bgpm_AddEvent(hEvtSet, PEVT_IU_IS2_STALL_CYC);
    Bgpm_AddEvent(hEvtSet, PEVT_CYCLES);
    Bgpm_AddEvent(hEvtSet, PEVT_INST_ALL);
    Bgpm_Apply(hEvtSet);

    // Must normally use standard access reset function to clear counters for Fast access
    Bgpm_Reset(hEvtSet);

    Bgpm_Punit_Handles_t hCtrs;
    Bgpm_Punit_GetHandles(hEvtSet, &hCtrs);

    Bgpm_Punit_Start(hCtrs.allCtrMask);
    CreateEvents();
    Bgpm_Punit_Stop(hCtrs.allCtrMask);

    Upci_Delay(800); // counts not stable for 800 cycles (1600 cycles if BGPM_MODE_HWDISTRIB)

    printf("Counter Results:\n");
    int i;
    int numEvts = hCtrs.numCtrs;
    for (i=0; i<numEvts; i++) {
        printf("    0x%016lx <= %s\n",
                Bgpm_Punit_Read(hCtrs.hCtr[i]),
                Bgpm_GetEventLabel(hEvtSet, i));
    }

    return 0;
}

