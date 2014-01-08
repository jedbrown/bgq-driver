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

#ifndef BGMASTERCLIENTPROTOCOL_H
#define BGMASTERCLIENTPROTOCOL_H

#include "hlcs/include/c_api/BGMasterClientProtocolSpec.h"

#ifndef _SOCKETTYPES_H
#include "SocketTypes.h"
#endif

class BGMasterClientProtocol;

typedef boost::shared_ptr<BGMasterClientProtocol> BGMasterClientProtocolPtr;

//! \brief Basic BGMaster Protocol object for agent communications.  
class BGMasterClientProtocol: public BGMasterProtocol
{
public:
    BGMasterClientProtocol(bgq::utility::Properties::Ptr p) : BGMasterProtocol(p) {}
    ~BGMasterClientProtocol() {}

    /*!
     * Registration message.
     *
     * @param JoinRequest
     *           request object
     * @param JoinReply
     *           reply object
     */
    void join(const BGMasterClientProtocolSpec::JoinRequest& request, BGMasterClientProtocolSpec::JoinReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::JoinRequest::getClassName(), request, BGMasterClientProtocolSpec::JoinReply::getClassName(), reply);
    }

    /*!
     * Start a managed binary message.
     *
     * @param StartRequest
     *           request object
     * @param StartReply
     *           reply object
     */
    void start(const BGMasterClientProtocolSpec::StartRequest& request, BGMasterClientProtocolSpec::StartReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::StartRequest::getClassName(), request, BGMasterClientProtocolSpec::StartReply::getClassName(), reply);
    }


    /*!
     * Wait for a managed binary to complete.
     *
     * @param WaitRequest
     *           request object
     * @param WaitReply
     *           reply object
     */
    void wait(const BGMasterClientProtocolSpec::WaitRequest& request, BGMasterClientProtocolSpec::WaitReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::WaitRequest::getClassName(), request, BGMasterClientProtocolSpec::WaitReply::getClassName(), reply);
    }

    /*!
     * Stop a managed binary message.
     *
     * @param StopRequest
     *           request object
     * @param StopReply
     *           reply object
     */
    void stop(const BGMasterClientProtocolSpec::StopRequest& request, BGMasterClientProtocolSpec::StopReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::StopRequest::getClassName(), request, BGMasterClientProtocolSpec::StopReply::getClassName(), reply);
    }

    /*!
     * Status of managed binaries.
     *
     * @param StatusRequest
     *           request object
     * @param StatusReply
     *           reply object
     */
    void status(const BGMasterClientProtocolSpec::StatusRequest& request, BGMasterClientProtocolSpec::StatusReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::StatusRequest::getClassName(), request, BGMasterClientProtocolSpec::StatusReply::getClassName(), reply);
    }

    /*!
     * Failover a managed binary message.
     *
     * @param StopRequest
     *           request object
     * @param StopReply
     *           reply object
     */
    void failover(const BGMasterClientProtocolSpec::FailoverRequest& request, BGMasterClientProtocolSpec::FailoverReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::FailoverRequest::getClassName(), request, BGMasterClientProtocolSpec::FailoverReply::getClassName(), reply);
    }

    /*!
     * Ask agents to commit suicide.
     *
     * @param End_agentRequest
     *           request object
     * @param End_agentReply
     *           reply object
     */
    void end_agent(const BGMasterClientProtocolSpec::End_agentRequest& request, BGMasterClientProtocolSpec::End_agentReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::End_agentRequest::getClassName(), request, BGMasterClientProtocolSpec::End_agentReply::getClassName(), reply);
    }

    /*!
     * Ask master to commit suicide.
     *
     * @param TerminateRequest
     *           request object
     * @param TerminateReply
     *           reply object
     */
    void terminate(const BGMasterClientProtocolSpec::TerminateRequest& request, BGMasterClientProtocolSpec::TerminateReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::TerminateRequest::getClassName(), request, BGMasterClientProtocolSpec::TerminateReply::getClassName(), reply);
    }

    /*!
     * Get a list of the ids of connected clients.
     *
     * @param ClientsRequest
     *           request object
     * @param ClientsReply
     *           reply object
     */
    void clients(const BGMasterClientProtocolSpec::ClientsRequest& request, BGMasterClientProtocolSpec::ClientsReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::ClientsRequest::getClassName(), request, BGMasterClientProtocolSpec::ClientsReply::getClassName(), reply);
    }

    /*!
     * Get list of agents and their binaries.
     *
     * @param AgentlistRequest
     *           request object
     * @param AgentlistReply
     *           reply object
     */
    void agentlist(const BGMasterClientProtocolSpec::AgentlistRequest& request, BGMasterClientProtocolSpec::AgentlistReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::AgentlistRequest::getClassName(), request, BGMasterClientProtocolSpec::AgentlistReply::getClassName(), reply);
    }

    /*!
     * Reload the config file.
     *
     * @param ReloadRequest
     *           request object
     * @param ReloadReply
     *           reply object
     */
    void reload(const BGMasterClientProtocolSpec::ReloadRequest& request, BGMasterClientProtocolSpec::ReloadReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::ReloadRequest::getClassName(), request, BGMasterClientProtocolSpec::ReloadReply::getClassName(), reply);
    }

    /*!
     * Status of managed binaries.
     *
     * @param Exit_statusRequest
     *           request object
     * @param Exit_statusReply
     *           reply object
     */
    void exit_status(const BGMasterClientProtocolSpec::ExitStatusRequest& request, BGMasterClientProtocolSpec::ExitStatusReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::ExitStatusRequest::getClassName(), request, BGMasterClientProtocolSpec::ExitStatusReply::getClassName(), reply);
    }

    /*!
     * Return the status of bgmaster_server.
     *
     * @param MasterstatRequest
     *           request object
     * @param MasterstatReply
     *           reply object
     */
    void masterstat(const BGMasterClientProtocolSpec::MasterstatRequest& request, BGMasterClientProtocolSpec::MasterstatReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::MasterstatRequest::getClassName(), request, BGMasterClientProtocolSpec::MasterstatReply::getClassName(), reply);
    }

    /*!
     * Wait for a binary associated with an alias to start.
     *
     * @param Alias_waitRequest
     *           request object
     * @param Alias_waitReply
     *           reply object
     */
    void alias_wait(const BGMasterClientProtocolSpec::Alias_waitRequest& request, BGMasterClientProtocolSpec::Alias_waitReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::Alias_waitRequest::getClassName(), request, BGMasterClientProtocolSpec::Alias_waitReply::getClassName(), reply);
    }

    /*!
     * Get contents of bgmaster_server error ring buffer
     *
     * @param Get_errorsRequest
     *           request object
     * @param Get_errorsReply
     *           reply object
     */
    void get_errors(const BGMasterClientProtocolSpec::Get_errorsRequest& request, BGMasterClientProtocolSpec::Get_errorsReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::Get_errorsRequest::getClassName(), request, BGMasterClientProtocolSpec::Get_errorsReply::getClassName(), reply);
    }

    /*!
     * Get contents of bgmaster_server history ring buffer
     *
     * @param Get_historyRequest
     *           request object
     * @param Get_historyReply
     *           reply object
     */
    void get_history(const BGMasterClientProtocolSpec::Get_historyRequest& request, BGMasterClientProtocolSpec::Get_historyReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::Get_historyRequest::getClassName(), request, BGMasterClientProtocolSpec::Get_historyReply::getClassName(), reply);
    }

    /*!
     * Start a monitor of bgmaster_server for new events and errors.
     *
     * @param MonitorRequest
     *           request object
     * @param MonitorReply
     *           reply object
     */
    virtual void monitor(const BGMasterClientProtocolSpec::MonitorRequest& request, BGMasterClientProtocolSpec::MonitorReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::MonitorRequest::getClassName(), request, BGMasterClientProtocolSpec::MonitorReply::getClassName(), reply);
    }
 
   /*!
     * Stop a monitor of bgmaster_server for new events and errors.
     *
     * @param EndmonitorRequest
     *           request object
     * @param EndmonitorReply
     *           reply object
     */
    virtual void endmonitor(const BGMasterClientProtocolSpec::EndmonitorRequest& request, BGMasterClientProtocolSpec::EndmonitorReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::EndmonitorRequest::getClassName(), request, BGMasterClientProtocolSpec::EndmonitorReply::getClassName(), reply);
    }

    /*!
     * Change the logging levels of bgmaster_server.
     *
     * @param Log_levelRequest
     *           request object
     * @param Log_levelReply
     *           reply object
     */
    virtual void loglevel(const BGMasterClientProtocolSpec::LoglevelRequest& request, BGMasterClientProtocolSpec::LoglevelReply& reply)
    {
        sendReceive(BGMasterClientProtocolSpec::LoglevelRequest::getClassName(), request, BGMasterClientProtocolSpec::LoglevelReply::getClassName(), reply);
    }

    /*!
     * Get the aliases that are configured but not running.
     *
     * @param GetidleRequest
     *           request object
     * @param GetidleReply
     *           reply object
     */
    virtual void getidle(const BGMasterClientProtocolSpec::GetidleRequest& request, BGMasterClientProtocolSpec::GetidleReply& reply) 
    {
        sendReceive(BGMasterClientProtocolSpec::GetidleRequest::getClassName(), request, BGMasterClientProtocolSpec::GetidleReply::getClassName(), reply);
    }

};

#endif
