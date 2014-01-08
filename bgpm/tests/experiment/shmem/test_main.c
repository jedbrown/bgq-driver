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

#include "spi/include/kernel/location.h"


//! Define gymnastics to create a compile time AT string.
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define _AT_ __FILE__ ":" TOSTRING(__LINE__)

__INLINE__ void *Die(char *strg, int errNum, char *location)
{
    fprintf(stderr, "ERROR: %s %d at %s\n", strg, errNum, location);
    exit(errNum);
}


#define ASIZE 10
typedef struct {
    uint64_t data[ASIZE];
} Shmspace_t;


#define SHMPATH "bgpm-shmtest"

int main(int argc, char *argv[])
{
    int shmid;
    if ((shmid = shm_open(SHMPATH, O_RDWR | O_CREAT | O_EXCL, 0600)) < 0) {
        Die(strerror(errno), errno, _AT_);
    }
    printf(_AT_ "  shmid=%d\n", shmid);

    if (ftruncate( shmid, sizeof(Shmspace_t)) < 0) {
        Die(strerror(errno), errno, _AT_);
    }
    Shmspace_t *pShm = mmap(NULL, sizeof(Shmspace_t), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (pShm == MAP_FAILED) {
        Die(strerror(errno), errno, _AT_);
    }
    printf(_AT_ "  pShm=0x%016lx\n", (uint64_t)pShm);

    // init shm
    int i;
    for (i=0; i<ASIZE; i++) {
        pShm->data[i] = i;
    }

    // print shm contents
    printf(_AT_ "  shmspace=");
    for (i=0; i<ASIZE; i++) {
        printf(" (%ld)", pShm->data[i]);
    }
    printf("\n");

    // close shm
    munmap(pShm, sizeof(Shmspace_t));
    close(shmid);
    shm_unlink(SHMPATH);



    // Reopen shm
    if ((shmid = shm_open(SHMPATH, O_RDWR | O_CREAT | O_EXCL, 0600)) < 0) {
        Die(strerror(errno), errno, _AT_);
    }
    printf(_AT_ "  shmid=%d\n", shmid);

    if (ftruncate( shmid, sizeof(Shmspace_t)) < 0) {
        Die(strerror(errno), errno, _AT_);
    }
    pShm = mmap(NULL, sizeof(Shmspace_t), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (pShm == MAP_FAILED) {
        Die(strerror(errno), errno, _AT_);
    }
    printf(_AT_ "  pShm=0x%016lx\n", (uint64_t)pShm);

    // print newly opened contents
    printf(_AT_ "  shmspace=");
    for (i=0; i<ASIZE; i++) {
        printf(" (%ld)", pShm->data[i]);
    }
    printf("\n");


    munmap(pShm, sizeof(Shmspace_t));
    close(shmid);
    shm_unlink(SHMPATH);

    return 0;
}
