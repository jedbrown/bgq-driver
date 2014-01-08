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


uint64_t  sc_GETFLIGHTLOG( SYSCALL_FCN_ARGS)
{
    int    rc;
    size_t bufferSize = (size_t)r3;
    char*  buffer     = (char*)r4;
    
    if(bufferSize < 1024)
    {
        return CNK_RC_SPI(EINVAL);
    }
    
    if ( !VMM_IsAppAddress(buffer, bufferSize))
    {
        return CNK_RC_SPI(EFAULT);
    }
    
    rc = getFlightRecorder(bufferSize, buffer);
    if(rc)
    {
        return CNK_RC_SPI(rc);
    }
    return 0;
}
