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
 * \example bgpm/punit_mux/main.c
 *
 * Example showing Multiplex support for BGPM API.
 * this example shows how can we enable multiple on punit event set
 * and how to do manual switching between multiplex groups.  
 * * The output from this test looks similar to the following:
 * \verbatim
 *
 *Example for manual Multiplex with mux group normalization
=============================

Mux Stats: numGrps=2, numSwitches=100, elapsedCycles=0x00000000ce1780d6
grpCycles:
  cycles[00]=0x00000000670bbeba
  cycles[01]=0x00000000670bc21c
Counter Results:
      0x000000005f6a3ddb <= PEVT_AXU_IDLE
      0x0000000007a18440 <= PEVT_AXU_INSTR_COMMIT
      0x0000000000000034 <= PEVT_IU_IL1_MISS
      0x000000000000008e <= PEVT_L1P_BAS_LD
      0x0000000003d09020 <= PEVT_INST_QFPU_FMUL
      0x00000000670bc21c <= PEVT_XU_TIMEBASE_TICK
      0x00000000000033c3 <= PEVT_XU_ANY_FLUSH
      0x00000000356cde22 <= PEVT_IU_IS1_STALL_CYC
      0x000000000052de02 <= PEVT_L1P_STRM_WRT_INVAL
      0x000000000b721460 <= PEVT_LSU_COMMIT_AXU_LDS


Example for manual Multiplex without normalization
=============================

Mux Stats: numGrps=2, numSwitches=100, elapsedCycles=0x00000000ce178474
grpCycles:
  cycles[00]=0x00000000670bc0ae
  cycles[01]=0x00000000670bc3c6
Counter Results:
      0x000000005f6a3cae <= PEVT_AXU_IDLE
      0x0000000007a18400 <= PEVT_AXU_INSTR_COMMIT
      0x0000000000000035 <= PEVT_IU_IL1_MISS
      0x00000000000000c2 <= PEVT_L1P_BAS_LD
      0x0000000003d09000 <= PEVT_INST_QFPU_FMUL
      0x00000000670bc0ae <= PEVT_XU_TIMEBASE_TICK
      0x00000000000033f4 <= PEVT_XU_ANY_FLUSH
      0x00000000356cdc1f <= PEVT_IU_IS1_STALL_CYC
      0x000000000052de00 <= PEVT_L1P_STRM_WRT_INVAL
      0x000000000b721400 <= PEVT_LSU_COMMIT_AXU_LDS


Example for Automatic Multiplex with normalization
=============================

Mux Stats: numGrps=2, numSwitches=115, elapsedCycles=0x00000000ce1c8744
grpCycles:
  cycles[00]=0x0000000067b64b74
  cycles[01]=0x0000000066663bd0
Counter Results:
      0x0000000060084874 <= PEVT_AXU_IDLE
      0x0000000007ae0300 <= PEVT_AXU_INSTR_COMMIT
      0x00000000000003a9 <= PEVT_IU_IL1_MISS
      0x00000000000002e8 <= PEVT_L1P_BAS_LD
      0x0000000003ca585d <= PEVT_INST_QFPU_FMUL
      0x0000000067b64b74 <= PEVT_XU_TIMEBASE_TICK
      0x0000000000003937 <= PEVT_XU_ANY_FLUSH
      0x000000003516c25e <= PEVT_IU_IS1_STALL_CYC
      0x0000000000ea41d2 <= PEVT_L1P_STRM_WRT_INVAL
      0x000000000b5f7169 <= PEVT_LSU_COMMIT_AXU_LDS
\endverbatim
 */


#include "stdlib.h"
#include "stdio.h"

#include "bgpm/include/bgpm.h"

unsigned evtList[] =
{
      PEVT_AXU_IDLE,
      PEVT_AXU_INSTR_COMMIT,
      PEVT_IU_IL1_MISS,
      PEVT_L1P_BAS_LD,
      PEVT_INST_QFPU_FMUL,   
      PEVT_XU_TIMEBASE_TICK,
      PEVT_XU_ANY_FLUSH,
      PEVT_IU_IS1_STALL_CYC, 
      PEVT_L1P_STRM_WRT_INVAL,
      PEVT_LSU_COMMIT_AXU_LDS
};

int hEvtSet;


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
        for (j=0; j<5000; j++) {
            target[i] *= source[i];
            source[i] = j;
        }
    }
}



void PrintCounts(unsigned hEvtSet)
{
    Bgpm_MuxStats_t mStats;
    Bgpm_GetMultiplex(hEvtSet, &mStats);
    printf("\nMux Stats: numGrps=%d, numSwitches=%d, elapsedCycles=0x%016lx\n",
            mStats.numGrps, mStats.numSwitches, mStats.elapsedCycles);

    int i;
    printf("grpCycles: \n");
    for (i=0; i<mStats.numGrps; i++) {
        printf("  cycles[%02d]=0x%016lx\n", i, Bgpm_GetMuxElapsedCycles(hEvtSet, i));
    }
    printf("Counter Results:\n");
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("      0x%016lx <= %s\n", cnt, label);
    }
}



void multiplex(period,normalize)
{
    // initialize sw thread to use BGPM in sw thread distributed mode
    // Allows up to 12 simultaineous punit counter per thread when using only
    // 2 threads/core
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
  
    // Create event set and add events needed to calculate CPI,
    int hEvtSet = Bgpm_CreateEventSet();

    // Multiplexing must be enabled prior to adding events to a Punit event set.
    // With Multiplexing active on Punit event set, we can have as many Punit events as we like,
    // regardless of Max Events. Muxing will create as many groups as needed to hold all the events,
    // but by default only assigns up to 5 events per group (And reserves the 6th counter to track the number of
    // cycles a group has been active).
    // However, a few of the core-wide attributes used by events must still remain consistent
    // (like the l1p mode associated with the l1p events)
    //
    // Parameters:
    // period-> number of cycles between multiplex switches. A value of 0 disables multiplexing for this thread,
    //     which means you must explicitly call Bgpm_SwitchMux to switch between groups.
    // normalize->Pass in BGPM_NORMAL or BGPM_NOTNORMAL to choose whether event counts
    //     reported by Bgpm_ReadEvent() or Bgpm_ReadEventList() will be scaled to the to the maximum number of cycles spent
    //     by any mux group.
    //     With BGPM_NOTNORMAL, Bgpm_ReadEvent() and Bgpm_ReadEventList() report the raw numbers and you may use
    //     Bgpm_GetMultiplex() and Bgpm_GetMuxEventElapsedCycles() to do your own normalization as desired.
    Bgpm_SetMultiplex2(hEvtSet, period, normalize, BGPMMuxMinEvts);

    Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned) );

    Bgpm_Apply(hEvtSet);
    Bgpm_Start(hEvtSet);

    const int loops = 100;
    int i; 
    for (i=0; i<loops; i++) {
        CreateEvents();
        if(!period){Bgpm_SwitchMux(hEvtSet);} // Switching mutiplex is needed only in case of period=0.
    }

    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);
    Bgpm_Disable();
}



int main(int argc, char *argv[])
{
    printf("\n\nExample for manual Multiplex with mux group normalization\n");
    printf("=============================\n");
    int period=0;
    int normalize=BGPM_NORMAL;
    multiplex(period,normalize);

    printf("\n\nExample for manual Multiplex without normalization\n");
    printf("=============================\n");
    period=0;
    normalize=BGPM_NOTNORMAL;
    multiplex(period,normalize);

    printf("\n\nExample for Automatic Multiplex with normalization\n");
    printf("=============================\n");
    period=30000000;
    normalize=BGPM_NORMAL;
    multiplex(period,normalize);
    return 0;

}

