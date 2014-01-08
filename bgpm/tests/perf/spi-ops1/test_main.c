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
#include "hwi/include/bqc/A2_inlines.h"
#include "spi/include/kernel/location.h"

#include "spi/include/upci/testlib/upc_p_debug.h"

#include "spi/include/upci/mode.h"
#include "spi/include/upci/punit.h"


#define PRINTERR(...) printf("ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }


#define ADD_PUNIT_EVENT(unit, evt, thd, handle)  \
{                                    \
    int rc = Upci_Punit_AddEvent(&unit, evt, thd, &(handle));      \
    if (rc < 0) {                                                 \
        PRINTERR("Add of event %d Failed, rc=%d\n", evt, rc);       \
    } \
}


#define PRINT_PATHLEN(_title_) \
    Upci_Delay(800); \
    mstalls = Upci_Punit_LLPCtr_Read(hMEvt[0]); \
    mcycles = Upci_Punit_LLPCtr_Read(hMEvt[1]); \
    minstrs = Upci_Punit_LLPCtr_Read(hMEvt[2]); \
    mcpi = (1.0 * mcycles) / minstrs;           \
    printf("Line %03d   %-50s Instr=%04ld, Cycles=%04ld, StallCyc=%04ld, CPI=%0.2lf\n", \
            __LINE__, _title_ " pathlen: ", minstrs, mcycles, mstalls, mcpi);






#define GTB GetTimeBase()                //!< short name for convenience
typedef uint64_t cycle_t;                //!< cycle count types returned from GetTimeBase();


#define NOP asm volatile("nop;")
#define LOOPS 100


uint64_t mcycles;
uint64_t minstrs;
uint64_t mstalls;
double   mcpi;

// SPI Objects
Upci_Mode_t upciMode;
Upci_Punit_t measPunit;   // actual measurement punit
Upci_Punit_t testPunit;   // test punit to run operations to measure.
Upci_Ctr_CtlMask_t measCtlMask;  // measure ll counter control mask.
Upci_Ctr_CtlMask_t testCtlMask;  // test ll counter control mask.

// Arrays to hold on to event handles.
Upci_Punit_EventH_t hMEvt[24];   // measurement events
int cMNum = 0;

Upci_Punit_EventH_t hTEvt[24];   // test events.
int cTNum = 0;


int main(int argc, char *argv[])
{
    printf("Get path length of various Upci operations\n");
    //upc_p_mmio_t *const upc_p = UPC_P_Addr(UPC_LOCAL_UNIT);

    Upci_Mode_Init(&upciMode, 0, UPC_CM_INDEP, Kernel_ProcessorCoreID());
    Upci_Punit_Init(&measPunit, &upciMode, Kernel_ProcessorCoreID());

    // Add events in this order:
    //   1) l1p/Wakeup
    //   2) A2 XU
    //   other A2 3rd, Opcode events last (though none in this list)
    short thd = Kernel_PhysicalHWThreadID();
    ADD_PUNIT_EVENT(measPunit, PEVT_IU_IS1_STALL_CYC, thd, hMEvt[cMNum++]);
    ADD_PUNIT_EVENT(measPunit, PEVT_CYCLES, thd, hMEvt[cMNum++]);
    ADD_PUNIT_EVENT(measPunit, PEVT_INST_ALL, thd, hMEvt[cMNum++]);
    int rc = Upci_Punit_Apply(&measPunit);
    if (rc) {
        DIE("Upci_Punit_Apply failed; rc = %d\n", rc);
    }
    measCtlMask = Upci_Punit_GetLLCtlMask(&measPunit, 0);



    // ---------------------------
    // Check time to init a punit
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    Upci_Punit_Init(&testPunit, &upciMode, Kernel_ProcessorCoreID());

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_Init");

    //Upci_Punit_Dump(0, &testPunit);

    // Prevent test counters from colliding with measurement counters
    uint64_t sigMask = Upci_Punit_GetUsedSignalMask(&measPunit);
    uint32_t ctrMask = Upci_Punit_GetUsedCtrMask(&measPunit);
    Upci_Punit_SkipCtrs(&testPunit, sigMask, ctrMask);


    // ---------------------------
    // Check time to add 3 events
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    ADD_PUNIT_EVENT(testPunit, PEVT_IU_IS1_STALL_CYC, thd, hTEvt[cTNum++]);
    ADD_PUNIT_EVENT(testPunit, PEVT_CYCLES, thd, hTEvt[cTNum++]);
    ADD_PUNIT_EVENT(testPunit, PEVT_INST_ALL, thd, hTEvt[cTNum++]);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_AddEvent (3 events)");


    #if 0
    printf("\nMeas Punit:\n");
    Upci_Punit_Dump(4, &measPunit);
    printf("\nTest Punit:\n");
    Upci_Punit_Dump(4, &testPunit);
    printf("\n");
    #endif

    // ---------------------------
    // Check time to do heavyweight start w/o reset or delay
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    Upci_Punit_Start(&testPunit, 0);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_Start w/o reset or delay");



    // ---------------------------
    // Check time to do heavyweight stop w/o delay
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    Upci_Punit_Stop(&testPunit, 0);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_Stop w/o delay");



    // ---------------------------
    // Check time to do heavyweight start w delay no reset
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    Upci_Punit_Start(&testPunit, UPCI_CTL_DELAY);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_Start w delay no reset");



    // ---------------------------
    // Check time to do heavyweight stop w delay
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    Upci_Punit_Stop(&testPunit, UPCI_CTL_DELAY);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_Stop w delay");



    // ---------------------------
    // Check time to do heavyweight reset
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    Upci_Punit_Reset_Counts(&testPunit, 0);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_Reset_Counts (3 counters)");


    // ---------------------------
    // Check time to do heavyweight start w reset no delay
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    Upci_Punit_Start(&testPunit, UPCI_CTL_RESET);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_Start w reset no delay");



    // ---------------------------
    // Check time to do LL Start
    testCtlMask = Upci_Punit_GetLLCtlMask(&testPunit, 0);

    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    Upci_Punit_LLResetStart(testCtlMask);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_LLResetStart");



    // ---------------------------
    // Check time to do LL Start
    testCtlMask = Upci_Punit_GetLLCtlMask(&testPunit, 0);

    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    Upci_Punit_LLStart(testCtlMask);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_LLStart");



    // ---------------------------
    // Check time to do LL Stop
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    Upci_Punit_LLStop(testCtlMask);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_LLStop");



    // ---------------------------
    // Check time to do LL Read
    uint64_t data = 0;
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);

    data = Upci_Punit_LLPCtr_Read(hTEvt[0]);

    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Upci_Punit_LLRead");



    // ---------------------------
    // Check overhead events do to LL Self start/stop
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_LLResetStart(measCtlMask);
    Upci_Punit_LLStop(measCtlMask);
    PRINT_PATHLEN("Low Lat start/stop overhead counts.");


    // ---------------------------
    // Check overhead events do to heavyweight Self start/stop
    Kernel_Upci_Wait4MailboxEmpty();   // reduce likelyhood of prints in progress
    Upci_Punit_Start(&measPunit, UPCI_CTL_RESET);
    Upci_Punit_Stop(&measPunit, UPCI_CTL_DELAY);
    PRINT_PATHLEN("Heavyweight start/stop overhead counts.");




    return(0);
}
