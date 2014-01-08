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
#define __FWEXT__
#include "spi/include/kernel/memory.h"

uint64_t  sc_SETL1PPATTERNADDR(SYSCALL_FCN_ARGS)
{
    void* readptr  = (void*)r3;
    void* writeptr = (void*)r4;
    size_t size    = (size_t)r5;
    void* pa_readp   = NULL;
    void* pa_writep  = NULL;
    
    if((readptr != NULL) || (writeptr != NULL) || (size != 0))
    {
    if( !VMM_IsAppAddress(readptr, size) )
    {
	return CNK_RC_SPI(EFAULT);
    }
    if( !VMM_IsAppAddress(writeptr, size) )
    {
	return CNK_RC_SPI(EFAULT);
    }
    AppProcess_t* proc = GetMyProcess();
    int okay;

#define INRANGE(vabase, pa, start, end, startp) if(((uint64_t)vabase>=proc->start) && ((uint64_t)vabase<=proc->end)) {  pa = (void*)(proc->startp + ((uint64_t)vabase - proc->start)); okay = 1; }
    
    okay = 0;
    INRANGE(readptr, pa_readp, Text_VStart, Text_VEnd, Text_PStart);
    INRANGE(readptr, pa_readp, Data_VStart, Data_VEnd, Data_PStart);
    INRANGE(readptr, pa_readp, Heap_VStart, Heap_VEnd, Heap_PStart);
    INRANGE(readptr, pa_readp, Shared_VStart, Shared_VEnd, Shared_PStart);
    if (!okay)
    {
	return CNK_RC_SPI(EFAULT);
    }
    
    okay = 0;
    INRANGE(writeptr, pa_writep, Text_VStart, Text_VEnd, Text_PStart);
    INRANGE(writeptr, pa_writep, Data_VStart, Data_VEnd, Data_PStart);
    INRANGE(writeptr, pa_writep, Heap_VStart, Heap_VEnd, Heap_PStart);
    INRANGE(writeptr, pa_writep, Shared_VStart, Shared_VEnd, Shared_PStart);    
    if (!okay)
    {
	return CNK_RC_SPI(EFAULT);
    }    
#undef INRANGE
    }
    
    //printf("set pattern address 0x%p  0x%p\n", pa_readp, pa_writep);
    uint64_t rc = Kernel_L1pSetPatternAddress(pa_readp, pa_writep, size);    
    return CNK_RC_SPI(rc);
}


uint64_t  sc_GETL1PCURPATTERNADDR(SYSCALL_FCN_ARGS)
{
    void** readptr = (void**)r3;
    void** writeptr= (void**)r4;    
    void* preadptr;
    void* pwriteptr;
    AppProcess_t* proc = GetMyProcess();
    uint64_t ptr = 0;
    if( !VMM_IsAppAddress(readptr, sizeof(void*)) )
    {
	return CNK_RC_SPI(EFAULT);
    }
    if( !VMM_IsAppAddress(writeptr, sizeof(void*)) )
    {
	return CNK_RC_SPI(EFAULT);
    }
    uint64_t rc = Kernel_L1pGetCurrentPatternAddress(&preadptr, &pwriteptr);
    if(rc)
    {
	return CNK_RC_SPI(EINVAL);
    }
    
    int okay;
#define INRANGE(pabase, start, end, startp) if(((uint64_t)pabase>=proc->startp) && ((uint64_t)pabase<=proc->startp + (proc->end - proc->start))) { okay = 1; ptr = (uint64_t)pabase - proc->startp + proc->start; }
    
    okay = 0;
    INRANGE(preadptr, Text_VStart, Text_VEnd, Text_PStart);
    INRANGE(preadptr, Data_VStart, Data_VEnd, Data_PStart);
    INRANGE(preadptr, Heap_VStart, Heap_VEnd, Heap_PStart);
    INRANGE(preadptr, Shared_VStart, Shared_VEnd, Shared_PStart);
    if (!okay)
    {
	return CNK_RC_SPI(EINVAL);
    }
    *readptr = (void*)ptr;
    
    okay = 0;
    INRANGE(pwriteptr, Text_VStart, Text_VEnd, Text_PStart);
    INRANGE(pwriteptr, Data_VStart, Data_VEnd, Data_PStart);
    INRANGE(pwriteptr, Heap_VStart, Heap_VEnd, Heap_PStart);
    INRANGE(pwriteptr, Shared_VStart, Shared_VEnd, Shared_PStart);
    if (!okay)
    {
	return CNK_RC_SPI(EINVAL);
    }
    *writeptr = (void*)ptr;
    rc = 0;
#undef INRANGE
    return CNK_RC_SPI(rc);
}

uint64_t  sc_SETL1PCURPATTERNADDR(SYSCALL_FCN_ARGS)
{
    void* readptr = (void*)r3;
    void* writeptr= (void*)r4;
    uint64_t ptr = 0;
    uint64_t preadptr;
    uint64_t pwriteptr;
    
    AppProcess_t* proc = GetMyProcess();
    int okay;
#define INRANGE(vabase, start, end, startp) if(((uint64_t)vabase>=proc->start) && ((uint64_t)vabase<=proc->start + (proc->end - proc->start))) { okay = 1; ptr = (uint64_t)vabase - proc->start + proc->startp; }
    
    okay = 0;
    INRANGE(readptr, Text_VStart, Text_VEnd, Text_PStart);
    INRANGE(readptr, Data_VStart, Data_VEnd, Data_PStart);
    INRANGE(readptr, Heap_VStart, Heap_VEnd, Heap_PStart);
    INRANGE(readptr, Shared_VStart, Shared_VEnd, Shared_PStart);
    if (!okay)
    {
	return CNK_RC_SPI(EFAULT);
    }
    preadptr = ptr;
    
    okay = 0;
    INRANGE(writeptr, Text_VStart, Text_VEnd, Text_PStart);
    INRANGE(writeptr, Data_VStart, Data_VEnd, Data_PStart);
    INRANGE(writeptr, Heap_VStart, Heap_VEnd, Heap_PStart);
    INRANGE(writeptr, Shared_VStart, Shared_VEnd, Shared_PStart);
    if (!okay)
    {
	return CNK_RC_SPI(EFAULT);
    }    
    pwriteptr = ptr;
#undef INRANGE

    uint64_t rc = Kernel_L1pSetCurrentPatternAddress((void*)preadptr, (void*)pwriteptr);
    return CNK_RC_SPI(rc);
}
