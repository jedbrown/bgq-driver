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


#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <omp.h>
#include <math.h>

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"

extern "C" void Bgpm_DebugDumpShrMem();


//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();

int numThreads;
int numProcs;

#define LOOPS 5000



unsigned evtList[] = {
    PEVT_INST_XU_MATCH,
    PEVT_INST_XU_GRP_MASK,
    PEVT_INST_QFPU_MATCH,
    PEVT_INST_QFPU_GRP_MASK,
    PEVT_INST_QFPU_FADD,
    PEVT_INST_QFPU_FDIV,
    PEVT_INST_QFPU_FSQ,
    PEVT_XU_PPC_COMMIT,
    PEVT_INST_XU_SYNC,
    PEVT_INST_XU_ST,
    PEVT_INST_XU_IMUL,
    PEVT_INST_XU_BITS,
};
int numEvts = sizeof(evtList)/sizeof(unsigned);





int Exercise()
{
    int i;
    volatile long a = 1;
    volatile long b = 2;
    volatile long c = 3;
    volatile double d = 4.0;
    volatile double e = 5.0;
    volatile double f = 6.0;
    volatile double g = 7.0;
    volatile uint64_t x = 1;
    for (i=0; i<LOOPS; ++i) {   // add 1
        a = b + c;              // add 2
        b = c * 7;              // imul 1
        c = b / 3;              // idiv 1
        d = d + 1.5;            // fadd 1   fp 1
        e = e * 0.99;           // fmul 1   fp 2
        f = d / e;              // fdiv 1   fp 3
        g = sqrt(f);            // fsqr 1   fp 4
        x <<= 1;
        x = 2;                  // st  7?
        mbar();                 // sync 1
    }
    return 0;
}




int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Sanity Check Group and Match instruction and FP counting);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();
    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);

    SHMC_Init_Barriers(numProcs, numThreads)


    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,numProcs,numFailures,   \
        evtList,numEvts)
    {
        int hEvtSet = -1;
        Bgpm_Init(BGPM_MODE_SWDISTRIB);
        hEvtSet = Bgpm_CreateEventSet();
        Bgpm_AddEventList(hEvtSet, evtList, numEvts);

        unsigned xuMatchIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_INST_XU_MATCH, 0);
        Bgpm_SetXuMatch(hEvtSet, xuMatchIdx, BGPM_INST_XFX_FORM_CODE(31,854), BGPM_INST_XFX_FORM_MASK);  // match mbar

        unsigned qfpuMatchIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_MATCH, 0);
        Bgpm_SetQfpuMatch(hEvtSet, qfpuMatchIdx, BGPM_INST_A_FORM_CODE(63,22), BGPM_INST_A_FORM_MASK, 2);  // match fsqrt
        //if (Kernel_ProcessorID() == 0) {
        //    Bgpm_PrintCurrentPunitReservations(hEvtSet);
        //    Bgpm_DumpEvtSet(hEvtSet, 0);
        //}
        Bgpm_SetQfpuFp(hEvtSet, qfpuMatchIdx, BGPM_TRUE);
        //if (Kernel_ProcessorID() == 0) {
        //    Bgpm_PrintCurrentPunitReservations(hEvtSet);
        //    Bgpm_DumpEvtSet(hEvtSet, 0);
        //}


        unsigned xuGrpIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_INST_XU_GRP_MASK, 0);
        Bgpm_SetXuGrpMask(hEvtSet, xuGrpIdx, UPC_P_XU_OGRP_SYNC | UPC_P_XU_OGRP_IMUL);

        unsigned qfpuGrpIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_GRP_MASK, 0);
        Bgpm_SetQfpuGrpMask(hEvtSet, qfpuGrpIdx, UPC_P_AXU_OGRP_FSQ | UPC_P_AXU_OGRP_FDIV );

        //Bgpm_DebugDumpShrMem();

        Bgpm_Apply(hEvtSet);

        //if (Kernel_ProcessorID() == 0) {
        //    Bgpm_PrintCurrentPunitReservations(hEvtSet);
        //    Bgpm_DumpEvtSet(hEvtSet, 0);
        //    UPC_P_Dump_State(-1);
        //}
        ShmBarrier2();

        uint16_t match, mask; ushort scale;
        Bgpm_GetXuMatch(hEvtSet, xuMatchIdx, &match, &mask);
        TEST_CHECK_EQUAL(match, BGPM_INST_XFX_FORM_CODE(31,854));
        TEST_CHECK_EQUAL(mask, BGPM_INST_XFX_FORM_MASK);

        Bgpm_GetQfpuMatch(hEvtSet, qfpuMatchIdx, &match, &mask, &scale);
        TEST_CHECK_EQUAL(match, BGPM_INST_A_FORM_CODE(63,22));
        TEST_CHECK_EQUAL(mask, BGPM_INST_A_FORM_MASK);
        TEST_CHECK_EQUAL(scale, 2);

        ushort countFp;
        Bgpm_GetQfpuFp(hEvtSet, qfpuMatchIdx, &countFp);
        TEST_CHECK_EQUAL(countFp, BGPM_TRUE);

        uint64_t grpMask;
        Bgpm_GetXuGrpMask(hEvtSet, xuGrpIdx, &grpMask);
        TEST_CHECK_EQUAL(grpMask, UPC_P_XU_OGRP_SYNC | UPC_P_XU_OGRP_IMUL);

        Bgpm_GetQfpuGrpMask(hEvtSet, qfpuGrpIdx, &grpMask);
        TEST_CHECK_EQUAL(grpMask, UPC_P_AXU_OGRP_FSQ | UPC_P_AXU_OGRP_FDIV);

        unsigned syncIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_INST_XU_SYNC, 0);
        unsigned imulIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_INST_XU_IMUL, 0);
        unsigned fsqIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_FSQ, 0);
        unsigned fdivIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_INST_QFPU_FDIV, 0);


        #pragma omp barrier

        Bgpm_Start(hEvtSet);
        int loop;
        for (loop=0; loop<LOOPS; loop++) {
            Exercise();
        }
        Bgpm_Stop(hEvtSet);

        // Print Results
        SHMC_SERIAL_Start(numProcs,numThreads);

        fprintf(stderr, _AT_ " Thread %d\n", Kernel_ProcessorID());
        PrintCountsInclOvf(hEvtSet);

        uint64_t ctrs[24];
        unsigned numCtrs = 24;
        Bgpm_ReadEventList(hEvtSet, ctrs, &numCtrs);

        TEST_CHECK_EQUAL(ctrs[xuMatchIdx], ctrs[syncIdx]);
        TEST_CHECK_EQUAL(ctrs[xuGrpIdx],   ctrs[syncIdx]+ctrs[imulIdx]);
        TEST_CHECK_EQUAL(ctrs[qfpuMatchIdx],  ctrs[fsqIdx]*4);
        TEST_CHECK_EQUAL(ctrs[qfpuGrpIdx],  ctrs[fsqIdx]+ctrs[fdivIdx]);

        SHMC_SERIAL_End();


        Bgpm_Disable();
    }


    TEST_RETURN();

}

