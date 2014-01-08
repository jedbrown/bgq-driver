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
 * \example bgpm/punit_lowlat/main.c
 *
 * BGPM API to count punit events with low latency access methods.
 * Also use event list to add events.
 *
 * Low latency mode allows for a fastest start/stop/read/reset of punit
 * counters, but the counters are only 14 bits in width and must be
 * used carefully.  BGPM_LLCOUNT_OVERFLOW macro can be used to indicate
 * if an individual count as overflowed.
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
    printf("Count punit events with Event list and Low Latency access methods\n");

    Bgpm_Init(BGPM_MODE_LLDISTRIB);  // only LLDISTRIB mode.

    int hEvtSet = Bgpm_CreateEventSet();
    unsigned evtList[] =
        { PEVT_IU_IS1_STALL_CYC,
          PEVT_IU_IS2_STALL_CYC,
          PEVT_CYCLES,
          PEVT_INST_ALL,
        };
    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned) );
    Bgpm_Apply(hEvtSet);

    Bgpm_Punit_LLHandles_t hCtrs;
    Bgpm_Punit_GetLLHandles(hEvtSet, &hCtrs);

    Bgpm_Punit_LLResetStart(hCtrs.allCtrMask);   // can reset with start in LL mode
    CreateEvents();
    Bgpm_Punit_LLStop(hCtrs.allCtrMask);

    // No delay needed - ctrs immediately stable

    printf("LL Counter Results:\n");
    int i;
    for (i=0; i<hCtrs.numCtrs; i++) {
        uint64_t cnt = Bgpm_Punit_LLRead(hCtrs.hCtr[i]);
        if (BGPM_LLCOUNT_OVERFLOW(cnt)) {
            printf("    0x%016lx <= %s : Overflowed\n",
                    cnt, Bgpm_GetEventLabel(hEvtSet, i));
        }
        else {
            printf("    0x%016lx <= %s\n",
                    cnt, Bgpm_GetEventLabel(hEvtSet, i));
        }
    }


    return 0;
}

