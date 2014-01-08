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
/* (C) Copyright IBM Corp.  2008, 2011                              */
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
#include "toolintf.h"

using namespace bgcios::toolctl;

LOG_DECLARE_FILE("cios.tests");

//! Maximum number of compute nodes to connect to.
const int MaxComputeNodes = 128;

int numFds = 0;

uint64_t numSentMessages = 0;
uint64_t numReplies = 0;

bool   validPollInfo = false;
pollfd pollInfo[MaxComputeNodes];

map<bgcios::LocalStreamSocketPtr, AttachAckMessage> attachMap;

void attach(JobPtr job)
{
    // Build Attach message (the same one is sent to every compute node).
    union
    {
        AttachMessage outMsg;
        char pad[32768];
    };
    memset(&outMsg.header, 0x00, sizeof(outMsg));
    outMsg.header.service = bgcios::ToolctlService;
    outMsg.header.version = ProtocolVersion;
    outMsg.header.type = Attach;
    outMsg.header.rank = 0;
    outMsg.header.length = sizeof(outMsg);
    outMsg.header.jobId = job->getJobId();
    outMsg.toolId = job->getToolId();
    outMsg.procSelect = RanksInNode;
    outMsg.priority = 10;
    
    // Attach to all of the compute node processes.
    for (Job::channel_list_iterator iter = job->toolChannels.begin(); iter != job->toolChannels.end(); ++iter) 
    {
        sendMessage(outMsg, iter->second);
    }
}


void detach(JobPtr job)
{
    // Build Attach message (the same one is sent to every compute node).
    // Build a Query message to get info about the process.
    int x;
    union
    {
        DetachMessage outMsg;
        char pad[32768];
    };
    memset(&outMsg.header, 0x00, sizeof(outMsg));
    outMsg.header.service = bgcios::ToolctlService;
    outMsg.header.version = ProtocolVersion;
    outMsg.header.type = Detach;
    outMsg.header.rank = 0;
    outMsg.header.length = sizeof(outMsg);
    outMsg.header.jobId = job->getJobId();
    outMsg.toolId = job->getToolId();
    outMsg.procSelect = RanksInNode;

    // Dettach to all of the compute node processes.
    for (Job::channel_list_iterator iter = job->toolChannels.begin(); iter != job->toolChannels.end(); ++iter) 
    {   
        sendMessage(outMsg, iter->second);
    }
    pollForData(job);
}



void
messageHandler(bgcios::LocalStreamSocketPtr toolChannel, JobPtr job)
{
    // Receive a message from the tool data channel.
    char inboundMessage[bgcios::SmallMessageDataSize];
    if(! receiveMessage((ToolMessage *)inboundMessage, job, toolChannel))
        return;
    
    // Handle the message.
    bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)inboundMessage;
    LOG_DEBUG_MSG(toString(msghdr->type) << " message available on tool data channel");
    switch (msghdr->type) {
            
        case AttachAck:
            numReplies++;
            attachAck((AttachAckMessage *)inboundMessage, toolChannel, job);
            break;
        case QueryAck:
            numReplies++;
            queryAck((QueryAckMessage *)inboundMessage, toolChannel, job);
            break;
        case UpdateAck:
            numReplies++;
            updateAck((UpdateAckMessage *)inboundMessage, job);
            break;
        case Notify:
            notify((NotifyMessage *)inboundMessage, toolChannel, job);
            break;
        case ErrorAck:
            LOG_ERROR_MSG(toString(msghdr->type) << " returnCode=" << msghdr->returnCode << " errorCode=" << msghdr->errorCode);
            break;
        case DetachAck:
            numReplies++;
            detachAck((DetachAckMessage*)inboundMessage, toolChannel, job);
            break;
        default:
            LOG_ERROR_MSG(toString(msghdr->type) << " message was ignored");
            break;
    }
    
    return;
}

void attachAck(AttachAckMessage *inMsg, bgcios::LocalStreamSocketPtr toolChannel, JobPtr job)
{
    // Check the return code.
    if (inMsg->header.returnCode != bgcios::Success) {
        LOG_ERROR_MSG("Attach message failed: " << bgcios::returnCodeToString(inMsg->header.returnCode));
        return;
    }
    attachMap[toolChannel] = *inMsg;
    job->addCoreFile(inMsg);
}

void detachAck(DetachAckMessage *inMsg, bgcios::LocalStreamSocketPtr toolChannel, JobPtr job)
{
    // Check the return code.
    if (inMsg->header.returnCode != bgcios::Success) {
        LOG_ERROR_MSG("Dettach message failed: " << bgcios::returnCodeToString(inMsg->header.returnCode));
        return;
    }
    job->toolChannels.remove(toolChannel->getSd());
    validPollInfo = false;
    return;
}

void queryAck(QueryAckMessage *inMsg, bgcios::LocalStreamSocketPtr toolChannel, JobPtr job)
{
    // Check the return code.
    if (inMsg->header.returnCode != bgcios::Success) {
        LOG_ERROR_MSG("Query message failed: " << bgcios::returnCodeToString(inMsg->header.returnCode));
        return;
    }
    
    // Get the core file for the rank.
    CoreFilePtr coreFile = job->coreFiles.get(inMsg->header.rank);
    
    // Handle each command in the message.
    for (uint16_t index = 0; index < inMsg->numCommands; ++index) {
        switch (inMsg->cmdList[index].type) {
            case GetProcessDataAck:
            {
                // Set the process data.
                GetProcessDataAckCmd *cmd = (GetProcessDataAckCmd *)((char *)inMsg + inMsg->cmdList[index].offset);
                coreFile->setProcessData(cmd);
                LOG_TRACE_MSG("Set process data for rank " << inMsg->header.rank);
                break;
            }
                
            case GetThreadListAck:
            {
                coreFile->clearThreads();
                
                // Create a CoreFileThread object for each thread in the list.
                GetThreadListAckCmd *cmd = (GetThreadListAckCmd *)((char *)inMsg + inMsg->cmdList[index].offset);
                LOG_TRACE_MSG(cmd->numthreads << " threads in thread list");
                for (uint32_t index = 0; index < cmd->numthreads; ++index) {
                    CoreFileThreadPtr thread = CoreFileThreadPtr(new CoreFileThread());
                    thread->setThreadId(cmd->threadlist[index].tid);
                    coreFile->addThread(thread);
                    LOG_TRACE_MSG("Added thread " << thread->getThreadId() << " to the core file for rank " << inMsg->header.rank);
                }
                break;
            }
                
            case GetThreadDataAck:
            {
                GetThreadDataAckCmd *cmd = (GetThreadDataAckCmd *)((char *)inMsg + inMsg->cmdList[index].offset);
                CoreFileThreadPtr thread = coreFile->getThreadById(cmd->threadID);
                thread->setThreadData(cmd);
                LOG_TRACE_MSG("Added data for thread " << cmd->threadID << " to the core file for rank " << inMsg->header.rank);
                break;
            }
                
            case GetGeneralRegsAck:
            {
                GetGeneralRegsAckCmd *cmd = (GetGeneralRegsAckCmd *)((char *)inMsg + inMsg->cmdList[index].offset);
                CoreFileThreadPtr thread = coreFile->getThreadById(cmd->threadID);
                thread->setGeneralRegs(cmd);
                LOG_TRACE_MSG("Added gprs for thread " << cmd->threadID << " to the core file for rank " << inMsg->header.rank);
                break;
            }
                
            case GetFloatRegsAck:
            {
                GetFloatRegsAckCmd *cmd = (GetFloatRegsAckCmd *)((char *)inMsg + inMsg->cmdList[index].offset);
                CoreFileThreadPtr thread = coreFile->getThreadById(cmd->threadID);
                thread->setFloatRegs(cmd);
                LOG_TRACE_MSG("Added fprs for thread " << cmd->threadID << " to the core file for rank " << inMsg->header.rank);
                break;
            }
                
            case GetSpecialRegsAck:
            {
                GetSpecialRegsAckCmd *cmd = (GetSpecialRegsAckCmd *)((char *)inMsg + inMsg->cmdList[index].offset);
                CoreFileThreadPtr thread = coreFile->getThreadById(cmd->threadID);
                thread->setSpecialRegs(cmd);
                LOG_TRACE_MSG("Added sprs for thread " << cmd->threadID << " to the core file for rank " << inMsg->header.rank);
                break;
            }
                
            case GetDebugRegsAck:
            {
                GetDebugRegsAckCmd *cmd = (GetDebugRegsAckCmd *)((char *)inMsg + inMsg->cmdList[index].offset);
                CoreFileThreadPtr thread = coreFile->getThreadById(cmd->threadID);
                thread->setDebugRegs(cmd);
                LOG_TRACE_MSG("Added dbrs for thread " << cmd->threadID << " to the core file for rank " << inMsg->header.rank);
                break;
            }
                
            default:
                LOG_ERROR_MSG("Command " << inMsg->cmdList[index].type << " in QueryAck message is not supported");
                break;
        }
    }
    return;
}

void
updateAck(UpdateAckMessage *inMsg, JobPtr job)
{
    // Check the return code.
    if (inMsg->header.returnCode != bgcios::Success) {
        LOG_ERROR_MSG("Update message failed: " << bgcios::returnCodeToString(inMsg->header.returnCode));
        return;
    }
    
    return;
}

void
notify(NotifyMessage *inMsg, bgcios::LocalStreamSocketPtr toolChannel, JobPtr job)
{
    switch (inMsg->notifyMessageType) {
            
        case NotifyMessageType_Signal:
        {
            LOG_DEBUG_MSG("Notify message for rank " << inMsg->header.rank << " says thread " << inMsg->type.signal.threadID << " received signal " << inMsg->type.signal.signum);
            break;
        }
        
        case NotifyMessageType_Termination:
        {
            if (WIFEXITED(inMsg->type.termination.exitStatus)) {
                LOG_DEBUG_MSG("Notify message for rank " << inMsg->header.rank << " says process exited with status " << WEXITSTATUS(inMsg->type.termination.exitStatus));
            }
            else if (WIFSIGNALED(inMsg->type.termination.exitStatus)) {
                LOG_DEBUG_MSG("Notify message for rank " << inMsg->header.rank << " says process ended by signal " << WTERMSIG(inMsg->type.termination.exitStatus));
            }
            else {
                LOG_ERROR_MSG("Notify message for rank " << inMsg->header.rank << " contains invalid exit status 0x" << std::hex << inMsg->type.termination.exitStatus);
            }
            break;
        }
            
        default:
            LOG_ERROR_MSG("Notify message contains invalid type " << (uint32_t)inMsg->notifyMessageType);
            break;
    }
    
    return;
}


//! \brief  Send a message to the control daemon.
//! \param  msg Message to send to control daemon.
//! \param  toolChannel Socket connected to control daemon.
//! \return Nothing.

void 
sendMessage(ToolMessage& msg, bgcios::LocalStreamSocketPtr toolChannel)
{
    // Make sure the tool channel is still open.
    if (toolChannel == NULL) {
        return;
    }
    
    // Send the message header and message body together.
    bgcios::MessageHeader *msghdr = &msg.header;
    try {
        toolChannel->send(msghdr, msghdr->length);
        numSentMessages++;
        LOG_DEBUG_MSG(toString(msghdr->type) << " message sent on tool data channel"); 
    } 
    catch (bgcios::SocketError& e) {
        LOG_ERROR_MSG("error sending message on tool data channel: " << e.what());
        return;
    }
    catch (bgcios::SocketClosed& e) {
        LOG_WARN_MSG("tool data channel closed when sending message: " << e.what());
        return;
    }
    catch (bgcios::SocketTimeout& e) {
        LOG_WARN_MSG("timeout sending message on tool data channel: " << e.what());
        return;
    }
    
    return;
}

//! \brief  Receive a message from the control daemon.
//! \param  msg Message received from control daemon.
//! \return Nothing.

bool receiveMessage(ToolMessage *msg, JobPtr job, bgcios::LocalStreamSocketPtr toolChannel)
{
    // Make sure the tool channel is still open.
    if (toolChannel == NULL) {
        return false;
    }
    
    // Receive the message header.
    bgcios::MessageHeader *msghdr = &(msg->header);
    try {
        toolChannel->recv(msghdr, sizeof(bgcios::MessageHeader));
    }
    catch (bgcios::SocketError& e) {
        LOG_ERROR_MSG("error receiving message header from data channel: " << e.what());
        return false;
    }
    catch (bgcios::SocketClosed& e) {
        LOG_WARN_MSG("data channel closed when receiving message header: " << e.what());
        job->toolChannels.remove(toolChannel->getSd());
        validPollInfo = false;
        return false;
    }
    catch (bgcios::SocketTimeout& e) {
        LOG_WARN_MSG("timeout receiving message header from data channel: " << e.what());
        return false;
    }
    
    // Receive the message body if needed.
    if (msghdr->length > sizeof(bgcios::MessageHeader)) {
        LOG_TRACE_MSG(toolChannel->getBytesAvailable() << " bytes available on data channel for message body");
        char *msgbody = (char *)msg + sizeof(bgcios::MessageHeader);
        try {
            toolChannel->recv(msgbody, bgcios::dataLength(msghdr));
        }
        catch (bgcios::SocketError& e) {
            LOG_ERROR_MSG("error receiving message body from data channel: " << e.what());
            return false;
        }
        catch (bgcios::SocketClosed& e) {
            LOG_WARN_MSG("data channel closed when receiving message body: " << e.what());
            job->toolChannels.remove(toolChannel->getSd());
            validPollInfo = false;
            return false;
        }
        catch (bgcios::SocketTimeout& e) {
            LOG_WARN_MSG("timeout receiving message body from data channel: " << e.what());
            return false;
        }
    }
    
    return true;
}

int pollForData(JobPtr job)
{
    int numExpectedReplies = numSentMessages;
    
    numReplies = 0;
    do 
    {
        if(validPollInfo == false)
        {
            numFds = 0;
            for (Job::channel_list_iterator iter = job->toolChannels.begin(); iter != job->toolChannels.end(); ++iter, ++numFds)
            {
                pollInfo[numFds].fd = iter->second->getSd();
                pollInfo[numFds].events = POLLIN;
                pollInfo[numFds].revents = 0;
            }
            validPollInfo = true;
        }
        
        // Wait for an event on one of the descriptors.
        int rc = poll(pollInfo, numFds, 10000);
        
        // There was no data so try again.
        if (rc == 0) {
            break;
            continue;
        } 
        
        // If interrupted, log the failure and try again.
        if (rc == -1) {
            int err = errno;
            if (err == EINTR) {
                LOG_TRACE_MSG("poll returned EINTR, continuing ...");
                continue;
            }
            
            LOG_ERROR_MSG("poll failed: " << bgcios::errorString(err));
            exit(1);
        } 
        
        // Handle a message from every descriptor that has one available.
        for (int index = 0; index < numFds; ++index) {
            if (pollInfo[index].revents & POLLIN) {
                LOG_TRACE_MSG("Input event available on fd " << pollInfo[index].fd);
                messageHandler(job->toolChannels.get(pollInfo[index].fd), job);
            }
        }
    } while(numReplies != numExpectedReplies);
    numSentMessages = 0;
}
