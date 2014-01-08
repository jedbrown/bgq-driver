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


using namespace bgcios::sysio;

int
sysioFS::init(void)
{
   // Make sure lock is reset.
   fetch_and_and(&_lock, 0);

   // Start sequence ids from 1.
   _sequenceId = 1;
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

   // Create a completion queue.
   if (  ( err = cnv_create_cq(&_completionQ, _context, CNV_MAX_WC) )  ) return err;
   if (  ( err = cnv_modify_cq_character(&_completionQ,CNVERBS_SEQUENCEID_CQ_CHAR) ) ) return err;

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
   if (err != 0) {
       RASBEGIN(2);
       RASPUSH(NodeState.ServiceDeviceAddr);
       RASPUSH(destAddress.sin_port);
       RASFINAL(RAS_KERNELCNVCONNECTFAIL);
       
       TRACE( TRACE_SysioFS, ("(E) sysioFS::init%s: cnv_connect() failed, error %d\n", whoami(), err) );
       return err;
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
   AppState_t *app = GetMyAppState();
   SetupJobMessage *requestMsg = (SetupJobMessage *)_outMessageRegion.addr;
   fillHeader(&(requestMsg->header), SetupJob, 
                  sizeof(SetupJobMessage)-sizeof(requestMsg->shortCircuitPath));
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
   const char *pathname;
   
   if (App_GetEnvString("BG_SYSIODSHORTCIRCUITPATH", &pathname)) {
      strncpy(requestMsg->shortCircuitPath, pathname, sizeof(requestMsg->shortCircuitPath));
      const unsigned int stringLength = strlen(pathname);
      if (stringLength>=sizeof(requestMsg->shortCircuitPath)){
        requestMsg->header.length += sizeof(requestMsg->shortCircuitPath);
      }
      else{
        requestMsg->header.length += stringLength+1;
      }
   }
   else {
      requestMsg->shortCircuitPath[0] = 0;
      requestMsg->header.length ++; 
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
   CleanupJobMessage *requestMsg = (CleanupJobMessage *)_outMessageRegion.addr;
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
   wr.opcode = CNV_WR_SEND_WITH_IMM;
   wr.send_flags = 0;
   wr.imm_data = ProcessorID() | CNV_DIRECTED_RECV;
   wr.remote_addr = -1;
   wr.rkey = 0;

   // Post a send for outbound message.
   cnv_send_wr *bad_wr;
   int err = cnv_post_send_seqID(&_queuePair, &wr, &bad_wr,sequenceID);
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


int
sysioFS::getCompletions(int totalCompletions, void **inMsg)
{
   bool sendFailed = false;
   bool recvFailed = false;
   
   // Wait for completions for the work requests posted previously.
   int numCompletions = totalCompletions;
   cnv_wc completions[totalCompletions];
   int foundCompletions = 0;
   
   // Keep looking for completions until all of them are found.
   while (numCompletions > 0) {
      // Remove available completions from the completion queue after getting completion event.
     int err = cnv_poll_cq(&_completionQ, numCompletions, completions, &foundCompletions, ProcessorID());
      if (err != 0) {
         printf("(E) sysioFS::getCompletions: failed to remove completions, error %d\n", err);
         return err;
      }

      // Process each found completion.
      
      for (int index = 0; index < foundCompletions; ++index) {
         numCompletions -= 1;
         if (completions[index].opcode == CNV_WC_RECV) {
            if (completions[index].status == CNV_WC_SUCCESS) {
               *inMsg = (void *)completions[index].wr_id; // Address was saved in this field        
               Kernel_WriteFlightLog(FLIGHTLOG, FL_GETCOMPLT, totalCompletions, foundCompletions, index, completions[index].wr_id);
               
               uint64_t * data = (uint64_t * )*inMsg;
               Kernel_WriteFlightLog(FLIGHTLOG, FL_SYSMSGRCV, data[0],data[1],data[2],data[3] );

            }
            else {
               printf("(E) sysioFS::getCompletions: recv failed, status %d\n", completions[index].status);
               recvFailed = true;
            }
         }
         if ((completions[index].opcode == CNV_WC_SEND) && (completions[index].status != CNV_WC_SUCCESS)) {
            printf("(E) sysioFS::getCompletions: send failed, status %d\n", completions[index].status);
            sendFailed = true;
         }
      }

      // There is a completion available, but it was not for us.  Add a delay to give the other thread a chance to get the completion.
      if (foundCompletions == 0) {
          Delay(100);
      }
   }

   if (sendFailed || recvFailed) {
      return EINVAL;
   }

   return 0;
}

uint64_t
sysioFS::exchangeMessages(cnv_sge *sendList, int sendListSize, void **inMsg, uint32_t sequenceID)
{
   // Post a receive for the inbound reply message.
  int err = postRecv(getMyFirstRecvBuffer(), sequenceID);
   if (err != 0) {
      return CNK_RC_FAILURE(err);
   }

   // Post a send for the outbound request message.
   err = postSend(sendList, sendListSize,sequenceID);
   if (err != 0) {
      return CNK_RC_FAILURE(err);
   }

   // Get the work completions for the two posted operations.
   err = getCompletions(2, inMsg);
   if (err != 0) {
      return CNK_RC_FAILURE(err);
   }

   // Check the return code in the received message.
   bgcios::MessageHeader *replyMsg = (bgcios::MessageHeader *)*inMsg;
   uint64_t rc = CNK_RC_SUCCESS(0);
   if (replyMsg->returnCode != bgcios::Success) {
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
   if ((requestMsg->length + pathlen) <= bgcios::ImmediateMessageSize) {
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
   AccessMessage *requestMsg = (AccessMessage *)_outMessageRegion.addr;
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
   AcceptMessage *requestMsg = (AcceptMessage *)_outMessageRegion.addr;
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
   BindMessage *requestMsg = (BindMessage *)_outMessageRegion.addr;
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
   AppProcess_t *process = GetMyProcess();

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Save the descriptor to the previous working directory so it can be closed later.
   int previousDirFD = File_GetCurrentDirFD();

   // Build request message in outbound message buffer.
   OpenMessage *requestMsg = (OpenMessage *)_outMessageRegion.addr;
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
         CloseMessage *closeMsg = (CloseMessage *)_outMessageRegion.addr;
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
   ChmodMessage *requestMsg = (ChmodMessage *)_outMessageRegion.addr;
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
   ChownMessage *requestMsg = (ChownMessage *)_outMessageRegion.addr;
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
   CloseMessage *requestMsg = (CloseMessage *)_outMessageRegion.addr;
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
   if (CNK_RC_IS_SUCCESS(rc)) {
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
   ConnectMessage *requestMsg = (ConnectMessage *)_outMessageRegion.addr;
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
   FchmodMessage *requestMsg = (FchmodMessage *)_outMessageRegion.addr;
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
   FchownMessage *requestMsg = (FchownMessage *)_outMessageRegion.addr;
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
   // Make sure file descriptor is valid.
   int remoteFD = File_GetRemoteFD(fd);
   if (remoteFD < 0) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Obtain the lock to serialize message exchange with sysiod.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   FcntlMessage *requestMsg = (FcntlMessage *)_outMessageRegion.addr;
   fillHeader(&(requestMsg->header), Fcntl, sizeof(FcntlMessage));
   requestMsg->fd = remoteFD;
   requestMsg->cmd = cmd;

   uint64_t rc = CNK_RC_SUCCESS(0);
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
   Fstat64Message *requestMsg = (Fstat64Message *)_outMessageRegion.addr;
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
   Fstatfs64Message *requestMsg = (Fstatfs64Message *)_outMessageRegion.addr;
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
   Ftruncate64Message *requestMsg = (Ftruncate64Message *)_outMessageRegion.addr;
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
   FsyncMessage *requestMsg = (FsyncMessage *)_outMessageRegion.addr;
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
   Getdents64Message *requestMsg = (Getdents64Message *)_outMessageRegion.addr;
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
   GetpeernameMessage *requestMsg = (GetpeernameMessage *)_outMessageRegion.addr;
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
   GetsocknameMessage *requestMsg = (GetsocknameMessage *)_outMessageRegion.addr;
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
   GetsockoptMessage *requestMsg = (GetsockoptMessage *)_outMessageRegion.addr;
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
   IoctlMessage *requestMsg = (IoctlMessage *)_outMessageRegion.addr;
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
   ChownMessage *requestMsg = (ChownMessage *)_outMessageRegion.addr;
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
   LinkMessage *requestMsg = (LinkMessage *)_outMessageRegion.addr;
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
   ListenMessage *requestMsg = (ListenMessage *)_outMessageRegion.addr;
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
   Lseek64Message *requestMsg = (Lseek64Message *)_outMessageRegion.addr;
   fillHeader(&(requestMsg->header), Lseek64, sizeof(Lseek64Message));
   requestMsg->fd = remoteFD;
   requestMsg->offset = offset;
   requestMsg->whence = whence;

   // Exchange messages with sysiod.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   // Set result argument.
   if (CNK_RC_IS_SUCCESS(rc)) {
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
   Stat64Message *requestMsg = (Stat64Message *)_outMessageRegion.addr;
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
   MkdirMessage *requestMsg = (MkdirMessage *)_outMessageRegion.addr;
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
   OpenMessage *requestMsg = (OpenMessage *)_outMessageRegion.addr;
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
   PollMessage *requestMsg = (PollMessage *)_outMessageRegion.addr;

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

   if (replyMsg->header.returnCode >= 0){
     rc = CNK_RC_SUCCESS(replyMsg->header.returnCode);    
   }
   else{
     rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
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
   Pread64Message *requestMsg = (Pread64Message *)_outMessageRegion.addr;
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
   Pwrite64Message *requestMsg = (Pwrite64Message *)_outMessageRegion.addr;
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
   if (CNK_RC_IS_SUCCESS(rc)) {
      Pwrite64AckMessage *replyMsg = (Pwrite64AckMessage *)inRegion1;
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
   ReadMessage *requestMsg = (ReadMessage *)_outMessageRegion.addr;
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
   ReadlinkMessage *requestMsg = (ReadlinkMessage *)_outMessageRegion.addr;
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
   RecvMessage *requestMsg = (RecvMessage *)_outMessageRegion.addr;
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
   RecvfromMessage *requestMsg = (RecvfromMessage *)_outMessageRegion.addr;
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
   RenameMessage *requestMsg = (RenameMessage *)_outMessageRegion.addr;
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
   UnlinkMessage *requestMsg = (UnlinkMessage *)_outMessageRegion.addr;
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
   SendMessage *requestMsg = (SendMessage *)_outMessageRegion.addr;
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
   SendtoMessage *requestMsg = (SendtoMessage *)_outMessageRegion.addr;
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
   SetsockoptMessage *requestMsg = (SetsockoptMessage *)_outMessageRegion.addr;
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
   ShutdownMessage *requestMsg = (ShutdownMessage *)_outMessageRegion.addr;
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
   SocketMessage *requestMsg = (SocketMessage *)_outMessageRegion.addr;
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
   Stat64Message *requestMsg = (Stat64Message *)_outMessageRegion.addr;
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
   Statfs64Message *requestMsg = (Statfs64Message *)_outMessageRegion.addr;
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
   SymlinkMessage *requestMsg = (SymlinkMessage *)_outMessageRegion.addr;
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
   Truncate64Message *requestMsg = (Truncate64Message *)_outMessageRegion.addr;
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
   UnlinkMessage *requestMsg = (UnlinkMessage *)_outMessageRegion.addr;
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
   UtimesMessage *requestMsg = (UtimesMessage *)_outMessageRegion.addr;
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
   WriteMessage *requestMsg = (WriteMessage *)_outMessageRegion.addr;
   if ( GetMyKThread()->KernelInternal == 1){
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

   TRACE( TRACE_SysioFS, ("(I) sysioFS::write%s: fd=%d buffer=%p length=%ld rc=%s\n",
                          whoami(), fd, buffer, length, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t
sysioFS::writev(int fd, const struct iovec *iov, int iovcnt)
{
   // Make this simple -- write each vector individually.
   int bytesWritten = 0;
   for ( int index = 0 ; index < iovcnt ; ++index ) {
      uint64_t rc = write(fd, iov[index].iov_base, iov[index].iov_len);
      if (CNK_RC_IS_FAILURE(rc)) {
         return rc;
      }
      bytesWritten += CNK_RC_VALUE(rc);
   }

   return CNK_RC_SUCCESS(bytesWritten);
}

uint64_t
sysioFS::writeRdmaVirt(int fd, const void *buffer, size_t length)
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
   WriteRdmaVirtMessage *requestMsg = (WriteRdmaVirtMessage *)_outMessageRegion.addr;
   if ( GetMyKThread()->KernelInternal == 1){
     fillHeader(&(requestMsg->header), WriteRdmaVirtKernelInternal, sizeof(WriteRdmaVirtMessage));
   }
   else{
     fillHeader(&(requestMsg->header), WriteRdmaVirt, sizeof(WriteMessage));
   }
   requestMsg->fd = rfd;
   if (length <  _rdmaBufferLength)
     requestMsg->data_length = length;
   else 
     requestMsg->data_length = _rdmaBufferLength;
   requestMsg->bufferRdmaVirtaddress=_rdmaBufferVirtAddr;
   requestMsg->offset = 0;  //offset into buffer;
   
   // Do the RDMA write ...

   struct cnv_sge sge;
   sge.addr =   (uint64_t)userRegion.addr;
   sge.length = length;
   sge.lkey =   userRegion.rkey;

  postRdmaWrite(&sge, 1, requestMsg->header.sequenceId , _rdmaBufferVirtAddr,_remotekey);

   // Exchange messages with I/O node.
   void *inRegion;
   uint64_t rc = exchange(requestMsg, &inRegion);

   WriteRdmaVirtAckMessage *replyMsg = (WriteRdmaVirtAckMessage *)inRegion;
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

void
sysioFS::fillHeader(bgcios::MessageHeader *header, uint16_t type, size_t length)
{
   header->service = bgcios::SysioService;
   header->version = ProtocolVersion;
   header->type = type;
   header->rank = GetMyProcess()->Rank;
   header->sequenceId = _sequenceId++;
   header->returnCode = 0;
   header->errorCode = 0;
   header->length = (uint32_t)length;
   header->jobId = GetMyAppState()->JobID;
   return;
}


bool 
sysioFS::isMatch(const char *path)
{
   return ((IsAppAgent()) ? false : true);
}

