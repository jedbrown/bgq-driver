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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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
#include "mcheck.h"


//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();




int numThreads;
int numProcs;



int main(int argc, char *argv[])
{
    mtrace();

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


#if 1
    InitCriticalLock();
    InitShmBarrier1(numProcs);
    ShmBarrier1();
    InitShmBarrier2(numProcs*numThreads);
#endif

    TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB), 0);


    TEST_CHECK_EQUAL(Bgpm_Disable(), 0);

    TEST_RETURN();
}

