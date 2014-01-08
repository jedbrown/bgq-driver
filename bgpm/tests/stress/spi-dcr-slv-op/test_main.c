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
#include "spi/include/kernel/location.h"
#include "spi/include/upci/upci.h"


UPC_Barrier_t barrierLock = 0;

int main(int argc, char *argv[])
{
    fprintf(stderr, "Stress of l1p dcr slave operations to other cores\n");

    int numThreads = 2;
    int seconds = 300;
    if (argc > 1) {
       seconds = atoi(argv[1]);
    }
    fprintf(stderr, "numThreads=%d, seconds=%d\n", numThreads, seconds);



    uint64_t targCycles = (uint64_t)1600 * 1024*1024 * seconds;
    uint64_t cpuSpeed = 1600;
    fprintf(stderr, "cpuspeed=%ld, targCycles=%ld\n", cpuSpeed, targCycles);

    uint64_t startTB = GetTimeBase();

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,targCycles,startTB,barrierLock,numThreads)
    {
        uint64_t totalLoops = 0;

        while ((GetTimeBase()-startTB) < targCycles) {

             //if ((totalLoops % 16) == Kernel_ProcessorCoreID()) {
             if (Kernel_ProcessorID() == 0) {

                upc_p_mmio_t *const upc_p = (upc_p_mmio_t *)0x000003ffe0033000;  // core 1 upc_p dcr addr
                //upc_p_mmio_t *const upc_p = (upc_p_mmio_t *)0x000003fde0053000;  // core 2 upc_p dcr addr
                #if 0
                if ((totalLoops % 1000 == 0)) {
                    fprintf(stderr, "loops=%ld, upc_p@=0x%016lx\n", totalLoops, (uint64_t)(&upc_p->p_config));
                }
                #endif
                uint64_t config;
                config = upc_p->p_config;
                totalLoops++;
             }
             else {
                upc_p_mmio_t *const upc_p = (upc_p_mmio_t *)0x00003ffe8003000;  // core 1 local mmio addr
                #if 0
                if ((totalLoops % 1000 == 0)) {
                    fprintf(stderr, "loops=%ld, upc_p@=0x%016lx\n", totalLoops, (uint64_t)(&upc_p->p_config));
                }
                #endif
                uint64_t config;
                config = upc_p->p_config;
                totalLoops++;
             }

            //UPC_Barrier(&barrierLock, numThreads, 0x00FFFFFFFFFFFFFFFULL);
            //#pragma omp barrier
        }
    }


    return(0);
}


