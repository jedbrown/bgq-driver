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

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

extern void Bgpm_DumpPunitHandles(Bgpm_Punit_Handles_t *pH, unsigned indent);



TEST_INIT();


// Test Data
#define N 10000


void Print_Punit_Counts(unsigned hEvtSet, Bgpm_Punit_Handles_t *phCtrs)
{
    printf("Fast Counter Results:\n");
    int i;
    int numEvts = phCtrs->numCtrs;
    for (i=0; i<numEvts; i++) {
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("    0x%016lx <= %s\n",
                Bgpm_Punit_Read(phCtrs->hCtr[i]),
                label);
    }
}



int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Punit Fast Access Methods - and that multiple counters get same non XU event counts);
    // Adding check that A2 CESR reg set correctly for non-XU events.
    // Assignment of more than two events of a A2 non-XU type will cause events to be assigned
    // to the PC A2 signal bus and the CESR reg to be set to direct these signals to
    // counters 16-24.  By comparing values from multiple counters collecting from the same
    // event type, we'll verify that it is set up correctly.

    if (argc < 2) {
        printf("ERROR: Pass in number of threads\n");
        exit(-1);
    }
    int numThreads = atoi(argv[1]);
    int numProcs = Kernel_ProcessCount();

    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,numFailures)
    {
        volatile double   a[N], b[N], c[N];
        int j;
        Bgpm_Punit_Handles_t hCtrs;

        Bgpm_Init(BGPM_MODE_SWDISTRIB);
        int hEvtSet = Bgpm_CreateEventSet();   // get container for events.

        Bgpm_AddEvent(hEvtSet, PEVT_CYCLES);
        Bgpm_AddEvent(hEvtSet, PEVT_INST_ALL);
        Bgpm_AddEvent(hEvtSet, PEVT_IU_IL1_MISS_CYC);
        Bgpm_AddEvent(hEvtSet, PEVT_AXU_INSTR_COMMIT);
        Bgpm_AddEvent(hEvtSet, PEVT_LSU_COMMIT_CACHEABLE_LDS);
        Bgpm_AddEvent(hEvtSet, PEVT_IU_IL1_MISS_CYC);
        Bgpm_AddEvent(hEvtSet, PEVT_AXU_INSTR_COMMIT);
        Bgpm_AddEvent(hEvtSet, PEVT_LSU_COMMIT_CACHEABLE_LDS);
        Bgpm_AddEvent(hEvtSet, PEVT_IU_IL1_MISS_CYC);
        Bgpm_AddEvent(hEvtSet, PEVT_AXU_INSTR_COMMIT);
        Bgpm_AddEvent(hEvtSet, PEVT_LSU_COMMIT_CACHEABLE_LDS);
        Bgpm_AddEvent(hEvtSet, PEVT_IU_IL1_MISS_CYC);
        Bgpm_AddEvent(hEvtSet, PEVT_AXU_INSTR_COMMIT);
        Bgpm_AddEvent(hEvtSet, PEVT_LSU_COMMIT_CACHEABLE_LDS);
        //Bgpm_AddEvent(hEvtSet, PEVT_LSU_COMMIT_CACHEABLE_LDS);

        Bgpm_Apply(hEvtSet);
        Bgpm_Punit_GetHandles(hEvtSet, &hCtrs);


        TEST_CHECK(hCtrs.hCtr[2] != hCtrs.hCtr[5]);
        TEST_CHECK(hCtrs.hCtr[2] != hCtrs.hCtr[8]);
        TEST_CHECK(hCtrs.hCtr[2] != hCtrs.hCtr[11]);
        TEST_CHECK(hCtrs.hCtr[3] != hCtrs.hCtr[6]);
        TEST_CHECK(hCtrs.hCtr[3] != hCtrs.hCtr[9]);
        TEST_CHECK(hCtrs.hCtr[3] != hCtrs.hCtr[12]);
        TEST_CHECK(hCtrs.hCtr[4] != hCtrs.hCtr[7]);
        TEST_CHECK(hCtrs.hCtr[4] != hCtrs.hCtr[10]);
        TEST_CHECK(hCtrs.hCtr[4] != hCtrs.hCtr[13]);

        //Bgpm_DumpEvtSet(hEvtSet, 0);
        //Bgpm_DumpPunitHandles(&hCtrs, 0);


        Bgpm_Punit_Start(hCtrs.allCtrMask);

        // Target code to measure
        // Loops are distributed across all parallel threads.
        for (j=0; j<N; j++) {
            a[j] = 1.0;
            b[j] = 2.0;
            c[j] = 0.0;
            c[j] = b[j] * a[j];
        }

        Bgpm_Punit_Stop(hCtrs.allCtrMask);


        #pragma omp critical (PrtEvts)
        {
            TEST_CHECK(Bgpm_Punit_Read(hCtrs.hCtr[2]) > 0);
            TEST_CHECK(Bgpm_Punit_Read(hCtrs.hCtr[3]) > 0);
            TEST_CHECK(Bgpm_Punit_Read(hCtrs.hCtr[4]) > 0);
            TEST_CHECK_EQUAL(Bgpm_Punit_Read(hCtrs.hCtr[2]), Bgpm_Punit_Read(hCtrs.hCtr[5]));
            TEST_CHECK_EQUAL(Bgpm_Punit_Read(hCtrs.hCtr[2]), Bgpm_Punit_Read(hCtrs.hCtr[8]));
            TEST_CHECK_EQUAL(Bgpm_Punit_Read(hCtrs.hCtr[5]), Bgpm_Punit_Read(hCtrs.hCtr[11]));
            TEST_CHECK_EQUAL(Bgpm_Punit_Read(hCtrs.hCtr[3]), Bgpm_Punit_Read(hCtrs.hCtr[6]));
            TEST_CHECK_EQUAL(Bgpm_Punit_Read(hCtrs.hCtr[3]), Bgpm_Punit_Read(hCtrs.hCtr[9]));
            TEST_CHECK_EQUAL(Bgpm_Punit_Read(hCtrs.hCtr[3]), Bgpm_Punit_Read(hCtrs.hCtr[12]));
            TEST_CHECK_EQUAL(Bgpm_Punit_Read(hCtrs.hCtr[4]), Bgpm_Punit_Read(hCtrs.hCtr[7]));
            TEST_CHECK_EQUAL(Bgpm_Punit_Read(hCtrs.hCtr[4]), Bgpm_Punit_Read(hCtrs.hCtr[10]));
            TEST_CHECK_EQUAL(Bgpm_Punit_Read(hCtrs.hCtr[4]), Bgpm_Punit_Read(hCtrs.hCtr[13]));

            Print_Punit_Counts(hEvtSet, &hCtrs);
        }
    }

    TEST_RETURN();
}


