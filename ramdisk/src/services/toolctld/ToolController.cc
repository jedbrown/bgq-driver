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

//! \file  ToolController.cc
//! \brief Methods for bgcios::toolctl::ToolController class.

// Includes
#include "ToolController.h"
#include <ramdisk/include/services/common/RdmaError.h>
#include <ramdisk/include/services/common/RdmaDevice.h>
#include <ramdisk/include/services/common/SymbolicLink.h>
#include <ramdisk/include/services/common/TextFile.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/ToolctlMessages.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/common/logging.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <stdio.h>

//#define ENABLETIMERS

using namespace bgcios::toolctl;

LOG_DECLARE_FILE("cios.toolctld");

const uint32_t DefaultToolId = 0;

ToolController::ToolController() : bgcios::ServiceController()
{
    // Set work directory.
    _workDirectory = bgcios::WorkDirectory;

    // Set path to iosd command channel.
    std::ostringstream iosdPath;
    iosdPath << _workDirectory << bgcios::IosctlCommandChannelName;
    _iosdCmdChannelPath = iosdPath.str();

    // Set private data members.
    _serviceId = 0;
    _waitingForAck = false;
    _lastResidentSetSize = 0;
    _waitingForAckRank = (uint32_t)(-1);
    _waitingForAckJobID = 0;
    _interruptMsgSent = false;
}

ToolController::~ToolController()
{
    // Destroy the memory region for outbound messages.
    LOG_CIOS_DEBUG_MSG("destroying outbound message region with local key " << _outMessageRegion->getLocalKey());
    _outMessageRegion.reset();
    _outMessageRegion2.reset();

    // Destroy all of the memory regions for inbound messages.
    for (mr_list_iterator iter = _inMessageRegions.begin(); iter != _inMessageRegions.end(); ++iter)
    {
        LOG_CIOS_DEBUG_MSG("destroying inbound message region with local key " << iter->second->getLocalKey());
        iter->second->release();
    }

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

    // Destroy the command channel socket.
    LOG_CIOS_DEBUG_MSG("destroying command channel " << _cmdChannel->getName());
    _cmdChannel.reset();

    // Destroy the tool listener socket.
    LOG_CIOS_DEBUG_MSG("destroying tool listener " << _toolListener->getName());
    _toolListener.reset();
}

int ToolController::startup(uint32_t serviceId)
{
    // Reset umask to known value.
    ::umask(0);

    // Build the path to the command channel.
    _serviceId = serviceId;
    std::ostringstream cmdChannelPath;
    cmdChannelPath << _workDirectory << bgcios::ToolctlCommandChannelName << "." << _serviceId;

    // Set path to sysiod command channel.
    std::ostringstream sysiodPath;
    sysiodPath << _workDirectory << bgcios::SysioCommandChannelName << "." << _serviceId;
    _sysiodCmdChannelPath = sysiodPath.str();

    // Set path to the stiod command channel.
    std::ostringstream stdiodPath;
    stdiodPath << _workDirectory << bgcios::StdioCommandChannelName;
    _stdiodCmdChannelPath = stdiodPath.str();

    // Create the command channel socket.
    try
    {
        _cmdChannel = LocalDatagramSocketPtr(new LocalDatagramSocket(cmdChannelPath.str()));
    }
    catch (bgcios::SocketError& e)
    {
        LOG_ERROR_MSG("error creating command channel '" << cmdChannelPath.str() << "': " << e.what());
        return e.errcode();
    }

    // Build the path to the tool listener.
    std::ostringstream listenerPath;
    listenerPath << _workDirectory << ToolctlDataChannelName << "." << _serviceId;
    _toolListenerPath = listenerPath.str();

    // Create the tool listener socket.
    try
    {
        _toolListener = LocalStreamSocketPtr(new LocalStreamSocket(_toolListenerPath));
    }
    catch (bgcios::SocketError& e)
    {
        LOG_ERROR_MSG("error creating tool listener '" << _toolListenerPath << "': " << e.what());
        return e.errcode();
    }

    // Prepare socket to listen for connections.
    try
    {
        _toolListener->listen(MaxActiveTools);
    }
    catch (bgcios::SocketError& e)
    {
        LOG_ERROR_MSG("error listening for new tool connections: " << e.what());
        return e.errcode();
    }

    // Find the address of the I/O link device.
    RdmaDevicePtr linkDevice;
    try
    {
        linkDevice = RdmaDevicePtr(new RdmaDevice(bgcios::RdmaDeviceName, bgcios::RdmaInterfaceName));
    }
    catch (bgcios::RdmaError& e)
    {
        LOG_ERROR_MSG("error opening InfiniBand device: " << e.what());
        return e.errcode();
    }
    LOG_CIOS_DEBUG_MSG("created InfiniBand device for " << linkDevice->getDeviceName() << " using interface " << linkDevice->getInterfaceName());

    // Create listener for RDMA connections.
    try
    {
        _rdmaListener = bgcios::RdmaServerPtr(new bgcios::RdmaServer(linkDevice->getAddress(), (in_port_t)(0)));
    }
    catch (bgcios::RdmaError& e)
    {
        LOG_ERROR_MSG("error creating listening RDMA connection: " << e.what());
        return e.errcode();
    }

    // Create a protection domain object.
    try
    {
        _protectionDomain = RdmaProtectionDomainPtr(new RdmaProtectionDomain(_rdmaListener->getContext()));
    }
    catch (bgcios::RdmaError& e)
    {
        LOG_ERROR_MSG("error allocating protection domain: " << e.what());
        return e.errcode();
    }
    LOG_CIOS_DEBUG_MSG("created protection domain " << _protectionDomain->getHandle());

    // Create a completion channel object.
    try
    {
        _completionChannel = RdmaCompletionChannelPtr(new RdmaCompletionChannel(_rdmaListener->getContext(), false));
    }
    catch (bgcios::RdmaError& e)
    {
        LOG_ERROR_MSG("error constructing completion channel: " << e.what());
        return e.errcode();
    }
    LOG_CIOS_DEBUG_MSG("created completion channel using fd " << _completionChannel->getChannelFd());

    // Create a completion queue object.
    try
    {
        _completionQ = RdmaCompletionQueuePtr(new RdmaCompletionQueue(_rdmaListener->getContext(), RdmaCompletionQueue::SingleNodeQueueSize, _completionChannel->getChannel())); 
    }
    catch (bgcios::RdmaError& e)
    {
        LOG_ERROR_MSG("error constructing completion queue: " << e.what());
        return e.errcode();
    }
    LOG_CIOS_DEBUG_MSG("created completion queue " << _completionQ->getHandle());

    // Add the completion queue to the completion channel.
    _completionChannel->addCompletionQ(_completionQ);

    // Create a memory region for inbound messages.
    RdmaMemoryRegionPtr inMessageRegion = RdmaMemoryRegionPtr(new RdmaMemoryRegion());
    int err = inMessageRegion->allocate64kB(_protectionDomain);
    if (err != 0)
    {
        LOG_ERROR_MSG("error allocating first inbound message region: " << bgcios::errorString(err));
        return err;
    }
    LOG_CIOS_DEBUG_MSG("created first inbound message region with local key " << inMessageRegion->getLocalKey());

    // Add the inbound message region to the list.
    _inMessageRegions.add(inMessageRegion->getLocalKey(), inMessageRegion);

    // Create a second memory region for inbound messages.
    RdmaMemoryRegionPtr inMessageRegion2 = RdmaMemoryRegionPtr(new RdmaMemoryRegion());
    err = inMessageRegion2->allocate64kB(_protectionDomain);
    if (err != 0)
    {
        LOG_ERROR_MSG("error allocating second inbound message region: " << bgcios::errorString(err));
        return err;
    }
    LOG_CIOS_DEBUG_MSG("created second inbound message region with local key " << inMessageRegion2->getLocalKey());

    // Add the inbound message region to the list.
    _inMessageRegions.add(inMessageRegion2->getLocalKey(), inMessageRegion2);

    // Create a memory region for outbound messages.
    _outMessageRegion = RdmaMemoryRegionPtr(new RdmaMemoryRegion());
    err = _outMessageRegion->allocate64kB(_protectionDomain);
    if (err != 0)
    {
        LOG_ERROR_MSG("error allocating outbound message region: " << bgcios::errorString(err));
        return err;
    }
    LOG_CIOS_DEBUG_MSG("created outbound message region with local key " << _outMessageRegion->getLocalKey());

    _outMessageRegion2 = RdmaMemoryRegionPtr(new RdmaMemoryRegion());
    err = _outMessageRegion2->allocate64kB(_protectionDomain);
    if (err != 0)
    {
        LOG_ERROR_MSG("error allocating outbound message region2: " << bgcios::errorString(err));
        return err;
    }
    LOG_CIOS_DEBUG_MSG("created outbound message region2 with local key " << _outMessageRegion2->getLocalKey());

    // Listen for connections.
    err = _rdmaListener->listen(1);
    if (err != 0)
    {
        LOG_ERROR_MSG("error listening for new RDMA connections: " << bgcios::errorString(err));
        return err;
    }
    LOG_CIOS_DEBUG_MSG("listening for new RDMA connections on fd " << _rdmaListener->getEventChannelFd());

    // Add a default tool so CNK can send messages during job setup and job termination.
    ToolPtr defaultTool = ToolPtr(new Tool(0, DefaultToolId, "default"));
    _tools.add(defaultTool->getToolId(), defaultTool);

    return 0;
}

int ToolController::cleanup(void)
{
    return 0;
}

void ToolController::eventMonitor(void)
{
    const int cmdChannel   = 0;
    const int compChannel  = 1;
    const int eventChannel = 2;
    const int toolListener = 3;
    const int toolChannel1 = 4;
    const int toolChannel2 = 5;
    const int toolChannel3 = 6;
    const int toolChannel4 = 7;
    const int numFds       = 8;

    pollfd pollInfo[numFds];
    int timeout = 2000; // 2 seconds. Give reasonable time for in-flight IO syscalls to complete before taking action.
    // Initialize the pollfd structure.
    pollInfo[cmdChannel].fd = _cmdChannel->getSd();
    pollInfo[cmdChannel].events = POLLIN;
    pollInfo[cmdChannel].revents = 0;
    LOG_CIOS_TRACE_MSG("added command channel using fd " << pollInfo[cmdChannel].fd << " to descriptor list");

    pollInfo[compChannel].fd = _completionChannel->getChannelFd();
    pollInfo[compChannel].events = POLLIN;
    pollInfo[compChannel].revents = 0;
    LOG_CIOS_TRACE_MSG("added completion channel using fd " << pollInfo[compChannel].fd << " to descriptor list");

    pollInfo[eventChannel].fd = _rdmaListener->getEventChannelFd();
    pollInfo[eventChannel].events = POLLIN;
    pollInfo[eventChannel].revents = 0;
    LOG_CIOS_TRACE_MSG("added event channel using fd " << pollInfo[eventChannel].fd << " to descriptor list");

    pollInfo[toolListener].fd = _toolListener->getSd();
    pollInfo[toolListener].events = POLLIN;
    pollInfo[toolListener].revents = 0;
    LOG_CIOS_TRACE_MSG("added tool channel listener using fd " << pollInfo[toolListener].fd << " to descriptor list");

    //! \todo Should we remember connected tools if this method gets driven again after a failure?

    // Wait for a connection before starting to monitor the tool channels.
    pollInfo[toolChannel1].fd = -1;
    pollInfo[toolChannel1].events = POLLIN;
    pollInfo[toolChannel1].revents = 0;

    pollInfo[toolChannel2].fd = -1;
    pollInfo[toolChannel2].events = POLLIN;
    pollInfo[toolChannel2].revents = 0;

    pollInfo[toolChannel3].fd = -1;
    pollInfo[toolChannel3].events = POLLIN;
    pollInfo[toolChannel3].revents = 0;

    pollInfo[toolChannel4].fd = -1;
    pollInfo[toolChannel4].events = POLLIN;
    pollInfo[toolChannel4].revents = 0;

    // Iterate through tool channel list setting poll fds.
    // 
    // Process events until told to stop.
    while (!_done)
    {
        // Wait for an event on one of the descriptors.
        int rc = poll(pollInfo, numFds, timeout);

        // There was no data. We timed out.
        if (rc == 0)
        {
            // If we are waiting for an ACK, we could be in a situation where the compute node is stuck 
            // in a function shipping syscall.  Send a message to sysiod over the command channel to 
            // break the compute node out of  any blocking IO operation that is not completing. 
            if (_waitingForAck && ((int32_t)_waitingForAckRank >= 0) && !_interruptMsgSent)
            {
                // Build and send the Interrupt message to sysiod
                bgcios::iosctl::InterruptMessage interruptMsg;
                interruptMsg.header.service = bgcios::ToolctlService;
                interruptMsg.header.version = bgcios::toolctl::ProtocolVersion;
                interruptMsg.header.type = bgcios::iosctl::Interrupt;
                interruptMsg.header.rank = _waitingForAckRank;
                interruptMsg.header.sequenceId = 1;
                interruptMsg.header.returnCode = bgcios::Success;
                interruptMsg.header.errorCode = 0;
                interruptMsg.header.length = sizeof(bgcios::iosctl::InterruptMessage);
                interruptMsg.header.jobId = _waitingForAckJobID;
                interruptMsg.signo = 0;
                int err = sendToCommandChannel(_sysiodCmdChannelPath, &interruptMsg);
                if (err != 0)
                {
                    LOG_ERROR_MSG("Job " << _waitingForAckJobID << ": error sending Interrupt message to '" << _sysiodCmdChannelPath << "': " << bgcios::errorString(err));
                }
                // Send the Interrupt message to stiod. This is done to interrupt a potential stdin operation
                interruptMsg.signo = 0;
                err = sendToCommandChannel(_stdiodCmdChannelPath, &interruptMsg);
                if (err != 0)
                {
                    LOG_ERROR_MSG("Job " << _waitingForAckJobID << ": error sending Interrupt message to '" << _sysiodCmdChannelPath << "': " << bgcios::errorString(err));
                }

                _interruptMsgSent = true;

            }
            continue;
        }

        // There was an error so log the failure and try again.
        if (rc == -1)
        {
            int err = errno;
            if (err == EINTR)
            {
                LOG_CIOS_TRACE_MSG("poll returned EINTR, continuing ...");
                continue;
            }

            LOG_ERROR_MSG("error polling socket descriptors: " << bgcios::errorString(err));
            return;
        }

        // Check for an event on the completion channel.
        if (pollInfo[compChannel].revents & POLLIN)
        {
            LOG_CIOS_TRACE_MSG("input event available on completion channel");
            completionChannelHandler();
            pollInfo[compChannel].revents = 0;
        }

        // Check for an event on the first tool data channel.
        if (pollInfo[toolChannel1].revents & POLLIN)
        {
            LOG_CIOS_TRACE_MSG("input event available on first tool data channel using fd " << pollInfo[toolChannel1].fd);
            bool closed = toolChannelHandler(_toolChannels.get(pollInfo[toolChannel1].fd));
            if (closed)
            {
                LOG_CIOS_TRACE_MSG("removed first tool data channel using fd " << pollInfo[toolChannel1].fd);
                pollInfo[toolChannel1].fd = -1;
            }
            pollInfo[toolChannel1].revents = 0;
        }

        // Check for an event on the second tool data channel.
        if (pollInfo[toolChannel2].revents & POLLIN)
        {
            LOG_CIOS_TRACE_MSG("input event available on second tool data channel using fd " << pollInfo[toolChannel2].fd);
            bool closed = toolChannelHandler(_toolChannels.get(pollInfo[toolChannel2].fd));
            if (closed)
            {
                LOG_CIOS_TRACE_MSG("removed second tool data channel using fd " << pollInfo[toolChannel2].fd);
                pollInfo[toolChannel2].fd = -1;
            }
            pollInfo[toolChannel2].revents = 0;
        }

        // Check for an event on the third tool data channel.
        if (pollInfo[toolChannel3].revents & POLLIN)
        {
            LOG_CIOS_TRACE_MSG("input event available on third tool data channel using fd " << pollInfo[toolChannel3].fd);
            bool closed = toolChannelHandler(_toolChannels.get(pollInfo[toolChannel3].fd));
            if (closed)
            {
                LOG_CIOS_TRACE_MSG("removed third tool data channel using fd " << pollInfo[toolChannel3].fd);
                pollInfo[toolChannel3].fd = -1;
            }
            pollInfo[toolChannel3].revents = 0;
        }

        // Check for an event on the fourth tool data channel.
        if (pollInfo[toolChannel4].revents & POLLIN)
        {
            LOG_CIOS_TRACE_MSG("input event available on fourth tool data channel using fd " << pollInfo[toolChannel4].fd);
            bool closed = toolChannelHandler(_toolChannels.get(pollInfo[toolChannel4].fd));
            if (closed)
            {
                LOG_CIOS_TRACE_MSG("removed fourth tool data channel using fd " << pollInfo[toolChannel4].fd);
                pollInfo[toolChannel4].fd = -1;
            }
            pollInfo[toolChannel4].revents = 0;
        }

        // Check for an event on the command channel.
        if (pollInfo[cmdChannel].revents & POLLIN)
        {
            LOG_CIOS_TRACE_MSG("input event available on command channel");
            commandChannelHandler();
            pollInfo[cmdChannel].revents = 0;
        }

        // Check for an event on the event channel.
        if (pollInfo[eventChannel].revents & POLLIN)
        {
            LOG_CIOS_TRACE_MSG("input event available on event channel");
            eventChannelHandler();
            pollInfo[eventChannel].revents = 0;
        }

        // Check for an event on the tool data channel listener.
        if (pollInfo[toolListener].revents & POLLIN)
        {
            LOG_CIOS_TRACE_MSG("input event available on tool channel listener");
            pollInfo[toolListener].revents = 0;

            // Make a new tool data channel connected to a tool.
            bgcios::LocalStreamSocketPtr toolChannel = bgcios::LocalStreamSocketPtr(new bgcios::LocalStreamSocket());
            try
            {
                _toolListener->accept(toolChannel);
            }
            catch (bgcios::SocketError& e)
            {
                LOG_ERROR_MSG("error accepting new tool channel connection: " << e.what());
                toolChannel.reset();
                continue;
            }

            // Make sure another tool can be attached.
            if (_toolChannels.size() >= MaxActiveTools)
            {
                LOG_ERROR_MSG("maximum number of tools (" << MaxActiveTools << ") already connected");

                // Build an error message and disconnect.
                ErrorAckMessage *outMsg = (ErrorAckMessage *)_outboundMessage;
                memset(outMsg, 0x00, sizeof(ErrorAckMessage));
                outMsg->header.service = bgcios::ToolctlService;
                outMsg->header.version = ProtocolVersion;
                outMsg->header.type = ErrorAck;
                outMsg->header.returnCode = bgcios::ToolMaxAttachedExceeded;
                outMsg->header.length = sizeof(ErrorAckMessage);
                SocketPtr socket = std::tr1::static_pointer_cast<Socket>(toolChannel);
                sendMessageToStream(socket, outMsg);
                toolChannel.reset();
                continue;
            }

            // Create a Tool object and add it to the list of connected tools.
            ToolPtr tool = ToolPtr(new Tool(toolChannel));
            _toolChannels.add(toolChannel->getSd(), tool);

            // Start monitoring the new tool channel.
            if (pollInfo[toolChannel1].fd == -1)
            {
                pollInfo[toolChannel1].fd = toolChannel->getSd();
                LOG_CIOS_DEBUG_MSG("first tool data channel is connected to '" << toolChannel->getPeerName() << "' using fd " << toolChannel->getSd());
            }
            else if (pollInfo[toolChannel2].fd == -1)
            {
                pollInfo[toolChannel2].fd = toolChannel->getSd();
                LOG_CIOS_DEBUG_MSG("second tool data channel is connected to '" << toolChannel->getPeerName() << "' using fd " << toolChannel->getSd());
            }
            else if (pollInfo[toolChannel3].fd == -1)
            {
                pollInfo[toolChannel3].fd = toolChannel->getSd();
                LOG_CIOS_DEBUG_MSG("third tool data channel is connected to '" << toolChannel->getPeerName() << "' using fd " << toolChannel->getSd());
            }
            else if (pollInfo[toolChannel4].fd == -1)
            {
                pollInfo[toolChannel4].fd = toolChannel->getSd();
                LOG_CIOS_DEBUG_MSG("fourth tool data channel is connected to '" << toolChannel->getPeerName() << "' using fd " << toolChannel->getSd());
            }
            else
            {
                LOG_ERROR_MSG("there are no free slots in the poll array for a new tool channel");
            }
        }

    }

    // Reset for next time.
    _done = 0;

    return;
}

int ToolController::commandChannelHandler(void)
{
    // Receive a message from the command channel.
    std::string peer;
    int err = recvFromCommandChannel(peer, _inboundMessage);

    // An error occurred receiving a message.
    if (err != 0)
    {
        LOG_ERROR_MSG("error receiving message from command channel: " << bgcios::errorString(err));
        if (err == EPIPE) // When command channel closes, stop handling events.
        {
            _done = true;
        }
        return err;
    }

    // Make sure the service field is correct.
    bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_inboundMessage;
    if ((msghdr->service != bgcios::ToolctlService) && (msghdr->service != bgcios::IosctlService))
    {
        LOG_ERROR_MSG("Job " << msghdr->jobId << ": message service " << msghdr->service << " is wrong, header: " << bgcios::printHeader(*msghdr));
        sendErrorAckToCommandChannel(peer, bgcios::WrongService, 0);
        return 0;
    }

    // Handle the message.
    LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << ": " << bgcios::toString(msghdr) << " message is available on command channel");
    switch (msghdr->type)
    {
    
    case bgcios::iosctl::Terminate:
        err = terminate();
        break;

    default:
        LOG_ERROR_MSG("Job " << msghdr->jobId << ": message type " << msghdr->type << " is not supported, header: " << bgcios::printHeader(*msghdr));
        err = sendErrorAckToCommandChannel(peer, bgcios::UnsupportedType, 0);
        break;
    }

    if (err != 0)
    {
        LOG_ERROR_MSG("Job " << msghdr->jobId << ": error sending ack message: " << bgcios::errorString(err));
    }

    return 0;
}

void ToolController::eventChannelHandler(void)
{
    int err;

    // Wait for the event (it should be here now).
    err = _rdmaListener->waitForEvent();
    if (err != 0)
    {
        return;
    }

    // Handle the event.
    rdma_cm_event_type type = _rdmaListener->getEventType();
    switch (type)
    {
    
    case RDMA_CM_EVENT_CONNECT_REQUEST:
        {
            // Construct a new RdmaConnection object for the new client.
            _client = RdmaConnectionPtr(new RdmaConnection(_rdmaListener->getEventId(), _protectionDomain, _completionQ, _completionQ));

            // Post receives for the inbound regions to begin accepting messages.
            for (mr_list_iterator iter = _inMessageRegions.begin(); iter != _inMessageRegions.end(); ++iter)
            {
                _client->postRecv(iter->second);
            }

            // Accept the connection from the new client.
            err = _client->accept();
            if (err != 0)
            {
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
            if (err == 0)
            {
                LOG_CIOS_INFO_MSG(_client->getTag() << "disconnected from " << _client->getRemoteAddressString());
            }
            else
            {
                LOG_ERROR_MSG(_client->getTag() << "error disconnecting from peer: " << bgcios::errorString(err));
            }

            // Set flags to stop processing messages.
            _done = true;
            _terminated = true;

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

void ToolController::completionChannelHandler(void)
{
    try
    {
        // Get the notification event from the completion channel.
        RdmaCompletionQueuePtr completionQ = _completionChannel->getEvent();
        if (completionQ == NULL)
        {
            // This can be a normal occurrence if we had a cqe arrive after the getEvent and before the removeCompletions()
            LOG_CIOS_INFO_MSG("there was no notification event available from completion channel");
            return;
        }

        // Remove the work completions from the completion queue.
        completionQ->removeCompletions(RdmaCompletionQueue::MaxQueueSize);

        // Loop through all of the returned completions
        struct ibv_wc *completion = completionQ->popCompletion();
        while (completion)
        {
            // Check the status in the completion queue entry.
            if (completion->status != IBV_WC_SUCCESS)
            {
                LOG_ERROR_MSG("failed work completion, status '" << ibv_wc_status_str(completion->status) << "' for operation " <<
                              completionQ->wc_opcode_str(completion->opcode) <<  " (" << completion->opcode << ")");
                return;
            }

            // Check the opcode in the completion queue entry.
            switch (completion->opcode)
            {
            case IBV_WC_RECV:
                {
                    LOG_CIOS_TRACE_MSG("receive operation completed successfully for queue pair " << completion->qp_num << " (received " << completion->byte_len << " bytes)");

                    // Get the memory region where the message was received to.
                    RdmaMemoryRegionPtr inMessageRegion = _inMessageRegions.get((uint32_t)completion->wr_id);
                    if (inMessageRegion == NULL)
                    {
                        LOG_ERROR_MSG("message for queue pair " << completion->qp_num << " ignored because local key " << completion->wr_id <<
                                      " in work completion does not match any memory regions from inbound list");
                        break;
                    }
                    LOG_CIOS_TRACE_MSG("receive operation stored message in memory region with local key " << inMessageRegion->getLocalKey());

                    // Make sure tool is attached.
                    ToolMessage *inMsg = (ToolMessage *)inMessageRegion->getAddress();
                    ToolPtr tool = _tools.get(inMsg->toolId);
                    if (tool != NULL)
                    {
                        LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": " << tool->getTag() << toString(inMsg->header.type) << " message from rank " <<
                                           inMsg->header.rank << " is available on completion channel (memory region " << inMessageRegion->getLocalKey() << ")");
                        switch (inMsg->header.type)
                        {
                        
                        case QueryAck:
                            queryAck(tool, inMessageRegion);
                            _waitingForAck = false;
                            break;

                        case UpdateAck:
                            updateAck(tool, inMessageRegion);
                            _waitingForAck = false;
                            break;

                        case Notify:
                            notify(tool, inMessageRegion);
                            break;

                        case ErrorAck:
                            errorAck(tool, inMessageRegion);
                            _waitingForAck = false;
                            break;

                        case SetupJob:
                            setupJob(inMessageRegion);
                            break;

                        case AttachAck:
                            attachAck(tool, inMessageRegion);
                            _waitingForAck = false;
                            break;

                        case DetachAck:
                            detachAck(tool, inMessageRegion);
                            _waitingForAck = false;
                            break;

                        case ControlAck:
                            controlAck(tool, inMessageRegion);
                            _waitingForAck = false;
                            break;

                        default:
                            {
                                LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": unsupported message type " << inMsg->header.type << " received from rank " <<
                                              inMsg->header.rank << " " << bgcios::printHeader(inMsg->header));

                                // Prepare the ack message. 
                                ErrorAckMessage *errAckMsg = (ErrorAckMessage *)_outMessageRegion2->getAddress();;
                                memcpy(&(errAckMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
                                errAckMsg->header.type = ErrorAck;
                                errAckMsg->header.length = sizeof(ErrorAckMessage);
                                errAckMsg->header.returnCode = bgcios::RequestFailed;
                                errAckMsg->header.errorCode = ENOTSUP;
                                _outMessageRegion2->setMessageLength(errAckMsg->header.length);
                                break;
                            }
                        }

                    }
                    else
                    {
                        LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": tool " << inMsg->toolId << " is not attached when handling " << toString(inMsg->header.type) <<
                                          " message from rank " << inMsg->header.rank);
                        _waitingForAck = false;

                        if (inMsg->header.type == Notify)
                        {
                            // Build NotifyAck in outbound message region.
                            NotifyAckMessage *outMsg = (NotifyAckMessage *)_outMessageRegion2->getAddress();
                            memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
                            outMsg->header.type = NotifyAck;
                            outMsg->header.length = sizeof(NotifyAckMessage);
                            outMsg->header.returnCode = bgcios::ToolIdError;
                            outMsg->header.errorCode = ESRCH;
                            outMsg->toolId = inMsg->toolId;
                            _outMessageRegion2->setMessageLength(outMsg->header.length);
                            LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": [Tool " << outMsg->toolId << "] NotifyAck message is ready for rank " << outMsg->header.rank);
                        }
                    }

                    // Post a receive to get next message.
                    _client->postRecv(inMessageRegion);

                    // Post a send if there is a message available.
                    if (_outMessageRegion2->isMessageReady())
                    {
                        _client->postSend(_outMessageRegion2); 
                    }
                    // If a queued message is available, start a new message exchange.
                    if ((!_waitingForAck) &&  (!_queuedMessages.empty()))
                    {
                        LOG_CIOS_TRACE_MSG("queued message available on list.");
                        ToolctlMessagePtr message = _queuedMessages.front();
                        routeMessageFromTool(message);
                        _queuedMessages.pop_front();
                    }

                    break;
                }

            case IBV_WC_SEND:
                {
                    LOG_CIOS_TRACE_MSG("send operation completed successfully for queue pair " << completion->qp_num << ". Reset waitingForSendComplete.");
                    // If a queued message is available, start a new message exchange.
                    break;
                }

            case IBV_WC_RDMA_WRITE:
                {
                    LOG_CIOS_DEBUG_MSG("rdma write operation completed successfully for queue pair " << completion->qp_num);
                    break;
                }

            case IBV_WC_RDMA_READ:
                {
                    LOG_CIOS_DEBUG_MSG("rdma read operation completed successfully for queue pair " << completion->qp_num);
                    break;
                }

            default:
                {
                    LOG_ERROR_MSG("unsupported operation " << completion->opcode << " in work completion");
                    break;
                }
            }
            // get the next cqe if it exists
            completion = completionQ->popCompletion();
        }
    }

    catch (const RdmaError& e)
    {
        LOG_ERROR_MSG("error handling work completions from completion queue: " << bgcios::errorString(e.errcode()));
    }

    return;
}

bool ToolController::toolChannelHandler(const ToolPtr& tool)
{
    // Make sure there is a Tool object associated with the tool data channel.
    if (tool == NULL)
    {
        LOG_ERROR_MSG("tool channel does not map to a connected tool");
        return true;
    }

    // Receive a message from the tool data channel.

    // If the outMessageRegion (destined for the compute) is available, put it directly in there to avoid unnecessary moving
    ToolMessage *msg = (ToolMessage *)_outMessageRegion->getAddress();
    bool send_msg = true;
    if (_waitingForAck)
    {
        send_msg = false;
        msg = (ToolMessage *)_inboundMessage;
    }
    // Receive the message from the tool
    int err = recvFromTool(tool, msg);
    ToolctlMessagePtr message = ToolctlMessagePtr(new ToolctlMessage(tool, msg));

    // An error occurred receiving a message.
    if (err != 0)
    {
        LOG_CIOS_DEBUG_MSG(tool->getPrefix() << "error receiving message from data channel using fd " << tool->getChannel()->getSd() << ": " << bgcios::errorString(err));
        // When tool data channel closes, cleanup after the tool.
        if (err == EPIPE)
        {
            removeTool(tool);
            return true;
        }
        return false;
    }

    LOG_CIOS_DEBUG_MSG("Job " << message->getJobId() << ": " << tool->getTag() << toString(message->getType()) << " message is available on tool data channel");

    if (send_msg)
    {
        routeMessageFromTool(message);
    }
    else
    {
        // We cannot put the message directly in the outbound region because the region is busy. Queue up the message.
        LOG_CIOS_DEBUG_MSG("Job " << message->getJobId() << ": " << tool->getTag() << toString(message->getType()) << " message added to queue");
        message->save();
        _queuedMessages.push_back(message);
    }
    return false;
}

void ToolController::routeMessageFromTool(ToolctlMessagePtr& message)
{
    // Handle the message.
    switch (message->getType())
    {
    case Query:
        query(message);
        break;

    case Update:
        update(message);
        break;

    case Attach:
        attach(message);
        break;

    case Detach:
        detach(message);
        break;

    case Control:
        control(message);
        break;

    default:
        LOG_ERROR_MSG("Job " << message->getJobId() << ": " << message->getTool()->getTag() << "message type " << message->getType() <<
                      " is not supported, header: " << bgcios::printHeader(*message->getHeader()));
        sendErrorAckToTool(message->getTool(), bgcios::UnsupportedType, 0);
        break;
    }

    // Destroy message.
    message.reset();

    return;
}

void ToolController::removeTool(const ToolPtr& tool)
{
    // Remove tool from the list of connected tool channels
    bgcios::LocalStreamSocketPtr toolChannel = tool->getChannel();
    _toolChannels.remove(toolChannel->getSd());
    toolChannel.reset();

    // get the toolid assocated with this tool
    uint32_t toolId = tool->getToolId();
    // Just ignore a request to remove the default tool from the list of attached tools.
    if (toolId == DefaultToolId)
    {
        return;
    }
    // Remove tool from the list of attached tools.
    _tools.remove(toolId);
    LOG_CIOS_INFO_MSG(tool->getPrefix() << "removed tool from list of connected and attached tools");

    return;
}

void ToolController::errorAck(ToolPtr& tool, RdmaMemoryRegionPtr& inMessageRegion)
{
    // Get pointer to inbound ErrorAck message.
    ErrorAckMessage *inMsg = (ErrorAckMessage *)inMessageRegion->getAddress();

    // Forward ErrorAck message to specified tool.
    sendToTool(tool, inMsg);
    LOG_CIOS_DEBUG_MSG(tool->getPrefix() << "ErrorAck message from rank " << inMsg->header.rank << " forwarded to tool");
    return;
}

void ToolController::setupJob(RdmaMemoryRegionPtr& inMessageRegion)
{
    // Get pointer to inbound SetupJob message.
    SetupJobMessage *inMsg = (SetupJobMessage *)inMessageRegion->getAddress();

    // Build ack message in outbound message region.
    SetupJobAckMessage *outMsg = (SetupJobAckMessage *)_outMessageRegion2->getAddress();
    memcpy(outMsg, inMsg, sizeof(MessageHeader));
    outMsg->header.type = SetupJobAck;
    outMsg->header.length = sizeof(SetupJobAckMessage);
    outMsg->header.returnCode = bgcios::Success;
    _outMessageRegion->setMessageLength(outMsg->header.length);

    // Make sure the message header has valid values for the service and version fields.  Note we only do these checks with a SetupJob message
    // so as to not have a performance hit on every message.
    if (inMsg->header.service != ToolctlService)
    {
        LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": message service " << inMsg->header.service << " is wrong");
        outMsg->header.returnCode = bgcios::WrongService;
        return;
    }

    if (inMsg->header.version != ProtocolVersion)
    {
        LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": SetupJob message protocol version " << (int)inMsg->header.version << " does not match toolctld version " <<
                      (int)ProtocolVersion << " (" << getVersionString("toolctld", (int)ProtocolVersion) << ")");
        outMsg->header.returnCode = bgcios::VersionMismatch;
        outMsg->header.errorCode = ProtocolVersion;
        return;
    }

    // Create a link to the listening socket for each rank that is using this daemon.
    for (uint16_t index = 0; index < inMsg->numRanks; ++index)
    {
        // Build the path of the link.
        std::ostringstream linkPath;
        linkPath << bgcios::JobsDirectory << inMsg->header.jobId << bgcios::ToolctlRankDirectory << inMsg->ranks[index];

        // Create the symbolic link.
        try
        {
            bgcios::SymbolicLink link(_toolListenerPath, linkPath.str());
            link.setOwner(inMsg->userId, inMsg->groupId);
            LOG_CIOS_TRACE_MSG("Job " << inMsg->header.jobId << ": created rank link '" << linkPath.str() << "'");
        }
        catch (bgcios::LinkError& e)
        {
            outMsg->header.returnCode = bgcios::RequestFailed;
            outMsg->header.errorCode = (uint32_t)e.errcode();
            LOG_ERROR_MSG("Job " << outMsg->header.jobId << ": error creating rank link '" << linkPath.str() << "': " << bgcios::errorString(e.errcode()));
        }
    }

    // Create a link to the listening socket for the compute node that is using this daemon.
    std::ostringstream linkPath;
    linkPath << bgcios::JobsDirectory << inMsg->header.jobId << bgcios::ToolctlNodeDirectory << inMsg->nodeId;
    try
    {
        bgcios::SymbolicLink link(_toolListenerPath, linkPath.str());
        link.setOwner(inMsg->userId, inMsg->groupId);
    }
    catch (bgcios::LinkError& e)
    {
        outMsg->header.returnCode = bgcios::RequestFailed;
        outMsg->header.errorCode = (uint32_t)e.errcode();
        LOG_ERROR_MSG("Job " << outMsg->header.jobId << ": error creating node link '" << linkPath.str() << "': " << bgcios::errorString(e.errcode()));
    }

    // Create protocol version file.
    std::ostringstream protocolPath;
    protocolPath << bgcios::JobsDirectory << inMsg->header.jobId << bgcios::ToolsDirectory << "protocol";
    std::ostringstream protocol;
    protocol << (int)ProtocolVersion;
    try
    {
        bgcios::TextFile protocolFile(protocolPath.str(), S_IRUSR|S_IRGRP|S_IROTH);
        protocolFile.setOwner(inMsg->userId, inMsg->groupId);
        protocolFile.writeString(protocol.str());
        protocolFile.close();
    }
    catch (bgcios::TextFileError& e)
    {
        outMsg->header.returnCode = bgcios::RequestFailed;
        outMsg->header.errorCode = (uint32_t)e.errcode();
        LOG_ERROR_MSG("Job " << outMsg->header.jobId << ": error creating protocol file '" << protocolPath.str() << "': " << bgcios::errorString(e.errcode()));
    }

    // Send SetupJobAck message to compute node.
    _outMessageRegion2->setMessageLength(outMsg->header.length);
    LOG_CIOS_DEBUG_MSG("Job " << outMsg->header.jobId << ": SetupJobAck message is ready for rank " << outMsg->header.rank);

    // Get resource usage information and make sure we are not leaking memory.
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    if (_lastResidentSetSize != 0)
    {
        if (usage.ru_maxrss > (_lastResidentSetSize + (_lastResidentSetSize / 100 * 2)))
        {
            LOG_CIOS_WARN_MSG("Job " << inMsg->header.jobId << ": resident set size of daemon " << _serviceId << " has grown by more than 2% since last job ended, current maxrss is " <<
                              usage.ru_maxrss << " kB and last maxrss is " << _lastResidentSetSize << " kB");
            _lastResidentSetSize = usage.ru_maxrss;
        }
    }
    else
    {
        _lastResidentSetSize = usage.ru_maxrss;
        LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": initial resident set size of daemon " << _serviceId << " is " << _lastResidentSetSize << " kB");
    }

    // Make sure the lists are reset from the last job.
    if (_toolChannels.size() > 0)
    {
        LOG_CIOS_WARN_MSG("Job " << inMsg->header.jobId << ": there are " << _toolChannels.size() << " tools in the list of connected tools when there should be 0");
        for (tool_channel_iterator iter = _toolChannels.begin(); iter != _toolChannels.end(); ++iter)
        {
            LOG_CIOS_WARN_MSG("Job " << inMsg->header.jobId << ": " << iter->second->getName() << " (" << iter->second->getToolId() << ") tool is still connected");
        }
    }

    if (_tools.size() > 1)
    {
        LOG_CIOS_WARN_MSG("Job " << inMsg->header.jobId << ": there are " << _tools.size() - 1 << " tools in the list of attached tools when there should be 0");
        for (tool_list_iterator iter = _tools.begin(); iter != _tools.end(); ++iter)
        {
            if (iter->first != 0)
            {
                LOG_CIOS_WARN_MSG("Job " << inMsg->header.jobId << ": " << iter->second->getName() << " (" << iter->second->getToolId() << ") tool is still attached");
            }
        }
    }

    return;
}

void ToolController::attach(ToolctlMessagePtr& message)
{
    // Make sure the message header has valid values for the service and version fields.  Note we only do these checks with an Attach message
    // so as to not have a performance hit on every message.
    ToolPtr tool = message->getTool();
    if (message->getService() != bgcios::ToolctlService)
    {
        LOG_ERROR_MSG("Job " << message->getJobId() << ": Attach message from tool " << message->getToolId() << " has wrong service " << (uint32_t)message->getService() <<
                      ", header: " << bgcios::printHeader(*message->getHeader()));
        sendErrorAckToTool(tool, bgcios::WrongService, 0);
        return;
    }

    if (message->getVersion() != ProtocolVersion)
    {
        LOG_WARN_MSG("Job " << message->getJobId() << ": Attach message from tool " << message->getToolId() << " has protocol version " << (int)message->getVersion() <<
                     " that is different than the toolctld version " << (int)ProtocolVersion << " (" << getVersionString("toolctld", (int)ProtocolVersion) << ")");
    }

    // Check for conflicts with other tools that are already attached.
    for (tool_list_iterator iter = _tools.begin(); iter != _tools.end(); ++iter)
    {
        ToolPtr otherTool = iter->second;

        // Make sure tool id is not already in use by another tool.
        if (otherTool->getToolId() == message->getToolId())
        {
            LOG_ERROR_MSG("Job " << message->getJobId() << ": tool " << message->getToolId() << " conflicts with attached tool " << otherTool->getToolId());

            // Build AttachAck message in outbound buffer.
            AttachAckMessage *outMsg = (AttachAckMessage *)initAckMessage(message->getHeader(), AttachAck);
            outMsg->header.length = sizeof(AttachAckMessage);
            outMsg->header.returnCode = bgcios::ToolIdConflict;
            outMsg->header.errorCode = EINVAL;
            sendToTool(tool, outMsg);
            LOG_CIOS_DEBUG_MSG("Job " << message->getJobId() << ": [Tool " << message->getToolId() << "] AttachAck message sent to tool (" <<
                               bgcios::returnCodeToString(outMsg->header.returnCode) << ")");
            return;
        }
    }

    // Set information about the tool and add it to the list of attached tools.
    AttachMessage *inMsg = (AttachMessage *)message->getAddress();
    tool->set(inMsg->header.jobId, inMsg->toolId, inMsg->toolTag);
    _tools.add(inMsg->toolId, tool);
    LOG_CIOS_INFO_MSG(tool->getPrefix() << "added tool to list of attached tools");

    // Copy the message to the outbound message region if it was saved after being received.
    if (message->isSaved())
    {
        memcpy(_outMessageRegion->getAddress(), message->getAddress(), message->getLength());
    }

    // Forward Attach message to compute node.
    try
    {
        _outMessageRegion->setMessageLength(message->getLength());
        if (inMsg->procSelect == RankInHeader)
        {
            _waitingForAckRank = inMsg->header.rank;
            _waitingForAckJobID = inMsg->header.jobId;
        }
        else
        {
            _waitingForAckRank = (uint32_t)(-1); // Attaching to all ranks. No specific rank to record.
            _waitingForAckJobID = inMsg->header.jobId;
        }
        _waitingForAck = true;
        _interruptMsgSent = false;
        _client->postSend(_outMessageRegion);
        LOG_CIOS_DEBUG_MSG(tool->getPrefix() << "Attach message forwarded to rank " << message->getRank());
    }
    catch (const RdmaError& e)
    {
        LOG_ERROR_MSG(tool->getPrefix() << "error posting Attach message to rank " << message->getRank() << ": " << bgcios::errorString(e.errcode()));
    }

#ifdef ENABLETIMERS
    _queryTimer.reset();
    _updateTimer.reset();
#endif

    return;
}

void ToolController::attachAck(ToolPtr& tool, RdmaMemoryRegionPtr& inMessageRegion)
{
    // Get pointer to inbound AttachAck message.
    AttachAckMessage *inMsg = (AttachAckMessage *)inMessageRegion->getAddress();

    // Forward AttachAck message to specified tool.
    if (sendToTool(tool, inMsg) == 0)
    {
        LOG_CIOS_DEBUG_MSG(tool->getPrefix() << "AttachAck message from rank " << inMsg->header.rank << " forwarded to tool");
    }

    return;
}

void ToolController::detach(ToolctlMessagePtr& message)
{
    // Make sure tool is attached.
    if (_tools.get(message->getToolId()) == NULL)
    {
        LOG_ERROR_MSG("Job " << message->getJobId() << ": tool " << message->getToolId() << " is not attached when handling Detach message for rank " << message->getRank());

        // Build DetachAck message in outbound buffer.
        DetachAckMessage *outMsg = (DetachAckMessage *)initAckMessage(message->getHeader(), DetachAck);
        outMsg->header.returnCode = bgcios::ToolIdError;
        outMsg->header.errorCode = EINVAL;
        outMsg->header.length = sizeof(DetachAckMessage);
        outMsg->toolId = message->getToolId();
        sendToTool(message->getTool(), outMsg);
        return;
    }

    // Copy the message to the outbound message region if it was saved after being received.
    if (message->isSaved())
    {
        memcpy(_outMessageRegion->getAddress(), message->getAddress(), message->getLength());
    }

    // Forward Detach message to compute node.
    try
    {
        _outMessageRegion->setMessageLength(message->getLength());
        _waitingForAckRank = message->getRank();
        _waitingForAckJobID = message->getJobId();;
        _waitingForAck = true;
        _interruptMsgSent = false;
        _client->postSend(_outMessageRegion);
        LOG_CIOS_DEBUG_MSG(message->getTool()->getPrefix() << "Detach message forwarded to rank " << message->getRank());
    }
    catch (const RdmaError& e)
    {
        LOG_ERROR_MSG(message->getTool()->getPrefix() << "error posting Detach message to rank " << message->getRank() << ": " << bgcios::errorString(e.errcode()));
    }

#ifdef ENABLETIMERS
    LOG_INFO_MSG_FORCED("stats for query: " << _queryTimer);
    LOG_INFO_MSG_FORCED("stats for update: " << _updateTimer);
#endif

    return;
}

void ToolController::detachAck(ToolPtr& tool, RdmaMemoryRegionPtr& inMessageRegion)
{
    // Get pointer to inbound DetachAck message.
    DetachAckMessage *inMsg = (DetachAckMessage *)inMessageRegion->getAddress();

    // Forward DetachAck message to specified tool.
    if (sendToTool(tool, inMsg) == 0)
    {
        LOG_CIOS_DEBUG_MSG(tool->getPrefix() << "DetachAck message from rank " << inMsg->header.rank << " forwarded to tool");
    }

    // Remove tool from the list of attached tools.
    _tools.remove(tool->getToolId());
    LOG_CIOS_INFO_MSG(tool->getPrefix() << "removed tool from list of attached tools and from list of connected tools");

    return;
}

void ToolController::query(ToolctlMessagePtr& message)
{

    // Make sure tool is attached.
    if (_tools.get(message->getToolId()) == NULL)
    {
        LOG_ERROR_MSG("Job " << message->getJobId() << ": tool " << message->getToolId() << " is not attached when handling Query message for rank " << message->getRank());

        // Build QueryAck message in outbound buffer.
        QueryAckMessage *outMsg = (QueryAckMessage *)initAckMessage(message->getHeader(), QueryAck);
        outMsg->header.returnCode = bgcios::ToolIdError;
        outMsg->header.errorCode = EINVAL;
        outMsg->header.length = sizeof(QueryAckMessage);
        outMsg->toolId = message->getToolId();
        outMsg->numCommands = 0;
        sendToTool(message->getTool(), outMsg);
        return;
    }

    // Copy the message to the outbound message region if it was saved after being received.
    if (message->isSaved())
    {
        memcpy(_outMessageRegion->getAddress(), message->getAddress(), message->getLength());
    }

    // Forward Query message to compute node.
    try
    {
        _outMessageRegion->setMessageLength(message->getLength());
        _waitingForAck = true;
        _interruptMsgSent = false;
        _waitingForAckRank = message->getRank();
        _waitingForAckJobID = message->getJobId();
#ifdef ENABLETIMERS
        _queryTimer.start();
#endif
        _client->postSend(_outMessageRegion);
        LOG_CIOS_DEBUG_MSG(message->getTool()->getPrefix() << "Query message forwarded to rank " << message->getRank());
    }
    catch (const RdmaError& e)
    {
        LOG_ERROR_MSG(message->getTool()->getPrefix() << ": error posting Query message to rank " << message->getRank() <<  ": " << bgcios::errorString(e.errcode()));
    }

    return;
}

void ToolController::queryAck(ToolPtr& tool, RdmaMemoryRegionPtr& inMessageRegion)
{
#ifdef ENABLETIMERS
    _queryTimer.stop();
#endif
    // Get pointer to inbound QueryAck message.
    QueryAckMessage *inMsg = (QueryAckMessage *)inMessageRegion->getAddress();

    // Forward QueryAck message to specified tool.
    //_sendMessageTimer.start();
    if (sendToTool(tool, inMsg) == 0)
    {
        LOG_CIOS_DEBUG_MSG(tool->getPrefix() << "QueryAck message from rank " << inMsg->header.rank << " forwarded to tool");
    }
    //_sendMessageTimer.stop();

    return;
}

void ToolController::update(ToolctlMessagePtr& message)
{

    // Make sure tool is attached.
    if (_tools.get(message->getToolId()) == NULL)
    {
        LOG_ERROR_MSG("Job " << message->getJobId() << ": tool " << message->getToolId() << " is not attached when handling Update message for rank " << message->getRank());

        // Build UpdateAck message in outbound buffer.
        UpdateAckMessage *outMsg = (UpdateAckMessage *)initAckMessage(message->getHeader(), UpdateAck);
        outMsg->header.returnCode = bgcios::ToolIdError;
        outMsg->header.errorCode = EINVAL;
        outMsg->toolId = message->getToolId();
        outMsg->numCommands = 0;
        sendToTool(message->getTool(), outMsg);
        return;
    }

    // Copy the message to the outbound message region if it was saved after being received.
    if (message->isSaved())
    {
        memcpy(_outMessageRegion->getAddress(), message->getAddress(), message->getLength());
    }

    // Forward Update message to compute node.
    try
    {
        _outMessageRegion->setMessageLength(message->getLength());
        #ifdef ENABLETIMERS
        _updateTimer.start();
        #endif
        _waitingForAckRank = message->getRank();
        _waitingForAckJobID = message->getJobId();
        _waitingForAck = true;
        _interruptMsgSent = false;
        _client->postSend(_outMessageRegion);
        LOG_CIOS_DEBUG_MSG(message->getTool()->getPrefix() << "Update message forwarded to rank " << message->getRank());
    }
    catch (const RdmaError& e)
    {
        LOG_ERROR_MSG(message->getTool()->getPrefix() << "error posting Update message to rank " << message->getRank() <<  ": " << bgcios::errorString(e.errcode()));
    }

    return;
}

void ToolController::updateAck(ToolPtr& tool, RdmaMemoryRegionPtr& inMessageRegion)
{
    #ifdef ENABLETIMERS
    _updateTimer.stop();
    #endif
    // Get pointer to inbound UpdateAck message.
    UpdateAckMessage *inMsg = (UpdateAckMessage *)inMessageRegion->getAddress();


    // Forward UpdateAck message to specified tool.
    //_sendMessageTimer.start();
    if (sendToTool(tool, inMsg) == 0)
    {
        LOG_CIOS_DEBUG_MSG(tool->getPrefix() << "UpdateAck message from rank " << inMsg->header.rank << " forwarded to tool");
    }
    //_sendMessageTimer.stop();

    return;
}

void ToolController::notify(ToolPtr& tool, RdmaMemoryRegionPtr& inMessageRegion)
{
    // Get pointer to inbound Notify message.
    NotifyMessage *inMsg = (NotifyMessage *)inMessageRegion->getAddress();

    // Forward Notify message to specified tool.
    sendToTool(tool, inMsg);
    LOG_CIOS_DEBUG_MSG(tool->getPrefix() << "Notify message from rank " << inMsg->header.rank << " forwarded to tool");

    // Build the Notify Ack message. 
    // 
    // Prepare the ack message. 
    NotifyAckMessage *notifyAckMsg = (NotifyAckMessage *)_outMessageRegion2->getAddress();;
    memcpy(&(notifyAckMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
    notifyAckMsg->header.type = NotifyAck;
    notifyAckMsg->header.length = sizeof(NotifyAckMessage);
    notifyAckMsg->header.returnCode = bgcios::Success;
    _outMessageRegion2->setMessageLength(notifyAckMsg->header.length);
    LOG_CIOS_DEBUG_MSG(tool->getPrefix() << "NotifyAck message is ready for rank " << notifyAckMsg->header.rank);

    return;
}

void ToolController::control(ToolctlMessagePtr& message)
{
    // Make sure tool is attached.
    if (_tools.get(message->getToolId()) == NULL)
    {
        LOG_ERROR_MSG("Job " << message->getJobId() << "tool " << message->getToolId() << " is not attached when handling Control message for rank " << message->getRank());

        // Build ControlAck message in outbound buffer.
        ControlAckMessage *outMsg = (ControlAckMessage *)initAckMessage(message->getHeader(), ControlAck);
        outMsg->header.returnCode = bgcios::ToolIdError;
        outMsg->header.errorCode = EINVAL;
        outMsg->toolId = message->getToolId();
        sendToTool(message->getTool(), outMsg);
        return;
    }

    // Copy the message to the outbound message region if it was saved after being received.
    if (message->isSaved())
    {
        memcpy(_outMessageRegion->getAddress(), message->getAddress(), message->getLength());
    }

    // Forward Control message to compute node.
    try
    {
        _outMessageRegion->setMessageLength(message->getLength());
        _waitingForAck = true;
        _interruptMsgSent = false;
        _waitingForAckRank = message->getRank();
        _waitingForAckJobID = message->getJobId();
        _client->postSend(_outMessageRegion);
        LOG_CIOS_DEBUG_MSG(message->getTool()->getPrefix() << "Control message forwarded to rank " << message->getRank());
    }
    catch (const RdmaError& e)
    {
        LOG_ERROR_MSG(message->getTool()->getPrefix() << "error posting Control message to rank " << message->getRank() << ": " << bgcios::errorString(e.errcode()));
    }

    return;
}

void ToolController::controlAck(ToolPtr& tool, RdmaMemoryRegionPtr& inMessageRegion)
{
    // Get pointer to inbound ControlAck message.
    ControlAckMessage *inMsg = (ControlAckMessage *)inMessageRegion->getAddress();

    // Forward ControlAck message to specified tool.
    if (sendToTool(tool, inMsg) == 0)
    {
        LOG_CIOS_DEBUG_MSG(tool->getPrefix() << "ControlAck message from rank " << inMsg->header.rank << " forwarded to tool");
    }

    return;
}

int ToolController::terminate(void)
{
    // Get pointer to Terminate message available from inbound buffer.
    bgcios::iosctl::TerminateMessage *inMsg = (bgcios::iosctl::TerminateMessage *)_inboundMessage;

    // Cleanup resources.
    int err = cleanup();

    // Set flags to stop processing messages.
    _done = true;
    _terminated = true;

    // Build TerminateAck message in outbound buffer.
    bgcios::iosctl::TerminateAckMessage *outMsg = (bgcios::iosctl::TerminateAckMessage *)_outboundMessage;
    memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
    outMsg->header.type = bgcios::iosctl::TerminateAck;
    outMsg->header.length = sizeof(bgcios::iosctl::TerminateAckMessage);
    if (err == 0)
    {
        outMsg->header.returnCode = bgcios::Success;
    }
    else
    {
        outMsg->header.returnCode = bgcios::RequestFailed;
        outMsg->header.errorCode = (uint32_t)err;
    }

    // Send TerminateAck message.
    LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": TerminateAck message is ready for command channel");
    return sendToCommandChannel(_iosdCmdChannelPath, outMsg);
}

int ToolController::sendErrorAckToTool(const ToolPtr& tool, uint32_t returnCode, uint32_t errorCode)
{
    // Build ErrorAck message in outbound message region.
    ErrorAckMessage *outMsg = (ErrorAckMessage *)_outboundMessage;
    memcpy(&(outMsg->header), _inboundMessage, sizeof(MessageHeader));
    outMsg->header.service = bgcios::ToolctlService;
    outMsg->header.type = ErrorAck;
    outMsg->header.returnCode = returnCode;
    outMsg->header.errorCode = errorCode;
    outMsg->header.length = sizeof(ErrorAckMessage);

    // Send ErrorAck message.
    return sendToTool(tool, outMsg);
}

