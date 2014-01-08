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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <omp.h>

#include "Lock.h"
#include "spi/include/kernel/location.h"
#include "spi/include/kernel/upc_hw.h"
#include "spi/include/kernel/upci.h"


int numfails = 0;
#define PRINTERR(...) { fprintf(stderr, "ERROR(" _AT_ ") " __VA_ARGS__); numfails++; }
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }

#include "spi/include/kernel/location.h"

using namespace bgpm;


#define ASIZE 10
typedef struct
{
    UPC_Lock_t lock;
    int initThread;
    uint64_t data[ASIZE];
    UPC_Lock_t lock1;
    UPC_Lock_t lock2;

} Shmspace_t;




UPC_Lock_t testLock = 0;

#define SHMPATH "bgpm-shmtest"

int shmid;

Shmspace_t *OpenShMem()
{
    bool shmAlreadyExists = false;
    if ( ((shmid = shm_open(SHMPATH, O_RDWR | O_CREAT | O_EXCL, 0600)) < 0) && (errno == EEXIST)) {
        shmAlreadyExists = true;
        shmid = shm_open(SHMPATH, O_RDWR, 0600);
    }
    if (shmid < 0) {
        DIE("shm_open errno=%d; %s\n", errno, strerror(errno));
    }

    fprintf(stderr, _AT_ "  shmid=%d, exists=%d\n", shmid, shmAlreadyExists);

    if (ftruncate( shmid, sizeof(Shmspace_t)) < 0) {
        DIE("ftruncate errno=%d; %s\n", errno, strerror(errno));
    }
    Shmspace_t *pShm = (Shmspace_t*)mmap(NULL, sizeof(Shmspace_t), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (pShm == MAP_FAILED) {
        DIE("mmap errno=%d; %s\n", errno, strerror(errno));
    }
    fprintf(stderr, _AT_ "  pShm=0x%016lx\n", (uint64_t)pShm);

    // init shm
    {
        ShMemLock guard(&(pShm->lock));
        if (!shmAlreadyExists) {
            pShm->initThread = Kernel_ProcessorID();
            for (int i=0; i<ASIZE; i++) {
                pShm->data[i] = i;
            }
        }
    }

    // print shm contents
    fprintf(stderr, _AT_ "  shmspace: initThread=%d, (%ld) (%ld) (%ld)\n",
            pShm->initThread, pShm->data[0], pShm->data[1], pShm->data[2]);

    return pShm;
}


void AtExitFunc()
{
    fprintf(stderr, "AtExiting hw thread %d\n", Kernel_ProcessorID());
}



int main(int argc, char *argv[])
{
    int numThreads = atoi(argv[1]);
    fprintf(stderr, "numThreads=%d\n", numThreads);

    //int mainId = Kernel_ProcessorID();

    Shmspace_t *pShm = OpenShMem();

    atexit(AtExitFunc);

    #pragma omp parallel default(none) num_threads(numThreads) \
            shared(stderr,numfails,pShm)
    {
        unsigned tid = upci_gettid();

        unsigned t = Kernel_ProcessorID();
        fprintf(stderr, "Check Lock Class : I'm hw thread %d\n", t);
        {
            // Check between processes with normal lock
            ShMemLock guard(&(pShm->lock1));
            unsigned targ = (tid<<8)+t+1;
            fprintf(stderr, "thread %d Got Lock - index = 0x%08x\n", t, pShm->lock1);
            if (pShm->lock1 != targ) {
                PRINTERR("testLock = 0x%08x. Should be 0x%08x\n", pShm->lock1, targ);
            }
            Upci_Delay(400);
            if (pShm->lock1 != targ) {
                PRINTERR("testLock = 0x%08x. Should be 0x%08x\n", pShm->lock1, targ);
            }

        }

        // Only have lesser threads delay - see what happens when upper thread exits 1st.
        if (t < 32) {
            Upci_Delay(10000000);
        }


        fprintf(stderr, "done. rc = %d\n", numfails);
    }

    // close shm
    munmap(pShm, sizeof(Shmspace_t));
    close(shmid);
    shm_unlink(SHMPATH);

    return(numfails);
}

