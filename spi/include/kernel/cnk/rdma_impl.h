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

#ifndef	_KERNEL_CNKOFED_IMPL_H_ /* Prevent multiple inclusion */
#define	_KERNEL_CNKOFED_IMPL_H_

#include <hwi/include/bqc/A2_inlines.h>
#include "cnk/include/SPI_syscalls.h"
#include "cnk/include/Config.h"
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>

__INLINE__
int Kernel_RDMAOpen(int* rdmaSocket)
{
    int rc = (int) CNK_SPI_SYSCALL_1(RDMA_OPEN, rdmaSocket);
    return rc;
}

__INLINE__
int Kernel_RDMAConnect(int RDMA_fd, int destination_port){

  int rc = (int) CNK_SPI_SYSCALL_2(RDMA_CONNECT, RDMA_fd, destination_port);

  return rc;
}


__INLINE__
int Kernel_RDMASend(int RDMA_fd, const void* buf, size_t len, uint32_t lkey){
  int rc = (int)CNK_SPI_SYSCALL_4(RDMA_SEND,RDMA_fd, buf, len, lkey);
  return rc;
}

__INLINE__
int Kernel_RDMARecv(int RDMA_fd, const void* buf, size_t len, uint32_t lkey){
  int rc = (int)CNK_SPI_SYSCALL_4(RDMA_RECV,RDMA_fd, buf, len, lkey);
  return rc;
}

__INLINE__
int Kernel_RDMAPollCQ(int RDMA_fd, int * num_entries, Kernel_RDMAWorkCompletion_t * WorkCompletionList){

  int rc = (int) CNK_SPI_SYSCALL_3(RDMA_POLL_CQ, RDMA_fd, num_entries, WorkCompletionList);
  return rc;
}
__INLINE__
int Kernel_RDMARegisterMem(int RDMA_fd,Kernel_RDMARegion_t * usingRegion4RDMA){

  int rc = (int) CNK_SPI_SYSCALL_2(RDMA_REG_MEM, RDMA_fd, usingRegion4RDMA);
  return rc;
}

__INLINE__
int Kernel_RDMADeregisterMem(int RDMA_fd, Kernel_RDMARegion_t *  usingRegion4RDMA){

  int rc = (int) CNK_SPI_SYSCALL_2(RDMA_DEREG_MEM, RDMA_fd, usingRegion4RDMA);
  return rc;
}


#endif /* _KERNEL_CNKOFED_IMPL_H_ */
