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
#include <pthread.h>
#include "../../test_utils.h"
#include "spi/include/kernel/location.h"


#define NUMTHREADS 64
uint64_t startTB;
uint64_t totalLoops;
pthread_barrier_t barr;
uint64_t cpuSpeed;
uint64_t targCycles;


void* RunThread(void *ptr)
{
    uint64_t curTB = startTB;
    printf(_AT_ " Begin thread %d\n", Kernel_ProcessorID());

    while ((GetTimeBase()-startTB) < targCycles) {

        // barrier causes problem - diff num of loops per thread are possible
        int rc = pthread_barrier_wait(&barr);
        if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
            printf("Could not wait on barrier\n");
            exit(-1);
        }


        //TEST_AT();
        if (Kernel_ProcessorID() == 0) {
            if (totalLoops++ % 2000 == 0) {
                uint64_t newTB = GetTimeBase();
                uint64_t deltaTB = newTB-curTB;
                curTB = newTB;
                printf("TotalLoops = %ld, elapsedTB=%14ld, deltaTB=%14ld\n", totalLoops, curTB-startTB, deltaTB);
                MemSizes_t memSizes;
                GetMemSizes(&memSizes);
                PrintMemSizes(_AT_, &memSizes);
            }
        }
    }
    printf(_AT_ " End thread %d\n", Kernel_ProcessorID());

    return NULL;
}


int main(int argc, char *argv[])
{
    fprintf(stderr, "Create hang of barrier and inability to interrupt in gdb\n");

    int seconds = 15;
    if (argc > 1) {
       seconds = atoi(argv[1]);
    }
    fprintf(stderr, "numThreads=%d, seconds=%d\n", NUMTHREADS, seconds);

    cpuSpeed = 1600; // Upci_GetNodeParms()->cpuSpeed;
    targCycles = cpuSpeed * 1024*1024*seconds;
    fprintf(stderr, "cpuspeed=%ld  targCycles=%ld\n", cpuSpeed, targCycles);

    long x;
    void *pRc;  // receive ptr to rc
    int rc;
    pthread_t tid[NUMTHREADS];
    pthread_attr_t attr;

    if (pthread_barrier_init(&barr, NULL, NUMTHREADS))     {
        printf("Could not create a barrier\n");
        return -1;
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setstacksize(&attr, 128*1024);


    int i;
    for (i=0; i<100; i++) {
        startTB = GetTimeBase();
        totalLoops = 0;

        for(x=0; x<NUMTHREADS; x++)
        {
            rc = pthread_create(&tid[x], &attr, RunThread, (void*)x);
            if (rc) { printf("pthread_create error %d\n", errno); }
        }

        for(x=0; x<NUMTHREADS; x++)
        {
            pthread_join(tid[x], &pRc);
            //printf("tid[%d] rc = %d\n", tid[x], *(int*)pRc);
        }
        printf("After OMP parallel attempt %d, totalLoops = %ld\n", i, totalLoops);

    }

    return(0);
}


