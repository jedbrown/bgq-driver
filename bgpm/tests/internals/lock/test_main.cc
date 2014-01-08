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
#include <omp.h>

#include "../../../../bgpm/src/Lock.h"
#include "spi/include/kernel/location.h"
#include "spi/include/kernel/upc_hw.h"
#include "spi/include/kernel/upci.h"
#include "../../test_utils.h"


TEST_INIT();


uint32_t GetLockVal (UPC_Lock_t *nonblkLock) {
    return *nonblkLock;
}

UPC_Lock_t testLock = 0;
volatile uint64_t   testCount = 0;
UPC_Lock_t nonblkLock = 0;
uint64_t   targCount = 0;

int main(int argc, char *argv[])
{
    int numThreads = atoi(argv[1]);
    fprintf(stderr, "numThreads=%d\n", numThreads);
    targCount = numThreads * 1024 * 1024;
    int loopsPerThd = targCount/numThreads;

    TEST_MODULE_PRINT0(Stress Locking Operations);

    //int mainId = Kernel_ProcessorID();

    #pragma omp parallel default(none) num_threads(numThreads) \
            shared(stderr,testLock,testCount,targCount,loopsPerThd,numThreads,numFailures,nonblkLock)
    {
        //unsigned t = Kernel_ProcessorID();
        //fprintf(stderr, "Check Lock Class : I'm hw thread %d\n", t);

        //unsigned tid = upci_gettid();

        TEST_CASE_PRINT0(Check Lock Class);
        testCount = 0;
        #pragma omp barrier

        int loop;
        for (loop=0; loop<loopsPerThd; loop++) {
            bgpm::Lock guard(&testLock);
            testCount++;
            testCount++;
        }
        #pragma omp barrier
        TEST_CHECK_EQUAL(testCount, targCount*2);
        #pragma omp barrier


        TEST_CASE_PRINT0(Check Raw UPC Lock);
        testCount = 0;
        #pragma omp barrier

        for (loop=0; loop<loopsPerThd; loop++) {
            UPC_Lock(&testLock);
            testCount++;
            testCount++;
            testCount++;
            UPC_Unlock(&testLock);
        }
        #pragma omp barrier
        TEST_CHECK_EQUAL(testCount, targCount*3);
        #pragma omp barrier


        TEST_CASE_PRINT0(Check Lock Class with cache flush in middle);

        // Create some data space to create cache misses
        int i;
        //long long j;
        // Array Sizes  cachelines    #dbls   factor to
        //              per l1 cache  /line   flush cache
        #define DSIZE   (256   *       8     *  2)           // size needed to flush cache
        volatile double target[DSIZE];
        volatile double source[DSIZE];
        for (i=0; i<DSIZE; i++) {
            source[i] = i;
            target[i] = i+1;
        }


        testCount = 0;
        targCount = numThreads * 256;
        loopsPerThd = targCount/numThreads;

        #pragma omp barrier

        for (loop=0; loop<loopsPerThd; loop++) {
            bgpm::Lock guard(&testLock);

            // do operations to flush cache
            testCount++;
            // stride by cache line size.
            for (i=DSIZE-1; i>=0; i-=8) {
                target[i] = source[i];  // here is where the misses should occur
            }
            testCount++;
        }
        #pragma omp barrier
        TEST_CHECK_EQUAL(testCount, targCount*2);
        #pragma omp barrier




        TEST_CASE_PRINT0(Check Nonblocking Lock Class with cache flush in middle);

        testCount = 0;
        targCount = numThreads * 256;
        loopsPerThd = targCount/numThreads;

        #pragma omp barrier

        for (loop=0; loop<loopsPerThd; loop++) {

            while (UPC_Lock_woBlock(&nonblkLock) < 0) {
                //fprintf(stderr, "nonblkLock owned by %d\n", GetLockVal(&nonblkLock));
                Upci_Delay(100);
                mbar();
            }

            // do operations to flush cache
            testCount++;
            // stride by cache line size.
            for (i=DSIZE-1; i>=0; i-=8) {
                target[i] = source[i];  // here is where the misses should occur
            }
            testCount++;

            UPC_Unlock(&nonblkLock);
        }
        #pragma omp barrier
        TEST_CHECK_EQUAL(testCount, targCount*2);
        #pragma omp barrier


    }
    TEST_RETURN();
}

