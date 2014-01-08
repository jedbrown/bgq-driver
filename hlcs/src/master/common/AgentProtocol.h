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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef MASTER_AGENT_PROTOCOL_H_
#define MASTER_AGENT_PROTOCOL_H_


#include "Protocol.h"

#include "protocol/BGMasterAgentProtocolSpec.h"

//! \brief Basic BGMaster Protocol object for agent communications.  
class AgentProtocol : public Protocol
{
public:
    AgentProtocol() : Protocol() {}

    /*!
     * Registration message.
     */
    void join(const BGMasterAgentProtocolSpec::JoinRequest& request, BGMasterAgentProtocolSpec::JoinReply& reply)
    {
        sendReceive(BGMasterAgentProtocolSpec::JoinRequest::getClassName(), request, BGMasterAgentProtocolSpec::JoinReply::getClassName(), reply);
    }

    /*!
     * Start a managed binary message.
     */
    void start(const BGMasterAgentProtocolSpec::StartRequest& request, BGMasterAgentProtocolSpec::StartReply& reply)
    {
        sendReceive(BGMasterAgentProtocolSpec::StartRequest::getClassName(), request, BGMasterAgentProtocolSpec::StartReply::getClassName(), reply);
    }

    /*!
     * Stop a managed binary message.
     */
    void stop(const BGMasterAgentProtocolSpec::StopRequest& request, BGMasterAgentProtocolSpec::StopReply& reply)
    {
        sendReceive(BGMasterAgentProtocolSpec::StopRequest::getClassName(), request, BGMasterAgentProtocolSpec::StopReply::getClassName(), reply);
    }

    /*!
     * Status of managed binaries.
     */
    void status(const BGMasterAgentProtocolSpec::StatusRequest& request, BGMasterAgentProtocolSpec::StatusReply& reply)
    {
        sendReceive(BGMasterAgentProtocolSpec::StatusRequest::getClassName(), request, BGMasterAgentProtocolSpec::StatusReply::getClassName(), reply);
    }

    /*!
     * Tell bgmaster that binary(ies) completed.
     */
    virtual void complete(const BGMasterAgentProtocolSpec::CompleteRequest& request, BGMasterAgentProtocolSpec::CompleteReply& reply)
    {
        sendReceive(BGMasterAgentProtocolSpec::CompleteRequest::getClassName(), request, BGMasterAgentProtocolSpec::CompleteReply::getClassName(), reply);
    }

    /*!
     * Tell bgmaster that binary(ies) failed.
     */
    virtual void failed(const BGMasterAgentProtocolSpec::FailedRequest& request, BGMasterAgentProtocolSpec::FailedReply& reply)
    {
        sendReceive(BGMasterAgentProtocolSpec::FailedRequest::getClassName(), request, BGMasterAgentProtocolSpec::FailedReply::getClassName(), reply);
    }
};

#endif
