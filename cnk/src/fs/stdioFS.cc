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
#include "stdioFS.h"

#include <ramdisk/include/services/StdioMessages.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include "NodeController.h"

using namespace bgcios::stdio;

   //! Storage for outbound messages.
static char _outMessage[bgcios::SmallMessageRegionSize] ALIGN_L1D_CACHE;

   //! Storage for inbound messages.
static char _inMessage[bgcios::SmallMessageRegionSize] ALIGN_L1D_CACHE;

int stdioFS::init(void)
{
   // Start sequence ids from 1.
   _sequenceId = 1;
   _isTerminated = false;

   // Where to initialize context?  How do we get context pointer here?
   int err = cnv_open_dev(&_context);
   if (err != 0) {
      //! \todo Send a RAS event.
      TRACE( TRACE_StdioFS, ("(E) stdioFS::init%s: cnv_open_dev() failed, error %d\n", whoami(), err) );
      return err;
   }

   // Allocate a protection domain.
   err = cnv_alloc_pd(&_protectionDomain, _context);
   if (err != 0) {
      //! \todo Send a RAS event.
      TRACE( TRACE_StdioFS, ("(E) stdioFS::init%s: cnv_create_pd() failed, error %d\n", whoami(), err) );
      return err;
   }

   // Register memory regions.
// int accessFlags = CNV_ACCESS_LOCAL_WRITE|CNV_ACCESS_REMOTE_WRITE|CNV_ACCESS_REMOTE_READ;
   err = cnv_reg_mr(&_inMessageRegion, &_protectionDomain, &_inMessage, sizeof(_inMessage), CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      //! \todo Send a RAS event.
      TRACE( TRACE_StdioFS, ("(E) stdioFS::init%s: cnv_reg_mr() failed for inbound message region, error %d\n", whoami(), err) );
      return err;
   }

   err = cnv_reg_mr(&_outMessageRegion, &_protectionDomain, &_outMessage, sizeof(_outMessage), CNV_ACCESS_LOCAL_WRITE);
   if (err != 0) {
      //! \todo Send a RAS event.
      TRACE( TRACE_StdioFS, ("(E) stdioFS::init%s: cnv_reg_mr() failed for outbound message region, error %d\n", whoami(), err) );
      return err;
   }

   // Create a completion queue.
   err = cnv_create_cq(&_completionQ, _context, 2);
   if (err != 0) {
      //! \todo Send a RAS event.
      TRACE( TRACE_StdioFS, ("(E) stdioFS::init%s: cnv_create_cq() failed, error %d\n", whoami(), err) );
      return err;
   }

   // Create queue pair.
   cnv_qp_init_attr attr;
   attr.send_cq = &_completionQ;
   attr.recv_cq = &_completionQ;

   err = cnv_create_qp(&_queuePair, &_protectionDomain, &attr);
   if (err != 0) {
      //! \todo Send a RAS event.
      TRACE( TRACE_StdioFS, ("(E) stdioFS::init%s: cnv_create_qp() failed, error %d\n", whoami(), err) );
      return err;
   }

   // Connect to stdiod on I/O node.
   struct sockaddr_in destAddress;
   destAddress.sin_family = AF_INET;
   destAddress.sin_port = BaseRdmaPort;
   destAddress.sin_addr.s_addr = NodeState.ServiceDeviceAddr;
   err = cnv_connect(&_queuePair, (struct sockaddr *)&destAddress);
   Node_ReportConnect(err, destAddress.sin_addr.s_addr, destAddress.sin_port);
   if (err != 0) {
       TRACE( TRACE_StdioFS, ("(E) stdioFS::init%s: cnv_connect() failed, error %d\n", whoami(), err) );
       Kernel_Crash(RAS_KERNELCNVCONNECTFAIL);
   }

   TRACE( TRACE_StdioFS, ("(I) stdioFS::init%s: connected to stdiod\n", whoami()) );
   return 0;
}

int stdioFS::term(void)
{
   int rc = 0;

   if (_isTerminated == true) {
       return 0;
   }

   Kernel_Lock(&_lock);
   // Just return if the method has already completed successfully.
   if (_isTerminated == true) {
      return 0;
   }

   // Disconnect from stdiod on I/O node.
   int err = cnv_disconnect(&_queuePair);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDISCNE, _queuePair.qp_num, _queuePair.handle, err, 0);
      TRACE( TRACE_StdioFS, ("(E) stdioFS::term%s: cnv_disconnect() failed, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }

   // Destroy queue pair.
   err = cnv_destroy_qp(&_queuePair);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDESQPE, _queuePair.qp_num, _queuePair.handle, err, 0);
      TRACE( TRACE_StdioFS, ("(E) stdioFS::term%s: cnv_destroy_qp() failed, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }

   // Destroy completion queue.
   err = cnv_destroy_cq(&_completionQ);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDESCQE, _completionQ.handle, err, 0, 0);
      TRACE( TRACE_StdioFS, ("(E) stdioFS::term%s: cnv_destroy_cq() failed, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }

   // Deregister memory regions.
   err = cnv_dereg_mr(&_outMessageRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)_outMessageRegion.addr, _outMessageRegion.length, _outMessageRegion.lkey, err);
      TRACE( TRACE_StdioFS, ("(E) stdioFS::term%s: cnv_dereg_mr() failed for outbound message region, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }
   err = cnv_dereg_mr(&_inMessageRegion);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)_inMessageRegion.addr, _inMessageRegion.length, _inMessageRegion.lkey, err);
      TRACE( TRACE_StdioFS, ("(E) stdioFS::term%s: cnv_dereg_mr() failed for inbound message region, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }

   // Deallocate protection domain.
   err = cnv_dealloc_pd(&_protectionDomain);
   if (err != 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDEAPDE, _protectionDomain.handle, err, 0, 0);
      TRACE( TRACE_StdioFS, ("(E) stdioFS::term%s: cnv_dealloc_pd() failed, error %d\n", whoami(), err) );
      if (rc == 0) rc = err;
   }

   _isTerminated = true;

   TRACE( TRACE_StdioFS, ("(I) stdioFS::term%s: termination is complete\n", whoami()) );
   return rc;
}

int stdioFS::setupJob(int fs)
{
   // Acquire message buffers.
   Kernel_Lock(&_lock);

   // Set flag for cleanup job processing.
   GetMyAppState()->closedStdio = 0;

   // Release message buffers.
   Kernel_Unlock(&_lock);

   return 0;
}

int stdioFS::cleanupJob(int fs)
{
   int rc = 0;

   // Acquire message buffers.
   Kernel_Lock(&_lock);

   // If stdiod has not been notified yet and the job loaded successfully, let stdiod know the job is done.
   if ( (!(GetMyAppState()->closedStdio)) && (GetMyAppState()->LoadState >= AppState_StartIssued) ) {
      // Build request message in outbound message buffer.
      CloseStdioMessage *requestMsg = (CloseStdioMessage *)_outMessageRegion.addr;
      fillHeader(&(requestMsg->header), CloseStdio);
      requestMsg->header.length = sizeof(CloseStdioMessage);
      requestMsg->header.rank = GetMyAppState()->Active_Processes;

      // Build scatter/gather element for inbound message.
      struct cnv_sge recv_sge;
      recv_sge.addr = (uint64_t)_inMessageRegion.addr;
      recv_sge.length = _inMessageRegion.length;
      recv_sge.lkey = _inMessageRegion.lkey;

      // Build scatter/gather element for outbound message.
      struct cnv_sge send_sge;
      send_sge.addr = (uint64_t)_outMessageRegion.addr;
      send_sge.length = requestMsg->header.length;
      send_sge.lkey = _outMessageRegion.lkey;

      // Exchange messages with I/O node.
      uint64_t * data = (uint64_t * )requestMsg;
      Kernel_WriteFlightLog(FLIGHTLOG, FL_STDMSGSND, data[0],data[1],data[2],data[3] );
      int err = exchangeMessages(&recv_sge, 1, &send_sge, 1);
      if (err != 0) {
         Kernel_Unlock(&_lock);
         return err;
      }

      // Process the reply message.
      CloseStdioAckMessage *replyMsg = (CloseStdioAckMessage *)_inMessageRegion.addr;
      data = (uint64_t * )replyMsg;
      Kernel_WriteFlightLog(FLIGHTLOG, FL_STDMSGRCV, data[0],data[1],data[2],data[3] );

      if (replyMsg->header.returnCode != bgcios::Success) {
         //! \todo Send a RAS event.
         printf("(E) stdioFS::cleanup%s closing stdio failed, error %d\n", whoami(), replyMsg->header.errorCode);
         rc = replyMsg->header.errorCode;
      }

      // Set flag so only one CloseStdio message is sent to stdiod.
      GetMyAppState()->closedStdio = 1;

      TRACE( TRACE_StdioFS, ("(I) stdioFS::cleanup%s: rc=%d error=%d\n", whoami(), replyMsg->header.returnCode, replyMsg->header.errorCode) );
   }

   // Release message buffers.
   Kernel_Unlock(&_lock);

   return rc;
}

// Please keep methods in alphabetical order for ease of maintenance.

uint64_t stdioFS::close(int fd)
{
   // Free the descriptor now that it is closed.
   File_FreeFD(fd);

   TRACE( TRACE_StdioFS, ("(I) stdioFS::close%s: fd=%d rc=0\n", whoami(), fd) );
   return CNK_RC_SUCCESS(0);
}

uint64_t stdioFS::fcntl(int fd, int cmd, uint64_t parm3)
{
   uint64_t rc;
   int newfd;

   switch (cmd) {
      case F_DUPFD:
         // Make sure a descriptor is available.
         newfd = File_GetFD((int)parm3);
         if (newfd == -1) {
            rc = CNK_RC_FAILURE(EBADF);
            break;
         }

         // Setup duplicated descriptor.
         File_SetFD(newfd, newfd, File_GetFDType(fd));
         rc = CNK_RC_SUCCESS(0);
         break;

      case F_GETFL:
      case F_GETFD:
         rc = CNK_RC_SUCCESS(0); // or do we need to set wronly or rdonly based on type?
         break;

      default:
         rc = CNK_RC_FAILURE(EINVAL);
         break;
   }

   TRACE( TRACE_StdioFS, ("(I) stdioFS::fcntl%s: fd=%d cmd=%d rc=%s\n", whoami(), fd, cmd, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t stdioFS::fstat(int fd, struct stat *statbuf)
{
   return fstat64(fd, (struct stat64 *)statbuf);
}

uint64_t stdioFS::fstat64(int fd, struct stat64 *statbuf)
{
   // Fill in the stat structure so the descriptor looks like a terminal.
   memset(statbuf, 0, sizeof(struct stat64));
   statbuf->st_dev     = 11;
   statbuf->st_ino     = 1;
   statbuf->st_mode    = S_IFCHR | S_IRUSR | S_IWUSR | S_IWGRP;
   statbuf->st_nlink   = 1;
   statbuf->st_uid     = GetMyAppState()->UserID;
   statbuf->st_gid     = GetMyAppState()->GroupID;
   statbuf->st_rdev    = 0x8800; // Major 136, minor 0
   statbuf->st_size    = 0;
   statbuf->st_blksize = 64512;
   statbuf->st_blocks  = 0;
// statbuf->st_atime   = GetMyAppState()->JobStartTime;
// statbuf->st_mtime   = GetMyAppState()->JobStartTime;
// statbuf->st_ctime   = GetMyAppState()->JobStartTime;

   TRACE( TRACE_StdioFS, ("(I) stdioFS::fstat64%s: fd=%d rc=0\n", whoami(), fd) );
   return CNK_RC_SUCCESS(0);
}

uint64_t stdioFS::llseek(int fd, off64_t offset, off64_t *result, int whence)
{
   return CNK_RC_FAILURE(ESPIPE);
}

uint64_t stdioFS::lseek(int fd, off_t offset, int whence)
{
   return CNK_RC_FAILURE(ESPIPE);
}

uint64_t stdioFS::ioctl(int fd, unsigned long int cmd, void *parm3)
{
   uint64_t rc;

   switch (cmd) {
      case 0x402c7413: // Special value used by isatty()
         rc = CNK_RC_SUCCESS(0);
         break;

      case TCGETA:
      case TCGETS:
      {
         struct termios *termios = (struct termios *)parm3;
         memset(termios, 0, sizeof(struct termios));

         //! \todo Do we need to fill any fields?

         rc = CNK_RC_SUCCESS(0);
         break;
      }

      default:
         rc = CNK_RC_FAILURE(EINVAL);
         break;
   }

   TRACE( TRACE_StdioFS, ("(I) stdioFS::ioctl%s: fd=%d cmd=0x%lx parm3=0x%p rc=%s\n", whoami(), fd, cmd, parm3, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t stdioFS::read(int fd, void *buffer, size_t length)
{
   // Only allow read from a stdin descriptor.
   if (File_GetFDType(fd) != FD_STDIN) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Only allow read from stdin on designated rank and never allow read stdin for a sub-node job.
   if ( (GetMyProcess()->Rank != GetMyAppState()->RankForStdin) || (IsSubNodeJob()) ) {
      return CNK_RC_SUCCESS(0);
   }

   // Truncate to the maximum length.
   if (length > bgcios::SmallMessageDataSize) {
      length = bgcios::SmallMessageDataSize;
   }

   TRACE( TRACE_StdioFS, ("(I) stdioFS::read%s fd=%d buffer=%p length=%ld\n", whoami(), fd, buffer, length) );

   // Acquire message buffers.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   ReadStdinMessage *requestMsg = (ReadStdinMessage *)_outMessageRegion.addr;
   fillHeader(&(requestMsg->header), ReadStdin);
   requestMsg->header.length = sizeof(ReadStdinMessage);
   requestMsg->length = length;

   // Build scatter/gather element for inbound message.
   struct cnv_sge recv_sge;
   recv_sge.addr = (uint64_t)_inMessageRegion.addr;
   recv_sge.length = _inMessageRegion.length;
   recv_sge.lkey = _inMessageRegion.lkey;

   // Build scatter/gather element for outbound message.
   struct cnv_sge send_sge;
   send_sge.addr = (uint64_t)_outMessageRegion.addr;
   send_sge.length = requestMsg->header.length;
   send_sge.lkey = _outMessageRegion.lkey;

   // Exchange messages with I/O node.
   uint64_t * data = (uint64_t * )requestMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_STDMSGSND, data[0],data[1],data[2],data[3] );
   int err = exchangeMessages(&recv_sge, 1, &send_sge, 1);
   if (err != 0) {
      Kernel_Unlock(&_lock);
      return CNK_RC_FAILURE(err);
   }

   // Process the reply message.
   ReadStdinAckMessage *replyMsg = (ReadStdinAckMessage *)_inMessageRegion.addr;
   data = (uint64_t * )replyMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_STDMSGRCV, data[0],data[1],data[2],data[3] );

   uint64_t rc;
   if (replyMsg->header.returnCode == bgcios::Success) {
      uint32_t bytes = bgcios::dataLength(&(replyMsg->header));
      rc = CNK_RC_SUCCESS(bytes);
      if (bytes > 0) {
         memcpy(buffer, replyMsg->data, bytes); // Copy to user buffer
      }
   }
   else if (replyMsg->header.returnCode == bgcios::VersionMismatch) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_VERMISMAT, requestMsg->header.service, ProtocolVersion, replyMsg->header.errorCode, 0); 
      RASBEGIN(3);
      RASPUSH(requestMsg->header.service);
      RASPUSH(ProtocolVersion);
      RASPUSH(replyMsg->header.errorCode);
      RASFINAL(RAS_KERNELVERSIONMISMATCH);
      Kernel_Crash(1);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }

   // Release message buffers.
   Kernel_Unlock(&_lock);

   TRACE( TRACE_StdioFS, ("(I) stdioFS::read%s: fd=%d buffer=0x%p length=%ld rc=%s\n", whoami(), fd, buffer, length, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t stdioFS::write(int fd, const void *buffer, size_t length)
{
   // Length of zero bytes is a special case.  Just return success.
   if (length == 0) {
      return CNK_RC_SUCCESS(0);
   }

   // Truncate to the maximum length.
   if (length > bgcios::SmallMessageDataSize) {//65472 in MessageHeader.h in bgq/ramdisk/include/services
      length = bgcios::SmallMessageDataSize;
   }

   // Acquire message buffers.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   WriteStdioMessage *requestMsg = (WriteStdioMessage *)_outMessageRegion.addr;
   int type = File_GetFDType(fd);
   if (type == FD_STDOUT) {
      fillHeader(&(requestMsg->header), WriteStdout);
   }
   else if (type == FD_STDERR) {
      fillHeader(&(requestMsg->header), WriteStderr);
   }
   else {
      Kernel_Unlock(&_lock);
      return CNK_RC_FAILURE(EBADF);
   }
   requestMsg->header.length = sizeof(bgcios::MessageHeader) + length;

   // Copy data from the user buffer to outbound message.
   memcpy(requestMsg->data, buffer, length);

   // Build scatter/gather element for inbound message.
   struct cnv_sge recv_sge;
   recv_sge.addr = (uint64_t)_inMessageRegion.addr;
   recv_sge.length = _inMessageRegion.length;
   recv_sge.lkey = _inMessageRegion.lkey;

   // Build scatter/gather element for outbound message.
   struct cnv_sge send_sge;
   send_sge.addr = (uint64_t)_outMessageRegion.addr;
   send_sge.length = requestMsg->header.length;
   send_sge.lkey = _outMessageRegion.lkey;

   // Exchange messages with I/O node.
   uint64_t * data = (uint64_t * )requestMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_STDMSGSND, data[0],data[1],data[2],data[3] );
   int err = exchangeMessages(&recv_sge, 1, &send_sge, 1);
   if (err != 0) {
      Kernel_Unlock(&_lock);
      return CNK_RC_FAILURE(err);
   }

   // Process the reply message.
   WriteStdioAckMessage *replyMsg = (WriteStdioAckMessage *)_inMessageRegion.addr;
   data = (uint64_t * )replyMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_STDMSGRCV, data[0],data[1],data[2],data[3] );

   uint64_t rc;
   if (__LIKELY(replyMsg->header.returnCode == bgcios::Success) ){
      rc = CNK_RC_SUCCESS(length);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }

   // Release message buffers.
   Kernel_Unlock(&_lock);

   return rc;
}

uint64_t
stdioFS::writev(int fd, const struct iovec *iov, int iovcnt)
{
   if (__UNLIKELY(iovcnt==0)) return CNK_RC_SUCCESS(0);
     
   // Acquire message buffers.
   Kernel_Lock(&_lock);

   // Build request message in outbound message buffer.
   WriteStdioMessage *requestMsg = (WriteStdioMessage *)_outMessageRegion.addr;
   int type = File_GetFDType(fd);
   if (type == FD_STDOUT) {
      fillHeader(&(requestMsg->header), WriteStdout);
   }
   else if (type == FD_STDERR) {
      fillHeader(&(requestMsg->header), WriteStderr);
   }
   else {
      Kernel_Unlock(&_lock);
      return CNK_RC_FAILURE(EBADF);
   }
   
   // Copy data from the user buffer to outbound message.
   int length = 0;
   char * target = (char *)requestMsg->data;
   for ( int index = 0 ; index < iovcnt ; ++index ) {
      if (__LIKELY( (length + iov[index].iov_len)<= bgcios::SmallMessageDataSize) ){
        memcpy(target, iov[index].iov_base, iov[index].iov_len);
        length += iov[index].iov_len;
        target += iov[index].iov_len;
      }
      else{
         signed int lastbytes = bgcios::SmallMessageDataSize - iov[index].iov_len;
         if (lastbytes <= 0) break;  //nothing to copy
         memcpy(target,iov[index].iov_base,lastbytes);
         length=bgcios::SmallMessageDataSize;
         break; //buffer is full
      }    
   }
   requestMsg->header.length = sizeof(bgcios::MessageHeader) + length;

   // Build scatter/gather element for inbound message.
   struct cnv_sge recv_sge;
   recv_sge.addr = (uint64_t)_inMessageRegion.addr;
   recv_sge.length = _inMessageRegion.length;
   recv_sge.lkey = _inMessageRegion.lkey;

   // Build scatter/gather element for outbound message.
   struct cnv_sge send_sge;
   send_sge.addr = (uint64_t)_outMessageRegion.addr;
   send_sge.length = requestMsg->header.length;
   send_sge.lkey = _outMessageRegion.lkey;

   // Exchange messages with I/O node.
   uint64_t * data = (uint64_t * )requestMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_STDMSGSND, data[0],data[1],data[2],data[3] );
   int err = exchangeMessages(&recv_sge, 1, &send_sge, 1);
   if (err != 0) {
      Kernel_Unlock(&_lock);
      return CNK_RC_FAILURE(err);
   }

   // Process the reply message.
   WriteStdioAckMessage *replyMsg = (WriteStdioAckMessage *)_inMessageRegion.addr;
   data = (uint64_t * )replyMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_STDMSGRCV, data[0],data[1],data[2],data[3] );

   uint64_t rc;
   if (__LIKELY(replyMsg->header.returnCode == bgcios::Success) ){
      rc = CNK_RC_SUCCESS(length);
   }
   else {
      rc = CNK_RC_FAILURE(replyMsg->header.errorCode);
   }

   // Release message buffers.
   Kernel_Unlock(&_lock);

   return rc;
}

int
stdioFS::exchangeMessages(struct cnv_sge *recvList, int numRecvElements, struct cnv_sge *sendList, int numSendElements)
{
   // Build receive work request.
   struct cnv_recv_wr recvRequest;
   memset(&recvRequest, 0, sizeof(recvRequest));
   recvRequest.next = NULL;
   recvRequest.sg_list = recvList;
   recvRequest.num_sge = numRecvElements;

   // Build send work request.
   struct cnv_send_wr sendRequest;
   memset(&sendRequest, 0, sizeof(sendRequest));
   sendRequest.next = NULL;
   sendRequest.sg_list = sendList;
   sendRequest.num_sge = numSendElements;
   sendRequest.opcode = CNV_WR_SEND;

   int err = 0;
   // Post a receive for inbound message.
   cnv_recv_wr *badRecvRequest;
   err = cnv_post_recv(&_queuePair, &recvRequest, &badRecvRequest);
   if (err != 0) {
      return err;
   }

   // Post a send for outbound message.
   cnv_send_wr *badSendRequest;
   err = cnv_post_send(&_queuePair, &sendRequest, &badSendRequest);
   if (err != 0) {
      return err;
   }

   bool sendFailed = false;
   bool recvFailed = false;

   // Wait for completions for the work requests posted above.
   int numCompletions = 2;
   cnv_wc completions[numCompletions];
   int foundCompletions = 0;

   while (numCompletions > 0) {
      // Remove available completions from the completion queue after getting completion event.
      err = cnv_poll_cq(&_completionQ, numCompletions, completions, &foundCompletions, ProcessorID());
      if (err != 0) {
         printf("(E) failed to remove completions, error %d\n", err);
         return err;
      }

      // Process each found completion.
      for (int index = 0; index < foundCompletions; ++index) {
         numCompletions -= 1;
         if ((completions[index].opcode == CNV_WC_RECV) && (completions[index].status != CNV_WC_SUCCESS)) {
            printf("(E) recv failed, status %d\n", completions[index].status);
            recvFailed = true;
         }
         if ((completions[index].opcode == CNV_WC_SEND) && (completions[index].status != CNV_WC_SUCCESS)) {
            printf("(E) send failed, status %d\n", completions[index].status);
            sendFailed = true;
         }
      }
   }

   if (sendFailed || recvFailed) {
      return EINVAL;
   }

   return 0;
}

void
stdioFS::fillHeader(bgcios::MessageHeader *header, uint16_t type)
{
   header->service = bgcios::StdioService;
   header->version = ProtocolVersion;
   header->type = type;
   header->rank = GetMyProcess()->Rank;
   header->sequenceId = _sequenceId++;
   header->returnCode = 0;
   header->errorCode = 0;
   header->length = 0;
   header->jobId = GetMyAppState()->JobID;
   return;
}

