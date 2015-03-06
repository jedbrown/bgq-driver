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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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
#include "sysioFS.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include "NodeController.h"


using namespace bgcios::sysio;
   //! Storage for outbound messages.  
static char _outMessage[CONFIG_MAX_HWTHREADS * bgcios::ImmediateMessageSize] ALIGN_L1D_CACHE;

   //! Storage for inbound messages.
static char _inMessage[CONFIG_MAX_HWTHREADS * bgcios::ImmediateMessageSize] ALIGN_L1D_CACHE;

int
sysioFS::init(void)
{
   // Make sure lock is reset.
   fetch_and_and(&_lock, 0);

   // Start sequence ids from 1.
   for (int i=0;i<CONFIG_MAX_HWTHREADS;i++) _procSequenceId[i]= 1;
   _isTerminated = false;

   // Where to initialize context?  How do we get context pointer here?
   int err = cnv_open_dev(&_context);
   if (err != 0) {
      //! \todo Send a RAS event.
      TRACE( TRACE_SysioFS, ("(E) sysioFS::init%s: cnv_open_dev() failed, error %d\n", whoami(), err) );
      return err;
   }
   
   // Create a protection domain.
   err = cnv_alloc_pd(&_protectionDomain, _context);
   if (err != 0) {
      //! \todo Send a RAS event.
      TRACE( TRACE_SysioFS, ("(E) sysioFS::init%s: cnv_alloc_pd() failed, error %d\n", whoami(), err) );
      return err;
   }

   // Register memory regions.
   err = cnv_reg_mr(&_inMessageRegion, &_protectionDomain, &_inMessage, sizeof(_inMessage), CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)&_inMessage, sizeof(_inMessage), _protectionDomain.handle, err);
      TRACE( TRACE_SysioFS, ("(E) sysioFS::init%s: cnv_reg_mr() failed for inbound message region, error %d\n", whoami(), err) );
      return err;
   }

   err = cnv_reg_mr(&_outMessageRegion, &_protectionDomain, &_outMessage, sizeof(_outMessage), CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)&_outMessage, sizeof(_outMessage), _protectionDomain.handle, err);
      TRACE( TRACE_SysioFS, ("(E) sysioFS::init%s: cnv_reg_mr() failed for outbound message region, error %d\n", whoami(), err) );
      return err;
   }

   // initialize send message subregions in _outMessage
   initSendHeaders();

   // Create a completion queue.
   if (  ( err = cnv_create_cq(&_completionQ, _context, CNV_MAX_WC) )  ) return err;
   cnv_modify_cq_character(&_completionQ,CNVERBS_SEQUENCEID_CQ_CHAR); 

   // Create queue pair.
   cnv_qp_init_attr attr;
   attr.send_cq = &_completionQ;
   attr.recv_cq = &_completionQ;

   err = cnv_create_qp(&_queuePair, &_protectionDomain, &attr);
   if (err != 0) {
      //! \todo Send a RAS event.
      TRACE( TRACE_SysioFS, ("(E) sysioFS::init%s: cnv_create_qp() failed, error %d\n", whoami(), err) );
      return err;
   }

   // Connect to sysiod on I/O node.
   struct sockaddr_in destAddress;
   destAddress.sin_family = AF_INET;
   destAddress.sin_port = NodeState.sysiodPortAddressDestination;
   destAddress.sin_addr.s_addr = NodeState.ServiceDeviceAddr;
   
   err = cnv_connect(&_queuePair, (struct sockaddr *)&destAddress);
   Node_ReportConnect(err, destAddress.sin_addr.s_addr, destAddress.sin_port);
   if (err != 0) {
       TRACE( TRACE_SysioFS, ("(E) sysioFS::init%s: cnv_connect() failed, error %d\n", whoami(), err) );
       Kernel_Crash(RAS_KERNELCNVCONNECTFAIL);
   }

   _rdmaBufferVirtAddr = (uint64_t)-1;  
   _rdmaBufferLength = 0;  
   _remotekey = (uint32_t)-1 ;

   TRACE( TRACE_SysioFS, ("(I) sysioFS::init%s: connected to sysiod\n", whoami()) );
   return 0;
}

int
sysioFS::term(void)
{
   int rc = 0;

   if (_isTerminated == true) {
       return 0;
   }
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);
   
   // Just return if the method has already completed successfully.
   if (_isTerminated == true) {
      return 0;
   }

   // Disconnect from sysiod on I/O node.
   int err = cnv_disconnect(&_queuePair);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDISCNE, _queuePair.qp_num, _queuePair.handle, err, 0);
      TRACE( TRACE_SysioFS, ("(E) sysioFS::term%s: cnv_disconnect() failed, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }

   // Destroy queue pair.
   err = cnv_destroy_qp(&_queuePair);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDESQPE, _queuePair.qp_num, _queuePair.handle, err, 0);
      TRACE( TRACE_SysioFS, ("(E) sysioFS::term%s: cnv_destroy_qp() failed, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }

   // Destroy completion queue.
   err = cnv_destroy_cq(&_completionQ);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDESCQE, _completionQ.handle, err, 0, 0);
      TRACE( TRACE_SysioFS, ("(E) sysioFS::term%s: cnv_destroy_cq() failed, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }

   // Deregister memory regions.
   err = cnv_dereg_mr(&_outMessageRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)_outMessageRegion.addr, _outMessageRegion.length, _outMessageRegion.lkey, err);
      TRACE( TRACE_SysioFS, ("(E) sysioFS::term%s: cnv_dereg_mr() failed for outbound message region, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }
   err = cnv_dereg_mr(&_inMessageRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)_inMessageRegion.addr, _inMessageRegion.length, _inMessageRegion.lkey, err);
      TRACE( TRACE_SysioFS, ("(E) sysioFS::term%s: cnv_dereg_mr() failed for inbound message region, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }

   // Deallocate protection domain.
   err = cnv_dealloc_pd(&_protectionDomain);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDEAPDE, _protectionDomain.handle, err, 0, 0);
      TRACE( TRACE_SysioFS, ("(E) sysioFS::term%s: cnv_dealloc_pd() failed, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }

   _isTerminated = true;

   TRACE( TRACE_SysioFS, ("(I) sysioFS::term%s: termination is complete\n", whoami()) );
   return rc;
}

int
sysioFS::setupJob(int fs)
{
   // Just return if this is not the default descriptor type.
   if (fs != FD_FILE) {
      return 0;
   }

   int rc = 0;

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   AppState_t *app = GetMyAppState();
   SetupJobMessage *requestMsg = (SetupJobMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), SetupJob, 
                  sizeof(SetupJobMessage));
   requestMsg->userId = app->UserID;
   requestMsg->groupId = app->GroupID;
   requestMsg->numGroups = app->NumSecondaryGroups;
   memcpy(requestMsg->secondaryGroups, app->SecondaryGroups, sizeof(gid_t) * app->NumSecondaryGroups);
   uint32_t boolean;
   if (App_GetEnvValue("BG_SYSIODPOSIXMODE", &boolean)) {
      requestMsg->posixMode = boolean;
   }
   else {
      requestMsg->posixMode = -1;
   }
   if (App_GetEnvValue("BG_SYSIODLOGJOBSTATISTICS", &boolean)) {
      requestMsg->logJobStatistics = boolean;
   }
   else {
      requestMsg->logJobStatistics = -1;
   }
   if (App_GetEnvValue("BG_SYSIODLOGFUNCTIONSHIPERRORS", &boolean)) {
      requestMsg->logFunctionShipErrors = boolean;
   }
   else {
      requestMsg->logFunctionShipErrors = -1;
   }
   
   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t result = exchange(requestMsg, &inRegion);

   SetupJobAckMessage *replyMsg = (SetupJobAckMessage *)inRegion;
   uint64_t * data = (uint64_t * )(&replyMsg->sysiod_pid);
   Kernel_WriteFlightLog(FLIGHTLOG, FL_SSETUPACK, data[0],data[1],data[2],data[3] );
   _rdmaBufferVirtAddr = replyMsg -> rdmaBufferVirtAddr;     //!< RDMA buffer virtual address
   _rdmaBufferLength   = replyMsg -> rdmaBufferLength;       //!< length of said buffer  
   _remotekey = replyMsg -> memoryKeyOfBuffer;
   
   if (CNK_RC_IS_FAILURE(result)) {      
      App_SetLoadState(AppState_LoadFailed, replyMsg->header.returnCode, replyMsg->header.errorCode);
      if (replyMsg->header.returnCode == bgcios::VersionMismatch) {
         Kernel_WriteFlightLog(FLIGHTLOG, FL_VERMISMAT, requestMsg->header.service, ProtocolVersion, replyMsg->header.errorCode, 0); 
         RASBEGIN(3);
         RASPUSH(requestMsg->header.service);
         RASPUSH(ProtocolVersion);
         RASPUSH(replyMsg->header.errorCode);
         RASFINAL(RAS_KERNELVERSIONMISMATCH);
         Kernel_Crash(1);
      }
      rc = CNK_RC_FAILURE(result);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   return rc;
}

int
sysioFS::cleanupJob(int fs)
{
   // Just return if this is not the default descriptor type.
   if (fs != FD_FILE) {
      return 0;
   }

   uint64_t result = 0;

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   CleanupJobMessage *requestMsg = (CleanupJobMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), CleanupJob, sizeof(CleanupJobMessage));
   
   // Exchange messages with sysiod.
   void *inRegion;
   result = exchange(requestMsg, &inRegion);
   
   if (CNK_RC_IS_SUCCESS(result)) {
      CleanupJobAckMessage *replyMsg = (CleanupJobAckMessage *)inRegion;
      if (replyMsg->header.returnCode != bgcios::Success) {
         printf("(E) sysioFS::cleanupJob(): failed, %d\n", replyMsg->header.errorCode);
         result = CNK_RC_FAILURE(replyMsg->header.errorCode);
      }
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   return result;
}

int
sysioFS::postRecv(uint64_t addr, uint32_t sequenceID)
{
   // Build scatter/gather element for one inbound message.  Note an inbound message has a maximum size of a torus packet.
   struct cnv_sge sge;
   sge.addr = addr;
   sge.length = bgcios::ImmediateMessageSize;
   sge.lkey = _inMessageRegion.lkey;

   // Build receive work request.
   struct cnv_recv_wr wr;
   wr.wr_id = addr;  // Save address so it is available in work completion.
   wr.next = NULL;
   wr.sg_list = &sge;
   wr.num_sge = 1;
// printf("posted recv to addr %lu\n", addr);

   // Post a receive for inbound message.
   cnv_recv_wr *bad_wr;
   int err = cnv_post_recv_seqID(&_queuePair, &wr, &bad_wr, sequenceID);
   if (err != 0) {
      return err;
   }

   return 0;
}

int
sysioFS::postSend(struct cnv_sge *sgeList, int listSize, uint32_t sequenceID)
{
   // Build send work request.
   struct cnv_send_wr wr;
   wr.wr_id = 0;
   wr.next = NULL;
   wr.sg_list = sgeList;
   wr.num_sge = listSize;
   if (listSize > 1)  wr.opcode = CNV_WR_SEND_WITH_IMM;
   else  wr.opcode = CNV_WR_SEND;
   wr.send_flags = 0;
   wr.imm_data = sequenceID;
   wr.remote_addr = -1;
   wr.rkey = 0;

   // Post a send for outbound message.
   int err = cnv_post_send_no_comp(&_queuePair, &wr);
   if (err != 0) {
      return err;
   }

   return 0;
}

int
sysioFS::postRdmaWrite(struct cnv_sge *sgeList, int listSize, uint32_t sequenceID,uint64_t remoteAddr,uint32_t remoteKey)
{
   // Build send work request.
   struct cnv_send_wr wr;
   wr.wr_id = sequenceID;
   wr.next = NULL;
   wr.sg_list = sgeList;
   wr.num_sge = listSize;
   wr.opcode = CNV_WR_RDMA_WRITE;
   wr.send_flags = 0;
   wr.imm_data = ProcessorID() | CNV_DIRECTED_RECV;
   wr.remote_addr = remoteAddr;
   wr.rkey = remoteKey;

   // Post a send for outbound message.
   cnv_send_wr *bad_wr;
   int err = cnv_post_send_seqID(&_queuePair, &wr, &bad_wr,sequenceID);
   if (err != 0) return err;

   // Wait for completions for the work requests posted previously.
   cnv_wc completions[1];
   int foundCompletions = 0;

   err = cnv_poll_cq(&_completionQ, 1, completions, &foundCompletions, ProcessorID());
   if (err != 0) return err;
   if ((completions[0].wr_id == sequenceID) && (completions[0].status == CNV_WC_SUCCESS)) {
       
   }
   else return EINVAL;

   return 0;
}


uint64_t
sysioFS::exchangeMessages(cnv_sge *sendList, int sendListSize, void **inMsg, uint32_t sequenceID)
{
   // Post a receive for the inbound reply message.
  int procID = ProcessorID();
  uint64_t addr = getRecvBuffer(procID);
  *inMsg = (void *)addr;
  int err = postRecv(addr, sequenceID);
   if (err != 0) {
      return CNK_RC_FAILURE(err);
   }

   // Post a send for the outbound request message.
   err = postSend(sendList, sendListSize,sequenceID);
   if (__UNLIKELY(err != 0) ){
      return CNK_RC_FAILURE(err);
   }

   // Get the work completion for the Receive

   err = cnv_poll_cq_for_single_recv(&_completionQ, procID);
   
   if (__UNLIKELY (err != 0) ) {
      printf("err=%d \n",err);
      return CNK_RC_FAILURE(err);
   }
   
   uint64_t * data = (uint64_t * )*inMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_SYSMSGRCV, data[0],data[1],data[2],data[3] );

   // Check the return code in the received message.
   bgcios::MessageHeader *replyMsg = (bgcios::MessageHeader *)*inMsg;
   uint64_t rc = CNK_RC_SUCCESS(0);
   if (__UNLIKELY (replyMsg->returnCode != bgcios::Success) ){
      rc = CNK_RC_FAILURE(replyMsg->errorCode);
   }

   return rc;
}

uint64_t
sysioFS::exchange(void *outMsg, void **inMsg)
{
   // Build scatter/gather element for outbound request message.
   bgcios::MessageHeader *requestMsg = (bgcios::MessageHeader *)outMsg;
   uint64_t * data = (uint64_t * )outMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_SYSMSGSND, data[0],data[1],data[2],data[3] );
   struct cnv_sge sge;
   sge.addr = (uint64_t)outMsg;
   sge.length = requestMsg->length;
   sge.lkey = _outMessageRegion.lkey;

   uint64_t rc = exchangeMessages(&sge, 1, inMsg,requestMsg->sequenceId);
   return rc;
}
// FYI: PATH_MAX=4096 when checked on 7/5/2012
uint64_t
sysioFS::exchange(void *outMsg, const char *pathname, void **inMsg)
{
   struct cnv_mr userRegion;
   int err;

   // Build scatter/gather element list for outbound request message.
   bgcios::MessageHeader *requestMsg = (bgcios::MessageHeader *)outMsg;
   uint64_t * data = (uint64_t * )outMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_SYSMSGSND, data[0],data[1],data[2],data[3] );
   struct cnv_sge sgeList[2];
   sgeList[0].addr = (uint64_t)outMsg;
   sgeList[0].length = requestMsg->length;
   sgeList[0].lkey = _outMessageRegion.lkey;
   int listSize = 1;

   // If possible, pack the message and path into one packet.  Note this optimization assumes that the path is
   // always placed right after the message.
   uint32_t pathlen = strnlen(pathname, PATH_MAX) + 1;
   if (__LIKELY ((requestMsg->length + pathlen) <= bgcios::ImmediateMessageSize) ) {
      char *pathPtr = (char *)requestMsg + requestMsg->length;
      memcpy(pathPtr, pathname, pathlen);
      sgeList[0].length += pathlen;
   }

   // Get the path directly from the caller's buffer.
   else {
      // Register a memory region for the caller's buffer.
      err = cnv_reg_mr(&userRegion, &_protectionDomain, (void *)pathname, pathlen, CNV_ACCESS_LOCAL_WRITE);
      if (err != 0) {
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)pathname, pathlen, _protectionDomain.handle, err);
         return err;
      }
      sgeList[1].addr = (uint64_t)userRegion.addr;
      sgeList[1].length = pathlen;
      sgeList[1].lkey = userRegion.lkey;
      ++listSize;
   }

   // Update total length of message in request message header.
   requestMsg->length += pathlen;

   // Exchange messages with sysiod.
   uint64_t rc = exchangeMessages(sgeList, listSize, inMsg,requestMsg->sequenceId);

   // Deregister memory region for caller's data.
   if (listSize > 1) {
      err = cnv_dereg_mr(&userRegion);
      if (err != 0) {
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
      }
   }
   return rc;
}

uint64_t
sysioFS::exchange(void *outMsg, const char *pathname1, const char *pathname2, void **inMsg)
{
   struct cnv_mr userRegion1;
   struct cnv_mr userRegion2;
   int err;

   // Build scatter/gather element for outbound request message.
   bgcios::MessageHeader *requestMsg = (bgcios::MessageHeader *)outMsg;
   uint64_t * data = (uint64_t * )outMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_SYSMSGSND, data[0],data[1],data[2],data[3] );
   struct cnv_sge sgeList[3];
   sgeList[0].addr = (uint64_t)outMsg;
   sgeList[0].length = requestMsg->length;
   sgeList[0].lkey = _outMessageRegion.lkey;
   int listSize = 1;

   // If possible, pack the message and paths into one packet.  Note this optimization assumes that the paths are
   // always placed right after the message.
   uint32_t pathlen1 = strnlen(pathname1, PATH_MAX) + 1;
   uint32_t pathlen2 = strnlen(pathname2, PATH_MAX) + 1;
   if ((requestMsg->length + pathlen1 + pathlen2) <= bgcios::ImmediateMessageSize) {
      char *pathPtr = (char *)requestMsg + requestMsg->length;
      memcpy(pathPtr, pathname1, pathlen1);
      pathPtr += pathlen1;
      memcpy(pathPtr, pathname2, pathlen2);
      sgeList[0].length += pathlen1 + pathlen2;
   }

   // Get the paths directly from the caller's buffer.
   else {
      // Register memory regions for the caller's data.
      err = cnv_reg_mr(&userRegion1, &_protectionDomain, (void *)pathname1, pathlen1, CNV_ACCESS_LOCAL_WRITE);
      if (err != 0) {
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)pathname1, pathlen1, _protectionDomain.handle, err);
         return err;
      }
      sgeList[1].addr = (uint64_t)userRegion1.addr;
      sgeList[1].length = pathlen1;
      sgeList[1].lkey = userRegion1.lkey;
      ++listSize;

      err = cnv_reg_mr(&userRegion2, &_protectionDomain, (void *)pathname2, pathlen2, CNV_ACCESS_LOCAL_WRITE);
      if (err != 0) {
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)pathname2, pathlen2, _protectionDomain.handle, err);
         cnv_dereg_mr(&userRegion1);
         return err;
      }
      sgeList[2].addr = (uint64_t)userRegion2.addr;
      sgeList[2].length = pathlen2;
      sgeList[2].lkey = userRegion2.lkey;
      ++listSize;
   }

   // Update total length of message in request message header.
   requestMsg->length += pathlen1 + pathlen2;

   // Exchange messages with sysiod.
   uint64_t rc = exchangeMessages(sgeList, listSize, inMsg,requestMsg->sequenceId);

   // Deregister memory regions for caller's data.
   if (listSize > 1) {
      err = cnv_dereg_mr(&userRegion1);
      if (err != 0) {
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion1.addr, userRegion1.length, userRegion1.lkey, err);
      }

      err = cnv_dereg_mr(&userRegion2);
      if (err != 0) {
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion2.addr, userRegion2.length, userRegion2.lkey, err);
      }
   }
   return rc;
}

uint64_t
sysioFS::access(const char *pathname, int type)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   AccessMessage *requestMsg = (AccessMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Access, sizeof(AccessMessage));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->type = type;
   requestMsg->flags = 0;
   requestMsg->offset = sizeof(AccessMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::access%s: path=%s type=%d rc=%s\n", whoami(), pathname, type, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t 
sysioFS::accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(sockfd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

      // Allocate a descriptor before forwarding a message to sysiod.
   int fd = File_GetFD(0);
   if (fd == -1) {
      return CNK_RC_FAILURE(EMFILE);
   }

   // Make sure the address length is valid.
   if ( (addrlen!=NULL) && (*addrlen > MaxAddrLength) ){
      return CNK_RC_FAILURE(EINVAL);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   AcceptMessage *requestMsg = (AcceptMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Accept, sizeof(AcceptMessage));
   requestMsg->sockfd = remoteFD;
   if ( (addr!=NULL) && (addrlen!=NULL) ){
     if (*addrlen)  memcpy(requestMsg->addr, addr, *addrlen);
     requestMsg->addrlen = *addrlen;
   }
   else requestMsg->addrlen=0;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Wait for the final result if needed.
   AcceptAckMessage *replyMsg = (AcceptAckMessage *)inRegion;
   if ( (addr!=NULL) && (addrlen!=NULL) ){
     if (*addrlen){
        memcpy(addrlen,&replyMsg->addrlen,sizeof(socklen_t) );
        memcpy(addr,replyMsg->addr,*addrlen);
     }
     if (*addrlen < replyMsg->addrlen) *addrlen = replyMsg->addrlen;
   }

   if (CNK_RC_IS_SUCCESS(rc)) {
     File_SetFD(fd, replyMsg->sockfd, FD_SOCKET);
     rc = CNK_RC_SUCCESS(fd);
   }
   else {
      File_FreeFD(fd);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);
   
   return rc;
}

uint64_t
sysioFS::bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(sockfd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Make sure the address length is valid.
   if (addrlen > MaxAddrLength) {
      return CNK_RC_FAILURE(EINVAL);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   BindMessage *requestMsg = (BindMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Bind, sizeof(BindMessage));
   requestMsg->sockfd = remoteFD;
   memcpy(requestMsg->addr, addr, addrlen);
   requestMsg->addrlen = addrlen;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::bind%s: sockfd=%d addr=0x%p addrlen=%d rc=%s\n", whoami(), sockfd, addr, addrlen, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::chdir(const char *pathname)
{
    AppProcess_t *app = GetMyProcess();
    CNK_Descriptors_t *pFD = &(app->App_Descriptors);
    
   AppProcess_t *process = GetMyProcess();

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Save the descriptor to the previous working directory so it can be closed later.
   int previousDirFD = File_GetCurrentDirFD();

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   OpenMessage *requestMsg = (OpenMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Open, sizeof(OpenMessage));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->mode = 0;
   requestMsg->flags = O_RDONLY;
   requestMsg->offset = sizeof(OpenMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   if (CNK_RC_IS_SUCCESS(rc)) {
      OpenAckMessage *replyMsg = (OpenAckMessage *)inRegion;
      File_SetFD(CWD_FILENO, replyMsg->fd, FD_FILE);
      if (pathname[0] == '/') {
          strncpy(process->CurrentDir, pathname, sizeof(process->CurrentDir));
      }
      else {
         int len = strlen(process->CurrentDir);
         if (process->CurrentDir[len-1] != '/') {
             strncat(process->CurrentDir, "/", sizeof(process->CurrentDir));
         }
         strncat(process->CurrentDir, pathname, sizeof(process->CurrentDir));
      }

      // Close the previous working directory if one was open.
      if (previousDirFD >= 0) {
         // Build request message in outbound message buffer.
         int procID=ProcessorID();
         CloseMessage *closeMsg = (CloseMessage *)getSendBuffer(procID);
         fillHeader(&(requestMsg->header), Close, sizeof(CloseMessage));
         closeMsg->fd = previousDirFD;

         // Exchange messages with sysiod.
         uint64_t closerc = exchange(requestMsg, &inRegion);

         if (CNK_RC_IS_FAILURE(closerc)) {
            printf("(E) sysioFS::chdir%s: error closing previous working directory descriptor %d, rc=%s\n", whoami(), previousDirFD, CNK_RC_STRING(closerc));
         }
      }
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::chdir%s: path=%s rc=%s\n", whoami(), pathname, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::chmod(const char *pathname, mode_t mode)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   ChmodMessage *requestMsg = (ChmodMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Chmod, sizeof(ChmodMessage));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->mode = mode;
   requestMsg->flags = 0;
   requestMsg->offset = sizeof(ChmodMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::chmod%s: path=%s mode=0x%08x rc=%s\n", whoami(), pathname, mode, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::chown(const char *pathname, uid_t uid, gid_t gid)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   ChownMessage *requestMsg = (ChownMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Chown, sizeof(ChownMessage));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->uid = uid;
   requestMsg->gid = gid;
   requestMsg->flags = 0;
   requestMsg->offset = sizeof(ChownMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::chown%s: path=%s uid=%d gid=%d rc=%s\n", whoami(), pathname, uid, gid, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::close(int fd)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   CloseMessage *requestMsg = (CloseMessage *)getSendBuffer(procID);
      if ( GetMyKThread()->KernelInternal == 1){
     fillHeader(&(requestMsg->header), CloseKernelInternal, sizeof(CloseMessage)); 
   }
   else{
     fillHeader(&(requestMsg->header), Close, sizeof(CloseMessage)); 
   }
   requestMsg->fd = remoteFD;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);
   
   
   // Free the descriptor now that it is closed.
   if (CNK_RC_IS_SUCCESS(rc)) 
   {
       FLM_ReleaseFile(fd);
       File_FreeFD(fd);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::close%s: fd=%d rc=%s\n", whoami(), fd, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(sockfd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Make sure the address length is valid.
   if (addrlen > MaxAddrLength) {
      return CNK_RC_FAILURE(EINVAL);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   ConnectMessage *requestMsg = (ConnectMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Connect, sizeof(ConnectMessage));
   requestMsg->sockfd = remoteFD;
   memcpy(requestMsg->addr, addr, addrlen);
   requestMsg->addrlen = addrlen;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   //ConnectAckMessage *replyMsg = (ConnectAckMessage *)inRegion;
   
   // Release the lock.
   Kernel_Unlock(&_lock);
   
   TRACE( TRACE_SysioFS, ("(I) sysioFS::connect%s: sockfd=%d addr=0x%p addrlen=%d rc=%s\n", whoami(), sockfd, addr, addrlen, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::fchmod(int fd, mode_t mode)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   FchmodMessage *requestMsg = (FchmodMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Fchmod, sizeof(FchmodMessage)); 
   requestMsg->fd = remoteFD;
   requestMsg->mode = mode;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::fchmod%s: fd=%d mode=0x%08x rc=%s\n", whoami(), fd, mode, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::fchown(int fd, uid_t uid, gid_t gid)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   FchownMessage *requestMsg = (FchownMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Fchown, sizeof(FchownMessage));
   requestMsg->fd = remoteFD;
   requestMsg->uid = uid;
   requestMsg->gid = gid;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::fchown%s: fd=%d uid=%d gid=%d rc=%s\n", whoami(), fd, uid, gid, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::fcntl(int fd, int cmd, uint64_t parm3)
{
    bool retry;
    uint64_t rc = CNK_RC_SUCCESS(0);
    int cmd2 = cmd;
    uint64_t retrycount = 0;
    uint32_t fcntl_lock_workaround = 1;
    App_GetEnvValue("BG_SYSIODFCNTLWORKAROUND",  &fcntl_lock_workaround);
    do
    {
        retry = false;
        if(retrycount > 0)
        {
            uint64_t backoff_time = MIN(1000 * retrycount, 1000000);  // cap retry rate at 1/second
            Kernel_WriteFlightLog(FLIGHTLOG, FL_FLOCKRETY, cmd2, rc, retrycount, backoff_time);
            usleep(backoff_time);
        }
        retrycount++;
        
        // Make sure file descriptor is valid.
        int remoteFD = File_GetRemoteFD(fd);
        if (remoteFD < 0) {
            return CNK_RC_FAILURE(EBADF);
        }
        
        if((cmd == F_SETLKW) || (cmd == F_SETLK))
        {
            struct flock *lock = (struct flock *)parm3;
            rc = FLM_TouchFD(fd, lock->l_whence);
            if(rc)
                return rc;
        }
        // Obtain the lock to serialize message exchange with sysiod.
        Kernel_Lock(&_lock);
        
        if((cmd == F_SETLK) || (cmd == F_SETLKW))
        {
            dev_t     deviceID;
            __ino64_t inode;
            size_t    size;
            File_GetDeviceINode(fd, &deviceID, &inode, &size);
            if((deviceID == 0) && (inode == 0))
            {
                rc = CNK_RC_FAILURE(EBADF);
                retry = false;
                Kernel_Unlock(&_lock);
                continue;
            }
            
            struct flock *lock = (struct flock *)parm3;
            if(FLM_HasOverlap(fd, lock->l_start, lock->l_len, lock->l_whence))
            {
                Kernel_WriteFlightLog(FLIGHTLOG, FL_FLOCKLOCL, fd, lock->l_start, lock->l_len, lock->l_whence);
                retry = true;
                Kernel_Unlock(&_lock);
                continue;
            }
        }
        
        // Build request message in outbound message buffer.
        int procID=ProcessorID();
        FcntlMessage *requestMsg = (FcntlMessage *)getSendBuffer(procID);
        fillHeader(&(requestMsg->header), Fcntl, sizeof(FcntlMessage));
        requestMsg->fd = remoteFD;
        requestMsg->cmd = cmd;
        
        rc = CNK_RC_SUCCESS(0);
        int newfd = -1;
        
        switch (cmd) {
            case F_GETFL:
            case F_GETFD:
                requestMsg->arg = 0;
                break;
                
            case F_SETFD:
                requestMsg->arg = (int)parm3;
                break;
                
            case F_DUPFD:
            {
                // Make sure a descriptor is available.
                requestMsg->arg = (int)parm3;
                newfd = File_GetFD(requestMsg->arg);
                if (newfd == -1) {
                    rc = CNK_RC_FAILURE(EBADF);
                    break;
                }
                break;
            }
            
            case F_GETLK:
            case F_SETLK:
            case F_SETLKW:
            {
                struct flock *lock = (struct flock *)parm3;
                if (!VMM_IsAppAddress(lock, sizeof(struct flock))) {
                    rc = CNK_RC_FAILURE(EFAULT);
                    break;
                }
                if((fcntl_lock_workaround == 1) && (GetMyAppState()->Active_Processes > 1) && (cmd == F_SETLKW)) // do not block in sysiod if ppn>1.
                {
                    cmd2 = requestMsg->cmd = F_SETLK;
                }
                
                Kernel_WriteFlightLog(FLIGHTLOG, FL_SYSCFLOCK, lock->l_start, lock->l_len, (((uint64_t)remoteFD)<<32) | lock->l_type, (((uint64_t)cmd2<<48) | ((uint64_t)cmd)<<32) | lock->l_whence);
                memcpy(&(requestMsg->lock), lock, sizeof(struct flock));
                break;
            }
            
            default:
                rc = CNK_RC_FAILURE(EINVAL);
                break;
        }
        
        // Exchange messages with sysiod if no errors found so far.
        if (CNK_RC_IS_SUCCESS(rc)) {
            void *inRegion;
            rc = exchange(requestMsg, &inRegion);
            
            if (CNK_RC_IS_SUCCESS(rc)) {
                FcntlAckMessage *replyMsg = (FcntlAckMessage *)inRegion;
                rc = CNK_RC_SUCCESS(replyMsg->retval);
                switch (cmd) {
                    case F_DUPFD:
                        // Setup duplicated descriptor.
                        File_SetFD(newfd, replyMsg->retval, File_GetFDType(fd));
                        rc = CNK_RC_SUCCESS(newfd);
                        break;
                        
                    case F_GETLK:
                        memcpy((void *)parm3, &(replyMsg->lock), sizeof(struct flock));
                        break;
                        
                    case F_SETLK:
                    case F_SETLKW:
                        struct flock *lock = (struct flock *)parm3;
                        if(lock->l_type == F_UNLCK)
                        {
                            FLM_Release(fd, lock->l_start, lock->l_len, lock->l_whence);
                        }
                        else
                        {
                            FLM_Acquire(fd, lock->l_start, lock->l_len, lock->l_whence);
                        }

                        break;
                }
            }
            else {
                if (cmd == F_DUPFD) {
                    File_FreeFD(newfd);
                }
            }
        }
        
        // Release the lock.
        Kernel_Unlock(&_lock);

        if(fcntl_lock_workaround)
        {
            if((cmd == F_SETLKW) && ((rc == CNK_RC_FAILURE(EAGAIN)) || (rc == CNK_RC_FAILURE(EACCES))))
                retry = true;
            
            if((cmd2 == F_SETLK) && (rc == CNK_RC_FAILURE(EDEADLK)))
                retry = true;
        }
    }
    while(retry);
    
    Kernel_WriteFlightLog(FLIGHTLOG, FL_FLOCKCOMP, rc,0,0,0);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::fcntl%s: fd=%d cmd=%d parm3=%lu rc=%s\n", whoami(), fd, cmd, parm3, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::fstat(int fd, struct stat *statbuf)
{
   return fstat64(fd, (struct stat64 *)statbuf);
}

uint64_t
sysioFS::fstat64(int fd, struct stat64 *statbuf)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Fstat64Message *requestMsg = (Fstat64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Fstat64, sizeof(Fstat64Message));
   requestMsg->fd = remoteFD;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Copy stat structure to caller's storage.
   if (CNK_RC_IS_SUCCESS(rc)) {
      Fstat64AckMessage *replyMsg = (Fstat64AckMessage *)inRegion;
      memcpy(statbuf, &(replyMsg->buf), sizeof(struct stat64));
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::fstat64%s: fd=%d bufp=%p rc=%s\n", whoami(), fd, statbuf, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::fstatfs(int fd, struct statfs *statbuf)
{
   return fstatfs64(fd, (struct statfs64 *)statbuf);
}

uint64_t
sysioFS::fstatfs64(int fd, struct statfs64 *statbuf)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Fstatfs64Message *requestMsg = (Fstatfs64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Fstatfs64, sizeof(Fstatfs64Message)); 
   requestMsg->fd = remoteFD;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Copy statfs structure to caller's storage.
   if (CNK_RC_IS_SUCCESS(rc)) {
      Fstatfs64AckMessage *replyMsg = (Fstatfs64AckMessage *)inRegion;
      memcpy(statbuf, &(replyMsg->buf), sizeof(struct statfs64));
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::fstatfs64%s: fd=%d bufp=%p rc=%s\n", whoami(), fd, statbuf, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::ftruncate(int fd, off_t length)
{
   return ftruncate64(fd, (off64_t)length);
}

uint64_t
sysioFS::ftruncate64(int fd, off64_t length)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Ftruncate64Message *requestMsg = (Ftruncate64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Ftruncate64, sizeof(Ftruncate64Message));
   requestMsg->fd = remoteFD;
   requestMsg->length = length;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::ftruncate64%s: fd=%d length=%ld rc=%s\n", whoami(), fd, length, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::fsync(int fd)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   FsyncMessage *requestMsg = (FsyncMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Fsync, sizeof(FsyncMessage));
   requestMsg->fd = remoteFD;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::fsync%s: fd=%d rc=%s\n", whoami(), fd, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::getdents(int fd, struct dirent *buffer, unsigned int length)
{
   return getdents64(fd, buffer, length);
}

uint64_t
sysioFS::getdents64(int fd, struct dirent *buffer, unsigned int length)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Getdents64Message *requestMsg = (Getdents64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Getdents64, sizeof(Getdents64Message));
   requestMsg->fd = remoteFD;
   requestMsg->length = length;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // When successful, set return code to number of bytes returned.
   if (CNK_RC_IS_SUCCESS(rc)) {
      Getdents64AckMessage *replyMsg = (Getdents64AckMessage *)inRegion;
      rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }

   TRACE( TRACE_SysioFS, ("(I) sysioFS::getdents64%s: fd=%d buffer=%p length=%u rc=%s\n", whoami(), fd, buffer, length, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(sockfd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Make sure the address length value is valid.
   if (*addrlen > MaxAddrLength) {
      return CNK_RC_FAILURE(EINVAL);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   GetpeernameMessage *requestMsg = (GetpeernameMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Getpeername, sizeof(GetpeernameMessage));
   requestMsg->sockfd = remoteFD;
   requestMsg->addrlen = *addrlen;

   // Put reply message in inbound message buffer.

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Copy socket address to caller's storage.
   if (CNK_RC_IS_SUCCESS(rc)) {
      GetpeernameAckMessage *replyMsg = (GetpeernameAckMessage *)inRegion;
      memcpy(addr, replyMsg->addr, replyMsg->addrlen);
      *addrlen = replyMsg->addrlen;
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::getpeername%s: sockfd=%d addr=0x%p addrlen=%d rc=%s\n", whoami(), sockfd, addr, *addrlen, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(sockfd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Make sure the address length value is valid.
   if (*addrlen > MaxAddrLength) {
      return CNK_RC_FAILURE(EINVAL);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   GetsocknameMessage *requestMsg = (GetsocknameMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Getsockname, sizeof(GetsocknameMessage));
   requestMsg->sockfd = remoteFD;
   requestMsg->addrlen = *addrlen;

   // Put reply message in inbound message buffer.

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Copy socket address to caller's storage.
   if (CNK_RC_IS_SUCCESS(rc)) {
      GetsocknameAckMessage *replyMsg = (GetsocknameAckMessage *)inRegion;
      memcpy(addr, replyMsg->addr, replyMsg->addrlen);
      *addrlen = replyMsg->addrlen;
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::getsockname%s: sockfd=%d addr=0x%p addrlen=%d rc=%s\n", whoami(), sockfd, addr, *addrlen, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(sockfd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Make sure the option length value is valid.
   if (*optlen > MaxOptionLength) {
      return CNK_RC_FAILURE(EINVAL);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   GetsockoptMessage *requestMsg = (GetsockoptMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Getsockopt, sizeof(GetsockoptMessage)); 
   requestMsg->sockfd = remoteFD;
   requestMsg->level = level;
   requestMsg->name = optname;
   requestMsg->optlen = *optlen;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Copy socket option to caller's storage.
   if (CNK_RC_IS_SUCCESS(rc)) {
      GetsockoptAckMessage *replyMsg = (GetsockoptAckMessage *)inRegion;
      memcpy(optval, replyMsg->value, replyMsg->optlen);
      *optlen = replyMsg->optlen;
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::getsockopt%s: sockfd=%d level=%d optname=%d optval=0x%p optlen=%d rc=%s\n",
                          whoami(), sockfd, level, optname, optval, *optlen, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::gpfsfcntl(int fd, const void *buffer, size_t length, int* result)
{
   // \note gpfsfcntl behaves according to SPI return codes.  Not errno.
   
   // Make sure file descriptor is valid.
   int rfd = File_GetRemoteFD(fd);
   if ( __UNLIKELY(rfd < 0) ) {
      return CNK_RC_SPI(EBADF);
   }
   
   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, (void *)buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (__UNLIKELY (err != 0) ){
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_SPI(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   GpfsFcntlMessage *requestMsg = (GpfsFcntlMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), GpfsFcntl, sizeof(GpfsFcntlMessage));
   requestMsg->fd = rfd;
   requestMsg->data_length = length;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;

   // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   GpfsFcntlAckMessage *replyMsg = (GpfsFcntlAckMessage *)inRegion;
   if (__LIKELY  (replyMsg->header.returnCode == bgcios::Success) ){
         *result = replyMsg->gpfsresult;
   }
   rc = CNK_RC_SPI(replyMsg->header.errorCode);
   
   // Release the lock.
   Kernel_Unlock(&_lock);

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }
   
   TRACE( TRACE_SysioFS, ("(I) sysioFS::gpfsFcntl%s: fd=%d buffer=%p length=%ld rc=%ld\n",
                          whoami(), fd, buffer, length, rc) );
   return rc;
}

uint64_t
sysioFS::ioctl(int fd, unsigned long int cmd, void *parm3)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   IoctlMessage *requestMsg = (IoctlMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Ioctl, sizeof(IoctlMessage));
   requestMsg->fd = remoteFD;
   requestMsg->cmd = cmd;

   uint64_t rc = CNK_RC_SUCCESS(0);
   switch (cmd) {
      case FIONBIO:
      case 0x800866a8: // LL_IOC_GETOBDCOUNT for Lustre
         if (!VMM_IsAppAddress(parm3, sizeof(int))) {
            rc = CNK_RC_FAILURE(EFAULT);
         }
         else {
            requestMsg->arg = *((int *)parm3);
         }
         break;

      case FIONREAD:
      case 0x800466af: // LL_IOC_GET_MDTIDX for Lustre
         if (!VMM_IsAppAddress(parm3, sizeof(int))) {
            rc = CNK_RC_FAILURE(EFAULT);
         }
         break;

      case TCGETA:
      case TCGETS:
      case 0x402c7413: // Special value used by isatty()
         if (!VMM_IsAppAddress(parm3, sizeof(struct termios))) {
            rc = CNK_RC_FAILURE(EFAULT);
         }
         break;


      default:
         rc = CNK_RC_FAILURE(EINVAL);
         break;
   }

   // Exchange messages with sysiod if no errors found so far.
   if (CNK_RC_IS_SUCCESS(rc)) {
      void *inRegion;
      rc = exchange(requestMsg, &inRegion);

      if (CNK_RC_IS_SUCCESS(rc)) {
         IoctlAckMessage *replyMsg = (IoctlAckMessage *)inRegion;
         rc = CNK_RC_SUCCESS(replyMsg->retval);
         switch (cmd) {
            case FIONREAD:
            case 0x800866a8: // LL_IOC_GETOBDCOUNT for Lustre
            case 0x800466af: // LL_IOC_GET_MDTIDX for Lustre
               memcpy(parm3, &(replyMsg->retval), sizeof(int));
               break;

            case TCGETS:
            case TCGETA:
               memcpy(parm3, &(replyMsg->termios), sizeof(struct termios));
               break;
         }
      }
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::ioctl%s: fd=%d cmd=0x%lx parm3=%p rc=%s\n", whoami(), fd, cmd, parm3, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::lchown(const char *pathname, uid_t uid, gid_t gid)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   ChownMessage *requestMsg = (ChownMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Chown, sizeof(ChownMessage));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->uid = uid;
   requestMsg->gid = gid;
   requestMsg->flags = AT_SYMLINK_NOFOLLOW;
   requestMsg->offset = sizeof(ChownMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::lchown%s: path=%s uid=%d gid=%d rc=%s\n", whoami(), pathname, uid, gid, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::link(const char *oldpathname, const char *newpathname)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   LinkMessage *requestMsg = (LinkMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Link, sizeof(LinkMessage));
   requestMsg->olddirfd = File_GetCurrentDirFD();
   requestMsg->newdirfd = File_GetCurrentDirFD();
   requestMsg->flags = 0;
   requestMsg->oldoffset = sizeof(LinkMessage);
   requestMsg->newoffset = requestMsg->oldoffset + strlen(oldpathname) + 1;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, oldpathname, newpathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::link%s: oldpath=%s newpath=%s rc=%s\n", whoami(), oldpathname, newpathname, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::listen(int sockfd, int backlog)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(sockfd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   ListenMessage *requestMsg = (ListenMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Listen, sizeof(ListenMessage));
   requestMsg->sockfd = remoteFD;
   requestMsg->backlog = backlog;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::listen%s: sockfd=%d backlog=%d rc=%s\n", whoami(), sockfd, backlog, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::lseek(int fd, off_t offset, int whence)
{
   off64_t result;
   uint64_t rc = llseek(fd, (off64_t)offset, &result, whence);
   if (CNK_RC_IS_SUCCESS(rc)) {
      rc = CNK_RC_SUCCESS((uint64_t) result);
   }
   return rc;
}

uint64_t
sysioFS::llseek(int fd, off64_t offset, off64_t *result, int whence)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Lseek64Message *requestMsg = (Lseek64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Lseek64, sizeof(Lseek64Message));
   requestMsg->fd = remoteFD;
   requestMsg->offset = offset;
   requestMsg->whence = whence;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Set result argument.
   if (CNK_RC_IS_SUCCESS(rc)) 
   {
       File_SetCurrentOffset(fd, *result);
       Lseek64AckMessage *replyMsg = (Lseek64AckMessage *)inRegion;
       *result = replyMsg->result;
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::lseek64%s: fd=%d offset=%lu whence=%d rc=%s result=%ld\n",
                          whoami(), fd, offset, whence, CNK_RC_STRING(rc), *result) );
   return rc;
}

uint64_t
sysioFS::lstat(const char *pathname, struct stat *statbuf)
{
   return lstat64(pathname, (struct stat64 *)statbuf);
}

uint64_t
sysioFS::lstat64(const char *pathname, struct stat64 *statbuf)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Stat64Message *requestMsg = (Stat64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Stat64, sizeof(Stat64Message));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->flags = AT_SYMLINK_NOFOLLOW;
   requestMsg->offset = sizeof(Stat64Message);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Copy stat structure to caller's storage.
   if (CNK_RC_IS_SUCCESS(rc)) {
      Stat64AckMessage *replyMsg = (Stat64AckMessage *)inRegion;
      memcpy(statbuf, &(replyMsg->buf), sizeof(struct stat64));
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::lstat64%s: path=%s statbuf=%p rc=%s\n", whoami(), pathname, statbuf, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::mkdir(const char *pathname, mode_t mode)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   MkdirMessage *requestMsg = (MkdirMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Mkdir, sizeof(MkdirMessage));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->mode = mode & ~(GetMyProcess()->CurrentUmask);
   requestMsg->offset = sizeof(MkdirMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::mkdir%s: path=%s mode=0x%08x rc=%s\n", whoami(), pathname, mode, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::open(const char *pathname, int flags, mode_t mode)
{
   // Allocate a descriptor before forwarding a message to sysiod.
   int fd = File_GetFD(0);
   if (fd == -1) {
      return CNK_RC_FAILURE(EMFILE);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   OpenMessage *requestMsg = (OpenMessage *)getSendBuffer(procID);
   if ( GetMyKThread()->KernelInternal == 1){
     fillHeader(&(requestMsg->header), OpenKernelInternal, sizeof(OpenMessage)); 
   }
   else{
     fillHeader(&(requestMsg->header), Open, sizeof(OpenMessage)); 
   }
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->flags = flags;
   requestMsg->mode = mode & ~(GetMyProcess()->CurrentUmask);
   requestMsg->offset = sizeof(OpenMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   if (CNK_RC_IS_SUCCESS(rc)) {
      OpenAckMessage *replyMsg = (OpenAckMessage *)inRegion;
      
#if CONFIG_AVOID_READLINK
       AppProcess_t* proc = GetMyProcess();
       if(proc)
       {
           proc->LAST_OPEN_FD = replyMsg->fd;
           strncpy(proc->LAST_OPEN_FILENAME, pathname, sizeof(proc->LAST_OPEN_FILENAME));
       }
#endif
       
      File_SetFD(fd, replyMsg->fd, FD_FILE);
      rc = CNK_RC_SUCCESS(fd);
   }
   else {
      File_FreeFD(fd);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::open%s: path='%s' flags=0x%08x mode=0x%08x rc=%s\n",
                          whoami(), pathname, flags, mode, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::poll(struct pollfd *fds, nfds_t nfds, int timeout)
{

   // Make sure limit on number of descriptors has not been exceeded.
   if (nfds > PollSize) {
      printf("EINVAL nfds=%d \n",(int)nfds);
      return CNK_RC_FAILURE(EINVAL);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);
   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   PollMessage *requestMsg = (PollMessage *)getSendBuffer(procID);

   requestMsg->pollBasic.nfd = nfds;
   fillHeader(&(requestMsg->header), Poll, sizeof(PollMessage));  
   requestMsg->pollBasic.timeout = timeout;

   for (int i = 0; i < (int)nfds; ++i) {
     requestMsg->pollBasic.fds[i].fd = File_GetRemoteFD(fds[i].fd);
     requestMsg->pollBasic.fds[i].events = fds[i].events;
     requestMsg->pollBasic.fds[i].revents = fds[i].revents;
     //printf("i=%d userfd=%d ionodefd=%d event=%d \n",i, fds[i].fd, requestMsg->pollBasic.fds[i].fd, fds[i].events);
   }

   // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

    PollAckMessage *replyMsg = (PollAckMessage *)inRegion;

   for (int i = 0; i < (int)nfds; ++i) {
     fds[i].revents = replyMsg->pollBasic.fds[i].revents;
     //printf("i=%d userfd=%d ionodefd=%d revent=%d \n",i, fds[i].fd, requestMsg->pollBasic.fds[i].fd, fds[i].revents);
   }
   //printf(" returnCode=%d errorCode=%d \n", replyMsg->header.returnCode,replyMsg->header.errorCode);

   if (replyMsg->header.returnCode==uint32_t(-1))
     rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   else {
     rc = CNK_RC_SUCCESS(replyMsg->header.returnCode);    
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   return rc;
}

uint64_t
sysioFS::pread64(int fd, void *buffer, size_t length, off64_t position)
{
   // Make sure file descriptor is valid.
   int rfd = File_GetRemoteFD(fd);
   if (rfd < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::pread64%s: buffer=%p length=%ld position=%lu\n", whoami(), buffer, length, position) );

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Pread64Message *requestMsg = (Pread64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Pread64, sizeof(Pread64Message));
   requestMsg->fd = rfd;
   requestMsg->length = length;
   requestMsg->position = position;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;

   // Exchange messages with I/O node.
   void *inRegion1;
   uint64_t rc = exchange(requestMsg, &inRegion1);

   // When successful, set return code to number of bytes returned.
   if (CNK_RC_IS_SUCCESS(rc)) {
      Pread64AckMessage *replyMsg = (Pread64AckMessage *)inRegion1;
      File_AdjustCurrentOffset(fd, replyMsg->bytes);
      rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }

   TRACE( TRACE_SysioFS, ("(I) sysioFS::pread64%s: fd=%d buffer=%p length=%ld position=%lu rc=%s\n",
                          whoami(), fd, buffer, length, position, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::pwrite64(int fd, const void *buffer, size_t length, off64_t position)
{
   // Make sure file descriptor is valid.
   int rfd = File_GetRemoteFD(fd);
   if (rfd < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, (void *)buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Pwrite64Message *requestMsg = (Pwrite64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Pwrite64, sizeof(Pwrite64Message));
   requestMsg->fd = rfd;
   requestMsg->length = length;
   requestMsg->position = position;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;

   // Post a receive for second reply message.

   // Exchange messages with I/O node.
   void *inRegion1;
   uint64_t rc = exchange(requestMsg, &inRegion1);

   // When successful, set return code to number of bytes returned.
   if (CNK_RC_IS_SUCCESS(rc)) 
   {       
       Pwrite64AckMessage *replyMsg = (Pwrite64AckMessage *)inRegion1;
       File_AdjustCurrentOffset(fd, replyMsg->bytes);
       rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }

   TRACE( TRACE_SysioFS, ("(I) sysioFS::pwrite64%s: fd=%d buffer=%p length=%ld position=%lu rc=%s\n",
                          whoami(), fd, buffer, length, position, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::read(int fd, void *buffer, size_t length)
{
   // Make sure file descriptor is valid.
   int rfd = File_GetRemoteFD(fd);
   if (rfd < 0) {
      return CNK_RC_FAILURE(EBADF);
   }
   
   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   ReadMessage *requestMsg = (ReadMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Read, sizeof(ReadMessage));
   requestMsg->fd = rfd;
   requestMsg->length = length;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;

   // Exchange messages with I/O node.
   TRACE( TRACE_SysioFS, ("(I) sysioFS::read%s buffer=0x%p length=%ld\n", whoami(), buffer, length) );
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   ReadAckMessage *replyMsg = (ReadAckMessage *)inRegion;
   if (replyMsg->header.returnCode == bgcios::Success) {
         File_AdjustCurrentOffset(fd, replyMsg->bytes);
         rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }

   TRACE( TRACE_SysioFS, ("(I) sysioFS::read%s: fd=%d buffer=0x%p length=%ld rc=%s\n", whoami(), fd, buffer, length, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t sysioFS::readv(int fd, const struct iovec *iov, int iovcnt)
{
    uint64_t rc = 0;
    uint64_t totalbytes = 0;
    int x;
    for(x=0; x<iovcnt; x++)
    {
        rc = sysioFS::read(fd, iov[x].iov_base, iov[x].iov_len);
        if (CNK_RC_IS_FAILURE(rc))
        {
            return rc;
        }
        totalbytes += rc;
    }
    return CNK_RC_SUCCESS(totalbytes);
}


uint64_t
sysioFS::readlink(const char *pathname, void *buffer, size_t length)
{
   // Truncate length to fit in inbound message buffer.
   if (length > bgcios::SmallMessageDataSize) {
      length = bgcios::SmallMessageDataSize;
   }

   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   ReadlinkMessage *requestMsg = (ReadlinkMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Readlink, sizeof(ReadlinkMessage));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->length = length;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;
   requestMsg->offset = sizeof(ReadlinkMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // When successful, set return code to length of returned path name.
   if (CNK_RC_IS_SUCCESS(rc)) {
      ReadlinkAckMessage *replyMsg = (ReadlinkAckMessage *)inRegion;
      rc = CNK_RC_SUCCESS(replyMsg->length);
   }

      // Release the lock.
      Kernel_Unlock(&_lock); 

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }

   TRACE( TRACE_SysioFS, ("(I) sysioFS::readlink%s: path='%s' link='%s' rc=%s\n", whoami(), pathname, (char *)buffer, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::recv(int sockfd, void *buffer, size_t length, int flags)
{
   // Make sure file descriptor is valid.
   int rfd = File_GetRemoteFD(sockfd);
   if (rfd < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   RecvMessage *requestMsg = (RecvMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Recv, sizeof(RecvMessage));
   requestMsg->sockfd = rfd;
   requestMsg->length = length;
   requestMsg->flags = flags;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;

   // Exchange messages with I/O node.
   TRACE( TRACE_SysioFS, ("(I) sysioFS::recv%s buffer=0x%p length=%ld\n", whoami(), buffer, length) );
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   RecvAckMessage *replyMsg = (RecvAckMessage *)inRegion;

   if (replyMsg->header.returnCode == bgcios::Success) {
         rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }
    
    // Release the lock.
    Kernel_Unlock(&_lock);

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }

   TRACE( TRACE_SysioFS, ("(I) sysioFS::recv%s: sockfd=%d buffer=0x%p length=%ld flags=%d rc=%s\n", whoami(), sockfd, buffer, length, flags, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::recvfrom(int sockfd, void *buffer, size_t length, int flags, struct sockaddr *addr, socklen_t *addrlen)
{
   // Make sure file descriptor is valid.
   int rfd = File_GetRemoteFD(sockfd);
   if (rfd < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Make sure the address length value is valid.
   if (*addrlen > MaxAddrLength) {
      return CNK_RC_FAILURE(EINVAL);
   }

   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   RecvfromMessage *requestMsg = (RecvfromMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Recvfrom, sizeof(RecvfromMessage));
   requestMsg->sockfd = rfd;
   requestMsg->length = length;
   requestMsg->flags = flags;
   requestMsg->addrlen = *addrlen;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;

   // Exchange messages with I/O node.
   TRACE( TRACE_SysioFS, ("(I) sysioFS::recv%s buffer=0x%p length=%ld\n", whoami(), buffer, length) );
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Wait for the final result.
   RecvfromAckMessage *replyMsg = (RecvfromAckMessage *)inRegion;


   if (replyMsg->header.returnCode == bgcios::Success) {
         rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }
    
   // Release the lock.
   Kernel_Unlock(&_lock);

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }

   TRACE( TRACE_SysioFS, ("(I) sysioFS::recvfrom%s: sockfd=%d buffer=0x%p length=%ld flags=%d addr=0x%p addrlen=%d rc=%s\n",
                          whoami(), sockfd, buffer, length, flags, addr, *addrlen, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::rename(const char *oldpathname, const char *newpathname)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   RenameMessage *requestMsg = (RenameMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Rename, sizeof(RenameMessage));
   requestMsg->olddirfd = File_GetCurrentDirFD();
   requestMsg->newdirfd = File_GetCurrentDirFD();
   requestMsg->oldoffset = sizeof(RenameMessage);
   requestMsg->newoffset = requestMsg->oldoffset + strlen(oldpathname) + 1;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, oldpathname, newpathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::rename%s: oldpath=%s newpath=%s rc=%s\n", whoami(), oldpathname, newpathname, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::rmdir(const char *pathname)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   UnlinkMessage *requestMsg = (UnlinkMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Unlink, sizeof(UnlinkMessage));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->flags = AT_REMOVEDIR;
   requestMsg->offset = sizeof(UnlinkMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::rmdir%s: path=%s rc=%s\n", whoami(), pathname, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::send(int sockfd, const void *buffer, size_t length, int flags)
{
   // Make sure socket descriptor is valid.
   int rfd = File_GetRemoteFD(sockfd);
   if (rfd < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, (void *)buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   SendMessage *requestMsg = (SendMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Send, sizeof(SendMessage));
   requestMsg->sockfd = rfd;
   requestMsg->length = length;
   requestMsg->flags = flags;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;

   // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);


   // Wait for the final result.
   SendAckMessage *replyMsg = (SendAckMessage *)inRegion;

   if (replyMsg->header.returnCode == bgcios::Success) {
         rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }
    
   // Release the lock.
   Kernel_Unlock(&_lock);

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }

   TRACE( TRACE_SysioFS, ("(I) sysioFS::send%s: sockfd=%d buffer=0x%p length=%ld flags=%d rc=%s\n",
                          whoami(), sockfd, buffer, length, flags, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::sendto(int sockfd, const void *buffer, size_t length, int flags, const struct sockaddr *addr, socklen_t addrlen)
{
   // Make sure socket descriptor is valid.
   int rfd = File_GetRemoteFD(sockfd);
   if (rfd < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, (void *)buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   SendtoMessage *requestMsg = (SendtoMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Sendto, sizeof(SendtoMessage));
   requestMsg->sockfd = rfd;
   requestMsg->length = length;
   requestMsg->flags = flags;
   memcpy(requestMsg->addr, addr, addrlen);
   requestMsg->addrlen = addrlen;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;

   // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

#if 0
   // When successful, set return code to number of bytes returned.
   if (CNK_RC_IS_SUCCESS(rc)) {
      SendtoAckMessage *replyMsg = (SendtoAckMessage *)inRegion1;
      rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }
#endif

   // Wait for the final result.
   SendtoAckMessage *replyMsg = (SendtoAckMessage *)inRegion;

   if (replyMsg->header.returnCode == bgcios::Success) {
         rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }
    
   // Release the lock.
   Kernel_Unlock(&_lock);

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }

   TRACE( TRACE_SysioFS, ("(I) sysioFS::sendto%s: sockfd=%d buffer=0x%p length=%ld flags=%d addr=0x%p addrlen=%d rc=%s\n",
                          whoami(), sockfd, buffer, length, flags, addr, addrlen, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(sockfd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Make sure the option length value is valid.
   if (optlen > MaxOptionLength) {
      return CNK_RC_FAILURE(EINVAL);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   SetsockoptMessage *requestMsg = (SetsockoptMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Setsockopt, sizeof(SetsockoptMessage));
   requestMsg->sockfd = remoteFD;
   requestMsg->level = level;
   requestMsg->name = optname;
   requestMsg->optlen = optlen;
   memcpy(requestMsg->value, optval, optlen);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::setsockopt%s: sockfd=%d level=%d optname=%d optval=0x%p optlen=%d rc=%s\n",
                          whoami(), sockfd, level, optname, optval, optlen, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::shutdown(int sockfd, int how)
{
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(sockfd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   ShutdownMessage *requestMsg = (ShutdownMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Shutdown, sizeof(ShutdownMessage));
   requestMsg->sockfd = remoteFD;
   requestMsg->how = how;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::shutdown%s: sockfd=%d how=%d rc=%s\n", whoami(), sockfd, how, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::socket(int domain, int type, int protocol)
{
   // Allocate a descriptor before forwarding a message to sysiod.
   int fd = File_GetFD(0);
   if (fd == -1) {
      return CNK_RC_FAILURE(EMFILE);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   SocketMessage *requestMsg = (SocketMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Socket, sizeof(SocketMessage));
   requestMsg->domain = domain;
   requestMsg->type = type;
   requestMsg->protocol = protocol;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   if (CNK_RC_IS_SUCCESS(rc)) {
      SocketAckMessage *replyMsg = (SocketAckMessage *)inRegion;
      File_SetFD(fd, replyMsg->sockfd, FD_SOCKET);
      rc = CNK_RC_SUCCESS(fd);
   }
   else {
      File_FreeFD(fd);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::socket%s: domain=%d type=%d protocol=%d rc=%s\n", whoami(), domain, type, protocol, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::stat(const char *pathname, struct stat *statbuf)
{
   return stat64(pathname, (struct stat64 *)statbuf);
}

uint64_t
sysioFS::stat64(const char *pathname, struct stat64 *statbuf)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Stat64Message *requestMsg = (Stat64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Stat64, sizeof(Stat64Message));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->flags = 0;
   requestMsg->offset = sizeof(Stat64Message);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Copy stat structure to caller's storage.
   if (CNK_RC_IS_SUCCESS(rc)) {
      Stat64AckMessage *replyMsg = (Stat64AckMessage *)inRegion;
      memcpy(statbuf, &(replyMsg->buf), sizeof(struct stat64));
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::stat64%s: path=%s statbuf=%p rc=%s\n", whoami(), pathname, statbuf, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::statfs(const char *pathname, struct statfs *statbuf)
{
   return statfs64(pathname, (struct statfs64 *)statbuf);
}

uint64_t
sysioFS::statfs64(const char *pathname, struct statfs64 *statbuf)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Statfs64Message *requestMsg = (Statfs64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Statfs64, sizeof(Statfs64Message));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->offset = sizeof(Statfs64Message);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Copy statfs structure to caller's storage.
   if (CNK_RC_IS_SUCCESS(rc)) {
      Statfs64AckMessage *replyMsg = (Statfs64AckMessage *)inRegion;
      memcpy(statbuf, &(replyMsg->buf), sizeof(struct statfs64));
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::statfs64%s: path=%s bufp=%p rc=%s\n", whoami(), pathname, statbuf, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::symlink(const char *oldpathname, const char *newpathname)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   SymlinkMessage *requestMsg = (SymlinkMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Symlink, sizeof(SymlinkMessage));
   requestMsg->newdirfd = File_GetCurrentDirFD();
   requestMsg->oldoffset = sizeof(SymlinkMessage);
   requestMsg->newoffset = requestMsg->oldoffset + strlen(oldpathname) + 1;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, oldpathname, newpathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::symlink%s: oldpath=%s newpath=%s rc=%s\n", whoami(), oldpathname, newpathname, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::truncate(const char* pathname, off_t length)
{
   return truncate64(pathname, (off64_t)length);
}

uint64_t
sysioFS::truncate64(const char *pathname, off64_t length)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   Truncate64Message *requestMsg = (Truncate64Message *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Truncate64, sizeof(Truncate64Message));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->length = length;
   requestMsg->offset = sizeof(Truncate64Message);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::truncate64%s: path='%s' len=%lu rc=%s\n", whoami(), pathname, length, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::unlink(const char *pathname)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   UnlinkMessage *requestMsg = (UnlinkMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Unlink, sizeof(UnlinkMessage));
   requestMsg->dirfd = File_GetCurrentDirFD();
   requestMsg->flags = 0;
   requestMsg->offset = sizeof(UnlinkMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::unlink%s: path=%s rc=%s\n", whoami(), pathname, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::utime(const char *pathname, const struct utimbuf *buf)
{
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   UtimesMessage *requestMsg = (UtimesMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), Utimes, sizeof(UtimesMessage));
   requestMsg->dirfd = File_GetCurrentDirFD();
   if (buf == NULL) {
      requestMsg->now = true;
      requestMsg->newtimes[0].tv_sec = 0;
      requestMsg->newtimes[0].tv_usec = 0;
      requestMsg->newtimes[1].tv_sec = 0;
      requestMsg->newtimes[1].tv_usec = 0;
   }
   else {
      requestMsg->now = false;
      requestMsg->newtimes[0].tv_sec = buf->actime;
      requestMsg->newtimes[0].tv_usec = 0;
      requestMsg->newtimes[1].tv_sec = buf->modtime;
      requestMsg->newtimes[1].tv_usec = 0;
   }
   requestMsg->offset = sizeof(UtimesMessage);

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, pathname, &inRegion);

   // Release the lock.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_SysioFS, ("(I) sysioFS::utime%s: path=%s now=%u actime=0x%08lx modtime=0x%08lx rc=%s\n", whoami(), pathname,
                          requestMsg->now, requestMsg->newtimes[0].tv_sec, requestMsg->newtimes[1].tv_sec, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::write(int fd, const void *buffer, size_t length)
{

#if 0
   if (length <= 480){
     return writeImmediate(fd, buffer, length);
   }
#endif
   // Make sure file descriptor is valid.
   int rfd = File_GetRemoteFD(fd);
   if ( __UNLIKELY(rfd < 0) ) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Register a memory region for the caller's data.
   struct cnv_mr userRegion;
   int err = cnv_reg_mr(&userRegion, &_protectionDomain, (void *)buffer, length, CNV_ACCESS_LOCAL_WRITE);
   if (__UNLIKELY (err != 0) ){
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)buffer, length, _protectionDomain.handle, err);
      return CNK_RC_FAILURE(err);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   WriteMessage *requestMsg = (WriteMessage *)getSendBuffer(procID);
   if(__UNLIKELY  ( GetMyKThread()->KernelInternal == 1)){
     fillHeader(&(requestMsg->header), WriteKernelInternal, sizeof(WriteMessage));
   }
   else{
     fillHeader(&(requestMsg->header), Write, sizeof(WriteMessage));
   }
   requestMsg->fd = rfd;
   requestMsg->data_length = length;
   requestMsg->address = (uint64_t)userRegion.addr;
   requestMsg->rkey = userRegion.rkey;

   // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   WriteAckMessage *replyMsg = (WriteAckMessage *)inRegion;
   if (__LIKELY  (replyMsg->header.returnCode == bgcios::Success) ){
         File_AdjustCurrentOffset(fd, replyMsg->bytes);
         rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   // Deregister memory region for caller's data.
   err = cnv_dereg_mr(&userRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
   }

   TRACE( TRACE_SysioFS, ("(I) sysioFS::write%s: fd=%d buffer=%p length=%ld rc=%s\n",
                          whoami(), fd, buffer, length, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::writev(int fd, const struct iovec *iov, int iovcnt)
{
   // Make this simple -- write each vector individually.
   int bytesWritten = 0;
   for ( int index = 0 ; index < iovcnt ; ++index ) 
   {
       if((iov[index].iov_base == NULL) && (iov[index].iov_len == 0))
           continue;
       uint64_t rc = write(fd, iov[index].iov_base, iov[index].iov_len);
       if (CNK_RC_IS_FAILURE(rc)) {
           return rc;
       }
       bytesWritten += CNK_RC_VALUE(rc);
   }

   return CNK_RC_SUCCESS(bytesWritten);
}


uint64_t
sysioFS::writeImmediate(int fd, const void *buffer, size_t length)
{
   // Make sure file descriptor is valid.
   int rfd = File_GetRemoteFD(fd);
   if (rfd < 0) {
      return CNK_RC_FAILURE(EBADF);
   }
   if (length > 480){
      return CNK_RC_FAILURE(ERANGE);
   }

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   WriteImmediateMessage *requestMsg = (WriteImmediateMessage *)getSendBuffer(procID);
   if ( GetMyKThread()->KernelInternal == 1){
     fillHeader(&(requestMsg->header), WriteImmediateKernelInternal, sizeof(requestMsg->header)+length);
   }
   else{
     fillHeader(&(requestMsg->header), WriteImmediate, sizeof(requestMsg->header)+length);
   }
   requestMsg->header.returnCode= (uint32_t)rfd;
   memcpy(requestMsg->data,buffer,length);

   void *inRegion;
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);
   uint64_t rc = exchangeInline(requestMsg, &inRegion);
   // Release the lock.
   Kernel_Unlock(&_lock);

   WriteImmediateAckMessage *replyMsg = (WriteImmediateAckMessage *)inRegion;
   if (bgcios::RequestIncomplete == replyMsg->header.returnCode){
      rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }
   else if (bgcios::RequestFailed == replyMsg->header.returnCode){
     rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }
   else if (replyMsg->header.returnCode == bgcios::Success) {
         rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }
   else {
         rc = CNK_RC_SUCCESS(replyMsg->bytes);
   }


   return rc;
}


bool 
sysioFS::isMatch(const char *path)
{
   return ((IsAppAgent()) ? false : true);
}

int 
sysioFS::sendx(char * mInput){
  struct MsgInputs * msgInput = (struct MsgInputs *)mInput;
  if (!VMM_IsAppAddress(msgInput, sizeof(struct MsgInputs) ) ) {
            return CNK_RC_FAILURE(EFAULT);
  }
  // check mInput address is valid and length for user
  if (msgInput->options==MSG_DATA_PLUS ){
    return sendxDataPlus(msgInput);
  }
  else if (msgInput->options==MSG_DATA_ONLY){
    return sendxDataOnly(msgInput);
  }
  else {
    return CNK_RC_FAILURE(ENOSYS);
  }
}


int 
sysioFS::sendxDataOnly(struct MsgInputs * msgInput){

  if ( (msgInput->data_length>0) && !VMM_IsAppAddress(msgInput->dataRegion,msgInput->data_length ) ) {
            return CNK_RC_FAILURE(EFAULT);
  }
  if ( msgInput->data_length > UserMessageDataSize ){
        return CNK_RC_FAILURE(ERANGE);
  }
  if ( (msgInput->recv_length>0) && !VMM_IsAppAddress(msgInput->recvMessage,msgInput->recv_length ) ) {
            return CNK_RC_FAILURE(EFAULT);
  }
  //printf("sendxDataOnly msgInput->recv_length=%ld\n",(long int)msgInput->recv_length);
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   bgcios::UserMessage * requestMsg = (bgcios::UserMessage *)getSendBuffer(procID);
   fillUserHeader(requestMsg, msgInput->version, msgInput->type, bgcios::SysioUserService);
   if (msgInput->data_length){
      memcpy(requestMsg->MessageData,msgInput->dataRegion,msgInput->data_length);
      requestMsg->header.length = sizeof(bgcios::MessageHeader) + msgInput->data_length;
   }
   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);
   bgcios::UserMessage *replyMsg = (bgcios::UserMessage *)inRegion;
   //printf("UserMessage rc=%lld \n",(long long unsigned int)rc);
   //printf(" msgInput->recv_length=%lld ",(long long unsigned int)msgInput->recv_length);
   //printf(" replyMsg->header.length=%lld ",(long long unsigned int)replyMsg->header.length);
      if (msgInput->recv_length){
        
        if (msgInput->recv_length<replyMsg->header.length)
          memcpy(msgInput->recvMessage, replyMsg, msgInput->recv_length);
        else
          memcpy(msgInput->recvMessage, replyMsg, replyMsg->header.length ); 
      }

   // Release the lock.
   Kernel_Unlock(&_lock);

   return rc;

}

int 
sysioFS::sendxDataPlus(struct MsgInputs * msgInput){

  if ( (msgInput->data_length>0) && !VMM_IsAppAddress(msgInput->dataRegion,msgInput->data_length ) ) {
            return CNK_RC_FAILURE(EFAULT);
  }
  if ( msgInput->data_length > UserMessageFdRDMADataSize){
        return CNK_RC_FAILURE(ERANGE);
  }
  if ( (msgInput->recv_length>0) && !VMM_IsAppAddress(msgInput->recvMessage,msgInput->recv_length ) ) {
            return CNK_RC_FAILURE(EFAULT);
  }
  if ( msgInput->numberOfRdmaRegions > MostRdmaRegions){
        return CNK_RC_FAILURE(ERANGE);
  }

   // Make sure file descriptor is valid.
   int rfd1 = 0;
   if (msgInput->cnkFileDescriptor[0]){
      rfd1 = File_GetRemoteFD(msgInput->cnkFileDescriptor[0]);
      if (rfd1 < 0) {
         return CNK_RC_FAILURE(EBADF);
      }
   }
   int rfd2 = 0;
   if (msgInput->cnkFileDescriptor[1]){
      rfd2 = File_GetRemoteFD(msgInput->cnkFileDescriptor[1]);
      if (rfd2 < 0) {
         return CNK_RC_FAILURE(EBADF);
      }
   }


   for (int i=0;i<msgInput->numberOfRdmaRegions;i++){
      if (!VMM_IsAppAddress( msgInput->cnkUserRDMA[i].cnk_address, msgInput->cnkUserRDMA[i].cnk_bytes) )
         return CNK_RC_FAILURE(EFAULT);
   }
   
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Register a memory region for the caller's data.
   struct cnv_mr userRegion[msgInput->numberOfRdmaRegions];
   
   int err = 0;
   for (int i=0;i<msgInput->numberOfRdmaRegions;i++){
     err = cnv_reg_mr(userRegion+i, &_protectionDomain, msgInput->cnkUserRDMA[i].cnk_address, msgInput->cnkUserRDMA[i].cnk_bytes, CNV_ACCESS_LOCAL_WRITE);
     if (__UNLIKELY (err != 0) ){
       Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)msgInput->cnkUserRDMA[i].cnk_address, msgInput->cnkUserRDMA[i].cnk_bytes, _protectionDomain.handle, err);
       for(int k=0;k<i;k++){
          // Deregister memory region for caller's data.
          cnv_dereg_mr(userRegion+k);
       }
       // Release the lock.
       Kernel_Unlock(&_lock);
       return CNK_RC_FAILURE(err);
     };
   }


   // Build request message in outbound message buffer.
   int procID=ProcessorID();
   bgcios::UserMessageFdRDMA * requestMsg = (bgcios::UserMessageFdRDMA *)getSendBuffer(procID);
   fillUserHeader( (bgcios::UserMessage *)requestMsg, msgInput->version, msgInput->type, bgcios::SysioUserServiceFdRDMA);
   requestMsg->header.length = sizeof(bgcios::UserMessageFdRDMA) - UserMessageFdRDMADataSize;
   requestMsg->ionode_fd[0]=rfd1;
   requestMsg->ionode_fd[1]=rfd2;

   requestMsg->numberOfRdmaRegions = msgInput->numberOfRdmaRegions;
   for (int i=0;i<msgInput->numberOfRdmaRegions;i++){
     requestMsg->uRDMA[i].cnk_address = (uint64_t)userRegion[i].addr;
     requestMsg->uRDMA[i].cnk_bytes = msgInput->cnkUserRDMA[i].cnk_bytes;
     requestMsg->uRDMA[i].cnk_memkey = userRegion[i].rkey;
   }

   if (msgInput->data_length){
      memcpy(requestMsg->MessageData,msgInput->dataRegion,msgInput->data_length);
      requestMsg->header.length +=  msgInput->data_length;
   }

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   for(int k=0;k<msgInput->numberOfRdmaRegions;k++){
      // Deregister memory region for caller's data.
      cnv_dereg_mr(userRegion+k);
   }
   //bgcios::UserMessage *replyMsg = (bgcios::UserMessage *)inRegion;
   //printf("rc=%ld\n",rc);
   // Copy response message to caller storage.
   if (msgInput->recv_length){
        bgcios::UserMessage *replyMsg = (bgcios::UserMessage *)inRegion;
        if (msgInput->recv_length<replyMsg->header.length)
          memcpy(msgInput->recvMessage, replyMsg, msgInput->recv_length);
        else
          memcpy(msgInput->recvMessage, replyMsg, replyMsg->header.length ); 
   }

   // Release the lock.
   Kernel_Unlock(&_lock);

   return rc;

}

uint64_t
sysioFS::pathgetXattr(const char *path, const char *name, void *value, size_t size, uint16_t type){
   if (name==NULL) return CNK_RC_FAILURE(EINVAL);
   int nameLen = strlen(name);
   if (nameLen==0) return CNK_RC_FAILURE(EINVAL); 

   if (path==NULL) return CNK_RC_FAILURE(EINVAL);
   int pathLen = strlen(path);
   if (pathLen==0) return CNK_RC_FAILURE(EINVAL);

   // Register a memory region .
   struct cnv_mr userRegion;
   //An empty buffer of size 0 can be passed to get the current size
   if ( (size!=0) && (value!=NULL) ){
     int err = cnv_reg_mr(&userRegion, &_protectionDomain, (void *)value, size, CNV_ACCESS_LOCAL_WRITE);
     if (__UNLIKELY (err != 0) ){
        Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)value, size, _protectionDomain.handle, err);
        return CNK_RC_FAILURE(err);
     }
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);
   int procID=ProcessorID();
   FretrieveXattrMessage *requestMsg = (FretrieveXattrMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), type,  sizeof(FretrieveXattrMessage));
   if ( (size!=0) && (value!=NULL) ){
     requestMsg->address = (uint64_t)userRegion.addr;
     requestMsg->rkey = userRegion.rkey;
     requestMsg->userListNumBytes = (uint64_t)size;
   }
   else {
     requestMsg->address = 0;
     requestMsg->rkey = 0;
     requestMsg->userListNumBytes = 0;
   }
   requestMsg->fd= -1;
   memcpy(requestMsg->pathname,path,pathLen+1);
   requestMsg->header.length += pathLen+1;
   memcpy(requestMsg->pathname+pathLen+1,name,nameLen+1);
   requestMsg->header.length += nameLen + 1;
   requestMsg->nameSize = nameLen;
   requestMsg->pathSize = pathLen;

      // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg,&inRegion);

   if ( (size!=0) && (value!=NULL) ){
     // Deregister memory region for caller's data.
     int err = cnv_dereg_mr(&userRegion);
     if (err != 0) {
        Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
     }
   }

   FxattrMessageAck *replyMsg = (FxattrMessageAck *)inRegion;
   if (__LIKELY  (replyMsg->header.returnCode == bgcios::Success) ){
         rc = CNK_RC_SUCCESS(replyMsg->returnValue);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }
   // Release the lock.
   Kernel_Unlock(&_lock);
   return rc;
}

uint64_t 
sysioFS::pathlistXattr(const char *path, char *list, size_t size, uint16_t type){
   if (path==NULL) return CNK_RC_FAILURE(EINVAL);
   int pathLen = strlen(path);
   if (pathLen==0) return CNK_RC_FAILURE(EINVAL);

   // Register a memory region .
   struct cnv_mr userRegion;
   //An empty buffer of size 0 can be passed to get the current size of the named extended attribute
   if ( (size!=0) && (list!=NULL) ){
     int err = cnv_reg_mr(&userRegion, &_protectionDomain, (void *)list, size, CNV_ACCESS_LOCAL_WRITE);
     if (__UNLIKELY (err != 0) ){
        Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)list, size, _protectionDomain.handle, err);
        return CNK_RC_FAILURE(err);
     }
   } 


   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);
   int procID=ProcessorID();
   FretrieveXattrMessage *requestMsg = (FretrieveXattrMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), type,  sizeof(FretrieveXattrMessage));

   if ( (size!=0) && (list!=NULL) ){
     requestMsg->address = (uint64_t)userRegion.addr;
     requestMsg->rkey = userRegion.rkey;
     requestMsg->userListNumBytes = (uint64_t)size;
   }
   else {
     requestMsg->address = 0;
     requestMsg->rkey = 0;
     requestMsg->userListNumBytes = 0;
   }

   requestMsg->fd= -1;
   requestMsg->nameSize = 0;
   requestMsg->pathSize = pathLen;
   memcpy(requestMsg->pathname,path,pathLen+1);
   requestMsg->header.length += pathLen+1;

      // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);


   // Deregister memory region for caller's data.
   if ( (size!=0) && (list!=NULL) ){
     int err = cnv_dereg_mr(&userRegion);
     if (err != 0) {
        Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
     }
   }

   FxattrMessageAck *replyMsg = (FxattrMessageAck *)inRegion;
   if (__LIKELY  (replyMsg->header.returnCode == bgcios::Success) ){
         rc = CNK_RC_SUCCESS(replyMsg->returnValue);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }
   // Release the lock.
   Kernel_Unlock(&_lock);
   return rc;
}

uint64_t 
sysioFS::pathsetXattr(const char *path, const char *name, const void *value, size_t size, int flags, uint16_t type){
   if (name==NULL) return CNK_RC_FAILURE(EINVAL);
   int nameLen = strlen(name);
   if (nameLen==0) return CNK_RC_FAILURE(EINVAL); 

   if (path==NULL) return CNK_RC_FAILURE(EINVAL);
   int pathLen = strlen(path);
   if (pathLen==0) return CNK_RC_FAILURE(EINVAL);
   
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);
   int procID=ProcessorID();
   PathSetXattrMessage *requestMsg = (PathSetXattrMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), type,  sizeof(PathSetXattrMessage));
   requestMsg->flags = flags;
   requestMsg->valueSize = 0;
   requestMsg->nameSize = nameLen;
   requestMsg->pathSize = pathLen;
   if (value){
     if (size){
        requestMsg->valueSize = size;
        requestMsg->header.length += size;
        memcpy(requestMsg->value,value,size);
     }
   }
   
   // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, path, name, &inRegion);
   
   FxattrMessageAck *replyMsg = (FxattrMessageAck *)inRegion;
   if (__LIKELY  (replyMsg->header.returnCode == bgcios::Success) ){
         rc = CNK_RC_SUCCESS(0);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);
   return rc;
} 

uint64_t 
sysioFS::pathRemoveXattr(const char *path, const char *name, uint16_t type){
   if (name==NULL) return CNK_RC_FAILURE(EINVAL);
   int nameLen = strlen(name);
   if (nameLen==0) return CNK_RC_FAILURE(EINVAL); 

   if (path==NULL) return CNK_RC_FAILURE(EINVAL);
   int pathLen = strlen(path);
   if (pathLen==0) return CNK_RC_FAILURE(EINVAL);
   
   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);
   int procID=ProcessorID();
   PathRemoveXattrMessage *requestMsg = (PathRemoveXattrMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), type,  sizeof(PathRemoveXattrMessage));
   requestMsg->nameSize = nameLen;
   requestMsg->pathSize = pathLen;
   
   // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, path, name, &inRegion);
   
   FxattrMessageAck *replyMsg = (FxattrMessageAck *)inRegion;
   if (__LIKELY  (replyMsg->header.returnCode == bgcios::Success) ){
         rc = CNK_RC_SUCCESS(0);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);
   return rc;
} 

uint64_t 
sysioFS::fxattr_setOrRemove(int fd, const char *name, uint16_t type, const void *value, size_t size, int flags){
   int rfd = File_GetRemoteFD(fd);
   if ( __UNLIKELY(rfd < 0) ) {
      return CNK_RC_FAILURE(EBADF);
   }
   if (name==NULL) return CNK_RC_FAILURE(EINVAL);
   int nameLen = strlen(name);
   if (nameLen==0) return CNK_RC_FAILURE(EINVAL);

   //printf("name=%s\n",name);
   //printf("nameLen=%d\n",nameLen);

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);
   int procID=ProcessorID();
   FsetOrRemoveXattrMessage *requestMsg = (FsetOrRemoveXattrMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), type,  sizeof(FsetOrRemoveXattrMessage));
   requestMsg->nameSize = nameLen;
   requestMsg->fd=rfd;
   requestMsg->flags = flags;

   requestMsg->valueSize=0;
   if (value){
     if (size){
       requestMsg->valueSize=size;
       requestMsg->header.length += size;
       memcpy(requestMsg->value,value,size);
     }
   }
   char * namecpy = requestMsg->value + size;
   memcpy(namecpy,name,requestMsg->nameSize+1);
   requestMsg->header.length += requestMsg->nameSize + 1;
   
   // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   FxattrMessageAck *replyMsg = (FxattrMessageAck *)inRegion;
   if (__LIKELY  (replyMsg->header.returnCode == bgcios::Success) ){
         rc = CNK_RC_SUCCESS(0);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }

   // Release the lock.
   Kernel_Unlock(&_lock);
   return rc;
}

uint64_t 
sysioFS::fxattr_retrieve(int fd, const char *name, uint16_t type, void *target, size_t size){
   int rfd = File_GetRemoteFD(fd);
   if ( __UNLIKELY(rfd < 0) ) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Register a memory region .
   struct cnv_mr userRegion;
   //An empty buffer of size 0 can be passed to get the current size of the named extended attribute
   if ( (size!=0) && (target!=NULL) ){
     int err = cnv_reg_mr(&userRegion, &_protectionDomain, (void *)target, size, CNV_ACCESS_LOCAL_WRITE);
     if (__UNLIKELY (err != 0) ){
        Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVREGMRE, (uint64_t)target, size, _protectionDomain.handle, err);
        return CNK_RC_FAILURE(err);
     }
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);
   int procID=ProcessorID();
   FretrieveXattrMessage *requestMsg = (FretrieveXattrMessage *)getSendBuffer(procID);
   fillHeader(&(requestMsg->header), type,  sizeof(FretrieveXattrMessage));
   if ( (size!=0) && (target!=NULL) ){
     requestMsg->address = (uint64_t)userRegion.addr;
     requestMsg->rkey = userRegion.rkey;
     requestMsg->userListNumBytes = (uint64_t)size;
   }
   else {
     requestMsg->address = 0;
     requestMsg->rkey = 0;
     requestMsg->userListNumBytes = 0;
   }

   requestMsg->fd=rfd;

   if (name) {
     requestMsg->nameSize = strlen(name);
     memcpy(requestMsg->name,name,requestMsg->nameSize + 1);
     requestMsg->header.length += requestMsg->nameSize + 1;
   }
   else {
     requestMsg->nameSize = 0;
   }
   requestMsg->pathSize = 0;
      // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc =0;
   if (requestMsg->nameSize){
     rc = exchange(requestMsg, name, &inRegion);
   }
   else {
     rc = exchange(requestMsg, &inRegion);
   }

   FxattrMessageAck *replyMsg = (FxattrMessageAck *)inRegion;
   if (__LIKELY  (replyMsg->header.returnCode == bgcios::Success) ){
         rc = CNK_RC_SUCCESS(replyMsg->returnValue);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }

   // Deregister memory region for caller's data.
   if ( (size!=0) && (target!=NULL) ){
     int err = cnv_dereg_mr(&userRegion);
     if (err != 0) {
        Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)userRegion.addr, userRegion.length, userRegion.lkey, err);
     }
   }
   // Release the lock.
   Kernel_Unlock(&_lock);
   return rc;
}
