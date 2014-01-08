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

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "bgpm/include/bgpm.h"

#include "spi/include/upci/testlib/upci_debug.h"


#include "../../test_utils.h"
TEST_INIT();



extern int Bgpm_SetPreReserved(int hEvtSet);


#define DSIZE   (38 * 1024 * 1024 / 8)    // 32 MB needed to flush cache
uint64_t buffer[DSIZE];


void FlushCache()
{
    // Create 38 Mb data base to flush L2 cache with dummy data.
    int i;
    volatile uint64_t data;
    for (i=0; i<DSIZE; i++) {
        data = buffer[i];
    }


}

#define NOP asm volatile("nop;")
#define LOOPS 50


uint64_t mcycles100;
uint64_t minstrs100;
uint64_t mstalls100;
uint64_t mcpi100;

// BGPM Objects
int hPMeas;  // punit set to measure
int hPTest;  // punit set under test
Bgpm_Punit_Handles_t haPMeas;   // measurement punit fast handles

#define PRINT_PATHLEN(_loops_, _title_) \
    Upci_Delay(800); \
    mstalls100 = Bgpm_Punit_Read(haPMeas.hCtr[0]) * 100 / _loops_; \
    mcycles100 = Bgpm_Punit_Read(haPMeas.hCtr[1]) * 100 / _loops_; \
    minstrs100 = Bgpm_Punit_Read(haPMeas.hCtr[2]) * 100 / _loops_; \
    mcpi100 = (mcycles100 * 100) / minstrs100;        \
    fprintf(stderr, _AT_ " PERFMETRICS:  %-60s "       \
                         " AvgInstr=%04ld.%02ld, "    \
                         " AvgCycles=%04ld.%02ld, "         \
                         " AvgStallCyc=%04ld.%02ld, "       \
                         " AvgCPI=%04ld.%02ld \n",      \
            _title_ , minstrs100/100, minstrs100%100, \
                      mcycles100/100, mcycles100%100, \
                      mstalls100/100, mstalls100%100, \
                      mcpi100/100, mcpi100%100 );



int main(int argc, char *argv[])
{
    TEST_MODULE(Get path length of various Bgpm operations);

    int loop;
    memset((void*)buffer, 0, sizeof(buffer));

    PrintWhoAmI();
    Upci_DumpNodeParms(0);

    // Bgpm_Init
    cycle_t tbelapsed, tbstart, tbend;
    tbelapsed = 0;
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        tbstart = GetTimeBase();
        Bgpm_Init(BGPM_MODE_SWDISTRIB);
        tbend = GetTimeBase();
        tbelapsed += tbend-tbstart;
        Bgpm_Disable();
    }
    fprintf(stderr, _AT_ " PERFMETRICS:  %-60s  AvgCycles=%04ld.%02ld\n",
            "Bgpm_Init, 1stOnNode, Uncached :", tbelapsed/LOOPS, tbelapsed*100%100);


    // Bgpm_Disable
    tbelapsed = 0;
    for (loop=0; loop<LOOPS; ++loop) {
        Bgpm_Init(BGPM_MODE_SWDISTRIB);
        FlushCache();
        tbstart = GetTimeBase();
        Bgpm_Disable();
        tbend = GetTimeBase();
        tbelapsed += tbend-tbstart;
    }
    fprintf(stderr, _AT_ " PERFMETRICS:  %-60s  AvgCycles=%04ld.%02ld\n",
            "Bgpm_Disable, LastOnNode, NoEvtSets, Uncached: ", tbelapsed/LOOPS, tbelapsed*100%100);


    // active session for other measurements
    Bgpm_Init(BGPM_MODE_SWDISTRIB);


    // Set up measurement event set
    hPMeas = Bgpm_CreateEventSet();
    unsigned measList[] = {      // List of performance events to collect
        PEVT_IU_IS1_STALL_CYC,
        PEVT_CYCLES,
        PEVT_INST_ALL
    };
    int numMeasEvts = sizeof(measList) / sizeof(int);
    Bgpm_AddEventList(hPMeas, measList, numMeasEvts);
    Bgpm_SetPreReserved(hPMeas);
    Bgpm_Punit_GetHandles(hPMeas, &haPMeas);
    Bgpm_Apply(hPMeas);        // apply to hardware

    #ifdef __VERBOSE__
    printf(_AT_ "\n");
    UPC_P_Dump_State(0);
    #endif


    // ---------------------------
    // Check time to create an event set
    int hEvtSets[LOOPS];
    int nextHdl = 0;
    Bgpm_Reset(hPMeas);

    nextHdl = 0;
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        hEvtSets[nextHdl++] = Bgpm_CreateEventSet();
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
    }
    PRINT_PATHLEN(LOOPS, "Bgpm_CreateEventSet, Uncached, Pathlen :");


    // ---------------------------
    // Check time to add 3 events
    unsigned testList[] = {      // List of performance events to collect
        PEVT_IU_IS1_STALL_CYC,
        PEVT_CYCLES,
        PEVT_INST_ALL
    };
    int numTestEvts = sizeof(measList) / sizeof(int);

    Bgpm_Reset(hPMeas);
    nextHdl = 0;
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        Bgpm_AddEventList(hEvtSets[nextHdl++], testList, numTestEvts);
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
    }
    PRINT_PATHLEN(LOOPS, "Bgpm_AddEventList, 3 Events, NoReorder, Uncached, Pathlen :");


    // ---------------------------
    // Check time to apply events
    hPTest = hEvtSets[LOOPS-1];
    Bgpm_Reset(hPMeas);
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        Bgpm_Apply(hPTest);
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
    }
    PRINT_PATHLEN(LOOPS, "Bgpm_Apply, 3 Events, Uncached, Pathlen :");

    #ifdef __VERBOSE__
    printf(_AT_ "\n");
    UPC_P_Dump_State(0);
    #endif


    // ---------------------------
    // Check time to do heavyweight start w/o reset
    Bgpm_Reset(hPMeas);
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        Bgpm_Start(hPTest);
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
    }
    PRINT_PATHLEN(LOOPS, "Bgpm_Start, WOReset, Uncached, Pathlen :");





    // ---------------------------
    // Check time to do heavyweight stop w delay
    Bgpm_Reset(hPMeas);
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        Bgpm_Stop(hPTest);
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
    }
    PRINT_PATHLEN(LOOPS, "Bgpm_Stop, WDelay, Uncached, Pathlen :");




    // ---------------------------
    // Check time to do heavyweight start with reset
    Bgpm_Reset(hPMeas);
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        Bgpm_ResetStart(hPTest);
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
        Bgpm_Stop(hPTest);
    }
    PRINT_PATHLEN(LOOPS, "Bgpm_ResetStart, 3Counters, Uncached, Pathlen :");

    Bgpm_Stop(hPTest);



    // ---------------------------
    // Check time to do heavyweight reset
    Bgpm_Reset(hPMeas);
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        Bgpm_Reset(hPTest);
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
    }
    PRINT_PATHLEN(LOOPS, "Bgpm_Reset, 3Counters, Uncached, Pathlen :");



    // ---------------------------
    // Check counter overhead of heavyweight start/stop
    Bgpm_Reset(hPMeas);
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Start(hPMeas);
        Bgpm_Stop(hPMeas);
    }
    PRINT_PATHLEN(LOOPS, "Bgpm_Start/Bgpm_Stop, 3Counters, Uncached, Ctr_Overhead :");


    // ---------------------------
    // Check counter overhead of fast start/stop
    Bgpm_Reset(hPMeas);
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
    }
    PRINT_PATHLEN(LOOPS, "Bgpm_Punit_Start/Bgpm_Punit_Stop, 3Counters, Uncached, Ctr Overhead :");


    // ---------------------------
    // Check Delay Operation
    Bgpm_Reset(hPMeas);
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        Delay(800);
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
    }
    PRINT_PATHLEN(LOOPS, "Delay(800), Uncached, PathLen :");



    // ---------------------------
    // Check Upci_Delay Base Time Operation
    Bgpm_Reset(hPMeas);
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        Upci_Delay(800);
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
    }
    PRINT_PATHLEN(LOOPS, "Upci_Delay(800), Uncached, PathLen :");


    // ---------------------------
    // Check time to delete event sets
    Bgpm_Reset(hPMeas);
    nextHdl = 0;
    for (loop=0; loop<LOOPS; ++loop) {
        FlushCache();
        Bgpm_Punit_Start(haPMeas.allCtrMask);
        Bgpm_DeleteEventSet(hEvtSets[nextHdl++]);
        Bgpm_Punit_Stop(haPMeas.allCtrMask);
    }
    PRINT_PATHLEN(LOOPS, "Bgpm_DeleteEventSet, Uncached, Pathlen :");





    TEST_RETURN();
}
