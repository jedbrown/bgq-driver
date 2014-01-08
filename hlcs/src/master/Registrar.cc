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

#include <csignal>
#include <utility/include/portConfiguration/ServerPortConfiguration.h>
#include "BGMasterAgentProtocol.h"
#include "BGAgent.h"
#include "BGAgentBase.h"
#include "BGMaster.h"
#include "Thread.h"
#include "BGMasterExceptions.h"
#include "MasterRasMetadata.h"

LOG_DECLARE_FILE( "master" );

BGMasterController::Registrar BGMasterController::_agent_registrar;
BGMasterController::Registrar BGMasterController::_client_registrar;

using namespace CxxSockets;

BGMasterController::Registrar::Registrar() : _end(false), _failed(false), _my_tid(0) {
}

BGMasterController::Registrar::~Registrar() {
    _my_tid = 0;
}


void BGMasterController::Registrar::processNew(SecureTCPSocketPtr sock) {
    ThreadLog tl("Registrar New Connection");

    LOG_INFO_MSG(__FUNCTION__ << " from " << sock->getUserId().getUser());
    // Will need to read a message on the socket, determine if it is
    // a registration message, and if it is build a BGAgent object
    // and place it in the agent vector.

    // 1)  Create a new BGMasterProtocol object
    BGMasterProtocolPtr prot(new BGMasterProtocol(BGMasterController::_master_props));
    try {
        prot->initializeResponder(sock);
    } catch(CxxSockets::SockHardError& err) {
        // Something bad happended on the reply or connecting back
        std::ostringstream msg;
        msg << "Agent join attempt failed initializing the responder." << err.what();
        BGMasterController::handleErrorMessage(msg.str());
        // We aren't going to get in the list.  We'll just return
        return;
    }

    // 2)  Validate that we got a 'join' message
    std::string requestName = "";
    BGMasterAgentProtocolSpec::JoinRequest requestObject;
    try {
        prot->getName(requestName);
        prot->getObject(&requestObject);
    } catch(CxxSockets::CxxError& err) {
       // Something bad happended on the reply or connecting back
        std::ostringstream msg;
        msg << "Join attempt failed getting the request." << err.what();
        BGMasterController::handleErrorMessage(msg.str());
        // We aren't going to get in the list.  We'll just return
        return;
    }

    if(requestName != "JoinRequest") {
        std::ostringstream os;
        os << "Expected 'JoinRequest' got " << requestName;
        BGMasterController::handleErrorMessage(os.str());
        // Bail out of the thread
        return;
    }

    if(requestObject._initiator == "agent") {
        // 3)  If 2, then ctor a BGAgentRepPtr with our prot.  The
        //     BGAgentRep ctor sends the response
        BGMasterAgentProtocolPtr pt = boost::static_pointer_cast<BGMasterAgentProtocol>(prot);

        BGAgentRepPtr bgagent;

        BGMasterAgentProtocolSpec::JoinReply joinreply;
        joinreply._rc = BGMasterExceptions::OK;

        try {
            BGAgentRepPtr b(new BGAgentRep(pt, requestObject, joinreply));
            bgagent = b;
        } catch(CxxSockets::SockHardError& err) {
            // Something bad happended on the reply or connecting back
            std::ostringstream msg;
            msg << "Agent join attempt failed" << err.what();
            BGMasterController::handleErrorMessage(msg.str());
            // We aren't going to get in the list.  We'll just return
            return;
        } catch(BGMasterExceptions::APIUserError& e) {
            BGMasterController::handleErrorMessage(e.what());
            return;
        }

        LOG_INFO_MSG("new agent at ip address " << requestObject._ip_address << " port " << requestObject._port);

        // 4)  Then put it in the _agents list with AddNew()
        if(_agent_manager.addNew(bgagent) == false) {
            // Failed!
            joinreply._rc = -1;
            joinreply._rt = "Join failed.  Too many agents from this host.";
            pt->sendReply(joinreply.getClassName(), joinreply);
            LOG_INFO_MSG("Sent failed join reply to " << requestObject._ip_address << ":" << requestObject._port);
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
            LOG_INFO_MSG("Sent join reply to agent " << requestObject._ip_address << ":" << requestObject._port);

            // Initialize the requester.
            // This will connect back to the agent's listener.
            try {
                pt->initializeRequester(AF_UNSPEC, requestObject._ip_address,
                                        boost::lexical_cast<std::string>(requestObject._port), "secure");
            } catch(CxxSockets::SockHardError& err) {
                // Something bad happended on the reply or connecting back
                std::ostringstream msg;
                msg << "Agent join attempt failed.  Unable to initialize requesting socket. " << err.what();
                BGMasterController::handleErrorMessage(msg.str());
                // We aren't going to get in the list.  We'll just return
                return;
            }

            // Both sockets are set up, we're happy with the handshaking,
            // so now we'll start polling for new requests.
            bgagent->startPoller();
        }
        // Now we need to see if we need to start anyone automagically.
        // This will happen if we add an agent or server to the cluster
        // and we've got something configured to run on it.
        // We shouldn't have to manually go back and start all
        // of it's processes.
        if(BGMasterController::_start_servers) {
            std::map<std::string, std::string> failed_aliases;
            BGMasterController::startServers(failed_aliases, bgagent);
        }
    } else if(requestObject._initiator == "client") {
        // Create a new client controller which initializes itself
        BGMasterClientProtocolPtr pt = boost::static_pointer_cast<BGMasterClientProtocol>(prot);

        BGMasterController::ClientManager::ClientControllerPtr client;

        try {
            BGMasterController::ClientManager::ClientControllerPtr
                c(new BGMasterController::ClientManager::ClientController(pt, requestObject._ip_address, requestObject._port));
            client = c;
            client->setUserType(sock->getUserType());
        } catch(CxxSockets::SockHardError& err) {
            // Something bad happended on the reply or connecting back
            std::ostringstream msg;
            msg << "Agent join attempt failed" << err.what();
            BGMasterController::handleErrorMessage(msg.str());
            // We aren't going to get in the list.  We'll just return
            return;
        }

        LOG_INFO_MSG("new client at ip address " << requestObject._ip_address << " port " << requestObject._port);
        _client_manager.addClient(client);
    } else {
        // log it and go
        std::ostringstream msg;
        msg << "Protocol error:  Invalid type " << requestObject._initiator;
        BGMasterController::handleErrorMessage(msg.str());
    }
}


void BGMasterController::Registrar::listenForNew(bgq::utility::PortConfiguration::Pairs& portpairs) {
    LOG_INFO_MSG(__FUNCTION__);
    ThreadLog tl("Registrar Listener");
    _my_tid = pthread_self();
    _failed = false;
    _end = false;
    // Will listen on the registration listener then accept new
    // connections.  The new connections will be validated and
    // inserted into the agent vector in a separate thread
    SockAddrList masterlist; // One big list to rule them all!
    BOOST_FOREACH(bgq::utility::PortConfiguration::Pair& curr_pair, portpairs) {
        LOG_DEBUG_MSG("Will listen on " << curr_pair.first << ":" << curr_pair.second);
        try {
            SockAddrList salist(AF_UNSPEC, curr_pair.first, curr_pair.second);
            BOOST_FOREACH(SockAddr& curr_sockaddr, salist) {
                // Now copy every SockAddr in to the master list
                masterlist.push_back(curr_sockaddr);
            }
        } catch(CxxSockets::CxxError& e) {
            std::ostringstream saerror;
            saerror << "Unable to get addr info for " << curr_pair.first << ":" 
                    << curr_pair.second << " " << e.what();
            BGMasterController::handleErrorMessage(saerror.str());
        }
    }

    if(masterlist.size() == 0) {
        // If we can't create a registrar, we can't run.
        std::ostringstream msg;
        msg << "Unable to start a registrar. No valid TCP port/address pairs specified."
            <<  "bgmaster_server exiting.  Check properties file configuration.";
        BGMasterController::handleErrorMessage(msg.str());
        BGMasterController::_master_terminating = true;
        _failed = true;
        BGMasterController::_start_barrier.wait();
        std::map<std::string, std::string> details;
        details["PID"] = boost::lexical_cast<std::string>(getpid());
        details["ERROR"] = msg.str();
        BGMasterController::putRAS(MASTER_CONFIG_RAS, details);
        return;
    }
    
    try {
        CxxSockets::PollingListenerSetPtr ln(new PollingListenerSet(masterlist, SOMAXCONN, CxxSockets::SECURE));
        _registrationListener = ln;
    } catch (CxxSockets::CxxError& e) {
        // If we can't create a registrar, we can't run.
        std::ostringstream msg;
        msg << "Unable to start a registrar.  " << e.what() << "  bgmaster_server exiting.";
        BGMasterController::handleErrorMessage(msg.str());
        BGMasterController::_master_terminating = true;
        _failed = true;
        BGMasterController::_start_barrier.wait();
        return;
    }

    // Listener has started, signal the barrier
    BGMasterController::_start_barrier.wait();

    while(!_end) {
        bool accepted = false;
        while(!accepted && !_end) {
            SecureTCPSocketPtr sock(new SecureTCPSocket(CxxSockets::SECURE, CxxSockets::CRYPTALL));
            accepted = false;
            try {
                // Take either admin or command pc.
                bgq::utility::ServerPortConfiguration port_config(0, bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeCommand);
                port_config.setProperties(BGMasterController::_master_props, "");
                port_config.notifyComplete();
                accepted = _registrationListener->AcceptNew(sock, port_config);
            } catch (CxxSockets::SockHardError& e) {
                if(e.errcode != CxxSockets::SSL_ERROR) {
                    std::ostringstream msg;
                    msg << "Registrar listener failed " << e.what() << ".  Restarting.";
                    BGMasterController::handleErrorMessage(msg.str());
                    _end = true;
                    _failed = true;
                    //! Need to figure out if the listener or new client socket failed
                    //! and act accordingly.
                    return;
                } else {
                    // An SSL_ERROR means the child handshake failed but the server is OK.
                    LOG_WARN_MSG("Child connection failed SSL handshake. " << e.what());
                    continue;
                }
            } catch (CxxSockets::SockSoftError& e) {
                LOG_DEBUG_MSG("Accept processing failed.  Retrying. " << e.what());
                continue;  // Just try again
            }

            if(accepted) {
                LOG_INFO_MSG("Accepted a new connection");
                boost::thread procthread(&BGMasterController::Registrar::processNew, this, sock);
            }
        }
    }
}

void BGMasterController::Registrar::cancel() {
    LOG_INFO_MSG(__FUNCTION__ << " " << _my_tid);
    _end = true;
    pthread_kill(_my_tid, SIGUSR1);
    _listenerThread.join();
}

void BGMasterController::Registrar::run(bool agent) {
    // This starts our listener thread
    LOG_INFO_MSG(__FUNCTION__);
    std::string port;
    std::string host;
    bgq::utility::PortConfiguration::Pairs portpairs;

    std::string servname;
    std::string port_type;
    if(agent) {
        servname = "32041";
        port_type = "agent";
    } else {
        servname = "32042";
        port_type = "client";
    }

    try {
        bgq::utility::Properties::Section master_sect = _master_props->getValues("master.server");
        bgq::utility::ServerPortConfiguration port_config(servname, port_type, port_type);
        port_config.setProperties( _master_props, "master.server");
        port_config.notifyComplete();
        portpairs = port_config.getPairs();
    } catch(std::invalid_argument& e) {
        if(portpairs[0].first.length() == 0) {
            std::ostringstream failmsg;
            failmsg << "No port pairs or invalid port pairs specified";
            LOG_FATAL_MSG(failmsg.str() << e.what());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::FATAL, failmsg.str());
        }
    }

    _listenerThread = boost::thread(&BGMasterController::Registrar::listenForNew, this, portpairs);
}
