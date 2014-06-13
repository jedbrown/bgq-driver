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
#include "hwi/include/bqc/l2_central_inlines.h"
#include "hwi/include/bqc/l2_dcr.h"
#include "hwi/include/bqc/l2_util.h"
#include "hwi/include/bqc/testint_inlines.h"
#include "spi/include/kernel/spec.h"

uint32_t SIMPLE_ROLLBACK = 1;
uint64_t SpecDomainsAllocated K_ATOMIC;
uint64_t domainsConfigured;
Lock_Atomic_t FastPathsLock;
uint64_t FastPathsEnabled = 0;

uint64_t default_l2_scrub_rate[L2_DCR_num];
uint64_t default_l2_first_init = 1;



/*!
 * \brief Initializes speculation registers before the start of a job.
 */
int Speculation_Init()
{
    int slice;
    L2C_SPECID_t specid;
    
    if(TI_isDD1() || ((GetPersonality()->Kernel_Config.NodeConfig & PERS_ENABLE_DD1_Workarounds) != 0))
    {
    }
    else
    {
        SPEC_SetNumberOfDomains(1);
        SPEC_SetPrivMap(
            L2C_PRIVMAP_DISABLEWRITEFNC(L2C_PRIVMAP_FUNC_NUMDOM)  | 
            L2C_PRIVMAP_DISABLEWRITEFNC(L2C_PRIVMAP_FUNC_PRIVMAP) 
            );
        ppc_msync();

        for(specid=0; specid<128; specid++)
        {
            SPEC_TryChangeState_priv(specid, L2C_IDSTATE_PRED_SPEC | L2C_IDSTATE_INVAL);  
            SPEC_SetConflict_priv(specid, 0);
        }
        ppc_msync();
    }
    App_GetEnvValue("BG_SIMPLEROLLBACK", &SIMPLE_ROLLBACK);
    L2_AtomicStore(&SpecDomainsAllocated, 0);
    domainsConfigured = 0;
    
    // Reset the L2 scrub rate
    for(slice=0; slice<L2_DCR_num; slice++)
    {
        // Set the L2 scrub rate
        uint64_t l2_dcr_refctrl = DCRReadPriv(L2_DCR(slice, REFCTRL));
        if(default_l2_first_init)
            default_l2_scrub_rate[slice] = L2_DCR__REFCTRL__SCB_INTERVAL_get(l2_dcr_refctrl);
        L2_DCR__REFCTRL__SCB_INTERVAL_insert(l2_dcr_refctrl, default_l2_scrub_rate[slice]);
        DCRWritePriv(L2_DCR(slice, REFCTRL), l2_dcr_refctrl);
    }
    default_l2_first_init = 1;
    
    Speculation_ExitJailMode();
    return 0;
}

int l1p_init()
{
    // Restore A2 hardware thread priority
    ThreadPriority_Medium();
    
    if(PhysicalThreadID() != 0)
        return 0;
    
    // Restore prefetcher state
    CoreState_t* cs = GetCoreStateByCore(PhysicalProcessorID());
    if(cs->default_l1p_init)
    {
        out64((void *)L1P_CFG_SPEC,   cs->default_l1p_cfgspec);
        out64((void *)L1P_CFG_PF_USR, cs->default_l1p_cfgpfusr);
        out64((void *)L1P_CFG_PF_SYS, cs->default_l1p_cfgpfsys);
        ppc_msync();
    }
    else
    {
        ppc_msync();
        cs->default_l1p_cfgspec  = in64((void*)L1P_CFG_SPEC);
        cs->default_l1p_cfgpfusr = in64((void*)L1P_CFG_PF_USR);
        cs->default_l1p_cfgpfsys = in64((void*)L1P_CFG_PF_SYS);
        cs->default_l1p_init     = 1;
        ppc_msync();
    }
    
    // Direct L1p DCR violations to CNK
    out64_sync((void*)L1P_ESR_GEA, in64((void*)L1P_ESR_GEA) & ~L1P_ESR_err_mmio_priv);  // Steal priv bit from firmware GEA
    out64_sync((void*)L1P_ESR_BIC_CRITICAL, L1P_ESR_err_mmio_priv);                    // Set priv bit to core-critical
    return 0;
}

int Speculation_CleanupJob()
{
    SPEC_SetSpeculationIDSelf_priv(0x400);   // clear interrupt state, clear lower 9 bits as well

    // Restore the default system-call and standard-interrupt code sequences.
    // See Speculation_EnableFastSpeculationPath() for commentary on this
    // process.  In this case we need an IPI only for the "system" core,
    // because this routine is called on every application hardware thread.
    uint64_t ici_needed = 0;
    Kernel_Lock(&FastPathsLock);
    if (FastPathsEnabled)
    {
	extern uint32_t Vector_EI_trampoline;
	extern uint32_t Vector_SC_trampoline;

	uint64_t exceptionVector = mfspr(SPRN_IVPR);
	*((uint32_t *) (exceptionVector + IVO_EI)) = Vector_EI_trampoline;
	*((uint32_t *) (exceptionVector + IVO_SC)) = Vector_SC_trampoline;
	ppc_msync();   // make sure the stores have taken effect
	FastPathsEnabled = 0;
	ici_needed = 1;  // we can't hold the lock while sending IPI's
    }
    Kernel_Unlock(&FastPathsLock);

    // Flush the icache whether or not we're the thread that did the patching.
    // We only need to do this from one thread on each core.
    if (ProcessorThreadID() == 0)
    {
	isync();
	ici();
    }

    if (ici_needed)
    {
	// We still need an IPI for the "system" core.
	IPI_invalidate_icache(NodeState.NumCoresEnabled - 1);
	Kernel_WriteFlightLog(FLIGHTLOG_high, FL_SPCFEPDIS, 0,0,0,0);
    }
    
    // bqcbugs 1620
    l2_set_overlock_threshold(0);
    l2_set_spec_threshold(0);
    l2_set_prefetch_enables(1);
    // --
    
    return 0;
}


/*!
 * \brief Allocates a speculative domain. 
 * \note Each additional domain potentially decreases the number of speculative IDs assigned to each domain.
 * \note Domains must have all speculative IDs set to the available state
 */
int Speculation_AllocateDomain(unsigned int* domain)
{
#if 0
    const unsigned char domainmap[17] = { 1, 1, 2, 4, 4, 8, 8, 8, 8, 16, 16, 16, 16, 16, 16, 16, 16};
    if(NodeState.NumSpecDomains >= 16)
    {
        return ENOMEM;
    }
    if(!SPEC_AllAvailOrInvalid())
    {
        // if any ID is speculative or committed, we can not switch
        // \todo this needs to be made safe. 
        // While switching, we can not allow threads to alloce concurrently (race)
        // TM domains need to be made aware of changes to adapt their allocation mask
        // In short, all speculation RTS need to be shut down temporarily while changing number of domains
        return ENOMEM;
    }
    NodeState.NumSpecDomains++;
    SPEC_SetNumberOfDomains( domainmap[NodeState.NumSpecDomains] );
    SPEC_SetDomainMode_priv(NodeState.NumSpecDomains-1, L2C_DOMAINATTR_MODE_STM);
    ppc_msync();
    
    /* \todo Initialize commit, alloc, reclaim pointers??? */
    
    *domain = NodeState.NumSpecDomains-1;
#endif
    uint32_t domainAllocated = L2_AtomicLoadIncrement(&SpecDomainsAllocated);
    if(domainAllocated >= SPEC_GetNumberOfDomains())
    {
        return ENOMEM;
    }
    
    // bqcbugs 1620.
    l2_set_prefetch_enables(0);
    l2_unlock_all_with_address((void *) 0x200000);
    l2_set_overlock_threshold(0xA);                      // set L2 overlock and spec thresholds
    l2_set_spec_threshold(0xA);
    // --
    
    Kernel_WriteFlightLog(FLIGHTLOG_high, FL_SPCALCDOM, domainAllocated,0,0,0);
    *domain = domainAllocated;
    return 0;
}

int Speculation_SetAllocatedDomainCount(uint32_t domaincount)
{
    Kernel_WriteFlightLog(FLIGHTLOG_high, FL_SPCSETDOM, domaincount,0,0,0);
    domainsConfigured = domaincount;
    return 0;
}

int Speculation_GetAllocatedDomainCount(uint32_t* domaincount)
{
    *domaincount = domainsConfigured;
    Kernel_WriteFlightLog(FLIGHTLOG_high, FL_SPCGETDOM, domainsConfigured,0,0,0);
    return 0;
}


/*! \brief Modifies the current user process with speculative context for the given specid
 *  \param[in] specid_self SpecID that is the source of the restart
 *  \param[in] gpr3        Return code from the user-space longjmp call
 *  \internal Cannot be called externally to speculation.c
 */
static void Speculation_Restart(L2C_SPECID_t specid_self, uint64_t gpr3, Regs_t* context)
{
    HWThreadState_t *pHWT = GetMyHWThreadState();
    StoreConditional( pHWT->dummyStwcx.data, 0);
    SPEC_SetIDState_priv(specid_self, L2C_IDSTATE_INVAL);
    SPEC_SetSpeculationIDSelf_priv(0x400); // clear interrupt state, clear lower 9 bits as well
    uint64_t procid = ProcessorID();
    AppProcess_t* process = GetMyProcess();
    context->ip      = process->SpecState->hwt_state[procid].ip;
    context->gpr[1]  = process->SpecState->hwt_state[procid].gpr1;
    context->gpr[2]  = process->SpecState->hwt_state[procid].gpr2;
    context->gpr[3]  = gpr3;
    Speculation_ExitJailMode();

    Kernel_WriteFlightLog(FLIGHTLOG_high, FL_SPCRESTRT, specid_self, context->ip, context->gpr[1], context->gpr[3]);
    TRACE( TRACE_Spec, ("(I) Speculation_Restart: ip=%lx  sp=%lx  toc=%lx  gpr3=%lx  specid=%lx\n", context->ip, context->gpr[1], context->gpr[2], context->gpr[3], specid_self));    
}

#define GetSpecID(p) ( p->SpecState->hwt_state[ProcessorID()].specid )

/*! \brief Upon entry into the kernel, check to see if we are speculation mode.  If so, restart transaction 
 */
int Speculation_CheckJailViolation(Regs_t* context)
{
    HWThreadState_t *pHWT = GetMyHWThreadState();
    // Test to see of the source thread context that was originally in speculation has already been identified.
    // If it has, then the passed in context may not be for the proper thread. 
    if(!pHWT->pendingSpecRestartContext)
    {
        L2C_SPECID_t specid_self = SPEC_GetSpeculationIDSelf_priv();
        AppProcess_t* process = GetMyProcess();
        if((specid_self & 0xff) <= 0x01)
        {
            // We are non-spec, return indication that nothing has changed
            return 0;
        }
        // If SpecState was never set, transactional memory processing should not proceed.
        if(process->SpecState == NULL)
        {
           return 0;
        }
        if((specid_self & 0x80) == 0)
        {
            // We are already invalid, nothing to do here anymore, regular invalidation IRQ will clean up
            return 1;
        }
        specid_self &= 0x7f;  // SpecIDs saved in the L1p registers are offset by 0x80 or 0x180
        
        Speculation_Restart(specid_self, Kernel_SpecReturnCode_JMV, context);
        return 1;
    }
    return 0;
} 

void Speculation_CheckJailViolationDefer(Regs_t* context)
{
    HWThreadState_t *pHWT = GetMyHWThreadState();
    L2C_SPECID_t specid_self = SPEC_GetSpeculationIDSelf_priv();
    if(((specid_self & 0xff) <= 0x01) || // are we are non-spec
       (GetMyProcess()->SpecState == NULL) || // was spec state not set
       (pHWT->pendingSpecRestartContext)) // is there a pending restart already active
    {
        return;
    }
    pHWT->pendingSpecRestartContext = context;
    Kernel_WriteFlightLog(FLIGHTLOG_high, FL_SPCPNDRST, specid_self & 0x7f, 0,0,0);
} 

/*! \brief Upon return back to user state, check to see if we have a pending restart condition.  If so, restart transaction 
 */
int Speculation_CheckPendingRestart()
{
    HWThreadState_t *hwt = GetMyHWThreadState();
    Regs_t *context = hwt->pendingSpecRestartContext;
    if(context)
    {
        hwt->pendingSpecRestartContext = 0;
        L2C_SPECID_t specid_self = SPEC_GetSpeculationIDSelf_priv();
        AppProcess_t* process = GetMyProcess();
        if(((specid_self & 0xff) <= 0x01) ||   // are we non-spec
        (process->SpecState == NULL))  // was SpecState never set
        {
            return 0;
        }

        if ((specid_self & 0x80) == 0) // are we already invalid
        {
            return 1; // Want to not surface the exception as a signal, regular invalidation IRQ will clean up
        }
        specid_self &= 0x7f;  // SpecIDs saved in the L1p registers are offset by 0x80 or 0x180
        Speculation_Restart(specid_self, Kernel_SpecReturnCode_JMV, context);
        return 1;
    }
    return 0;
} 

void Speculation_SEAbortHandler(uint64_t lctn, uint64_t parm2)
{
    L2C_SPECID_t specid_self = SPEC_GetSpeculationIDSelf_priv();
    if((specid_self & 0x80) == 0)
    {
        // We are already invalid or committed, nothing to do here anymore
        return;
    }
    specid_self &= 0x7f;  // SpecIDs saved in the L1p registers are offset by 0x80 or 0x180
    
    Speculation_Restart(specid_self, Kernel_SpecReturnCode_SEConflictVictim, &GetMyKThread()->Reg_State);
}

/*! \brief Speculative Execution rollback 
 */
int Speculation_SERollback(L2C_SPECID_t specid)
{
    if(SIMPLE_ROLLBACK == 0)
    {
        AppProcess_t* process = GetMyProcess();
        int x;
        int numspecids  = 128/SPEC_GetNumberOfDomains();
        L2C_SPECID_t startspecid = (specid & ~(numspecids-1));
        
        uint64_t domain = (uint64_t)(startspecid / numspecids);
        L2C_SPECID_t allocateptr = SPEC_GetAllocPtr_priv(domain);
        int numyounger  = (int)((allocateptr - specid) & (numspecids-1));
        L2C_SPECID_t younger_specid;
        
        TRACE(TRACE_Spec, ("(I) SE event.  domain=%ld  AllocPtr=%ld  numspecids=%d\n", domain, allocateptr, numyounger));
        for(x=1; x<numyounger; x++)
        {
            younger_specid = ((specid + x - startspecid)&(numspecids-1)) + startspecid;
            TRACE( TRACE_Spec, ("(I) Sending IPI to younger thread %ld w/ specid %ld (timebase=%lx)\n", process->SpecState->id_state[younger_specid].hwthreadid, younger_specid, GetTimeBase()));
            
            IPI_DeliverInterrupt(process->SpecState->id_state[younger_specid].hwthreadid, Speculation_SEAbortHandler, 0, 0);
        }
    }
    
    // run time system releases the ID by clearing the A bit
    // SPEC_ClearConflictWithMask(specid, SPEC_GetConflict(specid));
    Speculation_Restart(specid, Kernel_SpecReturnCode_SEConflict, &GetMyKThread()->Reg_State);
    
    return 0;
}

int Speculation_EnterJailMode(bool longRunningSpec)
{
    AppProcess_t* process = GetMyProcess();
    assert(process != NULL);
    if (process != GetProcessByProcessorID(ProcessorID()))
    {
        Speculation_Restart(SPEC_GetSpeculationIDSelf_priv(), Kernel_SpecReturnCode_INVALID, &GetMyKThread()->Reg_State);
        return Kernel_SpecReturnCode_INVALID;
    }
    if(longRunningSpec)
    {
        uint64_t SpecPID;
        uint32_t ProcessOvercommit = 64 / GetMyAppState()->Active_Processes;
        if(ProcessOvercommit > 4) ProcessOvercommit = 4;
        vmm_getSpecPID(process->Tcoord, ProcessorThreadID() % ProcessOvercommit, &SpecPID);
        
        if(SpecPID)
        {
            mtspr(SPRN_PID, SpecPID);
            isync();
            
            // A2 does not reliably notify A2 of DCI
#if 0
            volatile uint64_t* pf_sys_p=(volatile uint64_t*)(SPEC_GetL1PBase_priv()+L1P_CFG_PF_SYS-L1P_ESR);
            uint64_t pf_sys=*pf_sys_p;
            *pf_sys_p=pf_sys | L1P_CFG_PF_SYS_pf_invalidate_all;
            *pf_sys_p=pf_sys & ~L1P_CFG_PF_SYS_pf_invalidate_all;
            dci();
#else
            asm volatile ("dci 2");
#endif
            ppc_msync();
        }
        else
        {
            Speculation_Restart(SPEC_GetSpeculationIDSelf_priv(), Kernel_SpecReturnCode_INVALID, &GetMyKThread()->Reg_State);
            return Kernel_SpecReturnCode_INVALID;
        }
    }
    return 0;
}

int Speculation_ExitJailMode()
{
    KThread_t* kthread = GetMyKThread();
    if((kthread->pAppProc != NULL) && (kthread->physical_pid != 0))
    {
        mtspr(SPRN_PID, kthread->physical_pid);
        isync();
    }
    return 0;
}

/*! \brief Interrupt handler for L2 central interrupts
 */
void IntHandler_L2Central(int intrp_sum_bitnum)
{
    TRACE( TRACE_Spec, ("(I) In IntHandler_L2Central. PUEA interrupt bit num %d (timebase=%lx)\n", intrp_sum_bitnum, GetTimeBase()));
    uint64_t CR_self;
    L2C_SPECID_t specid_self, specid_enemy, orig_specid_self;
    uint64_t priority_self, priority_enemy;
    Regs_t* reg = &GetMyKThread()->Reg_State;
    
    // Clear IRQ state, allowing us to catch the next event and avoid a race. Lower bits of the reg are not changed
    SPEC_SetSpeculationIDSelf_priv(0x600);
    
    // fast state/ID retrieval, works in most cases except providing ID when invalidated, strip off short running bit
    orig_specid_self = SPEC_GetSpeculationIDSelf_priv();
    specid_self = orig_specid_self & 0xff;
    Kernel_WriteFlightLog(FLIGHTLOG_high, FL_L2CENTINT, orig_specid_self, reg->ip, reg->lr, 0);
    
    // are we committed?
    if((specid_self <= 1) || (GetMyHWThreadState()->pendingSpecRestartContext))
    {
        // we are either committed, must have been a delayed interrupt, the end_tm/end_tls did our job already
        // \todo if we are rollback, then there is more todo, let's add this later
        Kernel_WriteFlightLog(FLIGHTLOG_high, FL_L2COMMIT_, specid_self, 0,0,0);
        return;
    }
    
    // We are either invalid or speculative. We do a big while(0) to allow easy "break" if invalidation is needed
    do{
        
        // Are we invalid?
        if(specid_self == 2)
        {
            // there is no conflict information available, so return INVALID and leave it up to the RTS
            CR_self = Kernel_SpecReturnCode_INVALID;
            // Exit to restore context
            break;
        }
        
        // We are assumed speculative and have the ID, let's try to survive
        specid_self &= 0x7f;  // SpecIDs saved in the L1p registers are offset by 0x80 or 0x180
        
        // if we are TLS, then we must have gotten a cross-domain conflict, caused by TM access. Just play by TM rules to resolve
        
        // now assuming TM, still speculative and conflict. Let's try to survive this conflict by being higher priority
        CR_self = SPEC_GetConflict_priv(specid_self);
        
        /* Check L2 Buffer overflow and others.  Restart transaction */
        if(CR_self & (L2C_CONFLICT_INVALIDATE |
                      L2C_CONFLICT_NONSPEC |
                      L2C_CONFLICT_MULTI))
        {
            // giving up on anything but single conflict
            // Rollback should never arrive here, any conflict is committing
            // Exit to restore context
            break;
        }
        
        // We do not even have a single conflict. Must have been a delayed interrupt, ignore.
        if((CR_self & L2C_CONFLICT_SINGLE) == 0)
        {
            Kernel_WriteFlightLog(FLIGHTLOG_high, FL_L2NOCNFLT, CR_self,0,0,0);
            return;
        }
        
        // Our enemy must have either only a single conflict with us or more conflicts. In both cases, it will not survive.
        // Unless our ID is recycled and gets a lower priority... we must avoid that case!!!!
        specid_enemy = (CR_self & L2C_CONFLICT_ID(-1));
        
        // Now fetch all the stuff we need for priority compares
        AppProcess_t* process = GetMyProcess();
        
        // If SpecState was never set, transactional memory processing should not proceed.
        if(process->SpecState == NULL)
        {
            // bailing out
            Kernel_WriteFlightLog(FLIGHTLOG_high, FL_L2NOSTRCT, (uint64_t)process->SpecState, 0,0,0);
            return;
        }    
        
        // Problem: if priorities of newly generated IDs grow, the chance of survival of older IDs will constantly diminish
        // making the protection via priority questionable
        // but with constantly decreasing priorities, reuse of an ID with a lower priority could cause the survival of an enemy ID that was assumed to die
        priority_self  = process->SpecState->id_state[specid_self ].priority_key;
        priority_enemy = process->SpecState->id_state[specid_enemy].priority_key;
        
        // If our priority is lesser (runtime sets priority to timebase, so we're an older thread), then we win, clear conflict and proceed
        if(priority_self > priority_enemy)
        {
            // Sorry, not a winner,
            // Rollback should never arrive here, any conflict is committing
            // Exit to restore context

            // TODO@mschaal: save enemies ID to notify it about my completion, bitstring?
            break;
        }
        
        // Our priority is greater, we win, clear conflict and survive!!!
        // Clear self conflict bit
        // SPEC_ClearConflictWithMask(specid_self, L2C_CONFLICT_SINGLE);
        // New approach: invalidate enemy directly, success only if enemy is still speculative
        // May cause a little havoc if enemy ID has been invalidated, reclaimed, reallocated and is again in use, but still functionally correct
        // Killing the other ID will clear the single conflict bit
        SPEC_TryChangeState_priv(specid_enemy, (4<<2) | L2C_IDSTATE_INVAL);
        // \todo Warning, we may drown in conflicts, other threads may continously respawn conflict, die, respawn...
        // yield after a few conflict survivals
        
        Kernel_WriteFlightLog(FLIGHTLOG_high, FL_L2PRTYWIN, specid_self, specid_enemy, priority_self, priority_enemy);
        return;
        
    }while(0);
    
    // This is the fall through case for invalidations
    // Now fetch all the stuff we need
    AppProcess_t* process = GetMyProcess();
    
    // If SpecState was never set, transactional memory processing should not proceed.
    // \todo returning as if IRQ never happened is not clean, thread may be in invalid state and hang. Better signal the app...
    if(process->SpecState == NULL)
    {
        Kernel_WriteFlightLog(FLIGHTLOG_high, FL_L2NOSTRCT, (uint64_t)process->SpecState, 0,0,0);
        return;
    }
    
    // Let's grab the ID from a safe place
    orig_specid_self = GetSpecID(process);
    specid_self = orig_specid_self & 0x7f;  // SpecIDs saved in the L1p registers are offset by 0x80 or 0x180
    
    // derive the domain number
    uint64_t n = specid_self >> 3;
    // bitreverse
    n = (n & 0x5) << 1 | (n & 0xA) >> 1;
    n = (n & 0x3) << 2 | (n & 0xC) >> 2;
    n &= SPEC_GetNumberOfDomains()-1;
    
    // It is really indexed by domain. 
    // the domain mode should be part of per hardware thread structure for fast retrieval, possibly even sharing a DW with the specid
    // going via GetDomainMode is just too slow, and the rest above adds up, too
    uint64_t domainmode = SPEC_GetDomainMode_priv(n) & L2C_DOMAINATTR_MODE(6);
    
    // Rollback should be invalidated under software control, no interrupt for that expected
    if((domainmode & (L2C_DOMAINATTR_MODE(-1) ^ L2C_DOMAINATTR_MODE(L2C_DOMAINATTR_MODE_LTLS ^ L2C_DOMAINATTR_MODE_STLS))) == 
       L2C_DOMAINATTR_MODE(L2C_DOMAINATTR_MODE_LTLS))
    {
        Speculation_SERollback(specid_self);
    }
    else
    {
        Speculation_Restart(specid_self, CR_self, &GetMyKThread()->Reg_State);
    }
    return;
}

/*! \brief Swap in speculation-optimized system-call and standard-interrupt
 *         code sequences.
 *
 * We enable the speculation-optimized paths by patching the exception
 * trampoline maintained by firmware in boot-edram.  This code modification
 * applies to the whole node and would normally have to be done very
 * carefully (with a rendezvous of all threads and coordinated icache-syncing).
 * In this case, however, we are writing just single instructions into two
 * unrelated paths, and both the old and new instructions are functionally
 * correct.  We can therefore dispense with the rendezvous and broadcast IPI's
 * to get all of the icaches flushed eventually.
 *
 * This interface is disallowed for sub-node jobs (see syscalls/spec.cc),
 * because the exception paths are global to the node.  For full-node jobs, the
 * system call needs to be made by at least one process, although there's no
 * harm in making the call multiple times.
 *
 * Because we're patching instructions in boot-edram, which may be cached in
 * L1 but is not backed by L2, we have to flush the whole icache on each core
 * with an ici.  More surgical icbi's could stumble over HW Issue 874 and
 * cause machine checks.
 */
int Speculation_EnableFastSpeculationPaths()
{
    uint64_t ici_needed = 0;

    if(GetMyHWThreadState()->PhysicalSpecPID == 0)
    {
        AppProcess_t *proc = GetMyProcess();
        if(proc != NULL)
        {
            // Send signal to this process.                                                                            
            Signal_Deliver(proc, 0, SIGNOSPECALIAS);
        }
        return ENOMEM;
    }
    
    // We use a lock to ensure that just one thread patches the exception
    // trampoline.  That thread then sends IPI's to all the other cores to
    // get the icaches flushed.
    Kernel_Lock(&FastPathsLock);
    if (!FastPathsEnabled)
    {
	extern uint32_t Vector_FastSpec_EI_trampoline;
	extern uint32_t Vector_FastSpec_SC_trampoline;

	uint64_t exceptionVector = mfspr(SPRN_IVPR);
	*((uint32_t *) (exceptionVector + IVO_EI)) =
			    Vector_FastSpec_EI_trampoline;
	*((uint32_t *) (exceptionVector + IVO_SC)) =
			    Vector_FastSpec_SC_trampoline;
	ppc_msync();  // make sure the stores have taken effect
	FastPathsEnabled = 1;
	ici_needed = 1;  // we can't hold the lock while sending IPI's
    }
    Kernel_Unlock(&FastPathsLock);

    if (ici_needed)
    {
	uint64_t core;
	for (core = 0; core < NodeState.NumCoresEnabled; core++)
	{
	    if (core == ProcessorCoreID())
	    {
		isync();
		ici();   // flush the local icache
	    }
	    else
	    {
		IPI_invalidate_icache(core);
	    }
	}
	Kernel_WriteFlightLog(FLIGHTLOG_high, FL_SPCFEPENA, 0,0,0,0);
    }
    return 0;
}
