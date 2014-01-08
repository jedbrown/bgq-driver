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

#include "PodSharePtr.h"
#include "ShmemBlk.h"
#include "spi/include/kernel/location.h"
#include "spi/include/kernel/upc_hw.h"

int numfails = 0;
#define PRINTERR(...) { fprintf(stderr, "ERROR(" _AT_ ") " __VA_ARGS__); numfails++; }
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }


using namespace bgpm;


PodSharedPtr<ShmemBlk> pPShm;


int main(int argc, char *argv[])
{
    printf("Sanity Check shmemblk pointer\n");
    int numThreads = atoi(argv[1]);
    fprintf(stderr, "numThreads=%d\n", numThreads);

    pPShm.Init();
    pPShm = GetShmemBlk();
    // pPShm->Init();  Done by GetShmemBlk now.
    //int mainId = Kernel_ProcessorID();

    #pragma omp parallel default(none) num_threads(numThreads) \
            shared(stderr,pPShm,numfails)
    {
        unsigned t = Kernel_ProcessorID();
        fprintf(stderr, "Check Shared Mem Smart Ptr and Structure : I'm hw thread %d\n", t);

        PodSharedPtr<ShmemBlk> pTShm;
        pTShm.Init();

        for (int i=0; i<3; ++i) {

            pTShm.Assign(pPShm);

            pTShm->Dump(0);

            pTShm.Release();

            if (pTShm != NULL) {
                fprintf(stderr, "Not Null  ");
                pTShm->Dump(0);
            }
            else {
                fprintf(stderr, "pTShm is null\n");
            }

            if (i==1) {
                #pragma omp barrier
            }
        }

        fprintf(stderr, "done rc = %d", numfails);
    }
    return(numfails);
}

