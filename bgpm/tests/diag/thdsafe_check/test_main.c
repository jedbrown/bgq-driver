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
extern Upci_Punit_t * DebugGetPunit(unsigned);
extern void Bgpm_DebugDumpShrMem();


//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


#define LOOPS 50000

int numThreads;
int numProcs;


__thread unsigned randSeed;
inline int RandNum(int max) {
    double randVal = rand_r(&randSeed);
    //fprintf(stderr, _AT_ " max=%d, randVal=%f, RAND_MAX=%d\n", max, randVal, RAND_MAX);
    int randNum = 1 + (int)(((double)max) * (randVal/(RAND_MAX + 1.0)));
    return randNum;
}

#define MAXHANDLES 1800


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


void AddDeleteCheck()
{

    TEST_CASE_PRINT0(START VARIATIONS: Stress Add/Delete of Event Sets);

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numFailures,              \
                AXUList, IUList, XUList, LSUList, MMUList, MMUList2, OPList, l1pBaseList)
    {
        Bgpm_ExitOnError(BGPM_FALSE);

        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);


        unsigned loop;
        for (loop=0; loop<LOOPS; ++loop) {

            int hEvtSet = Bgpm_CreateEventSet();
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, IUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pBaseList, 1), 0); // cause some event reordering
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, XUList, 1), 0);      // cause some event reordering
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 6);

            //if ((loop % 10) == 0) fprintf(stderr, _AT_ " loop=%d\n", loop);
            TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        }

        fprintf(stderr, _AT_ " Done with loops\n");

        Bgpm_Disable();
    }
}



void AddDeleteReadCheck()
{

    TEST_CASE_PRINT0(START VARIATIONS: Stress Add/Delete Start/Stop Read of Event Sets);

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numFailures,              \
                AXUList, IUList, XUList, LSUList, MMUList, MMUList2, OPList, l1pBaseList)
    {
        Bgpm_ExitOnError(BGPM_FALSE);

        TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);


        unsigned loop;
        for (loop=0; loop<LOOPS; ++loop) {

            int hEvtSet = Bgpm_CreateEventSet();
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, AXUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, IUList, 2), 0);
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, l1pBaseList, 1), 0); // cause some event reordering
            TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet, XUList, 1), 0);      // cause some event reordering
            TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 6);

            TEST_CHECK_EQUAL(Bgpm_Apply(hEvtSet), 0);
            TEST_CHECK_EQUAL(Bgpm_Start(hEvtSet), 0);

            Delay(5000);

            TEST_CHECK_EQUAL(Bgpm_Stop(hEvtSet), 0);
            uint64_t counterList[24];
            unsigned len = 24;
            TEST_CHECK_EQUAL(Bgpm_ReadEventList(hEvtSet, counterList, &len), 0);

            //if ((loop % 10) == 0) fprintf(stderr, _AT_ " loop=%d\n", loop);
            TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
        }

        fprintf(stderr, _AT_ " Done with loops\n");

        Bgpm_Disable();
    }
}



int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Stress Thread Safe Add/Delete of event sets);

    if (argc < 2) {
        DIE("Pass in number of threads per process\n");
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    if (Kernel_ProcessorID() == 0) {
        printf("numProcs=%d, numThreads=%d\n", numProcs, numThreads);
    }

    InitCriticalLock();
    InitShmBarrier1(numProcs);
    ShmBarrier1();
    InitShmBarrier2(numProcs*numThreads);


    AddDeleteCheck();
    ShmBarrier1();

    AddDeleteReadCheck();
    ShmBarrier1();


    TEST_RETURN();
}

