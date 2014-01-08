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


#include "Kernel.h"
#include <fcntl.h>
#include <spi/include/kernel/location.h>

__BEGIN_DECLS

uint64_t sc_INJECTRAWRAS(SYSCALL_FCN_ARGS)
{
    int rc;
    uint32_t  message_id = (uint32_t)r3;
    size_t    raslength  = (size_t)r4;
    uint64_t* rasdata    = (uint64_t*)r5;
    
    if ( !VMM_IsAppAddress(rasdata, raslength))
    {
        CNK_RC_SPI(EFAULT);
    }
    
    if(raslength % sizeof(uint64_t))
    {
        CNK_RC_SPI(EINVAL);
    }
    
    rc = NodeState.FW_Interface.writeRASEvent(message_id, raslength/sizeof(uint64_t), (fw_uint64_t*)rasdata);
    if(rc)
    {
        CNK_RC_SPI(EINVAL);
    }
    
    return CNK_RC_SPI(0); // return this value if job occupies the entire block
}

uint64_t sc_INJECTASCIIRAS(SYSCALL_FCN_ARGS)
{
    int rc;
    uint32_t  message_id = (uint32_t)r3;
    const char* rasstring    = (const char*)r4;
    
    if ( !VMM_IsAppAddress(rasstring, 256))
    {
        CNK_RC_SPI(EFAULT);
    }
    
    rc = NodeState.FW_Interface.writeRASString(message_id, rasstring);
    if(rc)
    {
        CNK_RC_SPI(EINVAL);
    }
    
    return CNK_RC_SPI(0); // return this value if job occupies the entire block
}

__END_DECLS
