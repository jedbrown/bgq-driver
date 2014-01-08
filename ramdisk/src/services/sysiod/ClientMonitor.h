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

//! \file  ClientMonitor.h
//! \brief Declaration and inline methods for bgcios::sysio::ClientMonitor class.

#ifndef SYSIO_CLIENTMONITOR_H
#define SYSIO_CLIENTMONITOR_H

// Includes
#include "SysioConfig.h"
#include "ClientMessage.h"
#include "ClientAckMessage.h"
#include "Job.h"
#include <ramdisk/include/services/common/Thread.h>
#include <ramdisk/include/services/common/Counter.h>
#include <ramdisk/include/services/common/RdmaCompletionChannel.h>
#include <ramdisk/include/services/common/RdmaServer.h>
#include <ramdisk/include/services/common/RdmaConnection.h>
#include <ramdisk/include/services/common/FileSystemUserIdentity.h>
#include <ramdisk/include/services/common/StopWatch.h>
#include <ramdisk/include/services/common/PointerMap.h>
#include <ramdisk/include/services/MessageHeader.h>
#include <ramdisk/include/services/SysioMessages.h>
#include <ramdisk/include/services/UserMessages.h>
#include <poll.h>
#include <boost/dynamic_bitset.hpp>
#include <boost/signals2/mutex.hpp>
#include <list>
#include <ramdisk/include/services/common/RdmaError.h>
#include <ramdisk/include/services/common/RdmaDevice.h>
#include <ramdisk/include/services/Plugin.h>
#include <sys/resource.h>
#include <utility/include/PluginHandle.h>

namespace bgcios
{


namespace sysio
{

  static char BLANK[]="\0";
  static MessageHeader NULL_MH={  0, 0, 0,0,0,0,.0,32,0};
//! \brief Monitor the client connection to a compute node and handle system I/O service messages.


typedef boost::shared_ptr<bgq::utility::PluginHandle<Plugin> > PluginHandleSharedPtr;

class ClientMonitor : public Thread
{
public:

   //! \brief  Default constructor.
   //! \param  ready Pointer to Counter object to tell main thread initialization is complete.
   //! \param  config Configuration from command line and properties.

   ClientMonitor(bgcios::CounterPtr ready, SysioConfigPtr config) 
: Thread()
, _dynamicLoadLibrary(config->getDynamicLoadLibrary() )
, _dynamicLoadLibrary_flags(config->getFlags() )
, _handle4PluginClass(new Plugin)
   {
      // Initialize private data.
      _ready = ready;
      _serviceId = config->getServiceId();
      _regionSize = config->getLargeRegionSize();
      _logJobStatisticsDefault = config->getLogJobStatistics();
      _shortCircuitPathDefault = config->getShortCircuitPath();
      _posixModeDefault = config->getPosixMode();
      _logFunctionShipErrors = _logFunctionShipErrorsDefault = config->getLogFunctionShipErrors();

      _currentDirFd = -1;
      _freePollSetSlots.resize(MaxPollSetSize, true);
      _lastResidentSetSize = 0;
       _syscall_mh=&NULL_MH;
       _syscallStartTimestamp=0;
       _syscallFileString1 = BLANK;
       _syscallFileString2 = BLANK;
       _syscallFd = -1;
   }

   //! \brief  Default destructor.

   ~ClientMonitor();

   //! \brief  Run the monitor function.
   //! \return Pointer to return code value (can be NULL).

   void *run(void);

   //! \brief  Check if the specified job is still running.
   //! \param  jobId Job identifier.
   //! \return True if job is running, otherwise false.

   bool isJobRunning(uint64_t jobId) 
 {
    _mutexActiveJobIdlist.lock();
    const std::list<uint64_t>::iterator r = std::find(_activeJobId.begin(),_activeJobId.end(),jobId);
    bool state = ( r != _activeJobId.end() );
    _mutexActiveJobIdlist.unlock();
    return state;
 }

   //! \brief  Mark job as NOT running.
   //! \param  jobId Job identifier.
   //! \return True if job is running, otherwise false.

   void stopJobInternalKernel(uint64_t jobId) 
   {
      JobPtr job = _jobs.get(jobId);
      if (job != NULL) {
        job->timedOutForKernelWrite();
      }
   }

   //! \brief  Mark the specified job as killed so subsequent messages do not start an I/O operation.
   //! \param  jobId Job identifier.
   //! \return Nothing.

   void markJobKilled(uint64_t jobId)
   {
      JobPtr job = _jobs.get(jobId);
      if (job != NULL) {
         job->markKilled();
      }
      return;
   }

   void addRankEINTRforJob(uint64_t jobId,uint32_t rank)
   {
      JobPtr job = _jobs.get(jobId);
      if (job != NULL) {
         job->addRankEINTR(rank);
      }
   }

   bgcios::RdmaServerPtr getRdmaListener() const {return  _rdmaListener;}

private:

   //! \brief  Allocate resources for monitor the client connection.
   //! \return 0 when succcessful, errno when unsuccessful.

   int startup(void);

   //! \brief  Handle events from rdma event channel.
   //! \return Nothing.

   void eventChannelHandler(void);

   //! \brief  Handle events from completion channel.
   //! \param  requestId Request id to check for in the completion events.
   //! \return True when completion event for specified request id was handled, otherwise false.
   //! \throws RdmaError.

   bool completionChannelHandler(uint64_t requestId);

   //! \brief  Route a message to its handler.
   //! \param  message Message to route.
   //! \return True if there is a handler for the message, otherwise false.

   bool routeMessage(const ClientMessagePtr& message);

   //! \brief  Route a message to user added handler.
   //! \param  message Message to route.
   //! \return True if there is a handler for the message, otherwise false.

   bool routeUserMessage(const ClientMessagePtr& message);

      //! \brief  Route a message to user added handler with file descriptors and RDMA info
   //! \param  message Message to route.
   //! \return True if there is a handler for the message, otherwise false.

   bool routeUserFdRDMAMessage(const ClientMessagePtr& message);

   //! \brief  Transfer data to the client from the large memory region.
   //! \param  address Address of remote memory region.
   //! \param  rkey Key of remote memory region.
   //! \param  length Length of data to transfer.
   //! \return 0 when successful, error when unsuccessful.

uint32_t putData(uint64_t address, uint32_t rkey, uint32_t length)
{
   uint32_t rc = 0;
   try {
      // Post a rdma write request to the send queue using the large message region.
      _largeRegion->setMessageLength(length);
      uint64_t reqID = (uint64_t)_largeRegion->getAddress();
      uint64_t& localAddress = reqID;
      uint32_t lkey = _largeRegion->getLocalKey();
      int err = _client->postRdmaWrite(reqID, rkey, address, //remote key and address
                     lkey,  localAddress, (ssize_t)length,IBV_SEND_SIGNALED);
      if (err) return (rc=(uint32_t)err);

      // Wait for notification that the rdma read completed.
      while (!completionChannelHandler(reqID));
   }

   catch (const RdmaError& e) {
      rc = (uint32_t)e.errcode();
   }

   return rc;
}

   //! \brief  Transfer data to the client from the large memory region without posting a completion
   //! \param  address Address of remote memory region.
   //! \param  rkey Key of remote memory region.
   //! \param  length Length of data to transfer.
   //! \return 0 when successful, error when unsuccessful.
   //! \note  Any postSend should really have the fence flag set

uint32_t putDataNoCompletion(uint64_t address, uint32_t rkey, uint32_t length)
{
   uint32_t rc = 0;
   try {
      // Post a rdma write request to the send queue using the large message region.
      _largeRegion->setMessageLength(length);
      uint64_t reqID = (uint64_t)_largeRegion->getAddress();
      uint64_t& localAddress = reqID;
      uint32_t lkey = _largeRegion->getLocalKey();
      int err = _client->postRdmaWriteWithAck(reqID, rkey, address, //remote key and address
                     lkey,  localAddress, (ssize_t)length,
                     _outMessageRegion , _ackMessage, _ackMessage->length); 
      _ackMessage=NULL;
      if (err) return (rc=(uint32_t)err);
   }

   catch (const RdmaError& e) {
      rc = (uint32_t)e.errcode();
   }

   return rc;
}


   //! \brief  Transfer data from the client into the large memory region.
   //! \param  address Address of remote memory region.
   //! \param  rkey Key of remote memory region.
   //! \param  length Length of data to transfer.
   //! \return 0 when successful, error when unsuccessful.

uint32_t getData( uint64_t address, uint32_t rkey, uint32_t length)
{
   uint32_t rc = 0;
   try {
      // Post a rdma read request to the send queue using the large message region.
      _largeRegion->setMessageLength(length);
      uint64_t reqID = (uint64_t)_largeRegion->getAddress();
      uint64_t& localAddress = reqID;
      uint32_t lkey = _largeRegion->getLocalKey();
      int err = _client->postRdmaRead(reqID, rkey, address, //remote key and address
                     lkey,  localAddress, (ssize_t)length);
      if (err) return (rc=(uint32_t)err);

      // Wait for notification that the rdma read completed.
      while (!completionChannelHandler(reqID));
   }

   catch (const RdmaError& e) {
      rc = (uint32_t)e.errcode();
   }

   return rc;
}

   //! \brief  Transfer data from the client into the large memory region.
   //! \param  address Address of remote memory region.
   //! \param  rkey Key of remote memory region.
   //! \param  length Length of data to transfer.
   //! \return 0 when successful, error when unsuccessful.

uint32_t getUserRdmaData( uint64_t address, uint32_t rkey, uint32_t length, uint64_t offset)
{
   uint32_t rc = 0;
   try {
      // Post a rdma read request to the send queue using the large message region.
      _largeRegion->setMessageLength(length);
      uint64_t reqID = (uint64_t)_largeRegion->getAddress() + offset;
      uint64_t& localAddress = reqID;
      uint32_t lkey = _largeRegion->getLocalKey();
      int err = _client->postRdmaRead(reqID, rkey, address, //remote key and address
                     lkey,  localAddress, (ssize_t)length);
      if (err) return (rc=(uint32_t)err);

      // Wait for notification that the rdma read completed (IBV_SEND_SIGNALED)
      while (!completionChannelHandler(reqID));
   }

   catch (const RdmaError& e) {
      rc = (uint32_t)e.errcode();
   }

   return rc;
}

   //! \brief  Transfer data from the large memory region into the client
   //! \param  address Address of remote memory region.
   //! \param  rkey Key of remote memory region.
   //! \param  length Length of data to transfer.
   //! \return 0 when successful, error when unsuccessful.

uint32_t putUserRdmaData( uint64_t address, uint32_t rkey, uint32_t length, uint64_t offset)
{
   uint32_t rc = 0;
   try {
      // Post a rdma read request to the send queue using the large message region.
      _largeRegion->setMessageLength(length);
      uint64_t reqID = (uint64_t)_largeRegion->getAddress() + offset;
      uint64_t& localAddress = reqID;
      uint32_t lkey = _largeRegion->getLocalKey();
      int err = _client->postRdmaWrite(reqID, rkey, address, //remote key and address
                     lkey,  localAddress, (ssize_t)length,IBV_SEND_SIGNALED);
      if (err) return (rc=(uint32_t)err);

      // Wait for notification that the rdma read completed (IBV_SEND_SIGNALED)
      while (!completionChannelHandler(reqID));
   }

   catch (const RdmaError& e) {
      rc = (uint32_t)e.errcode();
   }

   return rc;
}
   //! \brief  Send the current ack message to the client from the outbound message region.
   //! \return 0 when successful, error when unsuccessful.

   uint64_t sendAckMessage(void);

   //! \brief  Handle Accept message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void accept(const ClientMessagePtr& message);

   //! \brief  Handle an Access message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void access(const ClientMessagePtr& message);

   //! \brief  Handle a Bind message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void bind(const ClientMessagePtr& message);

   //! \brief  Handle a Chmod message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void chmod(const ClientMessagePtr& message);

   //! \brief  Handle a Chown message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void chown(const ClientMessagePtr& message);

   //! \brief  Handle a Close message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void close(const ClientMessagePtr& message);

   //! \brief  Handle a Connect message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void connect(const ClientMessagePtr& message);

   //! \brief  Handle a Fchmod message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void fchmod(const ClientMessagePtr& message);

   //! \brief  Handle a Fchown message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void fchown(const ClientMessagePtr& message);

   //! \brief  Handle a Fcntl message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void fcntl(const ClientMessagePtr& message);

   //! \brief  Handle a Fstat64 message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void fstat64(const ClientMessagePtr& message);

   //! \brief  Handle a Fstatfs64 message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void fstatfs64(const ClientMessagePtr& message);

   //! \brief  Handle a Ftruncate64 message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void ftruncate64(const ClientMessagePtr& message);

   //! \brief  Handle a Fsync message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void fsync(const ClientMessagePtr& message);

   //! \brief  Handle a Getdents64 message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void getdents64(const ClientMessagePtr& message);

   //! \brief  Handle a Getpeername message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void getpeername(const ClientMessagePtr& message);

   //! \brief  Handle a Getsockname message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void getsockname(const ClientMessagePtr& message);

   //! \brief  Handle a Getsockopt message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void getsockopt(const ClientMessagePtr& message);

   //! \brief  Handle a gpfs_fcntl message recevied from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void gpfsfcntl(const ClientMessagePtr& message);

   //! \brief  Handle an Ioctl message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void ioctl(const ClientMessagePtr& message);

   //! \brief  Handle a Link message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void link(const ClientMessagePtr& message);

   //! \brief  Handle a Listen message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void listen(const ClientMessagePtr& message);

   //! \brief  Handle a Lseek64 message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void lseek64(const ClientMessagePtr& message);

   //! \brief  Handle a Mkdir message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void mkdir(const ClientMessagePtr& message);

   //! \brief  Handle an Open message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void open(const ClientMessagePtr& message);

   //! \brief  Handle a Poll message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void pollForCN(const ClientMessagePtr& message);

   //! \brief  Handle a Pread64 message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void pread64(const ClientMessagePtr& message);

   //! \brief  Handle a Pwrite64 message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void pwrite64(const ClientMessagePtr& message);

   //! \brief  Handle a Read message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void read(const ClientMessagePtr& message);

   //! \brief  Handle a Readlink message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void readlink(const ClientMessagePtr& message);

   //! \brief  Handle a Recv message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void recv(const ClientMessagePtr& message);

   //! \brief  Handle a Recvfrom message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void recvfrom(const ClientMessagePtr& message);

   //! \brief  Handle a Rename message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void rename(const ClientMessagePtr& message);

   //! \brief  Handle a Send message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void send(const ClientMessagePtr& message);

   //! \brief  Handle a Sendto message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void sendto(const ClientMessagePtr& message);

   //! \brief  Handle a Setsockopt message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void setsockopt(const ClientMessagePtr& message);

   //! \brief  Handle a Shutdown message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void shutdown(const ClientMessagePtr& message);

   //! \brief  Handle a Socket message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void socket(const ClientMessagePtr& message);

   //! \brief  Handle a Stat64 message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void stat64(const ClientMessagePtr& message);

   //! \brief  Handle a Statfs64 message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void statfs64(const ClientMessagePtr& message);

   //! \brief  Handle a Symlink message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void symlink(const ClientMessagePtr& message);

   //! \brief  Handle a Truncate64 message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void truncate64(const ClientMessagePtr& message);

   //! \brief  Handle an Unlink message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void unlink(const ClientMessagePtr& message);

   //! \brief  Handle an Utimes message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void utimes(const ClientMessagePtr& message);

   //! \brief  Handle a Write message recevied from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void write(const ClientMessagePtr& message);

   //! \brief  Handle a WriteImmediate message recevied from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void writeImmediate(const ClientMessagePtr& message);

   //! \brief  Handle a SetupJob message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void setupJob(const ClientMessagePtr& message);

   //! \brief  Handle a CleanupJob message received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void cleanupJob(const ClientMessagePtr& message);

   //! \brief  Add a message to the list of blocked messages and add descriptor to poll set.
   //! \param  fd Descriptor to monitor.
   //! \param  events Set of events to monitor.
   //! \param  message Message that is blocked.
   //! \return Slot in poll set for specified descriptor.

   size_t addBlockedMessage(int fd, short events, ClientMessagePtr message);

   //! \brief  Handle a message FsetXattr or FremoveXattr received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.
   void fxattr_setOrRemove(const ClientMessagePtr& message);

   //! \brief  Handle a message FgetXattr or FlistXattr received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.
   void ffxattr_retrieve(const ClientMessagePtr& message);

   //! \brief  Handle a message PsetXattr or LsetXattr received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.
   void pathsetxattr(const ClientMessagePtr& message);

   //! \brief  Handle a message PremoveXattr or LremoveXattr received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.
   void pathremovexattr(const ClientMessagePtr& message);

   //! \brief  Handle a message PgetXattr or LgetXattr received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.
   void pathgetxattr(const ClientMessagePtr& message);

   //! \brief  Handle a message PlistXattr or LlistXattr received from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.
   void pathlistxattr(const ClientMessagePtr& message);


   //! Counter to increment when thread initialization is complete.
   bgcios::CounterPtr _ready;

   //! Unique identifier for this instance of the daemon.
   uint32_t _serviceId;

   //! Size of large memory region.
   size_t _regionSize;

   //! Default value for logging statistics from a job when it ends.
   bool _logJobStatisticsDefault;

   //! Default path to short circuit file.
   std::string _shortCircuitPathDefault;

   //! Default value for posix mode for read and write operations.
   bool _posixModeDefault;

   //! Default value for logging function ship operation errors.
   bool _logFunctionShipErrorsDefault;

   //! Indicator to control logging function ship operation errors.
   bool _logFunctionShipErrors;

   void logFunctionShipError(bgcios::MessageHeader *msghdr);

   //! Listener for RDMA connections.
   bgcios::RdmaServerPtr _rdmaListener;

   //! Protection domain for all resources.
   bgcios::RdmaProtectionDomainPtr _protectionDomain;

   //! Completion channel for all completion queues.
   bgcios::RdmaCompletionChannelPtr _completionChannel;

   //! Completion queue for both send and receive operations.
   bgcios::RdmaCompletionQueuePtr _completionQ;

   //! Connection for compute node client.
   bgcios::RdmaConnectionPtr _client;

   //! Memory region for inbound messages.
   RdmaMemoryRegionPtr _inMessageRegion;

   //! Memory region for outbound messages.
   RdmaMemoryRegionPtr _outMessageRegion;

   //! Memory region for in progress messages.
   //RdmaMemoryRegionPtr _progressMessageRegion;

   //! Large memory region for transferring data (used for both inbound and outbound data).
   bgcios::RdmaMemoryRegionPtr _largeRegion;

   //! Maximum number of descriptors to monitor with poll().
   static const nfds_t MaxPollSetSize = 128;

   //! Number of fixed entries in poll set.
   static const nfds_t FixedPollSetSize = 1;

   //! Index of completion channel descriptor in poll set.
   static const int CompChannel = 0;

   //! Set of descriptors to poll.
   struct pollfd _pollSet[MaxPollSetSize];

   //! Number of descriptors in poll set.
   nfds_t _pollSetSize;

   //! Free elements in pollSet array.
   boost::dynamic_bitset<> _freePollSetSlots;

   //! User identity for file system operations.
   bgcios::FileSystemUserIdentity _identity;

   //! List of messages queued while waiting for second step of an in progress message to complete.
   std::list <ClientMessagePtr> _queuedMessages;

   //! List of messages where the operation would block indexed by descriptor.
   bgcios::PointerMap <int, ClientMessagePtr> _blockedMessages;

   //! Current ack message to be sent to compute node client or NULL if no message is to be sent.
   bgcios::MessageHeader * _ackMessage;

   //! List of free memory pieces for building send requests for Ack Messages
   std::list <bgcios::MessageHeader *> _ackMemFreeList;

   uint32_t _numPiecesOutBound;

   //! \brief  Subdivide the RDMA Memoory Region allocated for outbound messages into pieces
   //! \return error from outMessageRegion allocate if it returns an error or return if not enough pieces
   //! \note  Need 128 pieces since that is the maximum number of ACK message send requests which
   //!        could be outstanding to the compute node--64 Final Ack
   //!        messages if the network is blocking the sends (which is unlikely but could happen if the 
   //!        torus network is misbehaving or a problem in the receiving packet software on the CN
   int allocateSendRequestPool(){
      _outMessageRegion = RdmaMemoryRegionPtr(new RdmaMemoryRegion());
       //pint err = _outMessageRegion->allocate(_protectionDomain, bgcios::SmallMessageRegionSize);
       int err = _outMessageRegion->allocate64kB(_protectionDomain);
       if (err != 0) {
        return err;
       }
       //carve up into pieces
       uint32_t size = _outMessageRegion->getLength();
      _numPiecesOutBound = 0;
       uint32_t total=0;
       char * address = (char *)_outMessageRegion->getAddress();
       while (total < size){ 
         _numPiecesOutBound ++;
         _ackMemFreeList.push_back((bgcios::MessageHeader *)address);
         total += ImmediateMessageSize;
         address += ImmediateMessageSize;
       }
       return 0; 
   }

    //! \param message is the inbound message requiring an Ack message
    //! \param inType is the message type value for the Ack message
    //! \param inLength is the length of the Ack Message
    //! \return bgciosMessageHeader pointer for the Ack Message

    bgcios::MessageHeader *allocateClientAckMessage(const ClientMessagePtr& message,uint16_t inType, uint32_t inLength=sizeof(bgcios::MessageHeader)){

      return    allocateClientAckMessage(message->getAddress(),inType, 0, inLength );            
    };  

     bgcios::MessageHeader * allocateClientAckMessage(bgcios::MessageHeader * header,uint16_t inType, uint32_t inReturnCode=0, uint32_t inLength=sizeof(bgcios::MessageHeader) ){
      // Should be plenty of outbound ack regions, so assert list is notempty
      assert(!_ackMemFreeList.empty());
      // Build ack message in outbound message region.
      bgcios::MessageHeader * mh = _ackMemFreeList.front();
      memcpy(mh,header,sizeof(bgcios::MessageHeader) );
      mh->type=inType;
      mh->returnCode=inReturnCode;
      mh->length = inLength;
      _ackMemFreeList.pop_front();
      _ackMemFreeList.push_back(mh); 
      return mh;  
    } ;  

   //! Descriptor to current working directory.
   int _currentDirFd;

   //! Map of currently active jobs indexed by job id.
   bgcios::PointerMap<uint64_t, JobPtr> _jobs;

   //! List for tracking job IDs shared by threads with an accompanying lock
    std::list <uint64_t> _activeJobId;  
    boost::mutex _mutexActiveJobIdlist;

   //! Stopwatch for timing a function ship operation.
   bgcios::StopWatch _operationTimer;

   //! Stopwatch for timing wait time between events.
   bgcios::StopWatch _waitEventTimer;

   //! Resident set size after last job ended.
   long _lastResidentSetSize;

   volatile struct MessageHeader * _syscall_mh;
   volatile uint64_t _syscallStartTimestamp;
   volatile char *   _syscallFileString1;
   volatile char *   _syscallFileString2;
   volatile int      _syscallFd;
   volatile size_t   _syscallAccessLength;

public:

uint64_t getSyscallStartTimeStamp(){
  return (_syscallStartTimestamp);
}

int getSyscallFd(){
  return (_syscallFd);
}

volatile struct MessageHeader * getSyscallMessageHdr(){
  return (_syscall_mh);
}

char * getSyscallFileString1(){
  return (char *)(_syscallFileString1);
}

char * getSyscallFileString2(){
  return (char *)(_syscallFileString2);
}

size_t getSyscallAccessLength() const {
    return _syscallAccessLength;
}

private:
uint64_t setSyscallStart(MessageHeader * mh, int fd=-1, char * pathname1=BLANK,char * pathname2=BLANK, size_t accessLength=0){
    _syscall_mh = mh;    
    _syscallFileString1 = pathname1;
    _syscallFileString2 = pathname2;
    _syscallFd=fd;
    _syscallAccessLength = accessLength;
    _syscallStartTimestamp = GetTimeBase();
  return _syscallStartTimestamp;
}


void clearSyscallStart(void){
  _syscall_mh=&NULL_MH;
  _syscallStartTimestamp=0;
  _syscallFileString1 = BLANK;
  _syscallFileString2 = BLANK;
  _syscallFd=-1;

}


const std::string _dynamicLoadLibrary;
const int _dynamicLoadLibrary_flags;
typedef boost::shared_ptr<bgcios::sysio::Plugin> PluginPtr;
PluginPtr _handle4PluginClass;
PluginHandleSharedPtr _pluginHandlePtr;


void updatePlugin();

//internal class to monitor for disconnect from compute node
class EventWaiter : public Thread
{ 
  friend class ClientMonitor;
  EventWaiter(ClientMonitor& cm):_clientMonitor(cm){}
  void * run(void);
  ClientMonitor& _clientMonitor;
};
friend class ClientMonitor::EventWaiter;


};
//! Smart pointer for ClientMonitor object.
typedef std::tr1::shared_ptr<ClientMonitor> ClientMonitorPtr;

} // namespace sysio

} // namespace bgcios

#endif // SYSIO_CLIENTMONITOR_H

