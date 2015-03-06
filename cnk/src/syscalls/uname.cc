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
/* (C) Copyright IBM Corp.  2007, 2012                              */
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
#include <sys/utsname.h>
#include <ctype.h>
#include "Kernel.h"

//! \brief  Implement the uname system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(descriptor number) or CNK_RC_FAILURE(errno).

uint64_t sc_uname(SYSCALL_FCN_ARGS)
{
    struct utsname *name = (struct utsname *)r3;

    TRACESYSCALL(("(I) %s%s: name=0x%p\n", __func__, whoami(), name));

    // Check for error conditions.
    if (!VMM_IsAppAddress(name, sizeof(*name)))
    {
        return CNK_RC_FAILURE(EFAULT);
    }

    // Copy the fields into the caller's storage.
    strncpy(name->sysname,  "CNK", sizeof(name->sysname));
    strncpy(name->nodename, NodeState.NodeName, sizeof(name->nodename));
    strncpy(name->release, NodeState.Release, sizeof(name->release));
    strncpy(name->version, "1", sizeof(name->version));
    strncpy(name->machine, "BGQ", sizeof(name->machine));
    
    return CNK_RC_SUCCESS(0);
}

    
extern const char* bgq_driver_level;
extern uint64_t cnk_efix_level;
uint64_t sc_KERNELVERSION(SYSCALL_FCN_ARGS)
{
    int x;
    uint64_t* verptr[4];
    verptr[0] = (uint64_t*)r3;
    verptr[1] = (uint64_t*)r4;
    verptr[2] = (uint64_t*)r5;
    verptr[3] = (uint64_t*)r6;
    
    char* parsestr = (char*)bgq_driver_level;
    for(x=0; x<4; x++)
    {
        if (!VMM_IsAppAddress(verptr[x], sizeof(uint64_t)))
        {
            return CNK_RC_SPI(EFAULT);
        }
        *verptr[x] = 0;
    }
    
    if(strncmp(bgq_driver_level, "/bgsys/drivers/", 14) != 0)
        return CNK_RC_SPI(0);
    
    for(x=0; x<3; x++)
    {
        while(*parsestr)
        {
            if(isdigit(*parsestr)) break;
            parsestr++;
        }
        *(verptr[x]) = strtoull_(parsestr, &parsestr, 10);
    }
    *(verptr[3]) = cnk_efix_level;
    return CNK_RC_SPI(0);
}
