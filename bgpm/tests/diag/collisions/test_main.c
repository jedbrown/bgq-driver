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
#include <omp.h>

#include "cnk/include/Config.h"
#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"

#ifdef INCL_BTMALLOC
#include "BackTraceMalloc.h"
#define  BTMCMD(_cmd_) _cmd_
#define  BTMSYNCCMD(_cmd_) ShmBarrier2(); _cmd_; ShmBarrier2()
#else
#define  BTMCMD(_cmd_)
#define  BTMSYNCCMD(_cmd_)
#endif

extern Upci_Punit_t * DebugGetPunit(unsigned);
extern void Bgpm_DebugDumpShrMem();

//#define DISABLE_FEEDBACK

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();



int numThreads;
int numProcs;


void OMP_Check()
{
    TEST_CASE_PRINT0(Just run OMP parallel to help see what leaks might be due to OMP);
    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numFailures)
    {

        ShmBarrier2();

    }

#if 0
    // just checking malloc trace stuff
    char *dummy=malloc(1024);
    char *dummy2=malloc(1024);
    dummy2 = realloc(dummy2, 512);
    dummy = realloc(dummy,8096);
    dummy = realloc(dummy,2048);
#endif
}



void BGPM_Mode_Check()
{
    TEST_CASE_PRINT0(Catch Bgpm Mode collision);
    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numFailures)
    {
        Bgpm_ExitOnError(BGPM_FALSE);

        if (Kernel_ProcessorID() == 0) {
            TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);
        }
        //TEST_AT();
        ShmBarrier2();
        //TEST_AT();
        if ((Kernel_ProcessorID() != 0) && ((Kernel_ProcessorID() % 7) == 0)) {
            TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_LLDISTRIB), BGPM_ECONF_MODE);
        }
        ShmBarrier2();

        if (Kernel_ProcessorID() == 0) {
            TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
        }
        else {
            TEST_CHECK_EQUAL(Bgpm_Disable(), BGPM_ENOT_INIT);
        }
    }
}


unsigned l1pBaseList[] = {
    PEVT_L1P_BAS_STRM_LINE_ESTB,
    PEVT_L1P_BAS_HIT,
    PEVT_CYCLES,
    PEVT_INST_ALL
};
int numL1pBaseEvts = sizeof(l1pBaseList) / sizeof(unsigned);

unsigned l1pStreamList[] = {
    PEVT_L1P_STRM_LINE_ESTB,
    PEVT_L1P_STRM_HIT_FWD,
    PEVT_CYCLES,
    PEVT_INST_ALL
};
int numL1pStreamEvts = sizeof(l1pStreamList) / sizeof(unsigned);

unsigned l1pSwitchList[] = {   // don't overlap with l1p base events for test (overlap == need some counters)
    PEVT_L1P_SW_MAS_SW_REQ_VAL,
    PEVT_L1P_SW_MAS_SW_REQ_GATE,
    PEVT_CYCLES,
    PEVT_INST_ALL
};
int numL1pSwitchEvts = sizeof(l1pSwitchList) / sizeof(unsigned);

unsigned l1pSwitchList2[] = {   // don't overlap with l1p switchlist events.
    PEVT_L1P_SW_MAS_SW_DATA_GATE,
    PEVT_L1P_SW_SR_MAS_RD_VAL_2,
    PEVT_CYCLES,
    PEVT_INST_ALL
};
int numL1pSwitchEvts2 = sizeof(l1pSwitchList2) / sizeof(unsigned);

unsigned l1pListList[] = {
    PEVT_L1P_LIST_SKIP_1,
    PEVT_L1P_LIST_SKIP_2,
    PEVT_CYCLES,
    PEVT_INST_ALL
};
int numL1pListEvts = sizeof(l1pListList) / sizeof(unsigned);


void L1P_Mode_Check()
{

    TEST_CASE_PRINT0(START VARIATIONS: Catch L1p Mode collisions);

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numFailures,              \
                l1pBaseList,   numL1pBaseEvts, \
                l1pSwitchList, numL1pSwitchEvts, \
                l1pSwitchList2, numL1pSwitchEvts2, \
                l1pStreamList, numL1pStreamEvts, \
                l1pListList,   numL1pListEvts)
    {
        Bgpm_ExitOnError(BGPM_FALSE);

        BTMSYNCCMD(BTMark("BeforeL1p_Init"));
        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);
        BTMSYNCCMD(BTMark("L1p_Init"));


        //#define DEBUG_L1P_DUMP_SHR_MEM
        #ifdef DEBUG_L1P_DUMP_SHR_MEM
        if (Kernel_ProcessorID() == 0) {
            TEST_AT();
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif

        // We'll select from 2 cores for each test variation.

        ShmBarrier2();
        TEST_CASE_PRINT0(Check l1p Mode on same core ok);
        ShmBarrier2();

        // Check that l1p mode event lists from threads on same core are ok (using switch events)
        int hEvtSet = Bgpm_CreateEventSet();
        BTMSYNCCMD(BTMark("L1p_Create"));


        //#define DEBUG_SERIALIZE1
        #ifdef DEBUG_SERIALIZE1
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());
        #endif

        // Test on Core 0 & 8
        if ((Kernel_ProcessorCoreID() % 8) == 0) {
            if (Kernel_ProcessorThreadID() == 1) {
                TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pSwitchList, numL1pSwitchEvts), 0);
            }
            else {
                TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pSwitchList2, numL1pSwitchEvts2), 0);
            }
        }

        #ifdef DEBUG_SERIALIZE1
        ShmAtomicIncr();
        #endif
        ShmBarrier2();

        BTMSYNCCMD(BTMark("L1p_Add1"));



        #ifdef DEBUG_L1P_DUMP_SHR_MEM
        if (Kernel_ProcessorID() == 0) {
            TEST_AT();
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif


        ShmBarrier2();
        TEST_CASE_PRINT0(Check that conflicting l1p Modes on same core fail);
        ShmBarrier2();

        // Check that conflicting modes (set later) on same core fail
        // Test on Core 1 & 9
        if (((Kernel_ProcessorCoreID() % 8) == 1) && (Kernel_ProcessorThreadID() == 1)) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pSwitchList, numL1pSwitchEvts), 0);
        }
        ShmBarrier2();

        BTMSYNCCMD(BTMark("L1p_Add2"));

        #ifdef DEBUG_L1P_DUMP_SHR_MEM
        if (Kernel_ProcessorID() == 0) {
            TEST_AT();
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif

        //#define DEBUG_SERIALIZE2
        #ifdef DEBUG_SERIALIZE2
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());
        #endif

        // try to add base events - which should be ok - make sure it doesn't end up clearing
        // the current l1p mode as well (that is - later l1p lists should still fail
        // with bad mode.
        //  Still on core 1 & 9
        if ((Kernel_ProcessorCoreID() % 8) == 1) {
            // check adding base events is fine
            if (Kernel_ProcessorThreadID() == 3) {
                TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pBaseList, numL1pBaseEvts), 0);
            }
        }
        BTMSYNCCMD(BTMark("L1p_Add3"));

        #ifdef DEBUG_L1P_DUMP_SHR_MEM
        if (Kernel_ProcessorID() == 0) {
            TEST_AT();
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif

        ShmBarrier2();

        // Still just check core 1 & 9
        if ((Kernel_ProcessorCoreID() % 8) == 1) {
            // check that Stream evts confliict
            if (Kernel_ProcessorThreadID() == 0) {
                TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pStreamList, numL1pStreamEvts), BGPM_EL1PMODERESERVED);
            }
            // check that list evts conflict
            else if (Kernel_ProcessorThreadID() == 2) {
                TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pListList, numL1pListEvts), BGPM_EL1PMODERESERVED);
            }
        }
        #ifdef DEBUG_SERIALIZE2
        ShmAtomicIncr();
        #endif
        ShmBarrier2();
        BTMSYNCCMD(BTMark("L1p_Add4"));

        #ifdef DEBUG_L1P_DUMP_SHR_MEM
        if (Kernel_ProcessorID() == 0) {
            TEST_AT();
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif


        ShmBarrier2();
        TEST_CASE_PRINT0(Check that we can successfully upgrade l1p mode when active base l1p events);
        ShmBarrier2();

        // Check that can upgrade from l1p base events
        // Try on cores 2 & 10
        if (((Kernel_ProcessorCoreID() % 8) == 2) && (Kernel_ProcessorThreadID() == 2)) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pBaseList, numL1pBaseEvts), 0);
        }
        ShmBarrier2();
        if ((Kernel_ProcessorCoreID() % 8) == 2) {
            // check conflicting Stream evts
            if (Kernel_ProcessorThreadID() != 2) {
                if (Kernel_ProcessorCoreID() == 2) {
                    TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pStreamList, numL1pStreamEvts), 0);
                }
                // check conflicting list evts
                else if (Kernel_ProcessorCoreID() == 10) {
                    TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pListList, numL1pListEvts), 0);
                }
            }
        }

        #ifdef DEBUG_L1P_DUMP_SHR_MEM
        if (Kernel_ProcessorID() == 0) {
            TEST_AT();
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif


        BTMSYNCCMD(BTMark("L1p_Add5"));


        ShmBarrier2();
        TEST_CASE_PRINT0(Check that we can successfully add base l1p events when l1p mode has been set);
        ShmBarrier2();

        // use core 3 & 11
        if (((Kernel_ProcessorCoreID() % 8) == 3)) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pSwitchList, numL1pSwitchEvts), 0);
        }

        BTMSYNCCMD(BTMark("L1p_Add6"));

        #ifdef DEBUG_L1P_DUMP_SHR_MEM
        TEST_AT();
        ShmBarrier2();
        #endif
        if ((Kernel_ProcessorCoreID() % 8) == 3) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pBaseList, 2), 0);
        }
        ShmBarrier2();

        #ifdef DEBUG_L1P_DUMP_SHR_MEM
        if (Kernel_ProcessorID() == 0) {
            TEST_AT();
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif

        #if 0
        if (Kernel_ProcessorID() == 0) {
            TEST_AT();
            Bgpm_DumpEvtSetList(0);
        }
        ShmBarrier2();
        #endif



        ShmBarrier2();
        TEST_CASE_PRINT0(See that DeleteEvtSet has cleared current l1p mode);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        BTMSYNCCMD(BTMark("L1p_Delete1"));
        ShmBarrier2();



        hEvtSet = Bgpm_CreateEventSet();
        BTMSYNCCMD(BTMark("L1p_Create2"));
        ShmBarrier2();



        #ifdef DEBUG_L1P_DUMP_SHR_MEM
        if (Kernel_ProcessorID() == 0) {
            TEST_AT();
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif
        if (Kernel_ProcessorThreadID() == 1) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pSwitchList2, numL1pSwitchEvts2), 0);
        }
        BTMSYNCCMD(BTMark("L1p_Add7"));
        ShmBarrier2();




        ShmBarrier2();
        TEST_CASE_PRINT0(Check for failure when l1p conflict on same thread);
        ShmBarrier2();
        if (Kernel_ProcessorThreadID() == 1) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pListList, numL1pListEvts), BGPM_EL1PMODERESERVED);
        }
        BTMSYNCCMD(BTMark("L1p_Add8"));
        ShmBarrier2();




        TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
        BTMSYNCCMD(BTMark("L1p_Disable"));


    }
}



unsigned AXUList[] = {
    PEVT_AXU_INSTR_COMMIT+0,
    PEVT_AXU_INSTR_COMMIT+1,
    PEVT_AXU_INSTR_COMMIT+2,
    PEVT_AXU_INSTR_COMMIT+3,
    PEVT_AXU_INSTR_COMMIT+4,
    PEVT_AXU_INSTR_COMMIT+5
};
unsigned IUList[] = {
    PEVT_IU_IL1_MISS+0,
    PEVT_IU_IL1_MISS+1,
    PEVT_IU_IL1_MISS+2,
    PEVT_IU_IL1_MISS+3,
    PEVT_IU_IL1_MISS+4,
    PEVT_IU_IL1_MISS+5
};
unsigned XUList[] = {
    PEVT_XU_THRD_RUNNING+0,
    PEVT_XU_THRD_RUNNING+1,
    PEVT_XU_THRD_RUNNING+2,
    PEVT_XU_THRD_RUNNING+3,
    PEVT_XU_THRD_RUNNING+4,
    PEVT_XU_THRD_RUNNING+5
};
unsigned LSUList[] = {
    PEVT_LSU_COMMIT_STS+0,
    PEVT_LSU_COMMIT_STS+1,
    PEVT_LSU_COMMIT_STS+2,
    PEVT_LSU_COMMIT_STS+3,
    PEVT_LSU_COMMIT_STS+4,
    PEVT_LSU_COMMIT_STS+5
};
unsigned MMUList[] = {
    PEVT_MMU_TLB_HIT_DIRECT_IERAT+0,
    PEVT_MMU_TLB_HIT_DIRECT_IERAT+1,
    PEVT_MMU_TLB_HIT_DIRECT_IERAT+2,
    PEVT_MMU_TLB_HIT_DIRECT_IERAT+3,
    PEVT_MMU_TLB_HIT_DIRECT_IERAT+4,
    PEVT_MMU_TLB_HIT_DIRECT_IERAT+5
};
unsigned MMUList2[] = {
    PEVT_MMU_IERAT_MISS_TOT+0,
    PEVT_MMU_IERAT_MISS_TOT+1,
    PEVT_MMU_IERAT_MISS_TOT+2,
    PEVT_MMU_IERAT_MISS_TOT+3,
    PEVT_MMU_IERAT_MISS_TOT+4,
    PEVT_MMU_IERAT_MISS_TOT+5
};
unsigned OPList[] = {
    PEVT_INST_XU_FLD+0,
    PEVT_INST_XU_FLD+1,
    PEVT_INST_XU_FLD+2,
    PEVT_INST_XU_FLD+3,
    PEVT_INST_XU_FLD+4,
    PEVT_INST_XU_FLD+5,
    PEVT_INST_XU_FLD+6,
    PEVT_INST_XU_FLD+7,
    PEVT_INST_XU_FLD+8,
    PEVT_INST_XU_FLD+9,
    PEVT_INST_XU_FLD+10,
    PEVT_INST_XU_FLD+11,
    PEVT_INST_XU_FLD+12,
    PEVT_INST_XU_FLD+13,
    PEVT_INST_XU_FLD+14,
    PEVT_INST_XU_FLD+15
};


void A2_Units_Check()
{

    TEST_CASE_PRINT0(START VARIATIONS: Catch A2 Event Unit Capacity collisions);

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numFailures,              \
                AXUList, IUList, XUList, LSUList, MMUList, MMUList2, OPList)
    {
        Bgpm_ExitOnError(BGPM_FALSE);

        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);

        #if 0
        if (Kernel_ProcessorID() == 0) {
            Bgpm_DebugDumpShrMem();
        }
        #endif

        ShmBarrier2();


        TEST_CASE_PRINT0(Check Good Reservation of event lists);

        int hEvtSet = Bgpm_CreateEventSet();
        if (Kernel_ProcessorThreadID() == 0) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, IUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, LSUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 6);
        }
        else {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, XUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, MMUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 6);
        }
        ShmBarrier2();

        #if 0
        if (Kernel_ProcessorID() == 0) {
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif

        #if 0
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());

        Bgpm_DumpEvtSet(hEvtSet, 0);

        ShmAtomicIncr();
        ShmBarrier2();
        #endif

        #if 0
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());
        Bgpm_DebugDumpShrMem();
        TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        Bgpm_DebugDumpShrMem();
        ShmAtomicIncr();
        #else
        TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        #endif

        ShmBarrier2();




        TEST_CASE_PRINT0(Check that more than 2 events for 2 threads is ok, but others fail);
        ShmBarrier2();

        //#define SERIALIZE_A
        //#define SERIALIZE_B
        #ifdef SERIALIZE_A
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());
        //Bgpm_DebugDumpShrMem();
        #endif

        hEvtSet = Bgpm_CreateEventSet();

        if (Kernel_ProcessorThreadID() == 1) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 4), 0);
            //Bgpm_DumpEvtSet(hEvtSet, 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, XUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, MMUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 12);
        }
        else if (Kernel_ProcessorThreadID() == 2) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, LSUList, 4), 0);
            //Bgpm_DumpEvtSet(hEvtSet, 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, IUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, XUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 12);
        }

        #ifdef SERIALIZE_A
        //Bgpm_DebugDumpShrMem();
        ShmAtomicIncr();
        #endif
        ShmBarrier2();

        #ifdef SERIALIZE_B
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());
        //Bgpm_DebugDumpShrMem();
        #endif

        if ((Kernel_ProcessorThreadID() == 0) || (Kernel_ProcessorThreadID() == 3)) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 1), BGPM_ETHD_CTRS_RSV);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 0);
        }
        else {
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 12);
        }

        #ifdef SERIALIZE_B
        //Bgpm_DebugDumpShrMem();
        ShmAtomicIncr();
        #endif

        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
        ShmBarrier2();




        TEST_CASE_PRINT0(Check that can add opcode events into other thread counters - but other threads fail);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);
        hEvtSet = Bgpm_CreateEventSet();

        //#define SERIALIZE_C
        #ifdef SERIALIZE_C
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());
        //Bgpm_DebugDumpShrMem();
        #endif

        if (Kernel_ProcessorThreadID() == 3) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, XUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, MMUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, OPList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 16);
        }
        #ifdef SERIALIZE_C
        ShmAtomicIncr();
        #endif

        ShmBarrier2();
        //if (Kernel_ProcessorID()==0) { Bgpm_DebugDumpShrMem(); }

        #ifdef SERIALIZE_C
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());
        //Bgpm_DebugDumpShrMem();
        #endif

        if (Kernel_ProcessorThreadID() != 3) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, OPList, 1), BGPM_ETHD_CTRS_RSV);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 0);
        }
        else {
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 16);
        }

        #ifdef SERIALIZE_C
        //Bgpm_DebugDumpShrMem();
        ShmAtomicIncr();
        #endif

        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
        ShmBarrier2();




        TEST_CASE_PRINT0(Check fail of overcommitted A2 Unit);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);
        hEvtSet = Bgpm_CreateEventSet();

        if (Kernel_ProcessorThreadID() == 3) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, XUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, MMUList, 6), BGPM_ECTRRESERVED);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 8);
        }
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
        ShmBarrier2();



        TEST_CASE_PRINT0(Check success/fail of core-shared events for multiple threads per core);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);
        hEvtSet = Bgpm_CreateEventSet();

        if (Kernel_ProcessorThreadID() == 3) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, MMUList2, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 2);
        }
        ShmBarrier2();

        #if 0
        if (Kernel_ProcessorID() == 0) {
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif

        //#define SERIALIZE_D
        #ifdef SERIALIZE_D
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());
        #endif

        if (Kernel_ProcessorThreadID() == 1) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 2), BGPM_ETHD_CTRS_RSV);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 0);
        }
        if (Kernel_ProcessorThreadID() == 0) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, MMUList2, 2), BGPM_ETHD_CTRS_RSV);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 0);
        }
        if (Kernel_ProcessorThreadID() == 2) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, MMUList2, 2), BGPM_ETHD_CTRS_RSV);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 0);
        }

        #ifdef SERIALIZE_D
        ShmAtomicIncr();
        #endif
        ShmBarrier2();

        TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
        ShmBarrier2();




        TEST_CASE_PRINT0(Check fail of core-shared events if other thread already has counters);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);
        hEvtSet = Bgpm_CreateEventSet();

        if (Kernel_ProcessorThreadID() == 2) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 2);
        }
        ShmBarrier2();

        #if 0
        if (Kernel_ProcessorID() == 0) {
            fprintf(stderr, _AT_ "\n"); Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif

        #ifdef SERIALIZE_D
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());
        #endif

        if (Kernel_ProcessorThreadID() == 0) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, MMUList2, 2), BGPM_ECSEVT_REQALLTHDS);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 0);
        }

        #ifdef SERIALIZE_D
        ShmAtomicIncr();
        #endif
        ShmBarrier2();

        //Bgpm_EnableTracePrint();

        #if 0
        if (Kernel_ProcessorID() == 0) {
            fprintf(stderr, _AT_ "\n"); Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        if (Kernel_ProcessorID() == 3) {
            fprintf(stderr, _AT_ "\n"); Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
       #endif


        #ifdef SERIALIZE_D
        InitShmCount(0);
        ShmBarrier2();
        ShmAtomicWait(Kernel_ProcessorID());
        #endif
        if (Kernel_ProcessorThreadID() == 3) {
            #if 0
            fprintf(stderr, _AT_ "\n"); Bgpm_DebugDumpShrMem();
            #endif
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, MMUList2, 2), BGPM_ECSEVT_REQALLTHDS);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 0);
        }
        #ifdef SERIALIZE_D
        ShmAtomicIncr();
        #endif
        ShmBarrier2();

        TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        //Bgpm_DisableTracePrint();
        TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
        ShmBarrier2();




        TEST_CASE_PRINT0(Check that items reordered to make all fit);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);
        hEvtSet = Bgpm_CreateEventSet();

        if (Kernel_ProcessorThreadID() == 3) {
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, OPList, 12), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, XUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, MMUList, 4), 0);
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 24);
        }
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
        ShmBarrier2();
        BTMSYNCCMD(BTMark("A2Check_Disable"));


    }
}


extern __thread short  addEventErrFeedback;

void Feedback_Check()
{

    TEST_CASE_PRINT0(START VARIATIONS: Check that Feedback works or not - have to visually check);

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numFailures,              \
                AXUList, IUList, XUList, LSUList, MMUList, OPList)
    {
        Bgpm_ExitOnError(BGPM_FALSE);

        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);


        ShmBarrier2();
        TEST_CASE_PRINT0(Check that only the process leader spits out feedback - all threads should see a problem);
#ifndef DISABLE_FEEDBACK
        //addEventErrFeedback = 2;
#endif
        ShmBarrier2();

        int hEvtSet = Bgpm_CreateEventSet();
        TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 2), 0);
        TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, IUList, 2), 0);
        TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, LSUList, 2), 0);
        TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 6);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, XUList, 2), BGPM_ECTRRESERVED);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
        ShmBarrier2();




        ShmBarrier2();
        TEST_CASE_PRINT0(Check that only only the 1st time feedback is printed - all threads should see a problem);
        Bgpm_ExitOnError(BGPM_FALSE);
        ShmBarrier2();

        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);

        #if 0
        if (Kernel_ProcessorID() == 0) {
            TEST_AT();
            Bgpm_DebugDumpShrMem();
        }
        ShmBarrier2();
        #endif

#ifndef DISABLE_FEEDBACK
        //addEventErrFeedback = 1;
#endif
        ShmBarrier2();

        hEvtSet = Bgpm_CreateEventSet();
        TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 2), 0);
        TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, IUList, 2), 0);
        TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, LSUList, 2), 0);
        TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 6);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, XUList, 2), BGPM_ECTRRESERVED);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
        ShmBarrier2();

        BTMSYNCCMD(BTMark("Feedback_Disable"));


    }
}




int main(int argc, char *argv[])
{

    BTMCMD(BTMalloc());

    TEST_MODULE_PRINT0(Check Bgpm Config Collisions between threads);

    if (argc < 2) {
        DIE("Pass in number of threads per process\n");
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    if (Kernel_ProcessorID() == 0) {
        printf("numProcs=%d, numThreads=%d\n", numProcs, numThreads);
        printf("WARNING: Expect a lot of error messages - that's normal for this testcase\n");
    }
    BTMCMD(BTMark("WayBefore"));

    InitCriticalLock();
    InitShmBarrier1(numProcs);
    ShmBarrier1();
    InitShmBarrier2(numProcs*numThreads);
    BTMCMD(BTMark("Before"));

    OMP_Check();
    ShmBarrier1();
    BTMCMD(BTMark("OMPCheck"));

    BGPM_Mode_Check();
    ShmBarrier1();
    BTMCMD(BTMark("BgpmModeCheck"));

    L1P_Mode_Check();
    ShmBarrier1();
    BTMCMD(BTMark("L1pModeCheck"));

    A2_Units_Check();
    ShmBarrier1();
    BTMCMD(BTMark("UnitsCheck"));

#ifndef DISABLE_FEEDBACK
    Feedback_Check();
    ShmBarrier1();
    BTMCMD(BTMark("FeedbackCheck"));
#endif

    TEST_RETURN();
}

