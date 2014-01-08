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
/* (C) Copyright IBM Corp.  2009, 2012                              */
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
#include "util.h"
#include "rdmaFS.h"
#include <cnk/include/RdmaAddr.h>


   //! Storage for outbound messages.
static char _outMessage[bgcios::SmallMessageRegionSize] ALIGN_L1D_CACHE;

   //! Storage for inbound messages.
static char _inMessage[bgcios::SmallMessageRegionSize] ALIGN_L1D_CACHE;

int rdmaFS::init(){
     _isTerminated = false;

   // Where to initialize context?  How do we get context pointer here?
   int err = cnv_open_dev(&_context);
   if (err != 0) {
      return err;
   }

   // Allocate a protection domain.
   err = cnv_alloc_pd(&_protectionDomain, _context);
   if (err != 0) {
      return err;
   }

   // Register memory regions.
   err = cnv_reg_mr(&_inMessageRegion, &_protectionDomain, &_inMessage, sizeof(_inMessage), CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      return err;
   }

   err = cnv_reg_mr(&_outMessageRegion, &_protectionDomain, &_outMessage, sizeof(_outMessage), CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      return err;
   }

   // Create a completion queue.
   err = cnv_create_cq(&_completionQ, _context, CNV_MAX_WC);
   
   if (err != 0) {
      return err;
   }
   cnv_modify_cq_character(&_completionQ,CNVERBS_WC_LINKEDLIST_CQ_CHAR); 

   // Create queue pair.
   cnv_qp_init_attr attr;
   attr.send_cq = &_completionQ;
   attr.recv_cq = &_completionQ;

   err = cnv_create_qp(&_queuePair, &_protectionDomain, &attr);
   if (err != 0) {
      //! \todo Send a RAS event.
      return err;
   }
   Kernel_WriteFlightLog(FLIGHTLOG, RDMAFS_INITC, _queuePair.qp_num, _queuePair.handle, err, FD_RDMA);
   return 0;
}

int rdmaFS::term(void)
{
   int rc = 0;
   //int err = 0;

   if (_isTerminated == true) {
       return 0;
   }

   Kernel_WriteFlightLog(FLIGHTLOG, RDMAFS_TERMS, _queuePair.qp_num, _queuePair.handle, _fd, FD_RDMA);

   if (_fd) close(_fd);

   _isTerminated = true;

   return rc;
}


uint64_t 
rdmaFS::close(int fd){  

  if (File_GetFDType(fd) != FD_RDMA) {
      return CNK_RC_FAILURE(EBADF);
  }

  if (fd != _fd) return CNK_RC_FAILURE(EFAULT);

  // Free the descriptor now that it is closed.
  File_FreeFD(fd);
  _fd = 0;
  
  deregisterAllRegisteredMemory();
  _destination_port = 0;

  //close connection, reset qp send and receive and completion queues
  cnv_recycle_qp(&_queuePair);   

  return 0;
}

uint64_t 
rdmaFS::recv(int fd, void *buffer, size_t length, int lkey){
  if (File_GetFDType(fd) != FD_RDMA) {
      return CNK_RC_FAILURE(EBADF);
  }
  if (fd != _fd) return CNK_RC_FAILURE(EFAULT);
  if (lkey==0) return CNK_RC_FAILURE(EINVAL);
  
   //! \todo Validate lkey?  Make sure length doesn't exceed memory region?
  uint64_t baseva, basepa, basesz;
  vmm_TranslateAddress(buffer, &baseva, &basepa, &basesz);
  
   struct cnv_sge sge;
   sge.addr = ((uint64_t)buffer) - baseva + basepa + CONFIG_GLOBAL_BASE;
   sge.length = length;
   sge.lkey = lkey;

   // Build receive work request.
   struct cnv_recv_wr wr;
   wr.wr_id = (uint64_t)buffer;  // Save address so it is available in work completion.
   wr.next = NULL;
   wr.sg_list = &sge;
   wr.num_sge = 1;
// printf("posted recv to addr %lu\n", addr);

   // Post a receive for inbound message.
   int err = cnv_post_recv_linked_list(&_queuePair, &wr);
   if (err != 0) {
      return CNK_RC_FAILURE(err);
   }

   return 0;
}

uint64_t 
rdmaFS::send(int fd, const void *buffer, size_t length, int lkey){
    

  if (File_GetFDType(fd) != FD_RDMA) {
      return CNK_RC_FAILURE(EBADF);
  }
  if (fd != _fd) return CNK_RC_FAILURE(EFAULT);
  if (lkey==0) return CNK_RC_FAILURE(EINVAL);

   //! \todo Validate lkey?  Make sure length doesn't exceed memory region?.
   struct cnv_sge sge;
   sge.addr = (uint64_t)buffer;
   sge.length = length;
   sge.lkey = lkey;

   // Build receive work request.
   struct cnv_send_wr wr;
   wr.wr_id = (uint64_t)buffer;  // Save address so it is available in work completion.
   wr.next = NULL;
   wr.sg_list = &sge;
   wr.num_sge = 1;
// printf("posted recv to addr %lu\n", addr);

   // Post a receive for inbound message.
   int err = cnv_post_send_linked_list(&_queuePair, &wr);
   if (err != 0) {
      return CNK_RC_FAILURE(err);
   }
   return 0;
}

uint64_t 
rdmaFS::connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  if (File_GetFDType(sockfd) != FD_RDMA) {
      return CNK_RC_FAILURE(EBADF);
  }
  if (sockfd != _fd) CNK_RC_FAILURE(EFAULT);
  rdmaAddrInfo_t * rAddrInfo = (rdmaAddrInfo_t *)addr;
  _destination_port = rAddrInfo->destination_port;
   // Connect to stdiod on I/O node.
   _destAddress.sin_family = AF_INET;
   _destAddress.sin_port = _destination_port;
   _destAddress.sin_addr.s_addr = NodeState.ServiceDeviceAddr;
   int err = cnv_connect(&_queuePair, (struct sockaddr *)&_destAddress);
   
   if (err != 0) {
       _destination_port = 0;
       return err;
   }

return 0;
}

uint64_t
rdmaFS::socket(int domain, int type, int protocol)
{
   // Allocate a descriptor before forwarding a message to sysiod.
   _fd = File_GetFD(0);
   if (_fd == -1) { 
      //! \todo TODO How to return errno in the SPI macro?
      return CNK_RC_FAILURE(EMFILE);
   }
   File_SetFD(_fd, _fd, FD_RDMA);
   return CNK_RC_SUCCESS(_fd);

}
uint64_t 
rdmaFS::registerMemory(int fd,void * ptr)
{
   // Register a memory region for the caller's data.
   Kernel_RDMARegion_t * usingRegion4RDMA = (Kernel_RDMARegion_t * ) ptr;
   struct cnv_mr  * userRegion = findFreeUserRegion();
   int err = cnv_reg_mr(userRegion, &_protectionDomain, usingRegion4RDMA->address, usingRegion4RDMA->length, CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      if (userRegion==NULL) err = ENOBUFS;
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)usingRegion4RDMA->address, usingRegion4RDMA->length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }
   usingRegion4RDMA->lkey = userRegion->lkey;
   return 0;
}
uint64_t 

rdmaFS::deregisterMemory(int fd,void * ptr)
{
   Kernel_RDMARegion_t * usingRegion4RDMA = (Kernel_RDMARegion_t * ) ptr;
   struct cnv_mr * userRegion = findUserRegion(usingRegion4RDMA->lkey);
   int err = cnv_dereg_mr(userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion->addr, userRegion->length, userRegion->lkey, err);
      return err;
   }
   usingRegion4RDMA->lkey = 0;
   //note userRegion is zeroed by cnv_dereg_mr
   return 0;
}

uint64_t
rdmaFS::pollCQ(int fd, unsigned long int num_entries, Kernel_RDMAWorkCompletion_t* WorkCompletionList)
{
   if (File_GetFDType(fd) != FD_RDMA) {
      return CNK_RC_FAILURE(EBADF);
  }
  if (fd != _fd) CNK_RC_FAILURE(EFAULT);
  int rc = cnv_get_completions_linked_list(&_queuePair, num_entries, WorkCompletionList);
  if (rc < 0){
    return CNK_RC_FAILURE(-rc);
  }
  return rc;
}

