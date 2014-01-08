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

#include "Kernel.h"
#include "ctype.h"

//
// Functions which bridge the compile time structures for CNK and
// the UPC focused operations found in spi/lib/libupci_fwcnk.a
//
// That is, the spi/src/upci/upci_syscall.c functions running in kernel
// don't have compile time access to CNK stuctures.  And the cnk functions
// here don't have compile time access to UPC structures.
//
// These functions bridge that gap as necessary.
//

// Make sure the UPC hardware owning kthread is clear for all hw threads.
void UPC_KBridge_ClearAllActiveKThreads()
{
    int core, thd;
    for (core=0; core<CONFIG_MAX_CORES; core++) {
        CoreState_t *pCS = &(NodeState.CoreState[core]);
        for (thd=0; thd<CONFIG_HWTHREADS_PER_CORE; thd++) {
            HWThreadState_t *pHWT = &(pCS->HWThreads[thd]);
            pHWT->pUpci_ActiveKThread = NULL;
        }
    }
}



// Set UPC hardware owning kthread
void UPC_KBridge_SetActiveKThread()
{
    CoreState_t *pCS = &(NodeState.CoreState[ProcessorCoreID()]);
    HWThreadState_t *pHWT = &(pCS->HWThreads[ProcessorThreadID()]);
    pHWT->pUpci_ActiveKThread = GetMyKThread();
}



// Clear UPC hardware owning kthread
void UPC_KBridge_ClearActiveKThread()
{
    CoreState_t *pCS = &(NodeState.CoreState[ProcessorCoreID()]);
    HWThreadState_t *pHWT = &(pCS->HWThreads[ProcessorThreadID()]);
    pHWT->pUpci_ActiveKThread = NULL;
}



void UPC_KBridge_Signal_Deliver(int sig)
{
    int core = ProcessorCoreID();
    int hwtid = ProcessorThreadID();
    CoreState_t *pCS = &(NodeState.CoreState[core]);
    HWThreadState_t *pHWT = &(pCS->HWThreads[hwtid]);
    if (pHWT->pUpci_ActiveKThread) {
        Signal_Deliver(pHWT->pUpci_ActiveKThread->pAppProc, GetTID(pHWT->pUpci_ActiveKThread), sig);
    }
}


// set kthread ptr to indicate which cfg structure the scheduler will pass on sw thread swap
void UPC_KBridge_SetSwapCfg(void *pCfg)
{
    KThread_t *pKThread = GetMyKThread();
    pKThread->pUpci_Cfg = pCfg;
}


void UPC_KBridge_ClearSwapCfg()
{
    KThread_t *pKThread = GetMyKThread();
    pKThread->pUpci_Cfg = NULL;
}

