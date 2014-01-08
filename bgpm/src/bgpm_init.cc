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


#include "spi/include/upci/upci.h"
#include "bgpm/include/bgpm.h"
#include "SigHandler.h"

#define _BGPM_INIT_C_
#include "globals.h"

using namespace bgpm;

static UPC_Lock_t initLock = 0;             //!< Lock for Bgpm_Init synchronization
static unsigned   initRefCount = 0;

// In thread safe fashion, init process and thread global vars,
// set up shared memory pointers, and init shared mem space as needed.
// Needed because the thread objects must be POD (pretty ordinary types)
// which need explicit initialization.  We are letting
// the first Bgpm_Init on each thread do this.
int InitGlobalVars()
{
    // assumes critical section is locked by caller.

    // if 1st Bgpm_Init - let's create the global process vars.
    //fprintf(stderr, _AT_ " procInited=%d\n", procInited);

    if (!procInited) {
        pShMem.Init();
    }
    pThdShMem.Init();

    curHwThdId = Kernel_ProcessorID();

    Personality_t personality;
    Kernel_GetPersonality(&personality, sizeof(Personality_t));
    nodeConfig = personality.Kernel_Config.NodeConfig;

    if (pShMem == NULL) {
        pShMem = GetShmemBlk();
        if (pShMem == NULL) {
            BGPM_TRACE_PT1;
            return lastErr.LastErrNum(); // expect GetShmemBlk to handle last error when null.
        }
    }
    // make threaded copy to keep shmem active as long as this thread is using it.
    // Can use either copy (pThdshMem or pShMem) to access shared memory
    pThdShMem.Assign(pShMem);
    assert(pThdShMem != NULL);

    // Init threaded handle entries.
    swModeAppliedPuEvtSet = -1;
    swModeAppliedL2EvtSet = -1;
    swModeAppliedIOEvtSet = -1;

    BGPM_TRACE_DATA_L1(pThdShMem->Dump(1, Kernel_ProcessorCoreID()));

    return 0;
}



// Init global mode if necessary
int InitGlobalMode(Bgpm_Modes mode)
{
    if (! pShMem->globalModeInited) {
        // Make sure only process initializing at a time
        ShMemLock guard(&(pShMem->globalModeLock));
        // check again in case of race conditions
        if (! pShMem->globalModeInited) {

            BGPM_TRACE_PT1;
            int rc = 0;
            switch (mode) {
                case BGPM_MODE_LLDISTRIB: {
                    pShMem->globalMode = mode;
                    pShMem->perspect = BGPM_SW_PERSPECTIVE;
                    break;
                }
                case BGPM_MODE_SWDISTRIB: {
                    pShMem->globalMode = mode;
                    pShMem->perspect = BGPM_SW_PERSPECTIVE;
                    break;
                }
                case BGPM_MODE_HWDISTRIB: {
                    pShMem->globalMode = mode;
                    pShMem->perspect = BGPM_HW_PERSPECTIVE;
                    break;
                }
                default: {
                    rc = BGPM_ENOTIMPL;
                    break;
                }
            }

            if (rc) {
                BGPM_TRACE_PT1;
                return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
            }

            pShMem->globalModeInited = true;
             // note: lock destructor does the needed mbar;
         }
    }
    return 0;
}


// initialize the UPC mode for the process
// This initializes the hardware, and
// we might add some feedback from the kernel
// on hardware settings to be able to adjust for
// the spare core logical to physical mapping.
int InitProcessUPCMode(Bgpm_Modes mode)
{
    // Assumes critical section lock handled by caller
    if (! procInited) {

        BGPM_TRACE_PT1;
        // The following Mode init operations are also
        // reference counted in the kernel.  All threads must agree
        // on mode and 1st one wins.
        int rc = 0;
        switch (mode) {
            case BGPM_MODE_LLDISTRIB: {
                rc = Upci_Mode_Init(&(pShMem->upciMode), 0, UPC_CM_DISCON, 0);
                break;
            }
            case BGPM_MODE_SWDISTRIB: {
                rc = Upci_Mode_Init(&(pShMem->upciMode), 0, UPC_CM_INDEP, 0);
                break;
            }
            case BGPM_MODE_HWDISTRIB: {
                rc = Upci_Mode_Init(&(pShMem->upciMode), 0, UPC_CM_SYNC, 0);
                break;
            }
#if 0
            case BGPM_MODE_HWDETAIL: {
                rc = Upci_Mode_Init(&(pShMem->upciMode), 1, UPC_CM_SYNC, 0);
                break;
            }
#endif
            default: {
                rc = BGPM_ENOTIMPL;
                break;
            }
        }

        if (rc) {
            BGPM_TRACE_PT1;
            return lastErr.PrintOrExitOp(rc, "Upci_Mode_Init", BGPM_ERRLOC);
        }
    }
    return 0;
}




extern "C" int Bgpm_Init(Bgpm_Modes mode)
{
    #if (BGPM_TRACE || BGPM_TRACE_ALL)
    if (envVars.GetTraceEnableAtStart()) {
        BGPM_TRACE_PRINT_ENABLE();
    }
    #endif

    BGPM_TRACE_L1;

    // Initialize this thread's lastErr object.
    lastErr.Init();
    int rc = 0;

    lastErr.SetTopLevelFunc(__func__);


    // Reject if subnode job
    BG_JobCoords_t jobCoords;
    Kernel_JobCoords(&jobCoords);  // don't care whether sub-block job
    if (jobCoords.shape.core < CONFIG_MAX_APP_CORES) {
        return lastErr.PrintOrExit(BGPM_EINV_SUBNODE, BGPM_ERRLOC);
    }

    // Return if this sw thread has already been initialized
    if (UNLIKELY(thdInited)) {
        return lastErr.PrintOrExit(BGPM_EINITED, BGPM_ERRLOC);
    }

    BGPM_TRACE_DATA_L1(fprintf(stderr, "%s" _AT_ " hwtid=%02d, tid=%ld, procInited=%d, thdInited=%d\n", IND_STRG, Kernel_ProcessorID(), gettid(), procInited, thdInited));

    // Check and Update requested mode from environment values
    mode = envVars.GetEnvMode(mode);

    {
        Lock guard(&initLock);

        if ((rc = InitGlobalVars())) return rc;
        if ((rc = InitGlobalMode(mode))) return rc;

        int sig = envVars.PMSignal(UPCI_DEFAULT_PM_SIGNAL);
        if ((rc = RegisterSigHandler(sig))) return rc;

        envVars.AddEventErrFeedback();

        // Validate that this thread's mode is consistent with global mode
        if (mode != pShMem->globalMode) {
            lastErr.PrintMsg("Bgpm_Init Error: passed mode(%d) does not match global mode(%d)\n",
                                mode, pShMem->globalMode);
            // drop this threads and processes hold on the shared memory config.
            // When was not able to configure the mode.
            pThdShMem = NULL;
            if (initRefCount == 0) { pShMem = NULL; }
            return lastErr.PrintOrExit(BGPM_ECONF_MODE, BGPM_ERRLOC);
        }

        if ((rc = InitProcessUPCMode(mode))) {
            // drop this threads and processes hold on the shared memory config.
            // When was not able to configure the mode.
            pThdShMem = NULL;
            if (initRefCount == 0) { pShMem = NULL; }
            return rc;
        }

        procInited = true;
        initRefCount++;
        thdInited = true;
    }

    Kernel_Upci_SetBgpmThread(); // Associate sw thread with kernel hardware thread

    BGPM_TRACE_DATA_L1(fprintf(stderr, "%s" _AT_ " hwtid=%02d, tid=%ld, procInited=%d, thdInited=%d\n", IND_STRG, Kernel_ProcessorID(), gettid(), procInited, thdInited));

    return rc;
}




extern "C" int Bgpm_Disable()
{
    BGPM_TRACE_L1;

    if (! thdInited) {
        lastErr.Init();
        lastErr.SetTopLevelFunc(__func__);
        return lastErr.PrintOrExit(BGPM_ENOT_INIT, BGPM_ERRLOC);
    }
    lastErr.SetTopLevelFunc(__func__);

    Kernel_Upci_ClearBgpmThread(); // Drop Association of sw thread with kernel hardware thread.

    // Note: Destruction of global pointers will destroy references to event sets.

    Lock guard(&initLock);
    assert(initRefCount > 0);
    assert(procInited);

    //ShMemLock guard2(&(pShMem->dbgLock));  // <<<<<<< Debug

    // free this threads reference to event sets and shrmem
    BGPM_TRACE_DATA_L1(procEvtSets.Dump(IND_LVL));
    procEvtSets.RmvAllSets(curHwThdId);
    pThdShMem = NULL;
    thdInited = false;


    // if last active user of bgpm in process
    if (--initRefCount == 0) {

        // Free the Upci_Mode used by this process.
        // Does reference counting, so last use of shared mem
        // and last process using the Upci_Mode will free them
        // to be set to new values.
        assert(procInited);
        Upci_Mode_Free();
        procInited = false;

        // Free this process user of shared memory by setting
        // smart ptr to null.  If pShMem is the last user
        // the shared memory will be marked uninitialized -
        // which indirectly also frees all resources including the
        // global mode.
        assert(pShMem != NULL);
        BGPM_TRACE_DATA_L1(pShMem->Dump(IND_LVL));
        pShMem = NULL;
        FreeShmemBlk();
        assert(pShMem == NULL);
    }
    mbar();

    return BGPM_EOK;
}

