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

#ifndef MASTER_AGENT_REP_H_
#define MASTER_AGENT_REP_H_

#include "common/AgentBase.h"
#include "common/AgentProtocol.h"
#include "common/ClientProtocol.h"
#include "common/Ids.h"
#include "types.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include <iosfwd>
#include <string>

#include <pthread.h>


//! \brief Representation of a bgagent internal to bgmaster_server
//! Also available to the client api.
class AgentRep : public AgentBase, public boost::enable_shared_from_this<AgentRep>
{
    //! \brief allows agent to fail over its processes to another
    bool _failover;

    //! \brief bgmaster is ending this agent in an orderly fashion.
    bool _orderly;

    //! \brief Thread to wait for requests
    boost::thread _agent_socket_poller;

    pthread_t _my_tid;

    boost::mutex _agent_mutex;

    void doCompleteRequest(
            const BGMasterAgentProtocolSpec::CompleteRequest& compreq
            );

    void doFailedRequest(
            const BGMasterAgentProtocolSpec::FailedRequest& failreq
            );

    //! These functions are non-locking.  Functions that call them must lock the agent object!
    void executePolicyAndClear_nl(
            BinaryControllerPtr binptr, 
            AgentRepPtr rep_p, 
            int signo = 0
            );

    //! \brief Just execute the policy, don't clear.
    void executePolicy_nl(
            const BinaryId& reqbid, AgentRepPtr rep_p,
            AliasPtr al, 
            int signo = 0
            );

    BinaryId startBin_nl(
            const BGMasterAgentProtocolSpec::StartRequest& startreq,
            BGMasterAgentProtocolSpec::StartReply& startrep
            );

    void stopBin_nl(
            const BinaryId& bid,
            const BinaryLocation& binloc,
            const int signal,
            BGMasterAgentProtocolSpec::StopReply& stoprep,
            bool failover
            );

    void agentAbend(
            const std::ostringstream& msg
            );

public:
    AgentRep(
            AgentProtocolPtr p,
            const BGMasterAgentProtocolSpec::JoinRequest& joinreq,
            BGMasterAgentProtocolSpec::JoinReply& joinrep
            );

    ~AgentRep();

    //! process incoming =agent= requests
    //! \returns true if a soft error
    bool processRequest();

    void waitMessages();

    void startPoller();

    //! \brief Stop a binary and execute a policy for it.  If there is no policy,
    //!        this method will fail with an exception.
    void stopBinaryAndExecutePolicy(
            const BinaryId& bid,
            const BinaryLocation& binloc,
            const int signal,
            BGMasterAgentProtocolSpec::StopReply& stoprep,
            const std::string& trigger
            );

    BinaryId startBin(
            const BGMasterAgentProtocolSpec::StartRequest& startreq,
            BGMasterAgentProtocolSpec::StartReply& startrep
            );

    void stopBin(
            const BinaryId& bid,
            const BinaryLocation& binloc,
            int signal,
            BGMasterAgentProtocolSpec::StopReply& stoprep,
            bool failover
            );

    //! \brief Stop all binaries running on this agent.  Locking.
    void stopAllBins(
            BGMasterClientProtocolSpec::StopReply& stoprep, 
            int signal
            );

    //! \brief Cancel polling thread processing and optionally end the agent process.
    //! \param binaries Also end the managed binaries.
    //! \param signal Signal to use to end the binaries.
    void cancel(
            bool binaries, 
            int signal
            );

    size_t binCount() const { return this->get_binaries().size(); }
};


#endif
