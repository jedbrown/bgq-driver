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
#include <unistd.h>
#include <string.h>
#ifndef _LINUX_
#include "spi/include/kernel/location.h"
#else
#include <sys/syscall.h>
    #define gettid() syscall(207, 0) // not defined by glibc
    inline int Kernel_ProcessorID() { return gettid(); }
    inline int Kernel_ProcessCount() { return 1; }
#endif
#include <omp.h>

int numThreads;
int numProcs;


extern "C" void PrintVarState();


int main(int argc, const char *argv[], const char* env[])
{
    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }

    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();
    //numThreads = atoi(getenv("OMP_NUM_THREADS"));
    fprintf(stderr, "%02d: numProcesses=%d, numThreads=%d\n", Kernel_ProcessorID(), numProcs, numThreads);

#if 0
    int en;
    fprintf(stderr, "Env:\n");
    for (en=0; env[en] != NULL; en++) {
        fprintf(stderr, "   %s\n", env[en]);
    }
#endif


    #pragma omp parallel default(none) num_threads(numThreads) \
            shared(stderr,numThreads,numProcs)
    {
        PrintVarState();
    }
    return 0;
}

