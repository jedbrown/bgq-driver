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

#include "spi/include/kernel/upci.h"
#include "../../test_utils.h"

TEST_INIT();
int numThreads;
int numProcs;

#define LOOPS 50001

UPC_Atomic_Int_t  refCount = 0;

int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Check atomic reference counting routines);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();
    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);

    #pragma omp parallel default(none) num_threads(numThreads) \
            shared(stderr,refCount,numFailures,numThreads)
    {
        int i;
        for (i=0; i<LOOPS; i++) {
            UPC_Atomic_Incr(&refCount);
        }

        #pragma omp barrier
        TEST_CHECK_EQUAL(refCount, LOOPS*numThreads);
        #pragma omp barrier


        for (i=0; i<LOOPS; i++) {
            UPC_Atomic_Decr(&refCount);
        }

        #pragma omp barrier
        TEST_CHECK_EQUAL(refCount, 0);
    }

    TEST_RETURN();
}

