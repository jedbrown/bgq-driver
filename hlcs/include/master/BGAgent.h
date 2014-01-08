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

#ifndef _BGAGENT_H
#define _BGAGENT_H

#include <tr1/memory>
#include <string>
#include <vector>
#include <boost/thread.hpp>
#include "Properties.h"
#include "BGAgentBase.h"
#include "BinaryController.h"
#include "BGMasterAgentProtocol.h"

// Forward declares
class BGMasterAgentProtocol;

typedef boost::shared_ptr<XML::Serializable> MsgBasePtr;

//! \brief Class for the concrete implementation.
//! The base class provides the definition common
//! to the master and the agent.  This provides the
//! agent functionality.
class BGAgent : public BGAgentBase {

    //! \brief Socket for listening for master to connect back.
    CxxSockets::ListeningSocketPtr _masterListener;
    std::vector<ThreadPtr> _all_threads;

    //! \brief When the master goes down, buffer messages here
    std::list<MsgBasePtr> _buffered_messages;

    BGMasterAgentProtocolSpec::JoinRequest build_join_request(
                                             std::string hostaddr,
                                             int servname);
    static boost::mutex _uid_mutex;
protected:
    //! \brief Send messages buffered while the master connection is down.
    void sendBuffered();

    //! \brief Received a start request from the master, act on it.
    void processStartRequest(BGMasterAgentProtocolSpec::StartRequest& startreq);

    //! \brief Stop the binary requested by the master.
    void doStopRequest(BGMasterAgentProtocolSpec::StopRequest& stopreq);

    //! \brief Master wants the agent to die.
    //! \param diereq The die request from bgmaster_server
    //! \param reply_to_master If the request is internal and not from bgmaster, don't reply.
    void doEndAgentRequest(BGMasterAgentProtocolSpec::End_agentRequest& diereq, bool reply_to_master);
public:
    //! \brief Constructor.
    //! This will initiate the registration process
    BGAgent();

    //! \brief Destructor joins any and all threads.
    ~BGAgent();

    //! \brief Set the pairs used to connect to bgmaster_server.
    static void set_pairs(bgq::utility::PortConfiguration::Pairs pairs) { _portpairs = pairs; }

    //! \brief Set list of users this agent can be.
    static void set_users(std::string users) { _user_list = users; }

    //! \brief Start the main bgagent process.
    void startup(bgq::utility::Properties::Ptr& props);

    //! \brief set up communication with BGMaster.
    void join();

    //! \brief Poll the socket for new messages from bgmaster
    void waitMessages();

    //! \brief Figure out what to do with a new request from the master.
    void processRequest();

    //! \brief Clean this up and die
    void cleanup();

    static boost::mutex& getUidLock() { return _uid_mutex; }
    static std::string _user_list;
    static bgq::utility::PortConfiguration::Pairs _portpairs;
};

typedef boost::shared_ptr<BGAgent> BGAgentPtr;

#endif
