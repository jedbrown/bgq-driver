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

#ifndef _BGPM_SHMEM_BLK_H_  // Prevent multiple inclusion
#define _BGPM_SHMEM_BLK_H_

#include "Lock.h"
#include "bgpm/include/types.h"
#include "impl_types.h"
#include "SharedOvfQueue.h"
#include "SharedControlL2.h"
#include "SharedControlIO.h"


namespace bgpm {


#define MAX_HWSPECBKT_PIDS 4   //!< Maximum number of pids per node with speculative buckets using upc_c hardware accum
#define MAX_IOL2_SHARED_OVFS 6 //!< Max number of shared I/O or L2 events which we will support for simultaneous overflow


//! ShmemBlk structure
/*!
  A shared memory structure for data referenced across bgpm aware processes.
  The data space starts out zeroed by CNK, but the shared memory is
  never actually freed, even though the shared memory is unlinked.
*/
struct ShmemBlk
{
    UPC_Lock_t       refLock;       //!< cross thread shmem allocation lock.
    unsigned         refCount;      //!< indicate # of threads referencing shared block
    unsigned         timesInited;   //!< count the # of times shared memory has been inited (constructed). >1 when Bgpm_Disable()->Bgpm_Init()
    bool             inited;        //!< refCount inadequate - keep track of inited as well.

    UPC_Lock_t       dbgLock;       //!< lock for serialization during debug as needed

    UPC_Lock_t       globalModeLock;
    Bgpm_Modes       globalMode;        //!< keep track that all thread modes are consistent
    Bgpm_Perspective perspect;          //!< perspect updates only locked and set by Bgpm_Init
    Upci_Mode_t      upciMode;          //!< upciMode built only by Bgpm_Init.
    bool             globalModeInited;

    bool             evtFeedback;       //!< Atomically set if a thread has encountered a punit event allocation failure.

    UPC_Lock_t       l2ModeLock;
    unsigned         l2ModeRefCount;    //!< num L2 event sets using l2 mode
    bool             combineL2;         //!< true if l2 slice counts are to be combined.

    UPC_Lock_t       linkRefLock;
    unsigned         linkRefCounts[UPC_NW_NUM_LINKS];  //!< num references from diff threads.
    unsigned         linkController[UPC_NW_NUM_LINKS]; //!< controlling hw thread
    uint64_t         linkStartCycles[UPC_NW_NUM_LINKS];    //!< latest timebase start cycles (0 when not running)
    uint64_t         linkElapsedCycles[UPC_NW_NUM_LINKS];  //!< accumulated timebase cycles (updated only by controller)

    CoreReservations coreRsv[CONFIG_MAX_CORES];    //!< reserve structure common to each core

    SharedOvfQueue   coreOvfQue[CONFIG_MAX_CORES]; //!< shared overflow tracking for each core

    SharedControlL2  controlL2;                    //!< shared control of L2 state and overflows
    SharedControlIO  controlIO;                    //!< shared control of I/O state and overflows


    // Atomically use GetShmemBlk() and FreeShmemBlk() to create and free the shmem.
    // This delete override does nothing, but needed by PodSharePtr
    void operator delete(void *p);
    void Init();              //!< make sure all is starting state (except ref counts)
    void AddRef();            //!< SharePtr - each thread indicates that it's using ShmemBlk
    bool RmvRef();            //!< SharePtr - each thread indicates when it's done with ShmemBlk

    void Dump(unsigned indent, int core=-1);
};



/*!
  Return a raw pointer to the shared memory block for Bgpm
  and make sure the block is initialized.
  Should only do only once per process, but it should work otherwise.
*/
ShmemBlk * GetShmemBlk();

/*!
  Free allocated shared memory block.
*/
void FreeShmemBlk();






}

#endif
