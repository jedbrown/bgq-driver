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
#include <cnk/include/RdmaAddr.h>
#include "fs/virtFS.h"
#include "spi/include/kernel/memory.h"
#include "spi/include/kernel/rdma.h"
#include <sys/socket.h>
#include <sys/errno.h>
#include "fs/rdmaFS.h"

uint64_t  sc_RDMA_OPEN(SYSCALL_FCN_ARGS)
{
    int* fdPtr = (int*)r3;
    if(!VMM_IsAppAddress(fdPtr, sizeof(int)))
    {
        return CNK_RC_SPI(EFAULT);
    }
    
    //allocate RDMA_FD
    int local_RDMA_fd = File_GetFSPtrFromType(FD_RDMA)->socket(0, 0, 0);
    if (local_RDMA_fd <= 0)
    {
        //! \todo TODO How to return errno in the SPI macro after it was set in socket?
        return CNK_RC_SPI(EMFILE);
    }
    *fdPtr = local_RDMA_fd;
    return 0;
}

//! \brief  Implement the RDMA_CONNECT call
uint64_t  sc_RDMA_CONNECT(SYSCALL_FCN_ARGS)
{

    int RDMA_fd = (int)r3;
    rdmaAddrInfo_t rdmaAddrInfo;
    rdmaAddrInfo.destination_port = (int)r4;
    const struct sockaddr * addr = (const struct sockaddr *)&rdmaAddrInfo;
    //connect setup and issue
    int rc_conn = File_GetFSPtrFromType(FD_RDMA)->connect(RDMA_fd, addr, sizeof(rdmaAddrInfo_t) );
    return rc_conn;
}

uint64_t  sc_RDMA_SEND(SYSCALL_FCN_ARGS)
{
    int RDMA_fd = (int)r3;
    void * buf = (void *)r4;
    size_t len = (size_t)r5;
    int lkey = (int)r6;

    return File_GetFSPtrFromType(FD_RDMA)->send(RDMA_fd, buf, len, lkey);

}

uint64_t  sc_RDMA_RECV(SYSCALL_FCN_ARGS)
{
    int RDMA_fd = (int)r3;
    void * buf = (void *)r4;
    size_t len = (size_t)r5;
    int lkey = (int)r6;

    return File_GetFSPtrFromType(FD_RDMA)->recv(RDMA_fd, buf, len, lkey);
   
}

//int Kernel_RDMAPollCQ(int RDMA_fd, int * num_entries, Kernel_RDMAWorkCompletion_t * WorkCompletionList);
uint64_t  sc_RDMA_POLL_CQ(SYSCALL_FCN_ARGS){
    int RDMA_fd = (int)r3;
    int * num_entries = (int *)r4;
     Kernel_RDMAWorkCompletion_t * workCompletionList = (Kernel_RDMAWorkCompletion_t *)r5;
    if (RDMA_fd <= 0){    
      return CNK_RC_SPI(EBADF);
    }
    if( (num_entries==NULL) 
       || (workCompletionList==NULL)
    ){
     return CNK_RC_SPI(EFAULT);
    }
    if ( (*num_entries<=0)
        || (workCompletionList==NULL)
    ){     
      return CNK_RC_SPI(EINVAL);
    }
    //! \todo Investigate whether num_entries needs to be validated as a valid pointer/address.
    int num_completions = (int) File_GetFSPtrFromType(FD_RDMA)->ioctl(RDMA_fd, *num_entries, workCompletionList);
    if (num_completions<0) return -1;
    *num_entries = num_completions;
    return 0;
}

//int Kernel_RDMARegisterMem(int RDMA_fd,Kernel_RDMARegion_t * usingRegion4RDMA);
uint64_t  sc_RDMA_REG_MEM(SYSCALL_FCN_ARGS){
    int RDMA_fd = (int)r3;
    Kernel_RDMARegion_t * ptr = (Kernel_RDMARegion_t *)r4;
    if (RDMA_fd <= 0){      
      return CNK_RC_SPI(EBADF);
    }
    if (ptr==NULL){
      return CNK_RC_SPI(EFAULT);
    }
    return virtFSPtr[FD_RDMA]->registerMemory(RDMA_fd,ptr);
}
uint64_t  sc_RDMA_DEREG_MEM(SYSCALL_FCN_ARGS){
    int RDMA_fd = (int)r3;
    Kernel_RDMARegion_t * ptr = (Kernel_RDMARegion_t *)r4;
    if (RDMA_fd <= 0){      
      return CNK_RC_SPI(EBADF);
    }
    if (ptr==NULL){
      return CNK_RC_SPI(EFAULT);
    }
    return virtFSPtr[FD_RDMA]->deregisterMemory(RDMA_fd,ptr);
}
