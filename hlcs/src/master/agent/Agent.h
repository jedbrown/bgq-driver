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

class MasterConnection;

//! \brief Class for the concrete implementation.
//! The base class provides the definition common
//! to the master and the agent.  This provides the
//! agent functionality.
class Agent : public AgentBase
{
private:
    BGMasterAgentProtocolSpec::JoinRequest build_join_request(
            const std::string& hostaddr,
            int servname
            ) const;

    void doEndAgentRequest(
            const int signal
            );

    void sendBuffered();

    void processStartRequest(const BGMasterAgentProtocolSpec::StartRequest& startreq);

    void doStopRequest(const BGMasterAgentProtocolSpec::StopRequest& stopreq);

public:
    //! \brief Constructor.
    Agent(
            const bgq::utility::Properties::ConstPtr& props
         );

    //! \brief initiate the registration process
    void start(
            const bgq::utility::PortConfiguration::Pairs& pairs //!< [in]
            );

    //! \brief Set list of users this agent can be.
    void set_users(const std::string& users) { _user_list = users; }

    //! \brief set up communication with BGMaster.
    int join(
            const bgq::utility::PortConfiguration::Pair& port   //!< [in]
            );

    //! \brief Figure out what to do with a new request from the master.
    void processRequest();

    //! \brief Get the host name
    const CxxSockets::Host& get_hostname() const { return _hostname; }

private:
    friend class MasterConnection;
    boost::mutex _uid_mutex;
    std::string _user_list;
    const CxxSockets::Host _hostname;
    const bgq::utility::Properties::ConstPtr _properties;
    boost::mutex _buffered_messages_mutex;
    std::list<MsgBasePtr> _buffered_messages; //! \brief When the master goes down, buffer messages here
};

#endif
