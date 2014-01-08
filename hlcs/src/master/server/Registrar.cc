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

#include "AgentManager.h"
#include "AgentRep.h"
#include "ClientController.h"
#include "ClientManager.h"
#include "MasterController.h"
#include "Registrar.h"
#include "ras.h"

#include "../lib/exceptions.h"


#include <utility/include/cxxsockets/ListenerSet.h>
#include <utility/include/cxxsockets/SockAddrList.h>

#include <boost/foreach.hpp>


#include <pthread.h>
#include <signal.h>

LOG_DECLARE_FILE( "master" );

Registrar::Registrar() : _end(false), _failed(false), _my_tid(0)
{
    // Nothing to do
}

Registrar::~Registrar()
{
    _my_tid = 0;
}

void
Registrar::processNew(
        CxxSockets::TCPSocketPtr sock
        )
{
    bgq::utility::ServerPortConfiguration port_config(0, bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeCommand);
    port_config.setProperties(MasterController::getProps(), "");
    port_config.notifyComplete();

    CxxSockets::SecureTCPSocketPtr secure;
    try {
        secure.reset(
                new CxxSockets::SecureTCPSocket(
                    sock,
                    port_config
                    )
                );
    } catch (const CxxSockets::Error& e) {
        LOG_ERROR_MSG( e.what() );
        return;
    }

    LOG_TRACE_MSG("From " << secure->getUserId().getUser());
    // Will need to read a message on the socket, determine if it is
    // a registration message, and if it is build a BGAgent object
    // and place it in the agent vector.

    // 1) Create a new Protocol object
    const ProtocolPtr prot(new Protocol);
    try {
        prot->initializeResponder(secure);
    } catch (const CxxSockets::HardError& err) {
        // Something bad happened on the reply or connecting back
        std::ostringstream msg;
        msg << "Agent join attempt failed initializing the responder. " << err.what();
        MasterController::handleErrorMessage(msg.str());
        // We aren't going to get in the list.  We'll just return
        return;
    }

    // 2) Validate that we got a 'join' message
    std::string requestName;
    BGMasterAgentProtocolSpec::JoinRequest requestObject;
    try {
        prot->getName(requestName);
        prot->getObject(&requestObject);
    } catch (const CxxSockets::Error& err) {
        // Something bad happened on the reply or connecting back
        std::ostringstream msg;
        msg << "Join attempt failed getting the request. " << err.what();
        MasterController::handleErrorMessage(msg.str());
        // We aren't going to get in the list.  We'll just return
        return;
    }

    if (requestName != "JoinRequest") {
        std::ostringstream os;
        os << "Expected 'JoinRequest' got " << requestName;
        MasterController::handleErrorMessage(os.str());
        // Bail out of the thread
        return;
    }

    if (requestObject._initiator == "agent") {
        // 3) If 2, then ctor a AgentRepPtr with our prot. The AgentRep ctor sends the response.
        const AgentProtocolPtr pt( new AgentProtocol );
        pt->initializeResponder( secure );

        AgentRepPtr bgagent;

        BGMasterAgentProtocolSpec::JoinReply joinreply;
        joinreply._rc = exceptions::OK;

        try {
            const AgentRepPtr b(new AgentRep(pt, requestObject, joinreply));
            bgagent = b;
        } catch (const CxxSockets::HardError& err) {
            // Something bad happened on the reply or connecting back
            std::ostringstream msg;
            msg << "Agent join attempt failed. " << err.what();
            MasterController::handleErrorMessage(msg.str());
            // We aren't going to get in the list.  We'll just return
            return;
        } catch (const exceptions::APIUserError& e) {
            MasterController::handleErrorMessage(e.what());
            return;
        }

        LOG_INFO_MSG("Agent connected on IP address " << requestObject._ip_address << " port " << requestObject._port << ".");

        // 4) Then put it in the _agents list
        if (MasterController::get_agent_manager().addNew(bgagent) == false) {
            // Failed!
            joinreply._rc = -1;
            joinreply._rt = "Join failed. Too many agents from this host.";
            pt->sendReply(joinreply.getClassName(), joinreply);
            LOG_DEBUG_MSG("Sent failed join reply to " << requestObject._ip_address << ":" << requestObject._port);
        } else {
            // Send a Join reply indicating that we are satisfied the handshaking is complete.
            // With secure sockets, we can't send back our client info for the return connection
            // because we can't build it until after we've replied.
            joinreply._rc = 0;
            joinreply._rt = "join successful";
            joinreply._agent_ip = requestObject._ip_address;
            CxxSockets::SockAddr myaddr;
            prot->getResponder()->getSockName(myaddr);
            joinreply._master_ip = myaddr.getHostAddr();
            joinreply._master_port = boost::lexical_cast<std::string>(requestObject._port);

            pt->sendReply(joinreply.getClassName(), joinreply);
            LOG_DEBUG_MSG("Sent join reply to agent " << requestObject._ip_address << ":" << requestObject._port);

            // Initialize the requester. This will connect back to the agent's listener.
            try {
                pt->initializeRequester(
                        MasterController::getProps(), 
                        AF_UNSPEC, 
                        requestObject._ip_address, 
                        boost::lexical_cast<std::string>(requestObject._port)
                        );
            } catch (const CxxSockets::HardError& err) {
                // Something bad happened on the reply or connecting back
                std::ostringstream msg;
                msg << "Agent join attempt failed. Unable to initialize requesting socket. " << err.what();
                MasterController::handleErrorMessage(msg.str());
                // We aren't going to get in the list.  We'll just return
                return;
            }

            // Both sockets are set up, we're happy with the handshaking,
            // so now we'll start polling for new requests.
            bgagent->startPoller();
        }
        // Now we need to see if we need to start anyone automagically.
        // This will happen if we add an agent or server to the cluster and we've got something configured to run on it.
        // We shouldn't have to manually go back and start all of it's processes.
        if (MasterController::isStartServers()) {
            std::map<std::string, std::string> failed_aliases;
            MasterController::startServers(failed_aliases, bgagent);
        }
    } else if (requestObject._initiator == "client") {
        // Create a new client controller which initializes itself
        const ClientProtocolPtr pt( new ClientProtocol );
        pt->initializeResponder( secure );

        ClientControllerPtr client;

        try {
            client.reset(new ClientController(pt, requestObject._ip_address, requestObject._port, secure->getUserType()));
        } catch (const CxxSockets::HardError& err) {
            // Something bad happended on the reply or connecting back
            std::ostringstream msg;
            msg << "Agent join attempt failed. " << err.what();
            MasterController::handleErrorMessage(msg.str());
            // We aren't going to get in the list.  We'll just return
            return;
        }

        LOG_DEBUG_MSG(
                "Client (" << secure->getUserId().getUser() << ") connected at IP address " <<
                requestObject._ip_address << " port " << requestObject._port << "."
                );
        MasterController::get_client_manager().addClient(client);
    } else {
        // Log it and go
        std::ostringstream msg;
        msg << "Protocol error: Invalid type " << requestObject._initiator;
        MasterController::handleErrorMessage(msg.str());
    }
}

void
Registrar::listenForNew(
        const bgq::utility::PortConfiguration::Pairs& portpairs
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    _my_tid = pthread_self();
    _failed = false;
    _end = false;
    // Will listen on the registration listener then accept new
    // connections.  The new connections will be validated and
    // inserted into the agent vector in a separate thread.
    CxxSockets::SockAddrList masterlist; // One big list to rule them all!
    BOOST_FOREACH(const bgq::utility::PortConfiguration::Pair& curr_pair, portpairs) {
        LOG_DEBUG_MSG("Listening on " << curr_pair.first << ":" << curr_pair.second);
        try {
            CxxSockets::SockAddrList salist(AF_UNSPEC, curr_pair.first, curr_pair.second);
            BOOST_FOREACH(const CxxSockets::SockAddr& curr_sockaddr, salist) {
                // Now copy every SockAddr in to the master list
                masterlist.push_back(curr_sockaddr);
            }
        } catch (const CxxSockets::Error& e) {
            std::ostringstream saerror;
            saerror << "Unable to get address info for " << curr_pair.first << ":" << curr_pair.second << " " << e.what();
            MasterController::handleErrorMessage(saerror.str());
        }
    }

    if (masterlist.size() == 0) {
        // If we can't create a registrar, we can't run.
        std::ostringstream msg;
        msg << "Unable to start a registrar, bgmaster_server is ending. No valid TCP port/address pairs specified. Check bg.properties file configuration.";
        MasterController::handleErrorMessage(msg.str());
        MasterController::set_master_terminating();
        _failed = true;
        MasterController::waitStartBarrier();
        std::map<std::string, std::string> details;
        details["PID"] = boost::lexical_cast<std::string>(getpid());
        details["ERROR"] = msg.str();
        MasterController::putRAS(MASTER_CONFIG_RAS, details);
        return;
    }

    try {
        const CxxSockets::ListenerSetPtr ln(new CxxSockets::ListenerSet(masterlist, SOMAXCONN));
        _registrationListener = ln;
    } catch (const CxxSockets::Error& e) {
        // If we can't create a registrar, we can't run.
        std::ostringstream msg;
        msg << "Unable to start a registrar, bgmaster_server is ending. Error is: " << e.what();
        MasterController::handleErrorMessage(msg.str());
        MasterController::set_master_terminating();
        _failed = true;
        MasterController::waitStartBarrier();
        return;
    }

    // Listener has started, signal the barrier
    MasterController::waitStartBarrier();

    while (!_end) {
        bool accepted = false;
        while (!accepted && !_end) {
            const CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
            try {
                accepted = _registrationListener->AcceptNew(sock);
            } catch (const CxxSockets::HardError& e) {
                std::ostringstream msg;
                msg << "Registrar listener failed, restarting. Error is: " << e.what();
                MasterController::handleErrorMessage(msg.str());
                _end = true;
                _failed = true;
                //! Need to figure out if the listener or new client socket failed and act accordingly.
                return;
            } catch (const CxxSockets::SoftError& e) {
                LOG_DEBUG_MSG("Accept processing failed, retrying. Error is: " << e.what());
                continue;  // Just try again
            }

            if (accepted) {
                LOG_DEBUG_MSG("Accepted a new connection");
                boost::thread procthread(&Registrar::processNew, this, sock);
            }
        }
    }
}

void
Registrar::cancel()
{
    LOG_TRACE_MSG(__FUNCTION__ << " " <<  "0x" << std::hex << _my_tid);
    _end = true;
    if ( _my_tid ) {
        pthread_kill(_my_tid, SIGUSR1);
    }
    _listenerThread.join();
}

void
Registrar::run(
        bool agent
        )
{
    // This starts our listener thread
    LOG_TRACE_MSG(__FUNCTION__);
    bgq::utility::PortConfiguration::Pairs portpairs;

    const std::string servname = agent ? "32041" : "32042";
    const std::string port_type = agent ? "agent" : "client";

    try {
        bgq::utility::ServerPortConfiguration port_config(servname, port_type, port_type);
        port_config.setProperties( MasterController::getProps(), "master.server");
        port_config.notifyComplete();
        portpairs = port_config.getPairs();
    } catch (const std::invalid_argument& e) {
        if (portpairs[0].first.length() == 0) {
            std::ostringstream failmsg;
            failmsg << "No port pairs or invalid port pairs specified. ";
            LOG_FATAL_MSG(failmsg.str() << e.what());
            throw exceptions::ConfigError(exceptions::FATAL, failmsg.str());
        }
    }

    _listenerThread = boost::thread(&Registrar::listenForNew, this, portpairs);
}
