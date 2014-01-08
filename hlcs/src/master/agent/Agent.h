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

#ifndef MASTER_AGENT_H_
#define MASTER_AGENT_H_


#include "common/AgentBase.h"
#include "common/AgentProtocol.h"
#include "common/types.h"

#include <utility/include/Properties.h>

#include <utility/include/cxxsockets/ListeningSocket.h>

#include <utility/include/portConfiguration/PortConfiguration.h>

#include <boost/thread.hpp>

#include <list>
#include <string>

//! \brief Class for the concrete implementation.
//! The base class provides the definition common
//! to the master and the agent.  This provides the
//! agent functionality.
class Agent : public AgentBase
{
private:
    //! \brief Socket for listening for master to connect back.
    CxxSockets::ListeningSocketPtr _masterListener;

    //! \brief When the master goes down, buffer messages here
    std::list<MsgBasePtr> _buffered_messages;

    BGMasterAgentProtocolSpec::JoinRequest build_join_request(
            const std::string& hostaddr,
            int servname) const;

protected:
    //! \brief Send messages buffered while the master connection is down.
    void sendBuffered();

    //! \brief Received a start request from the master, act on it.
    void processStartRequest(const BGMasterAgentProtocolSpec::StartRequest& startreq);

    //! \brief Stop the binary requested by the master.
    void doStopRequest(const BGMasterAgentProtocolSpec::StopRequest& stopreq);

    //! \brief Master wants the agent to die.
    //! \param signal The signal that was sent to bgagent, to propagate to all the running binaries..
    void doEndAgentRequest(const int signal);

public:
    //! \brief Constructor.
    //! This will initiate the registration process
    Agent();

    //! \brief Set the pairs used to connect to bgmaster_server.
    void set_pairs(const bgq::utility::PortConfiguration::Pairs& pairs) { _portpairs = pairs; }

    //! \brief Set list of users this agent can be.
    void set_users(const std::string& users) { _user_list = users; }

    //! \brief Start the main bgagent process.
    void startup(const bgq::utility::Properties::ConstPtr& props);

    //! \brief set up communication with BGMaster.
    void join();

    //! \brief Poll the socket for new messages from bgmaster
    void waitMessages();

    //! \brief Figure out what to do with a new request from the master.
    void processRequest();

    //! \brief Clean this up and die
    void cleanup(const int signal);

    //! \brief Get the host name
    const CxxSockets::Host& get_hostname() const { return _hostname; }

private:
    boost::mutex _uid_mutex;
    std::string _user_list;
    bgq::utility::PortConfiguration::Pairs _portpairs;
    const CxxSockets::Host _hostname;
    bgq::utility::Properties::ConstPtr _properties;
};

#endif
