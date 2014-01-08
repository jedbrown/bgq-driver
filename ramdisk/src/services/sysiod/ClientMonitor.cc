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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  ClientMonitor.cc
//! \brief Methods for bgcios::sysio::ClientMonitor class.

// Includes
#include "ClientMonitor.h"
#include <ramdisk/include/services/common/RdmaError.h>
#include <ramdisk/include/services/common/RdmaDevice.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/resource.h>
#include <iomanip>
#include <ramdisk/include/services/common/logging.h>

using namespace bgcios::sysio;

LOG_DECLARE_FILE("cios.sysiod");

ClientMonitor::~ClientMonitor()
{
   // Destroy the large memory region.
   LOG_CIOS_DEBUG_MSG("destroying large memory region with local key " << _largeRegion->getLocalKey());
   _largeRegion.reset();

   // Destroy the memory region for outbound messages.
   LOG_CIOS_DEBUG_MSG("destroying outbound message region with local key " << _outMessageRegion->getLocalKey());
   _ackMemFreeList.clear();
   _outMessageRegion.reset();

   // Destroy the memory region for inbound messages.
   LOG_CIOS_DEBUG_MSG("destroying inbound message region with local key " << _inMessageRegion->getLocalKey());
   _inMessageRegion.reset();

   // Destroy the client.
   LOG_CIOS_DEBUG_MSG("destroying RDMA connection to client");
   _client.reset();

   // Destroy the listener.
   LOG_CIOS_DEBUG_MSG("destroying listening RDMA connection on port " << BaseRdmaPort + _serviceId);
   _rdmaListener.reset();

   // Remove the completion queue from the completion channel.
   _completionChannel->removeCompletionQ(_completionQ);

   // Destroy the completion queue.
   LOG_CIOS_DEBUG_MSG("destroying completion queue " << _completionQ->getHandle());
   _completionQ.reset();

   // Destroy the completion channel.
   LOG_CIOS_DEBUG_MSG("destroying completion channel using fd " << _completionChannel->getChannelFd());
   _completionChannel.reset();

   // Destroy the protection domain.
   LOG_CIOS_DEBUG_MSG("destroying protection domain " << _protectionDomain->getHandle());
   _protectionDomain.reset();
}

int
ClientMonitor::startup(void)
{
   // Get a descriptor for the current working directory.
   char cwd[1024];
   ::getcwd(cwd, sizeof(cwd));
   _currentDirFd = ::open(cwd, O_RDONLY, 0);

   // Find the address of the I/O link device.
   RdmaDevicePtr linkDevice;
   try {
      linkDevice = RdmaDevicePtr(new RdmaDevice(bgcios::RdmaDeviceName, bgcios::RdmaInterfaceName));
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error opening InfiniBand device: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created InfiniBand device for " << linkDevice->getDeviceName() << " using interface " << linkDevice->getInterfaceName());

   // Create listener for RDMA connections.
   try {
      _rdmaListener = bgcios::RdmaServerPtr(new bgcios::RdmaServer(linkDevice->getAddress(), (in_port_t)(0)));
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error creating listening RDMA connection: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created listening RDMA connection on port " << _rdmaListener->getLocalPort());

   // Create a protection domain object.
   try {
      _protectionDomain = RdmaProtectionDomainPtr(new RdmaProtectionDomain(_rdmaListener->getContext()));
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error allocating protection domain: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created protection domain " << _protectionDomain->getHandle());

   // Create a completion channel object.
   try {
      _completionChannel = RdmaCompletionChannelPtr(new RdmaCompletionChannel(_rdmaListener->getContext(), true));
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error constructing completion channel: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created completion channel using fd " << _completionChannel->getChannelFd());

   // Create a completion queue object.
   try {
      _completionQ = RdmaCompletionQueuePtr(new RdmaCompletionQueue(_rdmaListener->getContext(), 32, _completionChannel->getChannel())); 
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error constructing completion queue: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created completion queue " << _completionQ->getHandle());

   // Add the completion queue to the completion channel.
   _completionChannel->addCompletionQ(_completionQ);

   // Create a memory region for inbound messages.
   _inMessageRegion = RdmaMemoryRegionPtr(new RdmaMemoryRegion());
   int err = _inMessageRegion->allocate(_protectionDomain, bgcios::SmallMessageRegionSize);
   if (err != 0) {
      LOG_ERROR_MSG("error allocating inbound message region: " << bgcios::errorString(err));
      return err;
   }
   LOG_CIOS_DEBUG_MSG("created inbound message region with local key " << _inMessageRegion->getLocalKey());

   // Create a memory region for outbound (Ack) messages.
   err = allocateSendRequestPool();
   //LOG_INFO_MSG_FORCED(" _numPiecesOutBound ="<< _numPiecesOutBound  );
   if (err != 0) {
      LOG_ERROR_MSG("error allocating outbound message region: " << bgcios::errorString(err));
      return err;
   }
   LOG_CIOS_DEBUG_MSG("created outbound message region with local key " << _outMessageRegion->getLocalKey());


   // Create a large memory region.
   _largeRegion = RdmaMemoryRegionPtr(new RdmaMemoryRegion());
   err = _largeRegion->allocate(_protectionDomain, _regionSize);
   if (err != 0) {
      LOG_ERROR_MSG("error allocating large memory region: " << bgcios::errorString(err));
      return err;
   }
   LOG_CIOS_DEBUG_MSG("created large memory region with local key " << _largeRegion->getLocalKey());

   // Listen for connections.
   err = _rdmaListener->listen(1);
   if (err != 0) {
      LOG_ERROR_MSG("error listening for new RDMA connections: " << bgcios::errorString(err));
      return err;
   }
   LOG_CIOS_DEBUG_MSG("listening for new RDMA connections on fd " << _rdmaListener->getEventChannelFd());

   // Tell the main thread that startup is complete.
   _ready->increment();
   LOG_CIOS_DEBUG_MSG("ClientMonitor thread startup is complete");

   if (_serviceId==0) LOG_INFO_MSG_FORCED("Using large Region="<<_regionSize<<" small ="<<bgcios::SmallMessageRegionSize<<" Compile date="<<__DATE__<<" Time="<<__TIME__);

   return 0;
}

void *
ClientMonitor::run(void)
{
   // Allocate resources for monitoring the client connection.
   _lastError = startup();
   if (_lastError != 0) {
      _done = true;
      return NULL;
   }

   // Initialize the set of descriptors to monitor.
   _pollSet[CompChannel].fd = _completionChannel->getChannelFd();
   _pollSet[CompChannel].events = POLLIN;
   _pollSet[CompChannel].revents = 0;
   _freePollSetSlots.flip(CompChannel);
   LOG_CIOS_DEBUG_MSG("added completion channel using fd " << _pollSet[CompChannel].fd << " to poll descriptor list");

   _pollSet[EventChannel].fd = _rdmaListener->getEventChannelFd();
   _pollSet[EventChannel].events = POLLIN;
   _pollSet[EventChannel].revents = 0;
   _freePollSetSlots.flip(EventChannel);
   LOG_CIOS_DEBUG_MSG("added event channel using fd " << _pollSet[EventChannel].fd << " to poll descriptor list");

   int timeout = -1; // -1 = forever, 10000 = 10 second, 1000 = 1 second, 500 = 1/2 second, 250 = 1/4 second, 100 = 1/10 second
   _pollSetSize = FixedPollSetSize;

   // Process events until told to stop.
   while (!_done) {

      // Set the timeout based on the number of blocked messages.
      if (_blockedMessages.empty()) {
         timeout = -1; // No messages are blocked so we can wait forever for next event on either the completion channel or the event channel.
      }
      else {
         timeout = 100; // At least one message is blocked and we need to keep handling queued messages so make sure we don't get stuck.
      }

      // Wait for an event on one of the descriptors.
      LOG_CIOS_TRACE_MSG("waiting for events on " << _pollSetSize << " descriptors with timeout " << timeout << " ...");
      _waitEventTimer.start();
      int rc = poll(_pollSet, _pollSetSize, timeout);
      _waitEventTimer.stop();
#if 0
      // There was no data so try again.
      if (rc == 0) {
         if (!_queuedMessages.empty()) {
            LOG_CIOS_INFO_MSG(_queuedMessages.size() << " queued messages are available after poll wakeup");
            if (_queuedMessages.size() > 1) {
               LOG_CIOS_INFO_MSG("there are " << _queuedMessages.size() << " queued messages");
            }
            ClientMessagePtr message = _queuedMessages.front();
            routeMessage(message);
            _queuedMessages.pop_front();
         }
         continue;
      } 
#endif
      // There was an error so log the failure and try again.
      if (rc == -1) {
         int err = errno;
         if (err == EINTR) {
            if (_blockedMessages.empty()){
              //LOG_ERROR_MSG("poll:EINTR:NO BLOCKED MESSAGES pid="<<getpid() );
              continue;
            }
            else{
              //LOG_ERROR_MSG("poll:EINTR-->BLOCKED MESSAGES pid="<<getpid() );
            }
         }//end EINTR
         else{
           LOG_ERROR_MSG("poll: " << bgcios::errorString(err)<<" err="<<errno);
           return NULL;
         }
      } //end err==-1

      // Check for an event on the completion channel.
      if (_pollSet[CompChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("event available on completion channel");
         completionChannelHandler(0);
         _pollSet[CompChannel].revents = 0;
      }

      // Check for an event on the event channel.
      if (_pollSet[EventChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("event available on rdma event channel");
         eventChannelHandler();
         _pollSet[EventChannel].revents = 0;
      }

      
      for (nfds_t index = FixedPollSetSize; index < _pollSetSize; ++index) {
         if (_pollSet[index].fd == -1) continue;
         ClientMessagePtr message = _blockedMessages.get(_pollSet[index].fd);
         if (message==NULL){  
           LOG_INFO_MSG_FORCED("message is NULL at __LINE__="<<__LINE__<<" fd="<< _pollSet[index].fd);
            // Remove the blocked message from the list and free the poll set slot.
            //_blockedMessages.remove(_pollSet[index].fd);
            _pollSet[index].fd = -1;
            _pollSet[index].events = 0;
            _pollSet[index].revents = 0;
            _freePollSetSlots.flip(index);
           break;
         }
         JobPtr job = _jobs.get(message->getJobId());
         // Check for events on descriptors for blocked messages.
         if ( (_pollSet[index].revents != 0)  ||  (job->isKilled()) 
                                              || job->foundRankEINTR(message->getRank() ) ) {
            // Find the blocked message.
            LOG_CIOS_TRACE_MSG("event available on descriptor " << _pollSet[index].fd << " in slot " << index);
            

            // Remove the blocked message from the list and free the poll set slot.
            _blockedMessages.remove(_pollSet[index].fd);
            _pollSet[index].fd = -1;
            _pollSet[index].events = 0;
            _pollSet[index].revents = 0;
            _freePollSetSlots.flip(index);

            // Run the operation again.
            routeMessage(message);
         }
      }

      // Keep handling queued messages until there is a blocked message.
      while (!_queuedMessages.empty()) {
         LOG_CIOS_TRACE_MSG("queued message available on list");
         ClientMessagePtr message = _queuedMessages.front();
         routeMessage(message);
         _queuedMessages.pop_front();

         // If there is a blocked message, stop handling queued messages.
         if (!_blockedMessages.empty()) {
            LOG_CIOS_DEBUG_MSG("stopped handling queued messages to check for blocked messages, " << _queuedMessages.size() << " are available")
            break;
         }
      }
   }

   return NULL;
}

void
ClientMonitor::eventChannelHandler(void)
{
   int err;

   // Wait for the event (it should be here now).
   err = _rdmaListener->waitForEvent();
   if (err != 0) {
      return;
   }

   // Handle the event.
   rdma_cm_event_type type = _rdmaListener->getEventType();
   switch (type) {

      case RDMA_CM_EVENT_CONNECT_REQUEST:
      {
         // Construct a new RdmaConnection object for the new client.
         _client = RdmaConnectionPtr(new RdmaConnection(_rdmaListener->getEventId(), _protectionDomain, _completionQ, _completionQ, false));

         // Post a receive to get the first message.
         _client->postRecv(_inMessageRegion);

         // Accept the connection from the new client.
         err = _client->accept();
         if (err != 0) {
            LOG_ERROR_MSG("error accepting client connection: " << bgcios::errorString(err));
            _client->reject(); // Tell client the bad news
            break;
         }

         LOG_CIOS_DEBUG_MSG(_client->getTag() << "connection accepted from " << _client->getRemoteAddressString() << " is using completion queue " <<
                       _completionQ->getHandle());
         break;
      }

      case RDMA_CM_EVENT_ESTABLISHED:
      {
         LOG_CIOS_INFO_MSG(_client->getTag() << "connection established with " << _client->getRemoteAddressString());
         break;
      }

      case RDMA_CM_EVENT_DISCONNECTED:
      {
         // Complete disconnect initiated by peer.
         err = _client->disconnect(false);
         if (err == 0) {
            LOG_CIOS_INFO_MSG(_client->getTag() << "disconnected from " << _client->getRemoteAddressString());
         }
         else {
            LOG_ERROR_MSG(_client->getTag() << "error disconnecting from peer: " << bgcios::errorString(err));
         }

         // Set flag to stop processing messages.
         _done = true;
          exit(EXIT_SUCCESS);
         break;
      }

      default:
      {
         LOG_ERROR_MSG("event " << rdma_event_str(type) << " is not supported");
         break;
      }
   }

   // Acknowledge the event.
   _rdmaListener->ackEvent();

   return;
}

bool
ClientMonitor::completionChannelHandler(uint64_t requestId)
{
   // Get a notification event from the completion channel.
   int numCompletions = 0;
   RdmaCompletionQueuePtr completionQ = _completionChannel->getEvent();
   if (completionQ != NULL) {
      // Remove work completions from the completion queue.
      numCompletions = completionQ->removeCompletions(RdmaCompletionQueue::SingleNodeQueueSize);
      if (numCompletions > 3) {
         LOG_CIOS_WARN_MSG("there are " << numCompletions << " completions available from completion queue " << completionQ->getHandle() << " which is unexpected");
      }
   }
   else {
      LOG_CIOS_TRACE_MSG("there was no notification event available from completion channel");
      return false;
   }

   // Run the list of work completions.
   bool rc = false;
   for (int index = 0; index < numCompletions; ++index) {
      // Get the next work completion.
      struct ibv_wc *completion = completionQ->popCompletion();
      
      // All of the completions have already been handled (most likely there was a latent notification event from the completion channel).
      if (completion == NULL) {
         return false;
      }
      
      // Check the status in the work completion.
      if (completion->status != IBV_WC_SUCCESS) {
         LOG_ERROR_MSG("failed work completion, status '" << ibv_wc_status_str(completion->status) << "' for operation " <<
                       completionQ->wc_opcode_str(completion->opcode) <<  " (" << completion->opcode << ")");
         // Maybe throw a rdma error here?
         return false;
      }
      
      // Check the opcode in the completion queue entry.
      switch (completion->opcode) {
         case IBV_WC_RECV:
         {
            LOG_CIOS_TRACE_MSG("receive operation completed for queue pair " << completion->qp_num << " (received " << completion->byte_len <<
                          " bytes and immediate data 0x" << std::setw(8) << std::hex << completion->imm_data << ")");
            
            // Post a receive to get next message. Note this requires CNK to not send another message before getting an ack for this message.
            _client->postRecv(_inMessageRegion);
            
            // Handle the message.
            bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_inMessageRegion->getAddress();
            LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << ":" << msghdr->rank << ": " << toString(msghdr->type) <<
                          " message is available on completion channel from queue pair " << completion->qp_num);
            ClientMessagePtr inMsg = ClientMessagePtr(new ClientMessage(msghdr, completion->imm_data));
            
            // When there is no message in progress, handle the message right away.
            if (_inProgressMessage == 0) {
               if (!routeMessage(inMsg)) {
                  LOG_ERROR_MSG("unsupported message type " << msghdr->type << " received from client " << bgcios::printHeader(*msghdr));
                  
                  // Build ack message in outbound message region.
                  _ackMessage =  allocateClientAckMessage(inMsg,ErrorAck, sizeof(ErrorAckMessage));
                  ErrorAckMessage *outMsg = (ErrorAckMessage *)_ackMessage;
                  outMsg->header.returnCode = bgcios::RequestFailed;
                  outMsg->header.errorCode = ENOTSUP;
                  
                  // Send message to client.
                  sendAckMessage();
               }
            }
            
            // Put the message on the queue while waiting for in progress message to complete.
            else {
               inMsg->save();
               _queuedMessages.push_back(inMsg);
               LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << ":" << msghdr->rank << ": " << toString(msghdr->type) << " message queued while waiting for in progress message " <<
                             toString(_inProgressMessage) << " to complete (" << _queuedMessages.size() << " messages in queue)");
               LOG_CIOS_INFO_MSG("Job " << msghdr->jobId << ":" << msghdr->rank << ": " << toString(msghdr->type) << " message queued processing completion " << index <<
                            " of " << numCompletions);
            }
            
            break;
         }
      
         case IBV_WC_RDMA_WRITE:
         {
            LOG_CIOS_TRACE_MSG("rdma write operation completed for queue pair " << completion->qp_num);
            if (completion->wr_id == requestId) {
               rc = true;
            }
            break;
         }
         
         case IBV_WC_RDMA_READ:
         {
            if (completion->wr_id == requestId) {
               rc = true;
            }
            else {
               bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)completion->wr_id;
               if (msghdr->type == WriteAck){
#if 0
                 ClientMessagePtr AckMsg = ClientMessagePtr(new ClientMessage(msghdr, 0 ) );
                 writeAck(AckMsg);
                 sendAckMessage();
#endif
               }
            }
            break;
         }
         
         case IBV_WC_SEND:
         {
            LOG_CIOS_TRACE_MSG("send operation completed for queue pair " << completion->qp_num);
            if (completion->wr_id == requestId) {
               rc = true;
            }
            break;
         }
         
         default:
         {
            LOG_ERROR_MSG("unsupported operation " << completion->opcode << " in work completion");
            break;
         }
      }
   }

   return rc;
}

bool
ClientMonitor::routeMessage(const ClientMessagePtr& message)
{
   _operationTimer.start();

   uint16_t type = message->getType();
   JobPtr job = _jobs.get(message->getJobId());

   if ( (job != NULL) && (job->isKilled()) && ( (KINTERNALBIT & type)==0  ) ) {
      if (type==Close) close(message); //always do close
      else {
          // Build ack message in outbound message region.
         _ackMessage =  allocateClientAckMessage(message, ErrorAck, sizeof(ErrorAckMessage) );
          ErrorAckMessage *outMsg = (ErrorAckMessage *)_ackMessage;
          outMsg->header.returnCode = bgcios::RequestFailed;
          outMsg->header.errorCode = EINTR;
          LOG_CIOS_DEBUG_MSG("Job " << message->getJobId() << "." << message->getRank() << ": " << toString(type) << " message was interrupted because job was killed");
      }
   }
   else if ( (job != NULL) && (job-> removedRankEINTR( message->getRank() ) ) ){
          // Build ack message in outbound message region.
         _ackMessage =  allocateClientAckMessage(message, ErrorAck, sizeof(ErrorAckMessage) );
          ErrorAckMessage *outMsg = (ErrorAckMessage *)_ackMessage;
          outMsg->header.returnCode = bgcios::RequestFailed;
          outMsg->header.errorCode = EINTR;
          LOG_CIOS_DEBUG_MSG("Job " << message->getJobId() << "." << message->getRank() << ": " << toString(type) << " message was interrupted by toolctld for job/rank");

   }
   else {
      switch (type) {
         case Accept: accept(message); break;
         case Access: access(message); break;
         case Bind: bind(message); break;
         case Chmod: chmod(message); break;
         case Chown: chown(message); break;

         case Close: close(message); break;
         case CloseKernelInternal: close(message); break;

         case Connect: connect(message); break;
         case Fchmod: fchmod(message); break;
         case Fchown: fchown(message); break;
         case Fcntl: fcntl(message); break;
         case Fstat64: fstat64(message); break;
         case Fstatfs64: fstatfs64(message); break;
         case Ftruncate64: ftruncate64(message); break;
         case Fsync: fsync(message); break;
         case Getdents64: getdents64(message); break;
         case Getpeername: getpeername(message); break;
         case Getsockname: getsockname(message); break;
         case Getsockopt: getsockopt(message); break;
         case Ioctl: ioctl(message); break;
         case Link: link(message); break;
         case Listen: listen(message); break;
         case Lseek64: lseek64(message); break;
         case Mkdir: mkdir(message); break;

         case Open: open(message); break;
         case OpenKernelInternal: open(message); break;

         case Poll: pollForCN(message); break;
         case Pread64: pread64(message); break;
         case Pwrite64: pwrite64(message); break;
         case Read: read(message); break;
         case Readlink: readlink(message); break;
         case Recv: recv(message); break;
         case Recvfrom: recvfrom(message); break;
         case Rename: rename(message); break;
         case Send: send(message); break;
         case Sendto: sendto(message); break;
         case Shutdown: shutdown(message); break;
         case Setsockopt: setsockopt(message); break;
         case Socket: socket(message); break;
         case Stat64: stat64(message); break;
         case Statfs64: statfs64(message); break;
         case Symlink: symlink(message); break;
         case Truncate64: truncate64(message); break;
         case Unlink: unlink(message); break;
         case Utimes: utimes(message); break;

         case Write: write(message); break;
         case WriteKernelInternal: write(message); break;

         case WriteRdmaVirt: writeRdmaVirt(message); break;
         case WriteRdmaVirtKernelInternal: writeRdmaVirt(message); break;         

         case SetupJob: setupJob(message); break;
         case CleanupJob: cleanupJob(message); break;

         default: return false; break;
      }
 }

   // Send reply message in outbound message buffer to client if needed.
   if ( (job != NULL) && (job-> removedRankEINTR( message->getRank() ) ) ){
     LOG_CIOS_DEBUG_MSG("removed message->getRank="<<message->getRank());
   }
   sendAckMessage();

   _operationTimer.stop();

   return true;
}

void 
ClientMonitor::logFunctionShipError(bgcios::MessageHeader *msghdr){
   if (msghdr->returnCode != bgcios::Success) {
     LOG_ERROR_MSG("Job " << msghdr->jobId << ":" << msghdr->rank << ": " << toString(msghdr->type) << " message was not successful, return code: " <<
                       bgcios::returnCodeToString(msghdr->returnCode) << " (" << msghdr->returnCode << "), error code: " <<
                       bgcios::errorString((int)msghdr->errorCode) << " (" << msghdr->errorCode << ")");
   }
}

uint64_t
ClientMonitor::sendAckMessage(void)
{
   // Just return if there is no ack message ready.
   if (_ackMessage == NULL) {
      //LOG_ERROR_MSG("_ackMessage is NULL at LINE="<<__LINE__);
      return 0;
   }

   if (_logFunctionShipErrors) logFunctionShipError(_ackMessage);
      
   _client->postSend(_outMessageRegion , _ackMessage, _ackMessage->length, _ackMessage->sequenceId);

   _ackMessage=NULL;

   return 0;
}


void
ClientMonitor::waitForCompletion(uint16_t type, uint64_t requestId)
{
   // Initialize the set of descriptors to monitor.
   bool ready = false;
#if 0
   struct pollfd pollSet;
   pollSet.fd = _completionChannel->getChannelFd();
   pollSet.events = POLLIN;
   pollSet.revents = 0;

   // Wait for an event on the completion channel.
   do {
      int rc = poll(&pollSet, 1, -1);
      if (rc == -1) {
         int err = errno;
         LOG_ERROR_MSG("error polling descriptors while waiting for completion: " << bgcios::errorString(err));
         continue;
      }

      if (pollSet.revents & POLLIN) {
         _inProgressMessage = type;
         if (completionChannelHandler(requestId)) {
            ready = true;
         }
         _inProgressMessage = 0;
      }
   } while (!ready);
#endif

   _inProgressMessage = type;
   do {
      if (completionChannelHandler(requestId)) {
         ready = true;
      }

   } while (!ready);
   _inProgressMessage = 0;
   return;
}

uint32_t
ClientMonitor::putData(const ClientMessagePtr& message, uint64_t address, uint32_t rkey, uint32_t length)
{
   uint32_t rc = 0;
   try {
      // Post a rdma write request to the send queue using the large message region.
      _largeRegion->setMessageLength(length);
      uint64_t requestId = _client->postRdmaWrite(_largeRegion, address, rkey);
      LOG_CIOS_DEBUG_MSG("Job " << message->getJobId() << ":" << message->getRank() << ": " << toString(message->getType()) << " message posted rdma write to address 0x" <<
                    std::hex << address << std::dec << " using rkey " << rkey << " for " << length << " bytes");

      // Wait for notification that the rdma write completed.
      waitForCompletion(message->getType(), requestId);
   }

   catch (const RdmaError& e) {
      LOG_ERROR_MSG("error transferring data to client with rdma write: " << bgcios::errorString(e.errcode()));
      rc = (uint32_t)e.errcode();
   }

   return rc;
}

uint32_t
ClientMonitor::getData(const ClientMessagePtr& message, uint64_t address, uint32_t rkey, uint32_t length)
{
   uint32_t rc = 0;
   try {
      // Post a rdma read request to the send queue using the large message region.
      _largeRegion->setMessageLength(length);
      uint64_t requestId = _client->postRdmaRead(_largeRegion, address, rkey);
      LOG_CIOS_DEBUG_MSG("Job " << message->getJobId() << ":" << message->getRank() << ": " << toString(message->getType()) << " message posted rdma read from address 0x" <<
                    std::hex << address << std::dec << " using rkey " << rkey << " for " << length << " bytes");

      // Wait for notification that the rdma read completed.
      waitForCompletion(message->getType(), requestId);
   }

   catch (const RdmaError& e) {
      LOG_ERROR_MSG("error transferring data frim client with rdma read: " << bgcios::errorString(e.errcode()));
      rc = (uint32_t)e.errcode();
   }

   return rc;
}

void
ClientMonitor::accept(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   AcceptMessage *inMsg = (AcceptMessage *)message->getAddress();
   
   // Build ack message in outbound message region.
   _ackMessage =  allocateClientAckMessage(message, AcceptAck, sizeof(AcceptAckMessage));

   AcceptAckMessage *outMsg = (AcceptAckMessage *)_ackMessage;

   outMsg->addrlen = inMsg->addrlen;
   if (outMsg->addrlen) memcpy(&outMsg->addr,&outMsg->addr, outMsg->addrlen);

   // Run the operation.
   outMsg->sockfd = ::accept(inMsg->sockfd, (sockaddr *)outMsg->addr, &outMsg->addrlen);
   if (outMsg->sockfd== (-1) ){
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }
   return;
}

void
ClientMonitor::access(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   AccessMessage *inMsg = (AccessMessage *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =  allocateClientAckMessage(message, AccessAck, sizeof(AccessAckMessage));
   AccessAckMessage *outMsg = (AccessAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::faccessat(inMsg->dirfd, pathname, inMsg->type, inMsg->flags);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": AccessAck message is ready, pathname=" << pathname <<
                 " type=" << inMsg->type << " flags=" << inMsg->flags << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::bind(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   BindMessage *inMsg = (BindMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,  BindAck, sizeof(BindAckMessage));
   BindAckMessage *outMsg = (BindAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::bind(inMsg->sockfd, (const sockaddr *)inMsg->addr, inMsg->addrlen);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": BindAck message is ready, sockfd=" << inMsg->sockfd <<
                 " addrlen=" << inMsg->addrlen << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::chmod(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   ChmodMessage *inMsg = (ChmodMessage *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   ChmodAck, sizeof(ChmodAckMessage));
   ChmodAckMessage *outMsg = (ChmodAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::fchmodat(inMsg->dirfd, pathname, inMsg->mode, inMsg->flags);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": ChmodAck message is ready, pathname=" << pathname <<
                 " mode=" << inMsg->mode << " flags=" << inMsg->flags << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::chown(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   ChownMessage *inMsg = (ChownMessage *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   ChownAck, sizeof(ChownAckMessage));
   ChownAckMessage *outMsg = (ChownAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::fchownat(inMsg->dirfd, pathname, inMsg->uid, inMsg->gid, inMsg->flags);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": ChownAck message is ready pathname=" << pathname <<
                 " uid=" << inMsg->uid << " gid=" << inMsg->gid << " flags=" << inMsg->flags << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::close(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   CloseMessage *inMsg = (CloseMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   CloseAck, sizeof(CloseAckMessage));
   CloseAckMessage *outMsg = (CloseAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::close(inMsg->fd);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": CloseAck message is ready, fd=" << inMsg->fd <<
                 " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::connect(const ClientMessagePtr& message)
{
   // Get pointer to inbound message.
   ConnectMessage *inMsg = (ConnectMessage *)message->getAddress();

   // Save the message so inbound message region is available for next message.
   if (!message->isSaved()) {
      inMsg = (ConnectMessage *)message->save();
   }

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   ConnectAck, sizeof(ConnectAckMessage));
   ConnectAckMessage *outMsg = (ConnectAckMessage *)_ackMessage;
   outMsg->header.returnCode = bgcios::Success;

   // Run the operation 
   int rc = ::connect(inMsg->sockfd, (const sockaddr *)inMsg->addr, inMsg->addrlen);
   while ( (rc!=0) && 
   (   (errno==EINPROGRESS) || (errno==EALREADY)  )
         ){
     rc = ::connect(inMsg->sockfd, (const sockaddr *)inMsg->addr, inMsg->addrlen);     
   }
   if (rc != 0) {
        outMsg->header.returnCode = bgcios::RequestFailed;
        outMsg->header.errorCode = (uint32_t)errno;
   }

   // If operation completed, message is ready in outbound message region.
   if (_ackMessage != NULL) {
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": ConnectAck message is ready, sockfd=" << inMsg->sockfd <<
                    " addrlen=" << inMsg->addrlen << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   }

   return;
}

void
ClientMonitor::fchmod(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   FchmodMessage *inMsg = (FchmodMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   FchmodAck, sizeof(FchmodAckMessage));
   FchmodAckMessage *outMsg = (FchmodAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::fchmod(inMsg->fd, inMsg->mode);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": FchmodAck message is ready fd=" << inMsg->fd <<
                 " mode=" << inMsg->mode << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::fchown(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   FchownMessage *inMsg = (FchownMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   FchownAck, sizeof(FchownAckMessage));
   FchownAckMessage *outMsg = (FchownAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::fchown(inMsg->fd, inMsg->uid, inMsg->gid);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": FchownAck message is ready fd=" << inMsg->fd <<
                 " uid=" << inMsg->uid << " gid=" << inMsg->gid << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::fcntl(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   FcntlMessage *inMsg = (FcntlMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   FcntlAck, sizeof(FcntlAckMessage));
   FcntlAckMessage *outMsg = (FcntlAckMessage *)_ackMessage;

   // Set the value for the third parameter.
   int64_t parm3;
   switch (inMsg->cmd) {
      case F_GETLK:
         memcpy(&outMsg->lock, &inMsg->lock, sizeof(struct flock));
         parm3 = (int64_t)&outMsg->lock;
         break;

      case F_SETLK:
      case F_SETLKW:
         parm3 = (int64_t)&inMsg->lock;
         break;

      default:
         parm3 = inMsg->arg;
         break;
   }

   // Run the operation.
   outMsg->retval = ::fcntl(inMsg->fd, inMsg->cmd, parm3);
   if (outMsg->retval >= 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno;
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": FcntlAck message is ready fd=" << inMsg->fd <<
                 " cmd=" << inMsg->cmd << " arg=" << inMsg->arg << " rc=" << outMsg->retval << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::fstat64(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   Fstat64Message *inMsg = (Fstat64Message *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   Fstat64Ack, sizeof(Fstat64AckMessage));
   Fstat64AckMessage *outMsg = (Fstat64AckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::fstat64(inMsg->fd, &(outMsg->buf));
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Fstat64Ack message is ready, fd=" << inMsg->fd <<
                 " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::fstatfs64(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   Fstatfs64Message *inMsg = (Fstatfs64Message *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   Fstatfs64Ack, sizeof(Fstatfs64AckMessage));
   Fstatfs64AckMessage *outMsg = (Fstatfs64AckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::fstatfs64(inMsg->fd, &(outMsg->buf));
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Fstatfs64Ack message is ready, fd=" << inMsg->fd <<
                 " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::ftruncate64(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   Ftruncate64Message *inMsg = (Ftruncate64Message *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message, Ftruncate64Ack, sizeof(Ftruncate64AckMessage));
   Ftruncate64AckMessage *outMsg = (Ftruncate64AckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::ftruncate64(inMsg->fd, inMsg->length);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Ftruncate64Ack message is ready fd=" << inMsg->fd <<
                 "length=" << inMsg->length << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::fsync(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   FsyncMessage *inMsg = (FsyncMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   FsyncAck, sizeof(FsyncAckMessage));
   FsyncAckMessage *outMsg = (FsyncAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::fsync(inMsg->fd);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": FsyncAck message is ready, fd=" << inMsg->fd <<
                 " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::getdents64(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   Getdents64Message *inMsg = (Getdents64Message *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message, Getdents64Ack, sizeof(Getdents64AckMessage));
   Getdents64AckMessage *outMsg = (Getdents64AckMessage *)_ackMessage;

   // Truncate the length to fit in the large memory region.
   if (inMsg->length > _largeRegion->getLength()) {
      inMsg->length = _largeRegion->getLength();
   }

   // Run the operation (put the data in the large memory region).
   off64_t offset = 0; // Store to start of buffer
   outMsg->bytes = ::getdirentries64(inMsg->fd, (char *)_largeRegion->getAddress(), inMsg->length, &offset);
   if (outMsg->bytes >= 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno;
      outMsg->bytes = 0;
   }

   // Send the data to the remote node when successful and there is data.
   if ((outMsg->header.returnCode == bgcios::Success) && (outMsg->bytes > 0)) {
      uint32_t err = putData(message, inMsg->address, inMsg->rkey, (uint32_t)outMsg->bytes);
      if (err != 0) {
         outMsg->header.returnCode = bgcios::RequestFailed;
         outMsg->header.errorCode = err;
      }
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Getdents64Ack message is ready fd=" << inMsg->fd <<
                 " length=" << inMsg->length << " rc=" << outMsg->bytes << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::getpeername(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   GetpeernameMessage *inMsg = (GetpeernameMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message, GetpeernameAck, sizeof(GetpeernameAckMessage));
   GetpeernameAckMessage *outMsg = (GetpeernameAckMessage *)_ackMessage;

   // Run the operation.
   outMsg->addrlen = inMsg->addrlen;
   int rc = ::getpeername(inMsg->sockfd, (sockaddr *)outMsg->addr, &(outMsg->addrlen));
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Getpeername message is ready, sockfd=" << inMsg->sockfd <<
                 " addrlen=" << outMsg->addrlen << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::getsockname(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   GetsocknameMessage *inMsg = (GetsocknameMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message, GetsocknameAck, sizeof(GetsocknameAckMessage));
   GetsocknameAckMessage *outMsg = (GetsocknameAckMessage *)_ackMessage;

   // Run the operation.
   outMsg->addrlen = inMsg->addrlen;
   int rc = ::getsockname(inMsg->sockfd, (sockaddr *)outMsg->addr, &(outMsg->addrlen));
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Getsockname message is ready, sockfd=" << inMsg->sockfd <<
                 " addrlen=" << outMsg->addrlen << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::getsockopt(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   GetsockoptMessage *inMsg = (GetsockoptMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message, GetsockoptAck, sizeof(GetsockoptAckMessage));
   GetsockoptAckMessage *outMsg = (GetsockoptAckMessage *)_ackMessage;

   // Run the operation.
   outMsg->optlen = inMsg->optlen;
   int rc = ::getsockopt(inMsg->sockfd, inMsg->level, inMsg->name, outMsg->value, &(outMsg->optlen));
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Getsockopt message is ready, sockfd=" << inMsg->sockfd <<
                 " level=" << inMsg->level << " name=" << inMsg->name << " optlen=" << outMsg->optlen << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::ioctl(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   IoctlMessage *inMsg = (IoctlMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message, IoctlAck, sizeof(IoctlAckMessage));
   IoctlAckMessage *outMsg = (IoctlAckMessage *)_ackMessage;

   // Run the operation.
   switch (inMsg->cmd) {
      case FIONBIO:
         outMsg->retval = ::ioctl(inMsg->fd, inMsg->cmd, &(inMsg->arg));
         break;

      case FIONREAD:
      case 0x800466af: // LL_IOC_GET_MDTIDX for Lustre
         outMsg->retval = ::ioctl(inMsg->fd, inMsg->cmd, &(outMsg->arg));
         break;

      case TCGETA:
      case TCGETS:
      case 0x402c7413: // Special value used by isatty()
         outMsg->retval = ::ioctl(inMsg->fd, inMsg->cmd, &(outMsg->termios));
         break;

      case 0x800866a8: // LL_IOC_GETOBDCOUNT for Lustre
         outMsg->retval = ::ioctl(inMsg->fd, inMsg->cmd, &(inMsg->arg));
         outMsg->arg = inMsg->arg;
         break;

      default:
         outMsg->retval = -1;
         errno = EINVAL;
         break;
   }

   if (outMsg->retval >= 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno;
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": IoctlAck message is ready fd=" << inMsg->fd <<
                 " cmd=0x" << std::hex << inMsg->cmd << " arg=0x" << inMsg->arg << std::dec << " rc=" << outMsg->retval << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::link(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   LinkMessage *inMsg = (LinkMessage *)message->getAddress();
   char *oldpathname = (char *)inMsg + inMsg->oldoffset;
   char *newpathname = (char *)inMsg + inMsg->newoffset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message, LinkAck, sizeof(LinkAckMessage));
   LinkAckMessage *outMsg = (LinkAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::linkat(inMsg->olddirfd, oldpathname, inMsg->newdirfd, newpathname, inMsg->flags);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": LinkAck message is ready, oldpathname='" << oldpathname <<
                 "' newpathname='" << newpathname << "' flags=" << inMsg->flags << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::listen(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   ListenMessage *inMsg = (ListenMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   ListenAck, sizeof(ListenAckMessage));
   ListenAckMessage *outMsg = (ListenAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::listen(inMsg->sockfd, inMsg->backlog);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": ListenAck message is ready, sockfd=" << inMsg->sockfd <<
                 " backlog=" << inMsg->backlog << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

//! \brief  Process a Lseek64 message and reply with a Lseek64Ack message.
//! \return Nothing.

void
ClientMonitor::lseek64(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   Lseek64Message *inMsg = (Lseek64Message *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   Lseek64Ack, sizeof(Lseek64AckMessage));
   Lseek64AckMessage *outMsg = (Lseek64AckMessage *)_ackMessage;

   // Run the operation.
   outMsg->result = ::lseek64(inMsg->fd, inMsg->offset, inMsg->whence);
   if (outMsg->result != (off64_t)-1) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Lseek64Ack message is ready, fd=" << inMsg->fd <<
                 " offset=" << inMsg->offset << " whence=" << inMsg->whence << " result=" << outMsg->result << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::mkdir(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   MkdirMessage *inMsg = (MkdirMessage *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   MkdirAck, sizeof(MkdirAckMessage));
   MkdirAckMessage *outMsg = (MkdirAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::mkdirat(inMsg->dirfd, pathname, inMsg->mode);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": MkdirAck message is ready, pathname=" << pathname <<
                 " mode=" << inMsg->mode << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::open(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   OpenMessage *inMsg = (OpenMessage *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   OpenAck, sizeof(OpenAckMessage));
   OpenAckMessage *outMsg = (OpenAckMessage *)_ackMessage;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   
   if (job-> logJobStatistics() ) job->openTimer.start();
   // Run the operation.
   outMsg->fd = ::openat(inMsg->dirfd, pathname, inMsg->flags, inMsg->mode);
   if (outMsg->fd >= 0) {
      outMsg->header.returnCode = bgcios::Success;
      JobPtr job = _jobs.get(inMsg->header.jobId);
      job->setShortCircuitFd(pathname, outMsg->fd);
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
      if ( (EPERM==errno) || (EFAULT==errno) ) {
        LOG_INFO_MSG_FORCED("Job=" << inMsg->header.jobId << ":" << inMsg->header.rank << ": Open pathname='" << pathname << 
        " inMsg->dirfd="<<inMsg->dirfd<<std::hex << std::showbase << "' flags=" << inMsg->flags << std::hex << std::showbase <<
        " mode=" << inMsg->mode << " fd=" << outMsg->fd << " errno=" << outMsg->header.errorCode);
        //testing
        LOG_INFO_MSG_FORCED("::open euid="<<geteuid()<<" egid="<<getegid()<<" uid="<<getuid()<<" gid="<<getgid() );
        errno=0;
        int ngroups=getgroups(0, NULL);
        LOG_INFO_MSG_FORCED("Supplementary Group IDs number="<<ngroups<<" errno="<<errno);  
        if (ngroups > 0){
          gid_t group[ngroups];
          getgroups(ngroups, group);
          LOG_INFO_MSG_FORCED("List of groups next,  number="<<ngroups<<" errno="<<errno);         
          for (int i=0;i<ngroups;i++){
          int gid = (int)group[i];
          LOG_INFO_MSG_FORCED("group i="<<i<<" group[i]="<< gid<<" group name="<< getgrgid(group[i])->gr_name);
          }
        }
      }
   }
   job->openTimer.stop();

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Open pathname='" << pathname << 
" inMsg->dirfd="<<inMsg->dirfd<<std::hex << std::showbase << "' flags=" << inMsg->flags << std::hex << std::showbase <<
" mode=" << inMsg->mode << " fd=" << outMsg->fd << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::pollForCN(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   PollMessage *inMsg = (PollMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   PollAck, sizeof(PollAckMessage));
   PollAckMessage *outMsg = (PollAckMessage *)_ackMessage;

   memcpy(&outMsg->pollBasic,&inMsg->pollBasic,sizeof(struct PollBasic) );

   outMsg->header.returnCode = (uint32_t)::poll(outMsg->pollBasic.fds, outMsg->pollBasic.nfd,10);

  if (outMsg->header.returnCode==uint32_t(-1)) {
   outMsg->header.errorCode = (uint32_t)errno;
   return;
  }
  return;
}

void
ClientMonitor::pread64(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   Pread64Message *inMsg = (Pread64Message *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   Pread64Ack, sizeof(Pread64AckMessage));
   Pread64AckMessage *outMsg = (Pread64AckMessage *)_ackMessage;
   outMsg->bytes = 0;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   // For performance, I'm not logging an error if the job is not found.

   // Run the operation until all of the data is read or an error occurs.
   uint64_t address = inMsg->address;
   size_t length = 0;
   off64_t position = inMsg->position;
   size_t bytesLeft = inMsg->length;

   while (bytesLeft > 0) {
      // Calculate the length of data for this operation.
      if (bytesLeft > _largeRegion->getLength()) {
         length = _largeRegion->getLength();
      }
      else {
         length = bytesLeft;
      }

      // Run the operation, putting the data in the large memory region.
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Pread64 message started syscall, fd=" << inMsg->fd <<
                    " length=" << length << " position=" << position);
      if (job-> logJobStatistics() ) job->preadTimer.start();
      ssize_t rc;
      uint32_t error = 0;
      if (inMsg->fd != job->getShortCircuitFd()) {
         rc = ::pread64(inMsg->fd, _largeRegion->getAddress(), length, position);
         error = (uint32_t)errno;
      }
      else {
         rc = (ssize_t)length;
      }
      job->preadTimer.stop();
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Pread64 message completed syscall, fd=" << inMsg->fd <<
                    " rc=" << rc << " errno=" << error);

      // Send the data to the compute node when successful and there is data.
      if (rc > 0) {
         outMsg->header.returnCode = bgcios::Success;
         outMsg->bytes += rc;

         error = putData(message, address, inMsg->rkey, (uint32_t)rc); 
         if (error != 0) {
            outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
            outMsg->header.errorCode = error;
            bytesLeft = 0; // Force exit from loop because there was an error
            continue;
         }

         // Adjust for next operation.
         if (job->posixMode()) {
            bytesLeft = 0; // Force exit from loop because only one operation per message
         }
         else {
            address += (uint64_t)rc;
            position += (off64_t)rc;
            bytesLeft -= (size_t)rc;
         }
      }

      // There is no more data available from the descriptor.
      else if (rc == 0) {
         outMsg->header.returnCode = bgcios::Success;
         bytesLeft = 0; // Force exit from loop
      }

      // There was an error reading from the descriptor.
      else {
         outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
         outMsg->header.errorCode = error;
         bytesLeft = 0; // Force exit from loop
      }
   }

   // Pead64Ack message is ready in the outbound message region.
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Pread64Ack message is ready, fd=" << inMsg->fd <<
                 " length=" << inMsg->length << " bytes=" << outMsg->bytes << " rc=" << outMsg->header.returnCode << " errno=" << outMsg->header.errorCode);

   return;
}

void
ClientMonitor::pwrite64(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   Pwrite64Message *inMsg = (Pwrite64Message *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   Pwrite64Ack, sizeof(Pwrite64AckMessage));
   Pwrite64AckMessage *outMsg = (Pwrite64AckMessage *)_ackMessage;
   outMsg->bytes = 0;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   // For performance, I'm not logging an error if the job is not found.

   // Run the operation until all of the data is written or an error occurs.
   uint64_t address = inMsg->address;
   size_t length = 0;
   off64_t position = inMsg->position;
   size_t bytesLeft = inMsg->length;

   while (bytesLeft > 0) {
      // Calculate the length of data for this operation.
      if (bytesLeft > _largeRegion->getLength()) {
         length = _largeRegion->getLength();
      }
      else {
         length = bytesLeft;
      }

      // Receive the data from the compute node.
      uint32_t error = getData(message, address, inMsg->rkey, (uint32_t)length);
      if (error != 0) {
         outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
         outMsg->header.errorCode = error;
         bytesLeft = 0; // Force exit from loop
         continue;
      }

      // Run the operation.
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Pwrite64 message started syscall, fd=" << inMsg->fd <<
                    " length=" << length << " position=" << position);
      ssize_t rc;
      error = 0;
      if (job-> logJobStatistics() ) job->pwriteTimer.start();
      if (inMsg->fd != job->getShortCircuitFd()) {
         rc = ::pwrite64(inMsg->fd, _largeRegion->getAddress(), length, position);
         error = (uint32_t)errno;
      }
      else {
         rc = (ssize_t)length;
      }
      job->pwriteTimer.stop();
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Pwrite64 message completed syscall, fd=" << inMsg->fd <<
                    " rc=" << rc << " errno=" << error);

      if (rc >= 0) {
         outMsg->header.returnCode = bgcios::Success;
         outMsg->bytes += rc;
         address += (uint64_t)rc;
         position += (off64_t)rc;
         bytesLeft -= (size_t)rc;
      }
      else {
         outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
         outMsg->header.errorCode = (uint32_t)errno;
         bytesLeft = 0; // Force exit from loop
         continue;
      }
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Pwrite64Ack message is ready, fd=" << inMsg->fd <<
                 " length=" << inMsg->length << " bytes=" << outMsg->bytes << " rc=" << outMsg->header.returnCode << " errno=" << outMsg->header.errorCode);
   return;
}


//! Response message for read system call.

void
ClientMonitor::read(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   ReadMessage *inMsg = (ReadMessage *)message->getAddress();

   // Save the message so inbound message region is available for next message.
   if (!message->isSaved()) {
      inMsg = (ReadMessage *)message->save();
   }

   // Build ack message in outbond message region.
   _ackMessage = allocateClientAckMessage(message,   ReadAck, sizeof(ReadAckMessage));
   ReadAckMessage *outMsg = (ReadAckMessage *)_ackMessage;
   outMsg->bytes = 0;

   // When message is first received, let client know operation is in progress.
   if (!message->isInProgress()) {
      message->setInProgress(true);
   }

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   // For performance, I'm not logging an error if the job is not found.

   // Run the operation until all of the data is read or an error occurs.
   uint64_t address = inMsg->address;
   size_t length = 0;
   size_t bytesLeft = inMsg->length;

   while (bytesLeft > 0) {
      // Calculate the length of data for this operation.
      if (bytesLeft > _largeRegion->getLength()) {
         length = _largeRegion->getLength();
      }
      else {
         length = bytesLeft;
      }

      // Run the operation, putting the data in the large memory region.
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Read message started syscall, fd=" << inMsg->fd <<
                    " length=" << length);
      if (job-> logJobStatistics() ) job->readTimer.start();
      ssize_t rc;
      uint32_t error = 0;
      if (inMsg->fd != job->getShortCircuitFd()) {
         rc = ::read(inMsg->fd, _largeRegion->getAddress(), length);
         error = (uint32_t)errno;
      }
      else {
         rc = (ssize_t)length;
      }
      job->readTimer.stop();
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Read message completed syscall, fd=" << inMsg->fd <<
                    " rc=" << rc << " errno=" << error);

      // Send the data to the compute node when successful and there is data.
      if (rc > 0) {
         outMsg->header.returnCode = bgcios::Success;
         outMsg->bytes += rc;

         error = putData(message, address, inMsg->rkey, (uint32_t)rc); 
         if (error != 0) {
            outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
            outMsg->header.errorCode = error;
            bytesLeft = 0; // Force exit from loop because there was an error
            continue;
         }

         // Adjust for next operation.
         if (job->posixMode()) {
            bytesLeft = 0; // Force exit from loop because only one operation per message
         }
         else {
            address += (uint64_t)rc;
            bytesLeft -= (size_t)rc;
            inMsg->length -= (size_t)rc;
         }
      }

      // There is no more data available from the descriptor.
      else if (rc == 0) {
         outMsg->header.returnCode = bgcios::Success;
         bytesLeft = 0; // Force exit from loop
      }

      // There was an error reading from the descriptor.
      else {
         if (error == EWOULDBLOCK) {
            addBlockedMessage(inMsg->fd, POLLIN, message);
            _ackMessage=NULL;  //No ack message yet
            LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Read message is blocked for fd=" << inMsg->fd);
         }
         else {
            outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
            outMsg->header.errorCode = error;
         }
         bytesLeft = 0; // Force exit from loop
      }
   }

   // ReadAck message is ready in the outbound message region.
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": ReadAck message is ready, fd=" << inMsg->fd <<
                 " length=" << inMsg->length << " bytes=" << outMsg->bytes << " rc=" << outMsg->header.returnCode << " errno=" << outMsg->header.errorCode);

   return;
}

void
ClientMonitor::readlink(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   ReadlinkMessage *inMsg = (ReadlinkMessage *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   ReadlinkAck, sizeof(ReadlinkAckMessage));
   ReadlinkAckMessage *outMsg = (ReadlinkAckMessage *)_ackMessage;

   // Truncate the length to fit in the large memory region.
   if (inMsg->length > _largeRegion->getLength()) {
      inMsg->length = _largeRegion->getLength();
   }

   // Run the operation (put the data in the large memory region).
   outMsg->length = ::readlinkat(inMsg->dirfd, pathname, (char *)_largeRegion->getAddress(), inMsg->length);
   if (outMsg->length >= 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno;
      outMsg->length = 0;
   }

   // Send the data to the remote node when successful and there is data.
   if ((outMsg->header.returnCode == bgcios::Success) && (outMsg->length > 0)) {
      uint32_t err = putData(message, inMsg->address, inMsg->rkey, (uint32_t)outMsg->length);
      if (err != 0) {
         outMsg->header.returnCode = bgcios::RequestFailed;
         outMsg->header.errorCode = (uint32_t)err;
      }
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": ReadlinkAck message is ready, pathname=" << pathname <<
                 " length=" << inMsg->length << " rc=" << outMsg->length << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::recv(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   RecvMessage *inMsg = (RecvMessage *)message->getAddress();

   // Save the message so inbound message region is available for next message.
   if (!message->isSaved()) {
      inMsg = (RecvMessage *)message->save();
   }

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   RecvAck, sizeof(RecvAckMessage));
   RecvAckMessage *outMsg = (RecvAckMessage *)_ackMessage;
   outMsg->bytes = 0;

   // Truncate the length to fit in the large memory region.
   if (inMsg->length > _largeRegion->getLength()) {
      inMsg->length = _largeRegion->getLength();
   }

   // When message is first received, let client know operation is in progress.
   if (!message->isInProgress()) {
      message->setInProgress(true);
   }

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   // For performance, I'm not logging an error if the job is not found.

   // Run the operation (put the data in the large memory region).
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Recv message started syscall, sockfd=" << inMsg->sockfd <<
                 " length=" << inMsg->length);
   if (job-> logJobStatistics() ) job->recvTimer.start();
   ssize_t rc = ::recv(inMsg->sockfd, _largeRegion->getAddress(), inMsg->length, inMsg->flags);
   uint32_t err = (uint32_t)errno;
   if (job-> logJobStatistics() ) job->recvTimer.stop();
   if (rc >= 0) {
      outMsg->header.returnCode = bgcios::Success;
      outMsg->bytes = rc;
   }
   else {
      if (err == EWOULDBLOCK) {
         addBlockedMessage(inMsg->sockfd, POLLIN, message);
         _ackMessage=NULL; // Reset so second RecvAck message is not sent yet
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Recv message is blocked, sockfd=" << inMsg->sockfd);
      }
      else {
         outMsg->header.returnCode = bgcios::RequestFailed;
         outMsg->header.errorCode = err;
      }
   }
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Recv message completed syscall, sockfd=" << inMsg->sockfd <<
                 " rc=" << rc << " errno=" << outMsg->header.errorCode);

   // Send the data to the remote node when successful and there is data.
   if ((outMsg->header.returnCode == bgcios::Success) && (outMsg->bytes > 0)) {
      err = putData(message, inMsg->address, inMsg->rkey, (uint32_t)outMsg->bytes);
      if (err != 0) {
         outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
         outMsg->header.errorCode = (uint32_t)err;
      }
   }

   // RecvAck message is ready in the outbound message region.
   if (_ackMessage != NULL) {
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": RecvAck message is ready, sockfd=" << inMsg->sockfd <<
                    " length=" << inMsg->length << " bytes=" << outMsg->bytes << " rc=" << outMsg->header.returnCode << " errno=" << outMsg->header.errorCode);
   }
   return;
}

void
ClientMonitor::recvfrom(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   RecvfromMessage *inMsg = (RecvfromMessage *)message->getAddress();

   // Save the message so inbound message region is available for next message.
   if (!message->isSaved()) {
      inMsg = (RecvfromMessage *)message->save();
   }

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   RecvfromAck, sizeof(RecvfromAckMessage));
   RecvfromAckMessage *outMsg = (RecvfromAckMessage *)_ackMessage;
   outMsg->bytes = 0;

   // Truncate the length to fit in the large memory region.
   if (inMsg->length > _largeRegion->getLength()) {
      inMsg->length = _largeRegion->getLength();
   }

   // When message is first received, let client know operation is in progress.
   if (!message->isInProgress()) {
      message->setInProgress(true);
   }

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   // For performance, I'm not logging an error if the job is not found.

   // Run the operation (put the data in the large memory region).
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Recvfrom message started syscall, sockfd=" << inMsg->sockfd <<
                 " length=" << inMsg->length);
   outMsg->addrlen = inMsg->addrlen;
   if (job-> logJobStatistics() ) job->recvfromTimer.start();
   ssize_t rc = ::recvfrom(inMsg->sockfd, _largeRegion->getAddress(), inMsg->length, inMsg->flags, (sockaddr *)outMsg->addr, &(outMsg->addrlen));
   uint32_t err = (uint32_t)errno;
   if (job-> logJobStatistics() ) job->recvfromTimer.stop();
   if (rc >= 0) {
      outMsg->header.returnCode = bgcios::Success;
      outMsg->bytes = rc;
   }
   else {
      if (errno == EWOULDBLOCK) {
         addBlockedMessage(inMsg->sockfd, POLLIN, message);
         _ackMessage=NULL; // Reset so second RecvfromAck message is not sent yet
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Recvfrom message is blocked for sockfd=" << inMsg->sockfd);
      }
      else {
         outMsg->header.returnCode = bgcios::RequestFailed;
         outMsg->header.errorCode = err;
      }
   }
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Recvfrom message completed syscall, sockfd=" << inMsg->sockfd <<
                 " rc=" << rc << " errno=" << outMsg->header.errorCode);

   // Send the data to the remote node when successful and there is data.
   if ((outMsg->header.returnCode == bgcios::Success) && (outMsg->bytes > 0)) {
      err = putData(message, inMsg->address, inMsg->rkey, (uint32_t)outMsg->bytes);
      if (err != 0) {
         outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
         outMsg->header.errorCode = err;
      }
   }

   // RecvfromAck message is ready in the outbound message region.
   if (_ackMessage != NULL) {
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": RecvfromAck message is ready, sockfd=" << inMsg->sockfd <<
                    " length=" << inMsg->length << " bytes=" << outMsg->bytes << " rc=" << outMsg->header.returnCode << " errno=" << outMsg->header.errorCode);
   }
   return;
}

void
ClientMonitor::rename(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   RenameMessage *inMsg = (RenameMessage *)message->getAddress();
   char *oldpathname = (char *)inMsg + inMsg->oldoffset;
   char *newpathname = (char *)inMsg + inMsg->newoffset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   RenameAck, sizeof(RenameAckMessage));
   RenameAckMessage *outMsg = (RenameAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::renameat(inMsg->olddirfd, oldpathname, inMsg->newdirfd, newpathname);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": RenameAck message is ready, oldpathname='" << oldpathname <<
                 "' newpathname='" << newpathname << "' rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::send(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   SendMessage *inMsg = (SendMessage *)message->getAddress();

   //LOG_CIOS_DEBUG_MSG("entry to ClientMonitor::send "<<message);
   // Save the message so inbound message region is available for next message.
   if (!message->isSaved()) {
      inMsg = (SendMessage *)message->save();
   }

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   SendAck, sizeof(SendAckMessage));
   SendAckMessage *outMsg = (SendAckMessage *)_ackMessage;
   outMsg->bytes = 0;

   // Truncate the length to fit in the large memory region.
   if (inMsg->length > _largeRegion->getLength()) {
      inMsg->length = _largeRegion->getLength();
   }

   // Get the data from the remote node.
   LOG_CIOS_DEBUG_MSG("getData-pre ClientMonitor::send "<<message);
   uint32_t err = getData(message, inMsg->address, inMsg->rkey, (uint32_t)inMsg->length);
   LOG_CIOS_DEBUG_MSG("getData-post ClientMonitor::send "<<message);

   // When message is first received, let client know operation is in progress.
   if (!message->isInProgress()) {
      message->setInProgress(true);
   }

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   // For performance, I'm not logging an error if the job is not found.

   // Run the operation.
   ssize_t rc = -1;
   if (err == 0) {
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Send message started syscall, sockfd=" << inMsg->sockfd <<
                    " length=" << inMsg->length);
      if (job-> logJobStatistics() ) job->sendTimer.start();
      rc = ::send(inMsg->sockfd, _largeRegion->getAddress(), inMsg->length, inMsg->flags);
      err = (uint32_t)errno;
      if (job-> logJobStatistics() ) job->sendTimer.stop();
      if (rc >= 0) {
         outMsg->header.returnCode = bgcios::Success;
         outMsg->bytes = rc;
      }
      else {
         // Add descriptor to the poll set to monitor for ability to write.
         if (err == EWOULDBLOCK) {
            _ackMessage=NULL; // Reset so second SendAck message is not sent yet
           LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Send message is blocked and added to poll set slot " <<
                          addBlockedMessage(inMsg->sockfd, POLLOUT, message) << ", sockfd=" << inMsg->sockfd);
         }
         else {
            outMsg->header.returnCode = bgcios::RequestFailed;
            outMsg->header.errorCode = err; 
         }
      }
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = err;
   }

   // If operation completed, message is ready in outbound message region.
   if (_ackMessage != NULL) {
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": SendAck message is ready, sockfd=" << inMsg->sockfd <<
                    " length=" << inMsg->length << " bytes=" << outMsg->bytes << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   }
   //LOG_CIOS_DEBUG_MSG("exit to ClientMonitor::send "<<message);
   return;
}

void
ClientMonitor::sendto(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   SendtoMessage *inMsg = (SendtoMessage *)message->getAddress();

   // Save the message so inbound message region is available for next message.
   if (!message->isSaved()) {
      inMsg = (SendtoMessage *)message->save();
   }

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   SendtoAck, sizeof(SendtoAckMessage));
   SendtoAckMessage *outMsg = (SendtoAckMessage *)_ackMessage;
   outMsg->bytes = 0;

   // Truncate the length to fit in the large memory region.
   if (inMsg->length > _largeRegion->getLength()) {
      inMsg->length = _largeRegion->getLength();
   }

   // Get the data from the remote node.
   uint32_t err = getData(message, inMsg->address, inMsg->rkey, (uint32_t)inMsg->length);

   // When message is first received, let client know operation is in progress.
   if (!message->isInProgress()) {
      message->setInProgress(true);
   }

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   // For performance, I'm not logging an error if the job is not found.

   // Run the operation.
   ssize_t rc = -1;
   if (err == 0) {
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Sendto message started syscall, sockfd=" << inMsg->sockfd <<
                    " length=" << inMsg->length);
      if (job-> logJobStatistics() ) job->sendtoTimer.start();
      rc = ::sendto(inMsg->sockfd, _largeRegion->getAddress(), inMsg->length, inMsg->flags, (const sockaddr *)inMsg->addr, inMsg->addrlen);
      err = (uint32_t)errno;
      if (job-> logJobStatistics() ) job->sendtoTimer.stop();
      if (rc >= 0) {
         outMsg->header.returnCode = bgcios::Success;
         outMsg->bytes = rc;
      }
      else {
         // Add descriptor to the poll set to be notified when operation would not block.
         if (errno == EWOULDBLOCK) {
            addBlockedMessage(inMsg->sockfd, POLLOUT, message);
            _ackMessage=NULL; // Reset so second SendtoAck message is not sent yet
            LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Sendto message is blocked for sockfd=" << inMsg->sockfd);
         }
         else {
            outMsg->header.returnCode = bgcios::RequestFailed;
            outMsg->header.errorCode = err; 
         }
      }
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = err;
   }

   // If operation completed, message is ready in outbound message region.
   if (_ackMessage != NULL) {
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": SendtoAck message is ready, sockfd=" << inMsg->sockfd <<
                    " length=" << inMsg->length << " bytes=" << outMsg->bytes << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   }
   return;
}

void
ClientMonitor::setsockopt(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   SetsockoptMessage *inMsg = (SetsockoptMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   SetsockoptAck, sizeof(SetsockoptAckMessage));
   SetsockoptAckMessage *outMsg = (SetsockoptAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::setsockopt(inMsg->sockfd, inMsg->level, inMsg->name, inMsg->value, inMsg->optlen);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": SetsockoptAck message is ready, sockfd=" << inMsg->sockfd <<
                 " level=" << inMsg->level << " name=" << inMsg->name << " optlen=" << inMsg->optlen << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::shutdown(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   ShutdownMessage *inMsg = (ShutdownMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   ShutdownAck, sizeof(ShutdownAckMessage));
   ShutdownAckMessage *outMsg = (ShutdownAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::shutdown(inMsg->sockfd, inMsg->how);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": ShutdownAck message is ready, sockfd=" << inMsg->sockfd <<
                 " how=" << inMsg->how << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::socket(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   SocketMessage *inMsg = (SocketMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   SocketAck, sizeof(SocketAckMessage));
   SocketAckMessage *outMsg = (SocketAckMessage *)_ackMessage;

   // Run the operation.
   outMsg->sockfd = ::socket(inMsg->domain, inMsg->type, inMsg->protocol);
   if (outMsg->sockfd >= 0) {
      outMsg->header.returnCode = bgcios::Success;

      // A socket is implicitly non-blocking.
      int nonblock = 1;
      ::ioctl(outMsg->sockfd, FIONBIO, &nonblock);
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": SocketAck message is ready, sockfd=" << outMsg->sockfd <<
                 " domain=" << inMsg->domain << " type=" << inMsg->type << " protocol=" << inMsg->protocol << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::stat64(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   Stat64Message *inMsg = (Stat64Message *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   Stat64Ack, sizeof(Stat64AckMessage));
   Stat64AckMessage *outMsg = (Stat64AckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::fstatat64(inMsg->dirfd, pathname, &(outMsg->buf), inMsg->flags);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Stat64Ack message is ready, pathname='" << pathname <<
                 "' rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::statfs64(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   Statfs64Message *inMsg = (Statfs64Message *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   Statfs64Ack, sizeof(Statfs64AckMessage));
   Statfs64AckMessage *outMsg = (Statfs64AckMessage *)_ackMessage;

   //! \note Swapping the current directory is not protected by a lock since sysiod is single threaded.

   // Swap the current working directory for a relative path name (would be nice if there was a fstatfsat() function).
   if (pathname[0] != '/') {
      if (fchdir(inMsg->dirfd) != 0) {
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Truncate64Ack message is ready, " <<
                       "error swapping current working directory using fd " << inMsg->dirfd);
         outMsg->header.returnCode = bgcios::RequestFailed;
         outMsg->header.errorCode = (uint32_t)errno;
         return;
      }
   }

   // Run the operation.
   int rc = ::statfs64(pathname, &(outMsg->buf));
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   // Swap the current working directory back to the original for a relative path name.
   if (pathname[0] != '/') {
      if (fchdir(_currentDirFd) != 0) {
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": error swapping current working directory back: " <<
                       bgcios::errorString(errno));
      }
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Statfs64Ack message is ready, pathname='" << pathname <<
                 "' rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::symlink(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   SymlinkMessage *inMsg = (SymlinkMessage *)message->getAddress();
   char *oldpathname = (char *)inMsg + inMsg->oldoffset;
   char *newpathname = (char *)inMsg + inMsg->newoffset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   SymlinkAck, sizeof(SymlinkAckMessage));
   SymlinkAckMessage *outMsg = (SymlinkAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::symlinkat(oldpathname, inMsg->newdirfd, newpathname);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": SymlinkAck message is ready, oldpathname='" << oldpathname <<
                 "' newpathname='" << newpathname << "' rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::truncate64(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   Truncate64Message *inMsg = (Truncate64Message *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message, Truncate64Ack, sizeof(Truncate64AckMessage));
   Truncate64AckMessage *outMsg = (Truncate64AckMessage *)_ackMessage;

   //! \note Swapping the current directory is not protected by a lock since sysiod is single threaded.

   // Swap the current working directory for a relative path name (would be nice if there was a ftruncateat() function).
   if (pathname[0] != '/') {
      if (fchdir(inMsg->dirfd) != 0) {
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Truncate64Ack message is ready, " <<
                       "error swapping current working directory using fd " << inMsg->dirfd);
         outMsg->header.returnCode = bgcios::RequestFailed;
         outMsg->header.errorCode = (uint32_t)errno;
         return;
      }
   }

   // Run the operation.
   int rc = ::truncate64(pathname, inMsg->length);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   // Swap the current working directory back to the original for a relative path name.
   if (pathname[0] != '/') {
      if (fchdir(_currentDirFd) != 0) {
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": error swapping current working directory back: " <<
                       bgcios::errorString(errno));
      }
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Truncate64Ack message is ready, pathname='" << pathname <<
                 "' length=" << inMsg->length << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::unlink(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   UnlinkMessage *inMsg = (UnlinkMessage *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message, UnlinkAck, sizeof(UnlinkAckMessage));
   UnlinkAckMessage *outMsg = (UnlinkAckMessage *)_ackMessage;

   // Run the operation.
   int rc = ::unlinkat(inMsg->dirfd, pathname, inMsg->flags);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": UnlinkAck message is ready, pathname='" << pathname <<
                 "' flags=" << inMsg->flags << " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::utimes(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   UtimesMessage *inMsg = (UtimesMessage *)message->getAddress();
   char *pathname = (char *)inMsg + inMsg->offset;

   // Build ack message in outbound message region.
   _ackMessage =  allocateClientAckMessage(message,   UtimesAck, sizeof(UtimesAckMessage));
   UtimesAckMessage *outMsg = (UtimesAckMessage *)_ackMessage;

   // Run the operation.
   const struct timeval *newtimes = NULL;
   if (!inMsg->now) {
      newtimes = inMsg->newtimes;
   }
   int rc = ::futimesat(inMsg->dirfd, pathname, newtimes);
   if (rc == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)errno; 
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": UtimesAck message is ready, pathname='" << pathname <<
                 "now=" << inMsg->now << "' accesstime=" << inMsg->newtimes[0].tv_sec << " modtime=" << inMsg->newtimes[1].tv_sec <<
                 " rc=" << rc << " errno=" << outMsg->header.errorCode);
   return;
}
void
ClientMonitor::write(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   WriteMessage *inMsg = (WriteMessage *)message->getAddress();

   // Save the message so inbound message region is available for next message.
   if (!message->isSaved()) {
      inMsg = (WriteMessage *)message->save();
   }

   _ackMessage =   allocateClientAckMessage(message,   WriteAck, sizeof(WriteAckMessage));
   WriteAckMessage *outMsg = (WriteAckMessage *)_ackMessage;
   outMsg->bytes = 0;

   // When message is first received, let client know operation is in progress.
   if (!message->isInProgress()) {
      message->setInProgress(true);
   }

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   // For performance, I'm not logging an error if the job is not found.

   // Run the operation until all of the data is written or an error occurs.
   uint64_t address = inMsg->address;
   size_t length = 0;
   size_t bytesLeft = (size_t)inMsg->data_length;

   while (bytesLeft > 0) {
      // Calculate the length of data for this operation.
      if (bytesLeft > _largeRegion->getLength()) {
         length = _largeRegion->getLength();
      }
      else {
         length = bytesLeft;
      }

      // Receive the data from the compute node.
      uint32_t err = getData(message, address, inMsg->rkey, (uint32_t)length);
      if (err != 0) {
         outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
         outMsg->header.errorCode = (uint32_t)err;
         bytesLeft = 0; // Force exit from loop
         continue;
      }

      // Run the operation.
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Write message started syscall, fd=" << inMsg->fd <<
                    " length=" << length);
      ssize_t rc;
      if (job-> logJobStatistics() ) job->writeTimer.start();
      if (inMsg->fd != job->getShortCircuitFd()) {
         rc = ::write(inMsg->fd, _largeRegion->getAddress(), length);
         err = (uint32_t)errno;
      }
      else {
         rc = (ssize_t)length;
      }
      if (job-> logJobStatistics() ) job->writeTimer.stop();
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Write message completed syscall, fd=" << inMsg->fd <<
                    " rc=" << rc << " errno=" << err);

      if (rc >= 0) {
         outMsg->header.returnCode = bgcios::Success;
         outMsg->bytes += rc;
         if (job->posixMode()) {
            bytesLeft = 0; // Force exit from loop because only one operation per message
         }
         else {
            address += (uint64_t)rc;
            bytesLeft -= (size_t)rc;
            inMsg->data_length -= (ssize_t)rc;
         }
      }
      else {
         if (err == EWOULDBLOCK) {
            addBlockedMessage(inMsg->fd, POLLOUT, message);
            _ackMessage=NULL; // Reset so second WriteAck message is not sent yet  
            LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": Write message is blocked for fd=" << inMsg->fd);
         }
         else {
            outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
            outMsg->header.errorCode = err;
         }
         bytesLeft = 0; // Force exit from loop
      }
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": WriteAck message is ready, fd=" << inMsg->fd <<
                 " length=" << inMsg->length << " bytes=" << outMsg->bytes << " rc=" << outMsg->header.returnCode << " errno=" << outMsg->header.errorCode);
   return;
}

void
ClientMonitor::writeRdmaVirt(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
    message->setInProgress(true); 
    bgcios::MessageHeader * ackMessage =  allocateClientAckMessage(message,   WriteRdmaVirtAck, sizeof(WriteRdmaVirtAckMessage));
    WriteRdmaVirtAckMessage *outMsg = (WriteRdmaVirtAckMessage *)ackMessage;
    {
      WriteRdmaVirtMessage *inMsg = (WriteRdmaVirtMessage *)message->getAddress(); 
      outMsg->fd = inMsg->fd;
      outMsg->data_length = inMsg->data_length;
      outMsg->bufferRdmaVirtaddress = inMsg->bufferRdmaVirtaddress;
      outMsg->offset = inMsg->offset;
      outMsg->bytes = 0;
    }
    message->setMessage(ackMessage);  //lose context of inMsg, using ackMessage for context hereon

   // Validate the job id.
   JobPtr job = _jobs.get(outMsg->header.jobId);
   // For performance, not logging an error if the job is not found.

   size_t bytesLeft = (size_t)outMsg->data_length;
   uint64_t address   = outMsg->bufferRdmaVirtaddress + outMsg->offset;
   uint32_t err = 0;
   while (bytesLeft > 0) {

      ssize_t rc;
      if (job-> logJobStatistics() ) job->writeTimer.start();
      rc = ::write(outMsg->fd, (void *)address, bytesLeft);
      err = (uint32_t)errno;
      if (job-> logJobStatistics() ) job->writeTimer.stop();

      if (rc >= 0) {
         outMsg->bytes += rc;
         address += (uint64_t)rc;
         bytesLeft -= (size_t)rc;
      }
      else if ( (err == EWOULDBLOCK) && (!(job->posixMode() ) ) ) {
           addBlockedMessage(outMsg->fd, POLLOUT, message);
           return;
      }
      else {
            outMsg->header.returnCode = outMsg->bytes == 0 ? bgcios::RequestFailed : bgcios::RequestIncomplete;
            outMsg->header.errorCode = err;
     }
     
   }
   _ackMessage = ackMessage; //Sending the ack
   return;
}
 
void
ClientMonitor::setupJob(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   SetupJobMessage *inMsg = (SetupJobMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =   allocateClientAckMessage(message,   SetupJobAck, sizeof(SetupJobAckMessage));
   SetupJobAckMessage *outMsg = (SetupJobAckMessage *)_ackMessage;

   // Make sure the message header has valid values for the service and version fields.  Note we only do these checks with a SetupJob message
   // so as to not have a performance hit on every message.
   if (inMsg->header.service != SysioService) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": message service " << inMsg->header.service << " is wrong");
      outMsg->header.returnCode = bgcios::WrongService;
      return;
   }

   if (inMsg->header.version != ProtocolVersion) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": SetupJob message protocol version " << (int)inMsg->header.version << " does not match sysiod version " <<
                    (int)ProtocolVersion << " (BG/Q sysiod built at " <<  __DATE__ << " " << __TIME__ << " with protocol version " << (int)ProtocolVersion << ")");
      outMsg->header.returnCode = bgcios::VersionMismatch;
      outMsg->header.errorCode = ProtocolVersion;
      return;
   }

   // Swap to the new user's identity for file system operations.  All jobs running on the compute node must have the same user identity
   // so we only need to swap when there is not a job already running on the compute node.
   if (_jobs.empty()) {
#if 0
      LOG_INFO_MSG_FORCED("Swapto info in SetupJob: inMsg->userId="<<inMsg->userId<<" inMsg->groupId="<<inMsg->groupId<<" inMsg->numGroups="<<inMsg->numGroups);
      for (int i=0;i<inMsg->numGroups;i++){
        int gid = (int)inMsg->secondaryGroups[i];
        LOG_INFO_MSG_FORCED("i="<<i<<" groupId="<< gid );
      }
#endif 
      bgcios::MessageResult result = _identity.swapTo(inMsg->userId, inMsg->groupId, inMsg->numGroups, inMsg->secondaryGroups);
      result.setHeader(outMsg->header);
      _operationTimer.resetTotals();
      _waitEventTimer.resetTotals();

      // Set attributes for daemon.
      bool value = false;
      switch (inMsg->logFunctionShipErrors) {
         case -1: value = _logFunctionShipErrorsDefault; break;
         case 0:  value = false; break;
         default: value = true;  break;
      }
      _logFunctionShipErrors = value;
      outMsg->logFunctionShipErrors = value; 
   }

   // Create a Job object for managing this job and add it to the map of active jobs.
   JobPtr job = JobPtr(new Job(inMsg->header.jobId));
   _jobs.add(inMsg->header.jobId, job);

   // Set attributes for job.
   bool value = false;
   switch (inMsg->posixMode) {
      case -1: value = _posixModeDefault; break;
      case 0:  value = false; break;
      default: value = true;  break;
   }
   job->setPosixMode(value);
   outMsg->posixMode = value; 

   switch (inMsg->logJobStatistics) {
      case -1: value = _logJobStatisticsDefault; break;
      case 0:  value = false; break;
      default: value = true;  break;
   }
   job->setLogJobStatistics(value);
   outMsg->logJobStatistics = value; 

   if (inMsg->shortCircuitPath[0] != 0) {
      job->setShortCircuitPath(inMsg->shortCircuitPath); 
   }
   else {
      job->setShortCircuitPath(_shortCircuitPathDefault);
   }
   outMsg->isUsingShortCircuitPath = (int32_t)job->sizeShortCircuitPath(); 
   outMsg->sysiod_pid = getpid();
   outMsg->sysiod_serviceId = _serviceId;
   outMsg->rdmaBufferVirtAddr = (uint64_t) _largeRegion->getAddress();
   outMsg->rdmaBufferLength = (uint64_t) _largeRegion->getLength();
   outMsg->memoryKeyOfBuffer = (uint32_t)_largeRegion->getLocalKey();

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": SetupJobAck message is ready, rc=" << outMsg->header.returnCode <<
                 " error=" << outMsg->header.errorCode);

   return;
}

void
ClientMonitor::cleanupJob(const ClientMessagePtr& message)
{
   // Get pointer to message from inbound message region.
   CleanupJobMessage *inMsg = (CleanupJobMessage *)message->getAddress();

   // Build ack message in outbound message region.
   _ackMessage =  allocateClientAckMessage(message,   CleanupJobAck, sizeof(CleanupJobAckMessage) );
   CleanupJobAckMessage *outMsg = (CleanupJobAckMessage *)_ackMessage;


   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": job was not found when handling CleanupJob message");
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      return;
   }

   // Get resource usage information (used a couple of different ways below).
   struct rusage usage;
   getrusage(RUSAGE_SELF, &usage);

   // Log system I/O statistics if requested.
   if (job->logJobStatistics()) {
      LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for completion queue: " << *_completionQ);
      LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for function ship operations: " << _operationTimer);
      LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for wait between operations: " << _waitEventTimer);

      if (job->openTimer.getNumOperations() > 0) {
         LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for open: " << job->openTimer);
      }
      if (job->preadTimer.getNumOperations() > 0) {
         LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for pread: " << job->preadTimer);
      }
      if (job->pwriteTimer.getNumOperations() > 0) {
         LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for pwrite: " << job->pwriteTimer);
      }
      if (job->readTimer.getNumOperations() > 0) {
         LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for read: " << job->readTimer);
      }
      if (job->writeTimer.getNumOperations() > 0) {
         LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for write: " << job->writeTimer);
      }
      if (job->recvTimer.getNumOperations() > 0) {
         LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for recv: " << job->recvTimer);
      }
      if (job->sendTimer.getNumOperations() > 0) {
         LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for send: " << job->sendTimer);
      }
      if (job->recvfromTimer.getNumOperations() > 0) {
         LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for recvfrom: " << job->recvfromTimer);
      }
      if (job->sendtoTimer.getNumOperations() > 0) {
         LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": stats for sendto: " << job->sendtoTimer);
      }

      LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": maxrss=" << usage.ru_maxrss << " nvcsw=" << usage.ru_nvcsw << " nivcsw=" << usage.ru_nivcsw);
   }

   // Remove the job from the map of active jobs and destroy the Job object.
   if (job->isKilled()){
     LOG_CIOS_DEBUG_MSG("JobId=" << inMsg->header.jobId << " Received JobCleanup for pid="<<getpid() );
   }
   job->setWaitingForJobCleanup(false);
   _jobs.remove(job->getJobId());
   job.reset();

   // Swap back to the default user identity when there are no jobs running on the compute node.
   if (_jobs.empty()) {
      bgcios::MessageResult result = _identity.swapBack();
      result.setHeader(outMsg->header);

      // Make sure we are not leaking memory.
      if (_lastResidentSetSize != 0) {
         if (usage.ru_maxrss > (_lastResidentSetSize + (_lastResidentSetSize / 100 * 2))) {
            LOG_CIOS_WARN_MSG("Job " << inMsg->header.jobId << ": resident set size of daemon " << _serviceId << " has grown by more than 2% since last job ended, current maxrss is " <<
                         usage.ru_maxrss << "kB and last maxrss is " << _lastResidentSetSize << " kB");
         }
      }
      else {
         _lastResidentSetSize = usage.ru_maxrss;
         LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": initial resident set size of daemon " << _serviceId << " is " << _lastResidentSetSize << " kB");
      }
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank << ": CleanupJobAck message is ready, rc=" << outMsg->header.returnCode <<
                 " error=" << outMsg->header.errorCode);
   return;
}

size_t
ClientMonitor::addBlockedMessage(int fd, short events, ClientMessagePtr message)
{
   // Find the first free slot in the set.
   size_t slot = _freePollSetSlots.find_first();

   // Setup the slot in the poll set.
   _freePollSetSlots.flip(slot);
   _pollSet[slot].fd = fd;
   _pollSet[slot].events = events;
   _pollSet[slot].revents = 0;
   if (slot > (_pollSetSize -1)) {
       _pollSetSize = slot + 1;
       LOG_CIOS_TRACE_MSG("poll set size increased to " << _pollSetSize);
   }

   // Add the message to the list of blocked messages.
   _blockedMessages.add(fd, message);

   return slot;
}
