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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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


// Includes

#include "Kernel.h"
#include "hwi/include/bqc/l2_central_inlines.h"
#include "hwi/include/bqc/l2_dcr.h"

uint64_t  sc_SETSPECCONTEXT(SYSCALL_FCN_ARGS)
{
    TRACESYSCALL(("(I) %s[%d]: returning ENOSYS.\n",
                  __func__, ProcessorID() ));

    // SetSpecContext() is obsolete.
    return CNK_RC_SPI(ENOSYS);
}

uint64_t  sc_SETSPECTHREADMAP(SYSCALL_FCN_ARGS)
{
   TRACESYSCALL(("(I) %s[%d]: returning ENOSYS.\n",
                 __func__, ProcessorID() ));

    // SetSpecThreadMap() is obsolete.
    return CNK_RC_SPI(ENOSYS);
}

uint64_t sc_SETSPECSTATE(SYSCALL_FCN_ARGS)
{
   size_t             specstatesize = (size_t) r3;
   const SpecState_t* specstatebase = (const SpecState_t *) r4;

   TRACESYSCALL(("(I) %s[%d]: SpecState address 0x%lx, size 0x%lx.\n",
                 __func__, ProcessorID(), specstatebase, specstatesize ));
   
   if(specstatesize != sizeof(SpecState_t))
   {
       return CNK_RC_SPI(EINVAL);
   }
   if (!VMM_IsAppAddress(specstatebase, specstatesize))
   {
       return CNK_RC_SPI(EFAULT);
   }

   AppProcess_t* process = GetMyProcess();
   
   process->SpecState = specstatebase;

    // Cache the address in all of this process's HWThreadState structures
    // for the speculation-optimized standard-interrupt path.
    uint64_t hwtmask = process->HwthreadMask;
    for (int i = cntlz64(hwtmask); (hwtmask != 0); i++)
    {
        uint64_t curmask = _BN(i);
        if (hwtmask & curmask)
        {
	    HWThreadState_t *hwt = GetHWThreadStateByProcessorID(i);
	    hwt->SpecStateAddr = (uint64_t) specstatebase;
            hwtmask &= ~curmask;
        }
    }
   
    return CNK_RC_SPI(0);
}

uint64_t  sc_ENTERJAILMODE(SYSCALL_FCN_ARGS)
{
    TRACESYSCALL(("(I) %s[%d].\n",
                  __func__, ProcessorID() ));
    
    if(GetMyAppState()->Active_Processes == 64)
        return CNK_RC_SPI(ENOMEM);
    
    int rc;
    rc = Speculation_EnterJailMode(r3);
    
    return CNK_RC_SPI(rc);
}

uint64_t  sc_EXITJAILMODE(SYSCALL_FCN_ARGS)
{
    TRACESYSCALL(("(I) %s[%d].\n",
                  __func__, ProcessorID() ));
    
    if(GetMyAppState()->Active_Processes == 64)
        return CNK_RC_SPI(ENOMEM);
    
    int rc;
    rc = Speculation_ExitJailMode();
    
    return CNK_RC_SPI(rc);    
}

uint64_t sc_SPECALLOCATEDOMAIN(SYSCALL_FCN_ARGS)
{
    int rc;
    TRACESYSCALL(("(I) %s[%d].\n",
                  __func__, ProcessorID() ));
    
    unsigned int* domain = (unsigned int*)r3;
    if ( !VMM_IsAppAddress(domain, sizeof(unsigned int)))
    {
        return CNK_RC_SPI(EFAULT);
    }
    
    rc = Speculation_AllocateDomain(domain);
    return CNK_RC_SPI(rc);
}

uint64_t sc_GETNUMKERNELSPECDOMAINS(SYSCALL_FCN_ARGS)
{
    uint32_t* domaincount = (uint32_t*)r3;
    if ( !VMM_IsAppAddress(domaincount, sizeof(unsigned int)))
    {
        return CNK_RC_SPI(EFAULT);
    }
    
    *domaincount = 0;
    return CNK_RC_SPI(0);
}

uint64_t sc_GETNUMSPECDOMAINS(SYSCALL_FCN_ARGS)
{
    uint32_t* domaincount = (uint32_t*)r3;
    if ( !VMM_IsAppAddress(domaincount, sizeof(unsigned int)))
    {
        printf("efault!\n");
        return CNK_RC_SPI(EFAULT);
    }
    
    uint32_t count;
    Speculation_GetAllocatedDomainCount(&count);
    if(count == 0)
    {
        *domaincount = 0;
        return CNK_RC_SPI(0);
    }
    uint32_t power2 = 31-cntlz32(count);
    if((1UL << power2) != count)
    {
        count = (1 << (power2+1));
    }
    *domaincount = count;
    return CNK_RC_SPI(0);
}

uint64_t sc_SETNUMSPECDOMAINS(SYSCALL_FCN_ARGS)
{
    uint32_t domaincount = (uint32_t)r3;
    
    if((domaincount == 0) || (domaincount > 16))
    {
        return CNK_RC_SPI(EINVAL);
    }

    uint32_t dc;
    Speculation_GetAllocatedDomainCount(&dc);
    if(domaincount!=dc){
        Speculation_SetAllocatedDomainCount(domaincount);
        uint32_t power2 = 31-cntlz32(domaincount);
        if((1UL << power2) != domaincount)
        {
            domaincount = (1 << (power2+1));
        }
        SPEC_SetNumberOfDomains(domaincount);
    }
    return CNK_RC_SPI(0);
}

uint64_t sc_SPECSETDOMAINMODE(SYSCALL_FCN_ARGS)
{
    TRACESYSCALL(("(I) %s[%d].\n",
                  __func__, ProcessorID() ));
    
    unsigned int domain = r3;
    uint64_t domainmode = r4;
    
    if(domain >= NodeState.NumSpecDomains)
    {
        return CNK_RC_SPI(EINVAL);
    }
    SPEC_SetDomainMode(domain, domainmode);
    
    return CNK_RC_SPI(0);
}

uint64_t sc_ROLLBACKINDICATOR(SYSCALL_FCN_ARGS)
{
    uint64_t* indicator = (uint64_t*)r3;
    NodeState.RollbackIndicator = indicator;
    return CNK_RC_SPI(0);
}

uint64_t sc_SETL2SCRUBRATE(SYSCALL_FCN_ARGS)
{
    uint64_t scrub_rate = r3;
    if((scrub_rate<6) || (scrub_rate > 4096)) 
        return CNK_RC_SPI(EINVAL);
    
    for(int slice=0; slice<L2_DCR_num; slice++)
    {
        // Set the L2 scrub rate
        uint64_t l2_dcr_refctrl = DCRReadPriv(L2_DCR(slice, REFCTRL));
        L2_DCR__REFCTRL__SCB_INTERVAL_insert(l2_dcr_refctrl, scrub_rate-1);
        DCRWritePriv(L2_DCR(slice, REFCTRL), l2_dcr_refctrl);
    }
    return CNK_RC_SPI(0);
}

uint64_t sc_CLEARSPECCONFLICTINFO(SYSCALL_FCN_ARGS)
{
    int slice = (int)r3;
    
    if(slice >= L2_DCR_num)
    {
        return CNK_RC_SPI(EINVAL);
    }
    DCRWritePriv(_DCR_REG__INDEX(L2_DCR, slice, CONFL_STAT), 0);
    return CNK_RC_SPI(0);
}

uint64_t sc_GETSPECCONFLICTINFO(SYSCALL_FCN_ARGS)
{
    int slice;
    size_t infoSize = (size_t)r3;
    SpecConflictInfo_t* info = (SpecConflictInfo_t*)r4;
    AppProcess_t* proc = GetMyProcess();
    
    if(infoSize != sizeof(SpecConflictInfo_t))
        return CNK_RC_SPI(EINVAL);
    
    if(!VMM_IsAppAddress(info, sizeof(SpecConflictInfo_t)))
    {
        return CNK_RC_SPI(EFAULT);
    }
    
    for(slice=0; slice<L2_DCR_num; slice++)
    {
        uint64_t r=DCRReadPriv(_DCR_REG__INDEX(L2_DCR, slice, CONFL_STAT));
        info->l2slice[slice].was_conflictCaptured     = _GN(r, 0);
        
        if(info->l2slice[slice].was_conflictCaptured)
        {
            info->l2slice[slice].was_writeAfterRead       = _GN(r, 15);
            info->l2slice[slice].was_nonSpeculativeAccess = _GN(r, 16);
            info->l2slice[slice].specid_original          = _G7(r, 23);
            info->l2slice[slice].specid_violating         = _G7(r, 30);
            
            bool     found_vaddr = false;
            uint64_t paddr = _G33(r, 63) << 3;
            uint64_t vaddr = 0;

#define INRANGE(pabase, start, end, startp) if(((uint64_t)pabase>=proc->startp) && ((uint64_t)pabase<=proc->startp + (proc->end - proc->start))) { vaddr = pabase + proc->start - proc->startp; found_vaddr = true; }
            INRANGE(paddr, Text_VStart,   Text_VEnd,   Text_PStart);
            INRANGE(paddr, Data_VStart,   Data_VEnd,   Data_PStart);
            INRANGE(paddr, Heap_VStart,   Heap_VEnd,   Heap_PStart);
            INRANGE(paddr, Shared_VStart, Shared_VEnd, Shared_PStart);
            
            info->l2slice[slice].conflict_paddress = paddr;
            info->l2slice[slice].conflict_vaddress_valid = found_vaddr;
            info->l2slice[slice].conflict_vaddress = vaddr;
        }
    }
    return CNK_RC_SPI(0);
}

uint64_t sc_ENABLEFASTSPECULATIONPATHS(SYSCALL_FCN_ARGS)
{
    if (IsSubNodeJob())
    {
	// The exception paths are global for the whole node, so this system
	// call is not allowed for sub-node jobs.
	return CNK_RC_SPI(ENOSYS);
    }
    AppProcess_t *proc = GetMyProcess();
    if (proc->ThreadModel == CONFIG_THREAD_MODEL_ETA)
    {
        // The fast path depends on cached data in the hardware thread state object that may not
        // be valid on a thread running on a hardware thread that belongs to a different process.
        return CNK_RC_SPI(ENOSYS);
    }
    Speculation_EnableFastSpeculationPaths();
    return CNK_RC_SPI(0);
}

uint64_t sc_GETSPECIDSELF(SYSCALL_FCN_ARGS)
{
    uint64_t* specid = (uint64_t*)r3;
    if(!VMM_IsAppAddress(specid, sizeof(uint64_t)))
    {
        return CNK_RC_SPI(EFAULT);
    }
    
    *specid = SPEC_GetSpeculationIDSelf_priv();
    return CNK_RC_SPI(0);
}
