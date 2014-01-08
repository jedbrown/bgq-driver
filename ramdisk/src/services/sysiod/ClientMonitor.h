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
#include <poll.h>
#include <boost/dynamic_bitset.hpp>
#include <list>

namespace bgcios
{

namespace sysio
{

//! \brief Monitor the client connection to a compute node and handle system I/O service messages.

class ClientMonitor : public Thread
{
public:

   //! \brief  Default constructor.
   //! \param  ready Pointer to Counter object to tell main thread initialization is complete.
   //! \param  config Configuration from command line and properties.

   ClientMonitor(bgcios::CounterPtr ready, SysioConfigPtr config) : Thread()
   {
      // Initialize private data.
      _ready = ready;
      _serviceId = config->getServiceId();
      _regionSize = config->getLargeRegionSize();
      _logJobStatisticsDefault = config->getLogJobStatistics();
      _shortCircuitPathDefault = config->getShortCircuitPath();
      _posixModeDefault = config->getPosixMode();
      _logFunctionShipErrors = _logFunctionShipErrorsDefault = config->getLogFunctionShipErrors();
      _inProgressMessage = 0;
      _currentDirFd = -1;
      _freePollSetSlots.resize(MaxPollSetSize, true);
      _lastResidentSetSize = 0;
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
      JobPtr job = _jobs.get(jobId);
      if (job != NULL) {
         return job->getWaitingForJobCleanup();
      }
      return false;
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

   //! \brief  Transfer data to the client from the large memory region.
   //! \param  message Request message from client.
   //! \param  address Address of remote memory region.
   //! \param  rkey Key of remote memory region.
   //! \param  length Length of data to transfer.
   //! \return 0 when successful, error when unsuccessful.

   uint32_t putData(const ClientMessagePtr& message, uint64_t address, uint32_t rkey, uint32_t length);

   //! \brief  Transfer data from the client into the large memory region.
   //! \param  message Request message from client.
   //! \param  address Address of remote memory region.
   //! \param  rkey Key of remote memory region.
   //! \param  length Length of data to transfer.
   //! \return 0 when successful, error when unsuccessful.

   uint32_t getData(const ClientMessagePtr& message, uint64_t address, uint32_t rkey, uint32_t length);

   //! \brief  Wait for a work completion indicating that a message of the specified type is available using the specified request id.
   //! \param  type Type of message to wait for.
   //! \param  requestId Request id to check for in the completion events.
   //! \return Nothing.

   void waitForCompletion(uint16_t type, uint64_t requestId);

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

   //! \brief  Handle a Write message recevied from completion channel.
   //! \param  message Message from client.
   //! \return Nothing.

   void writeRdmaVirt(const ClientMessagePtr& message);

   //! \brief  Handle a WriteAck message recevied from rdma or blocked message (Write in progress)
   //! \param  message Message from client.
   //! \return Nothing.

   void writeRdmaVirtAck(const ClientMessagePtr& message);

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
   static const nfds_t MaxPollSetSize = 66;

   //! Number of fixed entries in poll set.
   static const nfds_t FixedPollSetSize = 2;

   //! Index of completion channel descriptor in poll set.
   static const int CompChannel = 0;

   //! Index of event channel descriptor in poll set.
   static const int EventChannel = 1;

   //! Set of descriptors to poll.
   struct pollfd _pollSet[MaxPollSetSize];

   //! Number of descriptors in poll set.
   nfds_t _pollSetSize;

   //! Free elements in pollSet array.
   boost::dynamic_bitset<> _freePollSetSlots;

   //! User identity for file system operations.
   bgcios::FileSystemUserIdentity _identity;

   //! Type of message that is in progress (i.e. takes two steps to complete).
   uint16_t _inProgressMessage;

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
       int err = _outMessageRegion->allocate(_protectionDomain, bgcios::SmallMessageRegionSize);
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

   //! Stopwatch for timing a function ship operation.
   bgcios::StopWatch _operationTimer;

   //! Stopwatch for timing wait time between events.
   bgcios::StopWatch _waitEventTimer;

   //! Resident set size after last job ended.
   long _lastResidentSetSize;

};

//! Smart pointer for ClientMonitor object.
typedef std::tr1::shared_ptr<ClientMonitor> ClientMonitorPtr;

} // namespace sysio

} // namespace bgcios

#endif // SYSIO_CLIENTMONITOR_H

