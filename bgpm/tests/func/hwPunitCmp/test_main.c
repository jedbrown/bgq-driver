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
extern Upci_Punit_t * DebugGetPunit(unsigned, unsigned);
extern Upci_Punit_t * DebugGetTargPunit(unsigned, unsigned);

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"




TEST_INIT();

int evtSets[CONFIG_MAX_HWTHREADS];
int mainEvtSets[CONFIG_MAX_HWTHREADS];
Upci_Punit_t *pPunits[CONFIG_MAX_HWTHREADS];

#define NUMTHREADS (CONFIG_MAX_HWTHREADS)
#define NUMAPPTHREADS (CONFIG_MAX_HWTHREADS-4)


UPC_Barrier_t bLock;
#define BTIMEOUT (1000*1000*1000)


int main(int argc, char *argv[])
{
    TEST_MODULE(Bgpm HW Punit Config Compare);


    volatile double d;  // keep later calcs below from being optomized out.

    //*****************************************
    TEST_CASE(Create slightly diff event sets for each thread and attach);
    Bgpm_Init(BGPM_MODE_HWDISTRIB);

    unsigned thdId;
    unsigned firstEvt = 50;  // start with arbitrary event, but ensure no core-shared events are used
    for (thdId=0; thdId<NUMTHREADS; thdId++) {
        evtSets[thdId] = Bgpm_CreateEventSet();
        Bgpm_AddEvent(evtSets[thdId], firstEvt);
        Bgpm_AddEvent(evtSets[thdId], firstEvt+1);
        firstEvt++;
    }
    for (thdId=0; thdId<NUMTHREADS; thdId++) {
        //fprintf(stderr, _AT_ " thdId=%d\n", thdId);
        //Bgpm_DumpEvtSet(evtSets[thdId], 0);
        uint64_t appThdMask     = UPC_APP_HWTID_MASK(thdId);
        uint64_t agentThdMask   = UPC_AGENT_HWTID_MASK(thdId);
        Bgpm_Attach(evtSets[thdId], appThdMask, agentThdMask);
    }

    TEST_CASE(Attempt Attach again - expect warning messages);
    for (thdId=0; thdId<NUMTHREADS; thdId++) {
        uint64_t appThdMask     = UPC_APP_HWTID_MASK(thdId);
        uint64_t agentThdMask   = UPC_AGENT_HWTID_MASK(thdId);
        TEST_CHECK_EQUAL(Bgpm_Attach(evtSets[thdId], appThdMask, agentThdMask), BGPM_WALREADY_ATTACHED);
    }

    TEST_CASE(Detach to leave back in initial state);
    for (thdId=0; thdId<NUMTHREADS; thdId++) {
        Bgpm_Detach(evtSets[thdId]);
    }




    //**************************************
    TEST_CASE(Create threads and have each configure same events (all but last 4 threads)- compare with thread 0 attached sets);

    int hEvtSet = Bgpm_CreateEventSet();
    firstEvt = 66;
    Bgpm_AddEvent(hEvtSet, firstEvt+1);
    Bgpm_AddEvent(hEvtSet, firstEvt+2);
    Bgpm_Attach(hEvtSet, ~0ULL, 0);  // Attach to all application threads

    unsigned idx;
    for (idx=0; idx<NUMAPPTHREADS; ++idx) {
        pPunits[idx] = DebugGetTargPunit(hEvtSet, idx);
    }

    #pragma omp parallel default(none) num_threads(NUMAPPTHREADS)      \
        shared(evtSets,mainEvtSets,stderr,numFailures,pPunits,firstEvt)
    {
        unsigned thdId = Kernel_ProcessorID();

        if (thdId != 0) {
            Bgpm_Init(BGPM_MODE_HWDISTRIB);  // gives thread it's own environment (thrd 0 already set)
        }

        int hThdEvtSet = Bgpm_CreateEventSet();
        Bgpm_AddEvent(hThdEvtSet, firstEvt+1);
        Bgpm_AddEvent(hThdEvtSet, firstEvt+2);

        Upci_Punit_t *pThdPunit = DebugGetPunit(hThdEvtSet, 0);
        pThdPunit->status = UPCI_UnitStatus_Applied; // pretend it's applied for comparison purposes
        if (memcmp(pPunits[thdId], pThdPunit, sizeof(Upci_Punit_t)) != 0) {
            #pragma omp critical (Miscompare)
            {
                numFailures++;
                fprintf(stderr, "Punits did not match.\n");
                fprintf(stderr, "Main Punit:\n");
                Upci_Punit_Dump(1, pPunits[thdId]);
                fprintf(stderr, "Thread(%d) Punit:\n", Kernel_ProcessorID());
                Upci_Punit_Dump(1, pThdPunit);
            }
        }

        if (thdId != 0) {
            Bgpm_Disable();
        }
    }

    // disable thread 0 last - it owns the base punits being compared above.
    Bgpm_Disable();



    //*********************************************
    TEST_CASE(Create sets for each thread, gather events to verify that kernel core2core apply is working.);
    Bgpm_Init(BGPM_MODE_HWDISTRIB);
    // Create separate event sets for reach hw thread with separate attach
    for (thdId=0; thdId<NUMAPPTHREADS; thdId++) {
        evtSets[thdId] = Bgpm_CreateEventSet();
        //Bgpm_SetContext(evtSets[thdId], BGPM_CTX_KERNEL);
        Bgpm_AllowMixedContext(evtSets[thdId]);
        Bgpm_AddEvent(evtSets[thdId], PEVT_AXU_INSTR_COMMIT);
        Bgpm_AddEvent(evtSets[thdId], PEVT_INST_QFPU_ALL);
        Bgpm_AddEvent(evtSets[thdId], PEVT_CYCLES);
        Bgpm_AddEvent(evtSets[thdId], PEVT_XU_INTS_TAKEN);
        //Bgpm_AddEvent(evtSets[thdId], PEVT_XU_THRD_RUNNING);
        Bgpm_AddEvent(evtSets[thdId], PEVT_XU_TIMEBASE_TICK);

        //Bgpm_AddEvent(evtSets[thdId], PEVT_XU_EXT_INT_TAKEN);
        //Bgpm_AddEvent(evtSets[thdId], PEVT_XU_CRIT_EXT_INT_TAKEN);
        //Bgpm_AddEvent(evtSets[thdId], PEVT_XU_PERF_MON_INT_TAKEN);
        //Bgpm_AddEvent(evtSets[thdId], PEVT_XU_DOORBELL_INT_TAKEN);
    }
    for (thdId=0; thdId<NUMAPPTHREADS; thdId++) {
        uint64_t appThdMask     = UPC_APP_HWTID_MASK(thdId);
        Bgpm_Attach(evtSets[thdId], appThdMask, 0);
    }

    // Have each thread run the same activity - but with varying number of loops
    // to be able to verify counts are from corret hw thread.
    #pragma omp parallel default(none) num_threads(NUMAPPTHREADS)      \
        shared(bLock,evtSets,stderr,numFailures,pPunits,d)
    {
        unsigned thdId = Kernel_ProcessorID();

        double a = 1.01;
        volatile double b = 10.0;
        int maxLoops = (thdId*1000)+1000;
        UPC_Barrier(&bLock, NUMAPPTHREADS, BTIMEOUT);
        if (thdId == 0) {
            Bgpm_Start(evtSets[0]);
        }
        // Use spin based barriers rather than the Futex based ones pthread based ones
        // employed by OMP.  The pthread involves syscalls and interrupts which muck
        // up the counts.
        UPC_Barrier(&bLock, NUMAPPTHREADS, BTIMEOUT);

        #if 1
        int i;
        int loops = 0;
        for (i=0; i<=maxLoops; i++) {
            b = b * a;
            loops++;
        }
        d = b;  // keep from being optimized out.
        #endif

        UPC_Barrier(&bLock, NUMAPPTHREADS, BTIMEOUT);
        if (thdId == 0) {
            Bgpm_Stop(evtSets[0]);
            //Upci_Delay(1600);
            #if 0
            //Bgpm_DumpEvtSet(evtSets[0], 0);
            //Upci_A2PC_Val_t a2Regs;
            //Kernel_Upci_A2PC_GetRegs(&a2Regs);
            //Upci_A2PC_DumpRegs(&a2Regs);
            UPC_P_Dump_State(0);
            //UPC_C_Dump_State();
            UPC_C_Dump_Counters(0,3);
            #endif
        }
        //! \todo Why is a delay needed to get counts to be the same??
        //!       Something is causing the cycle count to drop 4 counts otherwise.
        //Upci_Delay(1600);
        UPC_Barrier(&bLock, NUMAPPTHREADS, BTIMEOUT);
    }

    //UPC_C_Dump_Counters(0,3);
    //Upci_Delay(1000000);

    uint64_t targCycles = 0;
    for (thdId=0; thdId<NUMAPPTHREADS; thdId++) {
        uint64_t cnts[24];
        unsigned numEvts = 24; // max
        unsigned targValue = (thdId*1000)+1000;
        Bgpm_ReadEventList(evtSets[thdId], cnts, &numEvts);
        TEST_CHECK_EQUAL(numEvts, 5);
        if (thdId == 0) {
           targCycles = cnts[2];
           TEST_CHECK(targCycles != 0);
        }
        fprintf(stderr, "thd %02d: %s=%ld, %s=%ld, %s=%ld %s=%ld %s=%ld\n", thdId,
                Bgpm_GetEventLabel(evtSets[thdId],0), cnts[0],
                Bgpm_GetEventLabel(evtSets[thdId],1), cnts[1],
                Bgpm_GetEventLabel(evtSets[thdId],2), cnts[2],
                Bgpm_GetEventLabel(evtSets[thdId],3), cnts[3],
                Bgpm_GetEventLabel(evtSets[thdId],4), cnts[4]);
        if (thdId < 64) {
            TEST_CHECK_RANGE(cnts[0], targValue, targValue+50);
            TEST_CHECK_RANGE(cnts[1], targValue, targValue+50);
        }
        // \todo why are counts for thread 3 sometimes off by 4?
        //TEST_CHECK_RANGE(cnts[2], targCycles-4, targCycles+4);
        TEST_CHECK_EQUAL(cnts[2], targCycles);
        // If they are not equal, less try the read and test again
        if (cnts[2] != targCycles) {
            Bgpm_ReadEventList(evtSets[thdId], cnts, &numEvts);
            TEST_CHECK_EQUAL(numEvts, 5);
            fprintf(stderr, "thd %02d: %s=%ld, %s=%ld, %s=%ld %s=%ld %s=%ld\n", thdId,
                    Bgpm_GetEventLabel(evtSets[thdId],0), cnts[0],
                    Bgpm_GetEventLabel(evtSets[thdId],1), cnts[1],
                    Bgpm_GetEventLabel(evtSets[thdId],2), cnts[2],
                    Bgpm_GetEventLabel(evtSets[thdId],3), cnts[3],
                    Bgpm_GetEventLabel(evtSets[thdId],4), cnts[4]);
            TEST_CHECK_EQUAL(cnts[2], targCycles);
        }
    }
    Bgpm_Disable();



    //*********************************************
    TEST_CASE(Single EvtSet to all hwthread - compare aggregate read to each target read.);

    Bgpm_Init(BGPM_MODE_HWDISTRIB);
    // Create separate event sets for reach hw thread with separate attach
    hEvtSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hEvtSet, PEVT_AXU_INSTR_COMMIT);
    Bgpm_AddEvent(hEvtSet, PEVT_INST_QFPU_ALL);
    Bgpm_AddEvent(hEvtSet, PEVT_CYCLES);
    uint64_t appThdMask = 0;
    for (thdId=0; thdId<NUMAPPTHREADS; thdId++) {
        appThdMask |= UPC_APP_HWTID_MASK(thdId);
    }
    Bgpm_Attach(hEvtSet, appThdMask, 0);


    uint64_t threadCounts[NUMAPPTHREADS][24];  // capture counts when read by target threads.

    // Have each thread run the same activity - but with varying number of loops
    // to be able to verify counts are from corret hw thread.
    #pragma omp parallel default(none) num_threads(NUMAPPTHREADS)      \
        shared(bLock,evtSets,hEvtSet,stderr,numFailures,pPunits,d,threadCounts)
    {
        unsigned thdId = Kernel_ProcessorID();

        double a = 1.01;
        volatile double b = 100.0;
        int maxLoops = (thdId*1000)+1000;
        UPC_Barrier(&bLock, NUMAPPTHREADS, BTIMEOUT);
        if (thdId == 0) {
            Bgpm_Start(hEvtSet);
        }
        UPC_Barrier(&bLock, NUMAPPTHREADS, BTIMEOUT);

#if 1
        int i;
        int loops = 0;
        for (i=0; i<=maxLoops; i++) {
            b = b * a;
            loops++;
        }
        d = b;  // keep from being optimized out.
#endif

        UPC_Barrier(&bLock, NUMAPPTHREADS, BTIMEOUT);
        if (thdId == 0) {
            Bgpm_Stop(hEvtSet);
            #if 0
            Bgpm_DumpEvtSet(hEvtSet, 0);
            Upci_A2PC_Val_t a2Regs;
            Kernel_Upci_A2PC_GetRegs(&a2Regs);
            Upci_A2PC_DumpRegs(&a2Regs);
            UPC_P_Dump_State(0);
            UPC_C_Dump_State();
            #endif
        }

        // Capture counters values when read by the target thread
        // for later comparison for results when read by controlling thread (0)
        UPC_Barrier(&bLock, NUMAPPTHREADS, BTIMEOUT);
        if (thdId > 0) Bgpm_Init(BGPM_MODE_HWDISTRIB);
        Upci_Delay(1600);
        uint64_t appThdMask = UPC_APP_HWTID_MASK(thdId);
        TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(hEvtSet, 0, appThdMask, 0, &threadCounts[thdId][0]), 0);
        TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(hEvtSet, 1, appThdMask, 0, &threadCounts[thdId][1]), 0);
        TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(hEvtSet, 2, appThdMask, 0, &threadCounts[thdId][2]), 0);
        if (thdId > 0) Bgpm_Disable();
    }


    //Upci_Delay(1000000);

    uint64_t axuInstrTotal = 0;
    uint64_t allInstrTotal = 0;
    uint64_t cyclesTotal = 0;
    TEST_CHECK_EQUAL(Bgpm_ReadEvent(hEvtSet, 0, &axuInstrTotal), 0);
    TEST_CHECK_EQUAL(Bgpm_ReadEvent(hEvtSet, 1, &allInstrTotal), 0);
    TEST_CHECK_EQUAL(Bgpm_ReadEvent(hEvtSet, 2, &cyclesTotal), 0);
    fprintf(stderr, "Total Counts :\n%s=%ld, %s=%ld, %s=%ld\n",
            Bgpm_GetEventLabel(hEvtSet,0), axuInstrTotal,
            Bgpm_GetEventLabel(hEvtSet,1), allInstrTotal,
            Bgpm_GetEventLabel(hEvtSet,2), cyclesTotal);

    // Try to stress a repeated read.
    int loop;
    for (loop=0; loop<2; ++loop) {
        uint64_t axuInstrTotal2 = 0;
        uint64_t allInstrTotal2 = 0;
        uint64_t cyclesTotal2   = 0;
        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hEvtSet, 0, &axuInstrTotal2), 0);
        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hEvtSet, 1, &allInstrTotal2), 0);
        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hEvtSet, 2, &cyclesTotal2), 0);
        //fprintf(stderr, "Total Counts2:\n%s=%ld, %s=%ld, %s=%ld\n",
        //        Bgpm_GetEventLabel(hEvtSet,0), axuInstrTotal2,
        //        Bgpm_GetEventLabel(hEvtSet,1), allInstrTotal2,
        //        Bgpm_GetEventLabel(hEvtSet,2), cyclesTotal2);
        TEST_CHECK_EQUAL(axuInstrTotal, axuInstrTotal2);
        TEST_CHECK_EQUAL(allInstrTotal, allInstrTotal2);
        TEST_CHECK_EQUAL(cyclesTotal, cyclesTotal2);

    }



    uint64_t axuInstrCalcTotal = 0;
    uint64_t allInstrCalcTotal = 0;
    uint64_t cyclesCalcTotal = 0;
    uint64_t axuInstr = 0;
    uint64_t allInstr = 0;
    uint64_t cycles = 0;
    fprintf(stderr, "Thd counts:\n");
    for (thdId=0; thdId<NUMAPPTHREADS; thdId++) {
        uint64_t expTargValue = (thdId*1000)+1000;
        uint64_t appThdMask = UPC_APP_HWTID_MASK(thdId);
        TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(hEvtSet, 0, appThdMask, 0, &axuInstr), 0);
        TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(hEvtSet, 1, appThdMask, 0, &allInstr), 0);
        TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(hEvtSet, 2, appThdMask, 0, &cycles), 0);
        fprintf(stderr, "thd %02d: %s=%ld, %s=%ld, %s=%ld\n", thdId,
                Bgpm_GetEventLabel(hEvtSet,0), axuInstr,
                Bgpm_GetEventLabel(hEvtSet,1), allInstr,
                Bgpm_GetEventLabel(hEvtSet,2), cycles);
        TEST_CHECK(axuInstr != 0);
        TEST_CHECK(allInstr != 0);
        TEST_CHECK(cycles != 0);
        TEST_CHECK_EQUAL(axuInstr, threadCounts[thdId][0]);
        TEST_CHECK_EQUAL(allInstr, threadCounts[thdId][1]);
        TEST_CHECK_EQUAL(cycles,   threadCounts[thdId][2]);

        TEST_CHECK_RANGE(axuInstr, expTargValue, expTargValue+50);
        TEST_CHECK_RANGE(allInstr, expTargValue, expTargValue+50);

        axuInstrCalcTotal += axuInstr;
        allInstrCalcTotal += allInstr;
        cyclesCalcTotal += cycles;
    }

    TEST_CHECK_EQUAL(axuInstrTotal, axuInstrCalcTotal);
    TEST_CHECK_EQUAL(allInstrTotal, allInstrCalcTotal);
    TEST_CHECK_EQUAL(cyclesTotal,   cyclesCalcTotal);
    Bgpm_Disable();


    TEST_RETURN();
}

