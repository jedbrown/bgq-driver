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

#include "Kernel.h"

__BEGIN_DECLS

uint64_t sc_GETMAPPING(SYSCALL_FCN_ARGS)
{
    size_t    namesize    = (size_t)r3;
    char*     name        = (char*)r4;
    uint32_t* isFile      = (uint32_t*)r5;
    size_t    len;
    
    if(! VMM_IsAppAddress(name, namesize))
    {
        return CNK_RC_SPI(EFAULT);
    }
    if(! VMM_IsAppAddress(isFile, sizeof(*isFile)))
    {
        return CNK_RC_SPI(EFAULT);
    }
    
    AppState_t *appState = GetMyAppState();
    if(appState->mapFilePath[0] != 0)
    {
        len = MIN(namesize, strlen(appState->mapFilePath) + 1);
        memcpy(name, appState->mapFilePath, len);
        *isFile = 1;
    }
    else
    {
        len = MIN(namesize, strlen(appState->mapOrder) + 1);
        memcpy(name, appState->mapOrder, len);
        *isFile = 0;
    }
    return CNK_RC_SUCCESS(0);
}

__END_DECLS
