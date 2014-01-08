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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <assert.h>


#include "globals.h"
#include "ShmemBlk.h"
#include "Debug.h"

#define SHMPATH "bgpm-shm"


using namespace bgpm;




static UPC_Lock_t getShmLock = 0; // lock between threads.
ShmemBlk * pShm = NULL;
static int shmid = 0;



void ShmemBlk::Init()
{
    // don't clear refLock - it's in use
    //fprintf(stderr, "%s" _AT_ "\n", IND_STRG);

    refCount = 0;
    timesInited++;
    inited = true;

    // clear reset
    uint64_t offset = (uint64_t)(&(this->globalModeLock)) - (uint64_t)this;
    memset( ((uint8_t*)this) + offset, 0, sizeof(ShmemBlk)-offset);

    for (unsigned core=0; core<CONFIG_MAX_CORES; ++core) {
        coreOvfQue[core].Init();
    }
    controlL2.Init();
    controlIO.Init();
}



void ShmemBlk::operator delete(void *p)
{
    // Do nothing - caller must explicitly atomically allocate and delete shared memory object.
}



ShmemBlk * bgpm::GetShmemBlk()
{
    BGPM_TRACE_L2;

    // don't redo in process
    if (pShm == NULL) {
        Lock guard(&getShmLock);
        if (pShm == NULL) { // double-check in case of race conditions

            unsigned procCount = Kernel_ProcessCount();
            //fprintf(stderr, "%s" _AT_ " Kernel_ProcessCount() = %d\n", IND_STRG, procCount);
            if (procCount > 1) {

                BGPM_TRACE_PT2;

                if ((shmid = shm_open(SHMPATH, O_RDWR | O_CREAT, 0600)) < 0) {
                    lastErr.PrintOrExitOp(-errno, "shm_open", BGPM_ERRLOC);
                    return NULL;
                }
                BGPM_TRACE_DATA_L2( fprintf(stderr, "%s" _AT_ "  shmid=%d\n", IND_STRG, shmid) );

                if (ftruncate( shmid, sizeof(ShmemBlk)) < 0) {
                    lastErr.PrintOrExitOp(-errno, "ftruncate", BGPM_ERRLOC);
                    return NULL;
                }
                ShmemBlk *pTShm = (ShmemBlk*)mmap(NULL, sizeof(ShmemBlk), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
                if (pTShm == MAP_FAILED) {
                    lastErr.PrintOrExitOp(-errno, "mmap", BGPM_ERRLOC);
                    return NULL;
                }
                pShm = pTShm;
            }

            // One process - just use malloc
            else {
                BGPM_TRACE_PT2;
                //fprintf(stderr, "%s" _AT_ " Size of ShmemBlk = %ld\n", IND_STRG, sizeof(ShmemBlk));
                pShm = (ShmemBlk *)malloc(sizeof(ShmemBlk));
                if (pShm == NULL) {
                    lastErr.PrintOrExitOp(-errno, "malloc", BGPM_ERRLOC);
                    return NULL;
                }
                memset((void*)pShm, 0, sizeof(ShmemBlk));
            }


            BGPM_TRACE_DATA_L2( fprintf(stderr, "%s"_AT_ "  pShm=0x%016lx\n", IND_STRG, (uint64_t)pShm) );
        }
    }

    assert(pShm);
    assert(pShm->refCount >= 0);

    #ifdef __HWSTUBS__
    // If using stubbed version - only one 1 thread allowed, and need to always init the
    // shared memory
    pShm->inited = false;
    #endif

    return pShm;
}



void bgpm::FreeShmemBlk()
{
    BGPM_TRACE_L2;

//#if 0
    // don't redo in process
    if (pShm) {
        Lock guard(&getShmLock);
        if (pShm) { // double-check in case of race conditions
            unsigned procCount = Kernel_ProcessCount();
            //fprintf(stderr, "%s" _AT_ " Kernel_ProcessCount() = %d\n", IND_STRG, procCount);
            if (procCount > 1) {
                munmap(pShm, sizeof(ShmemBlk));
                close(shmid);
                shm_unlink(SHMPATH);  // <<< Should we being doing a referenced counted unlink?
            }
            else {
                free(pShm);
            }

            pShm = NULL;
            shmid = 0;
        }
    }
//#endif
}



void ShmemBlk::AddRef()
{
    ShMemLock guard(&refLock);
    //assert(pShm->inited);
    if (!inited) {
         BGPM_TRACE_PT2;
         Init();
    }
    refCount++;  // keep track to know when to un-init shmem
    mbar();
    //fprintf(stderr, "%s" _AT_ " refCount=%d\n", IND_STRG, pShm->refCount);
}



bool ShmemBlk::RmvRef()
{
    ShMemLock guard(&refLock);
    assert(refCount > 0);
    assert(inited);
    if (--refCount == 0) {
        inited = false;
    }
    //fprintf(stderr, "%s" _AT_ " refCount=%d\n", IND_STRG, pShm->refCount);
    mbar();
    return (refCount == 0);
}



