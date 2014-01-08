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
/* (C) Copyright IBM Corp.  2013, 2013                              */
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
#include "fs/virtFS.h"

__BEGIN_DECLS

uint64_t sc_GPFSFCNTL(SYSCALL_FCN_ARGS)
{
    int fd = r3;
    void *buf = (void *)r4;
    int* result = (int*)r5;
    uint32_t cnt;
    uint64_t rc;
    
    TRACESYSCALL(("(I) %s%s: fd=%d, buf=0x%p\n", __func__, whoami(), fd, buf));
    
    // Make sure the descriptor is valid.
    if (!File_IsFDAllocated(fd)) {
        return CNK_RC_SPI(EBADF);
    }
    
    // Check for valid address.
    if ( !VMM_IsAppAddress(buf, sizeof(size_t)) ) {
        return CNK_RC_SPI(EFAULT);
    }

    if ( !VMM_IsAppAddress(result, sizeof(int)) ) {
        return CNK_RC_SPI(EFAULT);
    }
    
    cnt = ((uint32_t*)buf)[0];
    
    if ( !VMM_IsAppAddress(buf, cnt) ) {
        return CNK_RC_SPI(EFAULT);
    }
    if(cnt > 65536)
    {
        return CNK_RC_SPI(EINVAL);
    }
    
    // Run file system's gpfsfcntl() method.
    rc = File_GetFSPtr(fd)->gpfsfcntl(fd, buf, cnt, result);
    if(rc != 0)
    {
        TRACESYSCALL(("(I) %s%s: gpfsfcntl function ship failed.  rc=0x%lx\n", __func__, whoami(), rc));
    }
    return rc;
}

__END_DECLS
