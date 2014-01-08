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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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
#include "fs/virtFS.h"
#include "spi/include/kernel/memory.h"
#include "spi/include/kernel/sendx.h"
#include <sys/socket.h>
#include <sys/errno.h>


//! \brief  Implement the sc_CNKOFEDCONNECT system call.

//int rc = (int) CNK_SPI_SYSCALL_3(SENDX, struct UserMessage * sendMessage,struct UserMessage * recvMessage, uint32_t recv_length);
uint64_t  sc_SENDX(SYSCALL_FCN_ARGS)
{

    char  * mInput = (char *)r3;

    int rc_sendx = File_GetFSPtrFromType(FD_FILE)->sendx(mInput);

    return CNK_RC_SPI(rc_sendx);

}

