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
/* (C) Copyright IBM Corp.  2009, 2012                              */
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
#ifndef _L1P_PERFECT_H_ // Prevent multiple inclusion.
#define _L1P_PERFECT_H_

#include <hwi/include/common/compiler_support.h>

__BEGIN_DECLS

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <spi/include/kernel/memory.h>
#include <spi/include/kernel/location.h>
#include <spi/include/l1p/types.h>
#include <hwi/include/bqc/l1p_mmio.h>
#include <hwi/include/bqc/A2_inlines.h>

extern L1P_PatternLimitPolicy_t _L1P_PatternLimitMode;
extern uint64_t                 _L1P_PatternLimit;
extern uint64_t                 _L1P_PatternCount;
extern L1P_SPIContext           _L1P_Context[68];

// sigaction flag specifying that CNK is to pause the list prefetcher before
// launching the signal handler and restore it afterwards
#define SA_BGQ_PP_PAUSE 0x00010000


/*!
  \brief 
  \internal
*/
__INLINE__ int L1P_SetupPattern(uint64_t n, L1P_Pattern_t* ptr)
{
    if(ptr == NULL)
        return L1P_PARMRANGE;
    
    ptr->size = n*L1P_MISSLISTENTRYSIZE + 63;
    if(ptr->size < 256) 
        ptr->size = 256;
    
    ptr->ReadPattern = malloc(ptr->size);
    if(ptr->ReadPattern == NULL)
        return L1P_NOMEMORY;
    
    ptr->WritePattern = malloc(ptr->size);
    if(ptr->WritePattern == NULL)
        return L1P_NOMEMORY;
    memset(ptr->ReadPattern, 0xff, ptr->size);  // Add end_of_list marker
    memset(ptr->WritePattern, 0xff, ptr->size);  // Add end_of_list marker
    return 0;
}

/*!
  \brief Destroys pattern storage
  \internal
*/
__INLINE__ int L1P_DestroyPattern(L1P_Pattern_t* ptr)
{
    if(ptr == NULL)
        return L1P_PARMRANGE;
    
    if(ptr->ReadPattern != NULL)
        free(ptr->ReadPattern);
    if(ptr->WritePattern != NULL)
        free(ptr->WritePattern);
    
    ptr->ReadPattern  = NULL;
    ptr->WritePattern = NULL;
    ptr->size         = 0;
    return 0;
}

/*!
  \brief Allocates storage to hold an L1p pattern of L1 miss addresses.  This allows for the application to allocate storage for uninitialized patterns.  This pattern storage can be passed to L1P_SetPattern().  Storage must be deallocated using L1P_DeallocatePattern().
*/
__INLINE__ int L1P_AllocatePattern(uint64_t n, L1P_Pattern_t** handle)
{
    int rc = 0;
    L1P_Pattern_t* ptr;
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    uint64_t pcount;
    if(handle == NULL)
        return L1P_NOMEMORY;
    
    pcount = Fetch_and_Add(&_L1P_PatternCount, 1);
    if(_L1P_PatternLimit > pcount)
    {
        switch(_L1P_PatternLimitMode)
        {
            case L1P_PatternLimit_Error:
                return L1P_NOMEMORY;
            case L1P_PatternLimit_Assert:
                assert(_L1P_PatternLimit > pcount);
                break;
            case L1P_PatternLimit_Prune:
                context->ignorePatternActions = 1;
                break;
            default:
            case L1P_PatternLimit_Disable:
                // do nothing
                break;
        }
    }
    
    ptr = *handle = (L1P_Pattern_t*)malloc(sizeof(L1P_Pattern_t));
    if(ptr == NULL)
        return L1P_PARMRANGE;
    
    rc = L1P_SetupPattern(n, ptr);
    return rc;
}


/*!
  \brief Deallocates storage previously assigned to the list of addresses
  This allows for the application to deallocate storage for patterns that have been detached from normal L1p SPI control.
*/
__INLINE__ int L1P_DeallocatePattern(L1P_Pattern_t* ptr)
{
    int rc = 0;
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    if(ptr == NULL)
        return L1P_PARMRANGE;
    
    if((ptr->ReadPattern == context->currentPattern.ReadPattern) ||
       (ptr->WritePattern == context->currentPattern.WritePattern))
        return L1P_PATTERNACTIVE;
    
    rc = L1P_DestroyPattern(ptr);    
    if(rc)
        return rc;
    
    free(ptr);
    Fetch_and_Add(&_L1P_PatternCount, -1);
    return 0;
}

/*!
  \brief Sets the perfect prefetcher's hardware registers with a given pattern.  
  This allows for retaining several patterns of memory accesses and finer control of the L1p.  It is not required for the default usage model. 
  The L1p SPI will not deallocate the structure. 
*/
__INLINE__ int L1P_SetPattern(L1P_Pattern_t* ptr)
{
    int rc = 0;
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    if(context->ignorePatternActions) 
        return 0;
    
    if(ptr != &context->currentPattern)
    {
        if(!context->valid)
            return L1P_NOTCONFIGURED;
        if(context->implicitPatternAllocate)
        {
            rc = Kernel_L1pSetPatternAddress(NULL, NULL, 0);
            if(rc)
                return rc;
            
            rc = L1P_DestroyPattern(&context->currentPattern);
            if(rc)
                return rc;
        }
        if(ptr != NULL)
        {
            memcpy(&context->currentPattern, ptr, sizeof(L1P_Pattern_t));
            context->implicitPatternAllocate = 0;
        }
        else
        {
            L1P_SetupPattern(1024, &context->currentPattern);
            context->implicitPatternAllocate = 1;
        }
    }
    
    *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_CTRL))   = L1P_PP_CTRL_DEPTH(7) | L1P_PP_CTRL_MAXLIST(7);
    *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_MAXTOL)) = 0xffffffff;
    
    rc = Kernel_L1pSetPatternAddress(context->currentPattern.ReadPattern, context->currentPattern.WritePattern, context->currentPattern.size);
    ppc_msync();
    return rc;
}


/*!
  \brief Returns pointers to the current L1p pattern.  
  
  Later, the pattern pointer can then be passed back into L1P_SetPattern().  
  Once L1P_GetPattern is called, the application will now own the pattern and must call L1P_DeallocatePattern() to reclaim that storage.  
  This allows pattern storage allocated via L1P_PatternConfigure() to be detached and retained for later usage.  
  This allows for retaining several patterns of memory accesses and finer control of the L1p.  It is not required for the default usage model.
*/
__INLINE__ int L1P_GetPattern(L1P_Pattern_t** handle)
{
    L1P_Pattern_t* ptr;
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    if(!context->valid)
        return L1P_NOTCONFIGURED;
    if(handle == NULL)
        return L1P_PARMRANGE;
    
    ptr = *handle = (L1P_Pattern_t*)malloc(sizeof(L1P_Pattern_t));
    if(ptr == NULL)
        return L1P_NOMEMORY;
    
    if(ptr != &context->currentPattern)
    {
        memcpy(ptr, &context->currentPattern, sizeof(L1P_Pattern_t));
        context->implicitPatternAllocate = 0;
    }
    
    return 0;
}


/*!
 \brief Allocates enough storage so that the perfect prefetcher can track up to <n> L1 misses.  
 \param[in] n The maximum number of L1 misses that can be tracked by the list.
 
 Storage is retained until:
 1)	L1P_Unconfigure() is performed.
 2)	L1P_SetPattern() is performed.
 
 If  the L1P_Configure() command is nested: 
 *	If nesting mode has been set to L1P_NestingSaveContext then the L1P SPI will push a L1P context structure onto a stack of L1P context structures.  
                When an L1P_Unconfigure() is called, this L1P context structure will be restored.  This is the default mode.
 *	If nesting mode has been set to L1P_NestingIgnore, then the L1P SPI will reference count the L1P_Configures.  When nested, the SPI will not write 
                new pattern addresses into the L1p hardware.  Once the same number of L1P_Unconfigure() routines have been called, the L1P SPI will return to normal function.  
 *	If the nesting node has been set to L1P_NestingFlat, then the L1P SPI will be reference count and ignore the L1P_Configures.  Any L1P_SetPattern() 
                calls will be ignored if they occur in a nested context.
 *	If nesting mode has been set to L1P_NestingError, the L1P SPI will display an error message and assert.  This will terminate the active process 
                with a corefile.  This mode is to be used for debug purposes.  
 */
__INLINE__ int L1P_PatternConfigure(uint64_t n)
{
    int rc = 0;
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    L1P_SPIContext* tmp;
    
    // check for nested configure:
    assert(! (context->valid && (context->nestPolicy == L1P_NestingError)));
    if(context->valid)
    {
        switch(context->nestPolicy)
        {
            case L1P_NestingFlat:
            case L1P_NestingIgnore:
                context->nestDepth++;
                if(context->nestPolicy == L1P_NestingIgnore)
                    context->ignorePatternActions = 1;
                return 0;
            default:
            case L1P_NestingSaveContext:
                tmp = (L1P_SPIContext*)malloc(sizeof(L1P_SPIContext));
                if(tmp == NULL)
                    return L1P_NOMEMORY;
                memcpy(tmp, context, sizeof(L1P_SPIContext));
                context->nestedContext = tmp;
                break;
        }
    }
    
    // initialize pattern:
    context->nestDepth = 0;
    context->ignorePatternActions = 0;
    
    rc = L1P_SetupPattern(n, &context->currentPattern);
    context->implicitPatternAllocate = 1;
    if(rc != 0)
        return rc;
    
    rc = L1P_SetPattern(&context->currentPattern);    
    if(rc != 0)
        return rc;
    
    context->valid = 1;
    
    return rc;
}

/*!
 \brief Deallocates storage used by the L1p SPI.
 If one is available, the L1P SPI will pop a L1P context structure from the stack of L1P context structures.  The context will then be used to restore the previous L1P pattern status and pointers.  
 */
__INLINE__ int L1P_PatternUnconfigure()
{
    int rc = 0;
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    L1P_SPIContext* tmp;
    if(!context->valid)
        return L1P_NOTCONFIGURED;
    
    if(context->nestDepth > 0)
    {
        context->nestDepth--;
        if(context->nestDepth == 0)
            context->ignorePatternActions = 0;
        return 0;
    }
    
    if(context->implicitPatternAllocate)
    {
        rc = Kernel_L1pSetPatternAddress(NULL, NULL, 0);
        if(rc) 
            return rc;
        
        rc = L1P_DestroyPattern(&context->currentPattern);
        if(rc)
            return rc;
        
        context->implicitPatternAllocate = 0;
    }
    
    if(context->nestedContext)
    {
        tmp = (L1P_SPIContext*)context->nestedContext;
        if(tmp == NULL)
            return L1P_NOMEMORY;
        memcpy(context, tmp, sizeof(L1P_SPIContext));
        free(tmp);
        rc = L1P_SetPattern(&context->currentPattern);
        if(rc)
            return rc;
    }
    else
    {
        context->valid = 0;
    }
    return rc;
}

/*!
 \brief The perfect prefetcher will start monitoring L1 misses and performing prefetch requests based on those misses.  
 
 \param[in] record Instructs the PatternStart to record the pattern of L1 misses for the next iteration.  
 This L1P_PatternStart() should be called at the beginning of every entrance into the section of code that has been recorded.  
 */
__INLINE__ int L1P_PatternStart(int record)
{
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    if(!context->valid) 
        return L1P_NOTCONFIGURED;
    
    if(context->ignorePatternActions) 
        return 0;
#if DD1_L1P_Workaround
    int offset;
    uint32_t freevalue;
    if(record)
    {
        offset = Kernel_ProcessorID();
        freevalue = 0;
        while(1)
        {
            if(Compare_and_Swap32(&_L1P_Context[offset].criticalatom, &freevalue, offset+1))
            {
                break;
            }
            ThreadPriority_Low();
            while (_L1P_Context[offset].criticalatom) { }
            ThreadPriority_Medium(); // Use high priority while we are holding the kernel lock                                                                                         
            freevalue = 0;
        }
        ppc_msync();
    }
#endif

  *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_STATUS)) = 0xf;
  *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_CTRL)) |= L1P_PP_CTRL_START | L1P_PP_CTRL_LOAD | ((record == 0)?L1P_PP_CTRL_INHIBIT:0);

#if DD1_L1P_Workaround
  if(record)
  {
      _L1P_Context[offset].criticalatom = 0;
      ppc_msync();
  }
#endif
  return 0;
}

/*!
 \brief Suspends the active perfect prefetcher.  
 The Linear Stream Prefetcher and the other 3 Perfect Prefetchers on the core will continue to execute.
 This routine can be used in conjunction with L1P_PatternResume() to avoid recording out-of-bound memory fetches - - such as instructions performing a periodic printf.    
 It can also be used to avoid sections of code that perform memory accesses that are inconsistent between iterations.  
 */
__INLINE__ int L1P_PatternPause()
{
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    if(!context->valid) 
        return L1P_NOTCONFIGURED;
    if(context->ignorePatternActions) return 0;
    *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_CTRL)) |= L1P_PP_CTRL_PAUSE;
    return 0;
}

/*! 
 \brief Resumes the perfect prefetcher from the last pattern offset location.  
 This routine can be used in conjunction with L1P_PatternPause() to avoid recording memory fetches that are not likely to repeat - - such as instructions 
 performing a periodic printf.    It can also be used to avoid sections of code that perform memory accesses that are inconsistent between iterations
*/
__INLINE__ int L1P_PatternResume()
{
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    if(!context->valid) 
        return L1P_NOTCONFIGURED;
    if(context->ignorePatternActions) return 0;
    *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_CTRL)) &= ~L1P_PP_CTRL_PAUSE;
    return 0;
}

/*!
 \brief Stops the perfect prefetcher and resets the list offsets to zero
 */
__INLINE__ int L1P_PatternStop()
{    
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    if(!context->valid) 
        return L1P_NOTCONFIGURED;
    if(context->ignorePatternActions) return 0;
    uint64_t ctrl = *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_CTRL));
#if DD1_L1P_Workaround
    int x;
    int offset = Kernel_ProcessorCoreID()*4;
    uint32_t freevalue = 0;
    uint64_t my_unique_index = Kernel_ProcessorID() + 1;

    // acquire all locks in this core
    for(x=0; x<4; x++)
    {
        while(1)
        {
            if(Compare_and_Swap32(&_L1P_Context[x + offset].criticalatom, &freevalue, my_unique_index))
            {
                break;
            }
            ThreadPriority_Low();
            while (_L1P_Context[x + offset].criticalatom) { }
            ThreadPriority_Medium(); // Use high priority while we are holding the kernel lock                                                                
            freevalue = 0;
        }
    }
    
    // check the recording state of each thread.  
    uint64_t record_status=0;
    for(x=0; x<4; x++) 
        record_status += (*((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * x + L1P_PP_CTRL))) & L1P_PP_CTRL_INHIBIT;
    
    // if any threads are recording, pause them.
    if(record_status < 4*L1P_PP_CTRL_INHIBIT) 
    {
        for(x=0; x<4; x++)
        {
            *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * x + L1P_PP_CTRL)) |= L1P_PP_CTRL_PAUSE;
        }
    }
    
    ppc_msync();
#endif
    *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_CTRL)) = ctrl & ~(L1P_PP_CTRL_START | L1P_PP_CTRL_LOAD | L1P_PP_CTRL_INHIBIT);
    
    // poll the list
    // finish & abandon & max_length_reach & end_of_list
    uint64_t list_status=0;
    while( (list_status & L1P_PP_STATUS_FINISHED) == 0 ) 
    {
        list_status = in64((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() +  L1P_PP_STATUS));
    }
#if DD1_L1P_Workaround
    if(record_status < 4*L1P_PP_CTRL_INHIBIT)
    {
        for(x=0; x<4; x++)
        {
            *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * x + L1P_PP_CTRL)) &= (~L1P_PP_CTRL_PAUSE);
        }
        ppc_msync();
    }
    for(x=0; x<4; x++)
    {
        // Consider checking that we own this lock before resetting it. This test may not be necessary if we can trust ourself!                                 
        // reset the lock                                                                                                                                       
        _L1P_Context[x + offset].criticalatom = 0;
        // Move our priority back down to normal. We were running with a high priority while holding the kernel lock                                            
        ThreadPriority_Medium(); // Use high priority while we are holding the kernel lock                                                                      
    }
    ppc_msync();
#endif

    if((ctrl & L1P_PP_CTRL_INHIBIT) == 0)
    {
        L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
        // swap generate and record lists to setup for next iteration
        
        void* tmp = context->currentPattern.ReadPattern;
        context->currentPattern.ReadPattern = context->currentPattern.WritePattern;
        context->currentPattern.WritePattern = tmp;
        Kernel_L1pSetPatternAddress(context->currentPattern.ReadPattern, context->currentPattern.WritePattern, context->currentPattern.size);
    }
    return 0;
}

/*!
 \brief Returns the current status for the L1 perfect prefetcher
 \param[out] status The current L1p status will be placed at this memory location.
 */
__INLINE__ int L1P_PatternStatus(L1P_Status_t* status)
{
    status->status = *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_STATUS));
    return 0;
}

/*! 
 \brief Returns the current pattern depths for the L1 perfect prefetcher.  
 The pattern depth is the current index into the pattern that the L1p is executing.  
 \param[out] fetch_depth Used to determine how far in the current pattern/sequence the L1p has progressed.  
 \param[out] generate_depth Used to optimize the pattern length parameter to L1P_PatternConfigure() in order to reduce the memory footprint of the L1p pattern
 */
__INLINE__ int L1P_PatternGetCurrentDepth(uint64_t* fetch_depth, uint64_t* generate_depth)
{
    int rc;
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    void* curread;
    void* curwrite;
    rc = Kernel_L1pGetCurrentPatternAddress(&curread, &curwrite);
    if(rc) return rc;
    *fetch_depth    = (uint64_t)curread - (uint64_t)context->currentPattern.ReadPattern;
    *generate_depth = (uint64_t)curwrite - (uint64_t)context->currentPattern.WritePattern;
    return 0;
}

/*!
 \brief Returns the current nesting mode for the L1 perfect prefetcher.
 \see L1P_PatternNest_t
 \see L1P_PatternSetNestingMode
 */
__INLINE__ int L1P_PatternGetNestingMode(L1P_PatternNest_t* mode)
{
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    *mode = context->nestPolicy;
    return 0;
}

/*!
 \brief Sets the nesting mode for the L1 perfect prefetcher.
 \see L1P_PatternNest_t
 \see L1P_PatternSetNestingMode
 */
__INLINE__ int L1P_PatternSetNestingMode(L1P_PatternNest_t mode)
{
    // \todo validate mode
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    context->nestPolicy = mode;
    return 0;
}

/*!
 \brief Sets the number of consecutive L1 misses that did not match the current location in the pattern.  
 Once this number has been exceeded, the prefetching activity will cease and the pattern will be marked as "Abandoned" in the L1P_Status_t structure returned by L1P_PatternStatus().
 */
__INLINE__ int L1P_PatternSetAbandonThreshold(uint64_t numL1misses)
{
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    if(context->ignorePatternActions) return 0;
    *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_MAXTOL)) = numL1misses;
    return 0;
}

/*!
 \brief Returns the number of consecutive L1 misses that did not match the current location in the pattern.  
 Once this number has been exceeded, the prefetching activity will cease and the pattern will be marked as "Abandoned" in the L1P_Status_t structure returned by L1P_PatternStatus().  
 */
__INLINE__ int L1P_PatternGetAbandonThreshold(uint64_t* numL1misses)
{
    *numL1misses = *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID() + L1P_PP_MAXTOL));
    return 0;
}
 
/*!
 \brief Sets a software enable/disable for L1p perfect prefetecher.  
 This can be used to ascertain whether the usage of the prefetcher is improving performance
 */
__INLINE__ int L1P_PatternSetEnable(int enable)
{
    L1P_SPIContext* context = &_L1P_Context[Kernel_ProcessorID()];
    if(context->ignorePatternActions) return 0;
    
    uint64_t l1p_cfg_pf_usr = *((uint64_t*)(L1P_CFG_PF_USR));
    if(enable)
        l1p_cfg_pf_usr |= L1P_CFG_PF_USR_pf_list_enable;
    else
        l1p_cfg_pf_usr &= ~L1P_CFG_PF_USR_pf_list_enable;
    *((uint64_t*)(L1P_CFG_PF_USR)) = l1p_cfg_pf_usr;
    return 0;
}

/*!
 \brief Returns the software enable/disable for L1p perfect prefetcher
 */
__INLINE__ int L1P_PatternGetEnable(int* enable)
{
    uint64_t l1p_cfg_pf_usr = *((uint64_t*)(L1P_CFG_PF_USR));
    if((l1p_cfg_pf_usr & L1P_CFG_PF_USR_pf_list_enable) != 0)
        *enable = 1;
    else
        *enable = 0;
    return 0;
}


/*!
  \brief Sets behavior when the number of allocated patterns that the application can have active exceeds an artifical limit.  
  This can be used to determine if there is a memory leak in the pattern allocations.  
*/
__INLINE__ int L1P_PatternSetPatternLimit(L1P_PatternLimitPolicy_t policy, uint64_t numactivepatterns)
{
    _L1P_PatternLimitMode = policy;
    _L1P_PatternLimit     = numactivepatterns;
    return 0;
}


/*!
  \brief Returns the current behavior when the number of allocated patterns that the application can have active exceeds that limit.  The current limit is also returned.
*/
__INLINE__ int L1P_PatternGetPatternLimit(L1P_PatternLimitPolicy_t* policy, uint64_t* numactivepatterns)
{
    if((policy == NULL) || (numactivepatterns == NULL))
        return L1P_PARMRANGE;
    
    *policy            = _L1P_PatternLimitMode;
    *numactivepatterns = _L1P_PatternLimit;
    return 0;
}

__END_DECLS

#endif // Add nothing below this line.
