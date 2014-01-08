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
#include "spi/include/upci/testlib/upc_p_debug.h"

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"


TEST_INIT();


// Test Data
#define N 100000
static double   a[N],
                b[N],
                c[N];



// Shared User data used to aggregate event counts from each thread.

unsigned puList[] = {      // List of performance events to collect
    PEVT_INST_QFPU_FPGRP1,
    PEVT_CYCLES,
    PEVT_INST_ALL
};
int puNumEvts = sizeof(puList) / sizeof(unsigned);


unsigned cnkList[] = {      // List of performance events to collect
        PEVT_CNKHWT_SYSCALL,
        PEVT_CNKHWT_STANDARD,
        PEVT_CNKHWT_CRITICAL,
        PEVT_CNKHWT_DECREMENTER,
        PEVT_CNKHWT_FIT,
        PEVT_CNKHWT_WATCHDOG,
        PEVT_CNKHWT_UDEC,
        PEVT_CNKHWT_PERFMON,
        PEVT_CNKHWT_UNKDEBUG,
        PEVT_CNKHWT_DEBUG,
        PEVT_CNKHWT_DSI,
        PEVT_CNKHWT_ISI,
        PEVT_CNKHWT_ALIGNMENT,
        PEVT_CNKHWT_PROGRAM,
        PEVT_CNKHWT_FPU,
        PEVT_CNKHWT_APU,
        PEVT_CNKHWT_DTLB,
        PEVT_CNKHWT_ITLB,
        PEVT_CNKHWT_VECTOR,
        PEVT_CNKHWT_UNDEF,
        PEVT_CNKHWT_PDBI,
        PEVT_CNKHWT_PDBCI,
        PEVT_CNKHWT_GDBI,
        PEVT_CNKHWT_GDBCI,
        PEVT_CNKHWT_EHVSC,
        PEVT_CNKHWT_EHVPRIV,
        PEVT_CNKHWT_LRATE,
};
int cnkNumEvts = sizeof(cnkList) / sizeof(unsigned);


int numThreads;
int numProcs;



// Target code to measure
void InitMeasureData()
{
    int j;
    for (j=0; j<N; j++) {
        a[j] = 0.8;
        b[j] = 0.5;
        c[j] = 0.1;
    }
}


// Target code to measure
void MeasureLoops(int loops)
{
    int i, j;
    for (i=0; i<loops; i++) {
        for (j=0; j<N; j++) {
            a[j] = a[j] * a[j] + 0.8;
            b[j] = b[j] * b[j] + 1.0;
            c[j] = c[j] * c[j] + 1.0;
        }
    }
}



void PrintCounts(unsigned hEvtSet)
{
    InitShmCount(0);
    ShmBarrier2();
    ShmAtomicWait(Kernel_ProcessorID() / numThreads);

    printf("Counter Results:\n");
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    fprintf(stderr, _AT_ " numEvts=%d\n", numEvts);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("    0x%016lx <= %s\n", cnt, label);
    }

    ShmAtomicIncr();
}



void Pass_AllCNK()
{

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,numProcs,numFailures, \
               puList,cnkList,puNumEvts,cnkNumEvts)
     {
        TEST_CASE_PRINT0(Collect ALL CNK Counts);

        Bgpm_Init(BGPM_MODE_SWDISTRIB);


        int hCnkEvtSet = Bgpm_CreateEventSet();

        TEST_CHECK_EQUAL(Bgpm_AddEventList(hCnkEvtSet, cnkList, cnkNumEvts), 0);

        uint64_t startSample;
        Bgpm_CNK_SampleEvent(hCnkEvtSet, 0, &startSample);

        TEST_CHECK_EQUAL(Bgpm_Start(hCnkEvtSet), 0);
        #if 0
        Bgpm_DumpEvtSet(hCnkEvtSet, 0);
        #endif

        int hPuEvtSet = Bgpm_CreateEventSet();   // get container for events.
        Bgpm_AddEventList(hPuEvtSet, puList, puNumEvts);
        Bgpm_Apply(hPuEvtSet);        // apply to hardware

        InitMeasureData();
        Bgpm_ResetStart(hPuEvtSet);   // start counting.

        MeasureLoops(1200);

        Bgpm_Stop(hPuEvtSet);   // stop counting - once per thread.

        TEST_CHECK_EQUAL(Bgpm_Stop(hCnkEvtSet), 0);

        #if 0
        Bgpm_DumpEvtSet(hCnkEvtSet, 0);
        #endif
        PrintCounts(hCnkEvtSet);



        ShmBarrier2();
        TEST_CASE_PRINT0(Try Multiple Starts/Stops, read while running);
        ShmBarrier2();
        uint64_t numSysCalls = 10;
        int i;
        TEST_CHECK_EQUAL(Bgpm_ResetStart(hCnkEvtSet), 0);   // start counting.

        // cause some syscalls
        Upci_KDebug_t kDebug;
        for (i=0; i<numSysCalls; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }

        TEST_CHECK_EQUAL(Bgpm_Start(hCnkEvtSet), BGPM_WALREADY_STARTED);

        // cause some syscalls
        for (i=0; i<numSysCalls; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }

        uint64_t ctrVal;
        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hCnkEvtSet, 0, &ctrVal), 0);
        TEST_CHECK_EQUAL(ctrVal, 2*numSysCalls+1);

        // cause some syscalls
        for (i=0; i<numSysCalls; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }

        TEST_CHECK_EQUAL(Bgpm_Stop(hCnkEvtSet), 0);
        TEST_CHECK_EQUAL(Bgpm_Stop(hCnkEvtSet), BGPM_WALREADY_STOPPED);

        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hCnkEvtSet, 0, &ctrVal), 0);
        TEST_CHECK_EQUAL(ctrVal, 3*numSysCalls+2);




        ShmBarrier2();
        TEST_CASE_PRINT0(Try Reset and ResetStart while running);
        ShmBarrier2();

        TEST_CHECK_EQUAL(Bgpm_ResetStart(hCnkEvtSet), 0);   // start counting.

        // cause some syscalls
        for (i=0; i<numSysCalls; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }

        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hCnkEvtSet, 0, &ctrVal), 0);
        TEST_CHECK_EQUAL(ctrVal, 1*numSysCalls+1);

        TEST_CHECK_EQUAL(Bgpm_Reset(hCnkEvtSet), 0);   // start counting.

        // cause some syscalls
        for (i=0; i<numSysCalls*3; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }

        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hCnkEvtSet, 0, &ctrVal), 0);
        TEST_CHECK_EQUAL(ctrVal, 3*numSysCalls+1);


        TEST_CHECK_EQUAL(Bgpm_ResetStart(hCnkEvtSet), 0);   // start counting.

        // cause some syscalls
        for (i=0; i<numSysCalls; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }

        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hCnkEvtSet, 0, &ctrVal), 0);
        TEST_CHECK_EQUAL(ctrVal, 1*numSysCalls+1);

        TEST_CHECK_EQUAL(Bgpm_Stop(hCnkEvtSet), 0);

        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hCnkEvtSet, 0, &ctrVal), 0);
        TEST_CHECK_EQUAL(ctrVal, 1*numSysCalls+2);




        ShmBarrier2();
        TEST_CASE_PRINT0(Try multiple start stop w/o reset);
        ShmBarrier2();

        TEST_CHECK_EQUAL(Bgpm_ResetStart(hCnkEvtSet), 0);   // start counting.

        // cause some syscalls
        for (i=0; i<numSysCalls; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }

        TEST_CHECK_EQUAL(Bgpm_Stop(hCnkEvtSet), 0);   // start counting.
        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hCnkEvtSet, 0, &ctrVal), 0);
        TEST_CHECK_EQUAL(ctrVal, 1*numSysCalls+1);

        // cause some syscalls
        for (i=0; i<numSysCalls*2; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }

        TEST_CHECK_EQUAL(Bgpm_Start(hCnkEvtSet), 0);   // start counting.

        // cause some syscalls
        for (i=0; i<numSysCalls; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }

        TEST_CHECK_EQUAL(Bgpm_Stop(hCnkEvtSet), 0);   // start counting.
        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hCnkEvtSet, 0, &ctrVal), 0);
        TEST_CHECK_EQUAL(ctrVal, 2*numSysCalls+2);





        ShmBarrier2();
        TEST_CASE_PRINT0(Try write operation while running);
        ShmBarrier2();

        TEST_CHECK_EQUAL(Bgpm_ResetStart(hCnkEvtSet), 0);   // start counting.

        // cause some syscalls
        for (i=0; i<numSysCalls; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }

        TEST_CHECK_EQUAL(Bgpm_WriteEvent(hCnkEvtSet, 0, 500), 0);

        // cause some syscalls
        for (i=0; i<numSysCalls; ++i) {
            Kernel_Upci_GetKDebug(&kDebug);
        }


        TEST_CHECK_EQUAL(Bgpm_Stop(hCnkEvtSet), 0);   // start counting.
        TEST_CHECK_EQUAL(Bgpm_ReadEvent(hCnkEvtSet, 0, &ctrVal), 0);
        TEST_CHECK_EQUAL(ctrVal, numSysCalls+501);




        ShmBarrier2();
        TEST_CASE_PRINT0(Check Sample Event);
        ShmBarrier2();
        TEST_CHECK_EQUAL(Bgpm_CNK_SampleEvent(hCnkEvtSet, 0, &ctrVal), 0);
        TEST_CHECK_RANGE(ctrVal-startSample, 200, 300);



        Bgpm_Disable();
    }

}



int main(int argc, char *argv[])
{
    TEST_MODULE(Examine CNK Event Counting);

    if (argc < 2) {
        printf("ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    printf("numProcesses=%d, numThreads=%d\n", numProcs, numThreads);


    InitCriticalLock();
    InitShmBarrier1(numProcs);
    ShmBarrier1();
    InitShmBarrier2(numProcs*numThreads);

    Pass_AllCNK();
    ShmBarrier1();

    TEST_RETURN();
}


