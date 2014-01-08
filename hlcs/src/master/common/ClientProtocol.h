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

#ifndef MASTER_CLIENT_PROTOCOL_H_
#define MASTER_CLIENT_PROTOCOL_H_

#include "Protocol.h"

#include "protocol/BGMasterClientProtocolSpec.h"


//! \brief Basic BGMaster Protocol object for client communications.  
class ClientProtocol : public Protocol
{
public:
    ClientProtocol() : Protocol() {}

    /*!
     * Registration message.
     */
    void join(const BGMasterClientProtocolSpec::JoinRequest& request, BGMasterClientProtocolSpec::JoinReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::JoinRequest::getClassName(), request, BGMasterClientProtocolSpec::JoinReply::getClassName(), reply);
    }

    /*!
     * Start a managed binary message.
     */
    void start(const BGMasterClientProtocolSpec::StartRequest& request, BGMasterClientProtocolSpec::StartReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::StartRequest::getClassName(), request, BGMasterClientProtocolSpec::StartReply::getClassName(), reply);
    }

    /*!
     * Wait for a managed binary to complete.
     */
    void wait(const BGMasterClientProtocolSpec::WaitRequest& request, BGMasterClientProtocolSpec::WaitReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::WaitRequest::getClassName(), request, BGMasterClientProtocolSpec::WaitReply::getClassName(), reply);
    }

    /*!
     * Stop a managed binary message.
     */
    void stop(const BGMasterClientProtocolSpec::StopRequest& request, BGMasterClientProtocolSpec::StopReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::StopRequest::getClassName(), request, BGMasterClientProtocolSpec::StopReply::getClassName(), reply);
    }

    /*!
     * Status of managed binaries.
     */
    void status(const BGMasterClientProtocolSpec::StatusRequest& request, BGMasterClientProtocolSpec::StatusReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::StatusRequest::getClassName(), request, BGMasterClientProtocolSpec::StatusReply::getClassName(), reply);
    }

    /*!
     * Failover a managed binary message.
     */
    void failover(const BGMasterClientProtocolSpec::FailoverRequest& request, BGMasterClientProtocolSpec::FailoverReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::FailoverRequest::getClassName(), request, BGMasterClientProtocolSpec::FailoverReply::getClassName(), reply);
    }

    /*!
     * Ask master to end
     */
    void terminate(const BGMasterClientProtocolSpec::TerminateRequest& request, BGMasterClientProtocolSpec::TerminateReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::TerminateRequest::getClassName(), request, BGMasterClientProtocolSpec::TerminateReply::getClassName(), reply);
    }

    /*!
     * Get a list of the ids of connected clients.
     */
    void clients(const BGMasterClientProtocolSpec::ClientsRequest& request, BGMasterClientProtocolSpec::ClientsReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::ClientsRequest::getClassName(), request, BGMasterClientProtocolSpec::ClientsReply::getClassName(), reply);
    }

    /*!
     * Get list of agents and their binaries.
     */
    void agentlist(const BGMasterClientProtocolSpec::AgentlistRequest& request, BGMasterClientProtocolSpec::AgentlistReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::AgentlistRequest::getClassName(), request, BGMasterClientProtocolSpec::AgentlistReply::getClassName(), reply);
    }

    /*!
     * Reload the config file.
     */
    void reload(const BGMasterClientProtocolSpec::ReloadRequest& request, BGMasterClientProtocolSpec::ReloadReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::ReloadRequest::getClassName(), request, BGMasterClientProtocolSpec::ReloadReply::getClassName(), reply);
    }

    /*!
     * Return the status of bgmaster_server.
     */
    void masterstat(const BGMasterClientProtocolSpec::MasterstatRequest& request, BGMasterClientProtocolSpec::MasterstatReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::MasterstatRequest::getClassName(), request, BGMasterClientProtocolSpec::MasterstatReply::getClassName(), reply);
    }

    /*!
     * Wait for a binary associated with an alias to start.
     */
    void alias_wait(const BGMasterClientProtocolSpec::Alias_waitRequest& request, BGMasterClientProtocolSpec::Alias_waitReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::Alias_waitRequest::getClassName(), request, BGMasterClientProtocolSpec::Alias_waitReply::getClassName(), reply);
    }

    /*!
     * Get contents of bgmaster_server error ring buffer
     */
    void get_errors(const BGMasterClientProtocolSpec::Get_errorsRequest& request, BGMasterClientProtocolSpec::Get_errorsReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::Get_errorsRequest::getClassName(), request, BGMasterClientProtocolSpec::Get_errorsReply::getClassName(), reply);
    }

    /*!
     * Get contents of bgmaster_server history ring buffer
     */
    void get_history(const BGMasterClientProtocolSpec::Get_historyRequest& request, BGMasterClientProtocolSpec::Get_historyReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::Get_historyRequest::getClassName(), request, BGMasterClientProtocolSpec::Get_historyReply::getClassName(), reply);
    }

    /*!
     * Start a monitor of bgmaster_server for new events and errors.
     */
    virtual void monitor(const BGMasterClientProtocolSpec::MonitorRequest& request, BGMasterClientProtocolSpec::MonitorReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::MonitorRequest::getClassName(), request, BGMasterClientProtocolSpec::MonitorReply::getClassName(), reply);
    }
 
    /*!
     * Stop a monitor of bgmaster_server for new events and errors.
     */
    virtual void endmonitor(const BGMasterClientProtocolSpec::EndmonitorRequest& request, BGMasterClientProtocolSpec::EndmonitorReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::EndmonitorRequest::getClassName(), request, BGMasterClientProtocolSpec::EndmonitorReply::getClassName(), reply);
    }

    /*!
     * Change the logging levels of bgmaster_server.
     */
    virtual void loglevel(const BGMasterClientProtocolSpec::LoglevelRequest& request, BGMasterClientProtocolSpec::LoglevelReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::LoglevelRequest::getClassName(), request, BGMasterClientProtocolSpec::LoglevelReply::getClassName(), reply);
    }

    /*!
     * Get the aliases that are configured but not running.
     */
    virtual void getidle(const BGMasterClientProtocolSpec::GetidleRequest& request, BGMasterClientProtocolSpec::GetidleReply& reply) 
    {
        sendReceive(BGMasterClientProtocolSpec::GetidleRequest::getClassName(), request, BGMasterClientProtocolSpec::GetidleReply::getClassName(), reply);
    }
};

#endif
