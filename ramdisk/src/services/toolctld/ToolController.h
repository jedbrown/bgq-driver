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

//! \file  ToolController.h
//! \brief Declaration and inline methods for bgcios::toolctl::ToolController class.

#ifndef TOOLCTL_TOOLCONTROLLER_H
#define TOOLCTL_TOOLCONTROLLER_H

// Includes
#include "Tool.h"
#include "ToolctlMessage.h"
#include <ramdisk/include/services/common/ServiceController.h>
#include <ramdisk/include/services/common/RdmaCompletionChannel.h>
#include <ramdisk/include/services/common/RdmaServer.h>
#include <ramdisk/include/services/common/RdmaConnection.h>
#include <ramdisk/include/services/common/UserIdentity.h>
#include <ramdisk/include/services/common/PointerMap.h>
#include <ramdisk/include/services/common/StopWatch.h>
#include <ramdisk/include/services/MessageHeader.h>
#include <ramdisk/include/services/ToolctlMessages.h>
#include <tr1/memory>
#include <list>

namespace bgcios
{

namespace toolctl
{

//! \brief Handle tool control service messages.

class ToolController : public bgcios::ServiceController
{

public:

   //! \brief  Default constructor.

   ToolController();

   //! \brief  Default destructor.

   ~ToolController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  serviceId Unique identifier for this instance of the daemon.
   //! \return 0 when successful, errno when unsuccessful.

   int startup(uint32_t serviceId);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanup(void);

   //! \brief  Monitor for events from all connections.
   //! \return Nothing.

   void eventMonitor(void);

  bgcios::RdmaServerPtr getRdmaListener(){return _rdmaListener;}

private:

   //! \brief  Handle events from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int commandChannelHandler(void);

   //! \brief  Handle events from rdma event channel.
   //! \return Nothing.

   void eventChannelHandler(void);

   //! \brief  Handle events from completion channel.
   //! \return Nothing.

   void completionChannelHandler(void);

   //! \brief  Handle events from a tool data channel.
   //! \param  toolChannel Tool channel with event.
   //! \return True if tool data channel is closed, otherwise false.

   bool toolChannelHandler(const ToolPtr& tool);

   //! \brief  Send a message to a tool data channel.
   //! \param  tool Tool to send message to.
   //! \param  buffer Pointer to buffer containing message.
   //! \return 0 when successful, errno when unsuccessful.

   inline int sendToTool(const ToolPtr& tool, void *buffer)
   {
      SocketPtr socket = std::tr1::static_pointer_cast<Socket>(tool->getChannel());
      int err = sendMessageToStream(socket, buffer);
      if (err == EPIPE) { // When tool data channel closes, cleanup after the tool.
         removeTool(tool);
      }
      return err;
   }

   //! \brief  Receive a message from a tool data channel.
   //! \param  tool Tool to receive message from.
   //! \param  buffer Pointer to buffer for storing message.
   //! \return 0 when successful, errno when unsuccessful.

   inline int recvFromTool(const ToolPtr& tool, ToolMessage *message)
   {
      SocketPtr socket = std::tr1::static_pointer_cast<Socket>(tool->getChannel());
      return recvMessageFromStream(socket, message);
   }

   //! \brief  Send an ErrorAck message to the specified tool data channel.
   //! \param  tool Tool to send message to.
   //! \param  returnCode Result of previous request.
   //! \param  errorCode Error detail of previous request.
   //! \return 0 when successful, errno when unsuccessful.

   int sendErrorAckToTool(const ToolPtr& tool, uint32_t returnCode, uint32_t errorCode);

   //! \brief  Remove and cleanup a tool in the list of active tools.
   //! \param  pointer to tool object.
   //! \return Nothing.

   void removeTool(const ToolPtr& tool);

   //! \brief  Route a message received from a tool or queued from within tool control daemon.
   //! \param  message to be sent to the compute node.
   //! \return Nothing.

   void routeMessageFromTool(ToolctlMessagePtr& message);

   //! \brief  Handle an ErrorAck message received from completion channel.
   //! \param  tool Tool to forward message to.
   //! \param  inMessageRegion Memory region where inbound message was received to.
   //! \return Nothing.

   void errorAck(ToolPtr& tool, bgcios::RdmaMemoryRegionPtr& inMessageRegion);

   //! \brief  Handle SetupJob message received from completion channel.
   //! \param  inMessageRegion Memory region where inbound message was received to.
   //! \return Nothing.

   void setupJob(bgcios::RdmaMemoryRegionPtr& inMessageRegion);

   //! \brief  Handle an Attach messsage received from a tool data channel.
   //! \param  message Message received from tool.
   //! \return Nothing.

   void attach(ToolctlMessagePtr& message);

   //! \brief  Handle an AttachAck message received from completion channel.
   //! \param  tool Tool to forward message to.
   //! \param  inMessageRegion Memory region where inbound message was received to.
   //! \return Nothing.

   void attachAck(ToolPtr& tool, bgcios::RdmaMemoryRegionPtr& inMessageRegion);

   //! \brief  Handle a Detach message received from a tool data channel.
   //! \param  message Message received from tool.
   //! \return Nothing.

   void detach(ToolctlMessagePtr& message);

   //! \brief  Handle a DetachAck message received from completion channel.
   //! \param  tool Tool to forward message to.
   //! \param  inMessageRegion Memory region where inbound message was received to.
   //! \return Nothing.

   void detachAck(ToolPtr& tool, bgcios::RdmaMemoryRegionPtr& inMessageRegion);

   //! \brief  Handle a Query message received from a tool data channel.
   //! \param  message Message received from tool.
   //! \return Nothing.

   void query(ToolctlMessagePtr& message);

   //! \brief  Handle a QueryAck message received from completion channel.
   //! \param  tool Tool to forward message to.
   //! \param  inMessageRegion Memory region where inbound message was received to.
   //! \return Nothing.

   void queryAck(ToolPtr& tool, bgcios::RdmaMemoryRegionPtr& inMessageRegion);

   //! \brief  Handle an Update message received from a tool data channel.
   //! \param  message Message received from tool.
   //! \return Nothing.

   void update(ToolctlMessagePtr& message);

   //! \brief  Handle a queued ack message the tool control daemon attempted to previously send.
   //! \param  message Message queued by the tool control daemon.
   //! \return Nothing.

   void updateAck(ToolPtr& tool, bgcios::RdmaMemoryRegionPtr& inMessageRegion);

   //! \brief  Handle a Notify message received from completion channel.
   //! \param  tool Tool to forward message to.
   //! \param  inMessageRegion Memory region where inbound message was received to.
   //! \return Nothing.

   void notify(ToolPtr& tool, bgcios::RdmaMemoryRegionPtr& inMessageRegion);

   //! \brief  Handle the Control message received from a tool data channel.
   //! \param  message Message received from tool.
   //! \return Nothing.

   void control(ToolctlMessagePtr& message);

   //! \brief  Handle an ControlAck message received from completion channel.
   //! \param  tool Tool to forward message to.
   //! \param  inMessageRegion Memory region where inbound message was received to.
   //! \return Nothing.

   void controlAck(ToolPtr& tool, bgcios::RdmaMemoryRegionPtr& inMessageRegion);

   //! \brief  Handle a Terminate message received from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int terminate(void);

   //! \brief  Initialize an ack message in the outbound message buffer.
   //! \note   Use this method for variable length ack messages on tool data channel.
   //! \param  type Type of outbound message.
   //! \return Pointer to header of outbound message.

   inline void *initAckMessage(bgcios::MessageHeader *inHdr, uint16_t type)
   {
      bgcios::MessageHeader *outHdr = (bgcios::MessageHeader *)_outboundMessage;
      memcpy(outHdr, inHdr, sizeof(MessageHeader));
      outHdr->type = type;
      return (void *)outHdr;
   }

   //! \brief  Initialize an ack message in the outbound message buffer.
   //! \note   Use this method for fixed length ack messages on tool data channel.
   //! \param  type Type of outbound message.
   //! \param  length Length of outbound message.
   //! \return Pointer to header of outbound message.

   inline void *initAckMessage(bgcios::MessageHeader *inHdr, uint16_t type, uint32_t length)
   {
      bgcios::MessageHeader *outHdr = (bgcios::MessageHeader *)_outboundMessage;
      memcpy(outHdr, inHdr, sizeof(MessageHeader));
      outHdr->type = type;
      outHdr->length = length;
      return (void *)outHdr;
   }

   //! Unique identifier for this instance of the daemon.
   uint32_t _serviceId;

   //! Listening socket for accepting connections from tools.
   bgcios::LocalStreamSocketPtr _toolListener;

   //! Path to listening socket for accepting connections from tools.
   std::string _toolListenerPath;

   //! List of connected tools indexed by socket descriptor.
   bgcios::PointerMap<int, ToolPtr> _toolChannels;

   //! Typedef for connected tools iterator.
   typedef bgcios::PointerMap<int, ToolPtr>::const_iterator tool_channel_iterator;

   //! List of attached tools indexed by tool id.
   bgcios::PointerMap<uint32_t, ToolPtr> _tools;

   //! Typedef for tool list iterator.
   typedef bgcios::PointerMap <uint32_t, ToolPtr>::const_iterator tool_list_iterator;

   //! List of messages queued while waiting for compute node to complete a message exchange.
   std::list <ToolctlMessagePtr> _queuedMessages;

   //! Indicator if a message exchange is in progress with compute node.
   bool _waitingForAck;

   //! Indicator if an Interrupt message was sent to sysiod during a poll timeout while waiting for Ack
   bool _interruptMsgSent;

   //! Rank associated with the waiting on Ack indicator.
   uint32_t _waitingForAckRank;

   //! JobID associated with the waiting on Ack indicator.
   uint64_t _waitingForAckJobID;

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

   //! List of inbound message regions indexed by local key.
   bgcios::PointerMap<uint32_t, bgcios::RdmaMemoryRegionPtr> _inMessageRegions;

   // Typedef for message region list iterator.
   typedef bgcios::PointerMap <uint32_t, bgcios::RdmaMemoryRegionPtr>::const_iterator mr_list_iterator;

   //! Memory region for outbound messages.
   bgcios::RdmaMemoryRegionPtr _outMessageRegion;

   //! Memory region for outbound messages.
   bgcios::RdmaMemoryRegionPtr _outMessageRegion2;

   //! User identity for file system operations.
   bgcios::UserIdentity _identity;

   //! Resident set size when last job started.
   long _lastResidentSetSize;

   //! Stopwatch for timing exchange of Query and QueryAck messages with compute node.
   bgcios::StopWatch _queryTimer;

   //! Stopwatch for timing exchange of Update and UpdateAck messages with compute node.
   bgcios::StopWatch _updateTimer;

   //! Path to sysiod command channel socket.
   std::string _sysiodCmdChannelPath;

   //! Path to stdiod command channel socket.
   std::string _stdiodCmdChannelPath;

};

//! Smart pointer for ToolController object.
typedef std::tr1::shared_ptr<ToolController> ToolControllerPtr;

} // namespace toolctl

} // namespace bgcios

#endif // TOOLCTL_TOOLCONTROLLER_H

