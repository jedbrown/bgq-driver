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
#include "hwi/include/bqc/BIC_inlines.h"
#include "hwi/include/bqc/gea_dcr.h"
#include "hwi/include/bqc/l2_central_inlines.h"
#include "spi/include/l2/atomic.h"
#include "rollback.h"

extern NodeState_t NodeState;


/*
 Assumptions:
    - There will be a testint register that will tell us that we are coming out of a rollback event
    - Rollback does not need to restore "non-PU" state, such as L2 slice, L2 central, GEA, DDR, MU, NU, SD, BeDRAM...
    - Rollback must restore A2, QPX, L1p, PUEA, MMU state.  
 */


int isRollback()
{
    // Are we initializing from a rollback event
    return 0;
}

int Rollback_Init()
{
    if( isRollback() )   // Check to see if this is a rollback occurance
    {
        // Restore hardware thread context
        // Resume hardware threads
    }
    return 0;
}

void IntHandler_RollbackSnapshot(int status_reg, int bitnum)
{
    GEA_DCR_PRIV_PTR->gea_interrupt_state__noncritical = GEA_DCR__GEA_INTERRUPT_STATE__TIMER0_INT_set(1);
    BIC_WriteGeaInterruptState(GEA_DCR__GEA_INTERRUPT_STATE__TIMER0_INT_set(1));
    GetMyKThread()->Pending |= KTHR_PENDING_SNAPSHOT;
}

int Rollback_Restore()
{
    // Check to see if MU was advanced
    if(L2_AtomicLoadClear(NodeState.RollbackIndicator) == 0)
    {
        return 0;
    }
    
    // Kernel has been traversed while in rollback window
    if(NodeState.RollbackKernel != 0)
    {
        return 0;
    }
#if 0
    // Check to see if snapshot occurred in the kernel
    if(GetMyHWThreadState()->RollbackState.ip < 16*1024*1024)
    {
        // Interrupt occurred in the kernel, likely scheduler.  
        return 1;
    }
    LC_RestoreFromRollback((Regs_t*)&GetMyHWThreadState()->RollbackState);
#endif    
    return 0;
}

void Rollback_Barrier()
{
    /// Taking an extremely simplistic approach for now, see L2 document for a more performant implementation.
    int oldspecid = 0;
    static int newspecid;
    Kernel_Barrier(Barrier_HwthreadsInNode);
    if(ProcessorID() == 0)
    {
        oldspecid = SPEC_GetSpeculationIDSelf_priv();
        newspecid = SPEC_TryAlloc(1<<(15-NodeState.RollbackDomain));
    }
    Kernel_Barrier(Barrier_HwthreadsInNode);
    SPEC_SetSpeculationIDSelf(0x80 + newspecid);
    ppc_msync();
    Kernel_Barrier(Barrier_HwthreadsInNode);
    if(ProcessorID() == 0)
    {
        if(oldspecid & 0x80)
        {
            L2_AtomicLoadClear(NodeState.RollbackIndicator);
            NodeState.RollbackKernel = 0;
            SPEC_TryCommit((oldspecid & 0x7f));
        }
    }
    Kernel_Barrier(Barrier_HwthreadsInNode);
}

int Rollback_JobInit()
{
    uint32_t snaprate = 0;
    
    /* Speculation_Init must run before Rollback_JobInit() */
    
    App_GetEnvValue("BG_SNAPSHOTRATE", &snaprate);
    if(snaprate)
    {
        Speculation_AllocateDomain(&NodeState.RollbackDomain);
        /* If rollback is specified for the job, it will always be in domain 0.  This makes allocating domains simplier. */
        
        BIC_WriteGeaInterruptControlRegisterHigh(GEA_DCR__GEA_INTERRUPT_STATE_CONTROL_HIGH__TIMER0_INT_set(3));
        DCRWritePriv(GEA_DCR(TIMER0_CONFIG), GEA_DCR__TIMER0_CONFIG__RELOAD_VAL_set(snaprate) | GEA_DCR__TIMER0_CONFIG__IE_set(1) | GEA_DCR__TIMER0_CONFIG__ARE_set(0));
        DCRWritePriv(GEA_DCR(TIMER0), GEA_DCR__TIMER0__COUNT_set(snaprate));
        
        SPEC_SetPrivMap(  SPEC_GetPrivMap() | L2C_PRIVMAP_DISABLEDOMAIN(NodeState.RollbackDomain) );
        SPEC_SetDomainMode(NodeState.RollbackDomain, L2C_DOMAINATTR_MODE(L2C_DOMAINATTR_MODE_ROLL));
        ppc_msync();
    }
    else
    {
        DCRWritePriv(GEA_DCR(TIMER0_CONFIG), 0);
    }
    return 0;
}
