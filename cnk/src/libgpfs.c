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

#include "hwi/include/common/compiler_support.h"
#include <cnk/include/SPI_syscalls.h>

int gpfs_fcntl(int fd, void* ptr)
{
    int gpfs_rc;
    uint64_t funcship_rc = (uint64_t) CNK_SPI_SYSCALL_3(GPFSFCNTL, fd, ptr, &gpfs_rc);
    if(funcship_rc == 0) 
        return gpfs_rc;
    else
        return -1;
}
