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

#include "Agent.h"
#include "MasterConnection.h"
#include "SignalHandler.h"

#include "common/BinaryController.h"

#include "lib/exceptions.h"

#include <utility/include/ExitStatus.h>

#include <boost/asio/io_service.hpp>

#include <grp.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


LOG_DECLARE_FILE( "master" );

Agent::Agent(
        const bgq::utility::Properties::ConstPtr& props
        ) :
    _uid_mutex(),
    _user_list(),
    _hostname( boost::asio::ip::host_name() ),
    _properties( props ),
    _buffered_messages_mutex(),
    _buffered_messages()
{
    // Nothing to do
}

void
Agent::start(
        const bgq::utility::PortConfiguration::Pairs& ports
        )
{
    _host = _hostname;
    LOG_INFO_MSG("Agent starting on " << _hostname.uhn() << ".");
    const BGAgentId id(0, _host.ip());
    _agent_id = id;

    // Be who I should be!
    if (getuid() != 0) {
        LOG_WARN_MSG("Not running as user id 'root'. Will not be able to change user ids.");
    } else {
        // I'm root, so find who I'm supposed to be!
        std::string my_name = "root";
        try {
            my_name = _properties->getValue("master.agent", "agentuid");
        } catch (const std::invalid_argument& e) {
            LOG_WARN_MSG("No agent userid " << e.what());
        }

        if (_user_list.empty())
            _user_list = "bgqadmin,bgqsysdb,bgws";

        if (_user_list.find(my_name) == std::string::npos) {
            LOG_ERROR_MSG("Invalid user id " << my_name << ". Not in list of valid users: " << _user_list);
            exit(EXIT_FAILURE);
        }

        LOG_INFO_MSG("Switching user id to " << my_name << " in user list " << _user_list);

        try {
            const bgq::utility::UserId uid( my_name );
            const gid_t my_gid = uid.getGroups().front().first;
            if (setegid(my_gid) != 0) {
                char errorText[256];
                LOG_ERROR_MSG("Cannot change effective gid to " << my_gid << ": " << std::string(strerror_r(errno, errorText, 256)));
                exit(EXIT_FAILURE);
            }
            // Extract gid_t from each group returned in the UserId object
            std::vector<gid_t> groups;
            BOOST_FOREACH( const bgq::utility::UserId::Group& i, uid.getGroups() ) {
                groups.push_back( i.first );
                LOG_DEBUG_MSG( "secondary group: " << i.second << " (" << i.first << ")" );
            }
            // Assuming the storage of a std::vector is contiguous memory
            if ( setgroups(groups.size(), &groups[0]) < 0 ) {
                char errorText[256];
                LOG_ERROR_MSG("Could not set secondary groups of this process: " << std::string(strerror_r(errno, errorText, 256)));
                exit( EXIT_FAILURE );
            }

            if (seteuid(uid.getUid()) != 0) { // Make the change
                char errorText[256];
                LOG_ERROR_MSG("Cannot change effective uid to " << uid.getUid() << ": " << std::string(strerror_r(errno, errorText, 256)));
                exit(EXIT_FAILURE);
            }
        } catch ( const std::exception& e ) {
            LOG_ERROR_MSG( "Invalid user id '" << my_name << "' specified for bgagentd, exiting.");
            exit( EXIT_FAILURE );
         }
    }

    boost::asio::io_service io_service;
    const SignalHandler::Ptr signalHandler(
            SignalHandler::create( io_service )
            );
    MasterConnection::create( io_service, ports, this );

    while ( 1 ) {
        try {
            io_service.run();

            // getting here means we received a signal, fall through
            break;
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( "Uncaught exception: " << e.what() );
            io_service.reset();
            sleep(5);
        }
    }

    // signal should be non-zero, if not something is wrong
    BOOST_ASSERT( signalHandler->getSignal() );
    this->doEndAgentRequest( signalHandler->getSignal() );
}

BGMasterAgentProtocolSpec::JoinRequest
Agent::build_join_request(
       const std::string& hostaddr,
       int servname
       ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    // For each running binary, add it to the join request.
    BGMasterAgentProtocolSpec::JoinRequest joinreq(hostaddr, servname, "agent", _host.uhn());
    BOOST_FOREACH( const BinaryControllerPtr& pbin, this->get_binaries() ) {
        const BGMasterAgentProtocolSpec::JoinRequest::WorkingBins bin(
                pbin->get_binid().str(),
                pbin->get_binary_bin_path(),
                pbin->get_alias_name(),
                pbin->get_user(),
                BinaryController::status_to_string(pbin->get_status())
                );
        joinreq._running_binaries.push_back(bin);
    }
    return joinreq;
}

int
Agent::join(
        const bgq::utility::PortConfiguration::Pair& port
        )
{
    LOG_TRACE_MSG(__FUNCTION__);

    const AgentProtocolPtr p(new AgentProtocol);

    try {
        // If [master.agent] section has multiple hosts defined we need to round-robin trying to connect,
        // otherwise if only one host is defined try to connect indefinitely

        p->initializeRequester(_properties, AF_UNSPEC, port.first, port.second);
        LOG_INFO_MSG("Connected to bgmaster_server on " << port.first << ":" << port.second);

        // fall through
    } catch (const CxxSockets::Error& err) {
        return errno;
    }

    _prot = p;
    CxxSockets::SockAddr localsockaddr;
    _prot->getRequester()->getSockName(localsockaddr);

    // This socket will listen for a connection back from bgmaster.
    // It'll be on the same IP address as the requester.
    const CxxSockets::SockAddr sa(static_cast<unsigned short>(localsockaddr.family()), "", "");
    typedef CxxSockets::ListeningSocketPtr Listener;
    const Listener listener(new CxxSockets::ListeningSocket(sa, 1));

    // Need to send back the info for our listener
    CxxSockets::SockAddr requestersockaddr;
    CxxSockets::SockAddr listenersockaddr;
    _prot->getRequester()->getSockName(requestersockaddr);
    listener->getSockName(listenersockaddr);

    // Note, this can throw an exception and dump us out of join processing if the hostname is unroutable.
    const CxxSockets::Host h(requestersockaddr.getHostAddr());
    _host = h;

    // The join request will be formulated with the local IP address of the requester
    // and the local port of the listener.  This is to ensure that the master has a
    // routable IP address (not loopback) and the correct port to connect back to.
    const BGMasterAgentProtocolSpec::JoinRequest joinreq(
            build_join_request(
                requestersockaddr.getHostAddr(),
                listenersockaddr.getServicePort()
                )
            );

    BGMasterAgentProtocolSpec::JoinReply joinrep;
    joinrep._rc = exceptions::OK;

    LOG_DEBUG_MSG(
            "Sending join request from " << requestersockaddr.getHostAddr() << ":" << listenersockaddr.getServicePort() <<
            " to " << port.first << ":" << port.second
            );
    _prot->join(joinreq, joinrep);

    if (joinrep._rc != 0) {
        LOG_ERROR_MSG("Join request denied by bgmaster_server. " << joinrep._rt);
        exit(EXIT_FAILURE);
    }

    LOG_DEBUG_MSG("Group joined");

    // Now look at the join reply to see if we need to kill any binaries
    BOOST_FOREACH(const std::string& badbin, joinrep._bad_bins) {
        const BinaryId bid(badbin);
        BinaryControllerPtr ptr;
        if (find_binary(bid, ptr)) {
            // Only one at a time because we are switching uids
            boost::mutex::scoped_lock lock(_uid_mutex);
            ptr->stop(SIGTERM);
            LOG_INFO_MSG("Stopped binary id " << bid.str());
            // Now take it out of the list
            this->removeController( ptr );
        } else {
            LOG_WARN_MSG("Binary id " << bid.str() << " not found, may have already ended.");
        }
    }

    // We've joined the group.  Now set up our responder.  We've created
    // the listener, so we need to sit on an accept until the master
    // connects back.  At that point, we are completely part of the cluster.
    const CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);
    listener->AcceptNew(sock);
    bgq::utility::ServerPortConfiguration port_config(0, bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeOnly);
    port_config.setProperties(_properties, "");
    port_config.notifyComplete();
    const CxxSockets::SecureTCPSocketPtr secure(
            new CxxSockets::SecureTCPSocket(
                sock,
                port_config
                )
            );
    LOG_INFO_MSG("Got a new connection from bgmaster_server.");
    _prot->initializeResponder(secure);

    // Spawn a thread to send any old buffered up messages so that we can immediately handle
    // incoming requests without deadlock.
    boost::thread t(&Agent::sendBuffered, this);
    t.detach();

    return 0;
}

void
Agent::sendBuffered()
{
    std::list<MsgBasePtr> buffered_messages;
    {
        boost::mutex::scoped_lock lock( _buffered_messages_mutex );
        std::swap( buffered_messages, _buffered_messages );
    }

    LOG_DEBUG_MSG("Sending " << buffered_messages.size() << " buffered messages to bgmaster_server.");
    BOOST_FOREACH(const MsgBasePtr& curr_msg, buffered_messages) {
        // we support failed and complete request messages here, everything else is dropped
        const boost::shared_ptr<BGMasterAgentProtocolSpec::FailedRequest> failed(
                boost::dynamic_pointer_cast<BGMasterAgentProtocolSpec::FailedRequest>(curr_msg)
                );
        const boost::shared_ptr<BGMasterAgentProtocolSpec::CompleteRequest> complete(
                boost::dynamic_pointer_cast<BGMasterAgentProtocolSpec::CompleteRequest>(curr_msg)
                );
        if ( failed ) {
            BGMasterAgentProtocolSpec::FailedReply failrep;
            failrep._rc = exceptions::OK;

            try {
                _prot->failed( *failed, failrep );
            } catch (const CxxSockets::SoftError& err) {
                char errorText[256];
                std::ostringstream msg;
                msg << "Connection to bgmaster_server failed: " << std::string(strerror_r(errno, errorText, 256));
                throw exceptions::CommunicationError(exceptions::INFO, msg.str());
            } catch (const CxxSockets::Error& err) {
                char errorText[256];
                std::ostringstream msg;
                msg << "Connection to bgmaster_server ended: " << std::string(strerror_r(errno, errorText, 256));
                throw exceptions::CommunicationError(exceptions::WARN, msg.str());
            }
            LOG_DEBUG_MSG("Sent buffered failed request for " << failed->_status._binary_id);
        } else if ( complete ) {
            BGMasterAgentProtocolSpec::CompleteReply comprep;
            comprep._rc = exceptions::OK;

            try {
                _prot->complete( *complete, comprep );
            } catch (const CxxSockets::SoftError& err) {
                char errorText[256];
                std::ostringstream msg;
                msg << "Connection to bgmaster_server failed: " << std::string(strerror_r(errno, errorText, 256));
                throw exceptions::CommunicationError(exceptions::INFO, msg.str());
            } catch (const CxxSockets::Error& err) {
                char errorText[256];
                std::ostringstream msg;
                msg << "Connection to bgmaster_server ended: " << std::string(strerror_r(errno, errorText, 256));
                throw exceptions::CommunicationError(exceptions::WARN, msg.str());
            }
            LOG_DEBUG_MSG("Sent buffered complete request for " << complete->_status._binary_id);
        } else {
            LOG_WARN_MSG( "Ignoring unknown message type" );
        }
    }
}

void
Agent::processStartRequest(
       const BGMasterAgentProtocolSpec::StartRequest& startreq
       )
{
    LOG_TRACE_MSG(__FUNCTION__);

    // We're happy to start as many instances of this binary as we're asked.
    // If only one is wanted, the client needs to enforce that.
    LOG_INFO_MSG("Received start request for alias " << startreq._alias << ".");

    const std::string log = startreq._logdir + "/" + _hostname.uhn() + "-" + startreq._alias + ".log";
    LOG_DEBUG_MSG(
            "Start request path=" << startreq._path <<
            (startreq._arguments.empty() ? " " : " " + startreq._arguments) <<
            "logdir=" << log << " user=" << startreq._user
            );

    const BinaryControllerPtr bin(
            new BinaryController(
                startreq._path,
                startreq._arguments,
                log,
                startreq._alias,
                _host,
                startreq._user
                )
            );
    this->addController(bin);

    BinaryId bid;
    const BGMasterAgentProtocolSpec::StartReply::BinaryStatus binstat(bid, 0);
    BGMasterAgentProtocolSpec::StartReply rep(0, "start successful", binstat);
    rep._status = binstat;
    rep._rc = exceptions::OK;

    try {
        boost::mutex::scoped_lock lock(_uid_mutex);
        bid = bin->startBinary(
                _user_list,
                _properties->getFilename()
                );
        if (bid.get_pid() == -1) {
            // There's an error, we've returned once and handled it.
            // This time we just bail.
            // sendreply = false;

            // FIXME fall through?
        }
    } catch (const exceptions::FileError& e) {
        LOG_ERROR_MSG("Exec failed with a file error: " << e.what());
        rep._rc = e.errcode;
        rep._rt = e.what();
    }

    rep._status._binary_id = bid.str();

    if (bid.get_pid() > 0) {
        // Don't create the current symlink now that we're using logrotate.
    } else {
        // Failed to start
        LOG_ERROR_MSG("Failed to start binary: " << bin->get_error_text());
        rep._rc = exceptions::WARN;
        rep._rt = bin->get_error_text();
    }

    try {
        LOG_DEBUG_MSG("Sending start reply");
        _prot->sendReply(rep.getClassName(), rep);
        LOG_DEBUG_MSG("Start reply sent for " << bid.str() << "|" << bin->get_binary_bin_path() << ": " << rep._rt);
    } catch (const CxxSockets::SoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_WARN_MSG("Connection to bgmaster_server interrupted while sending start reply in method " <<  __FUNCTION__);
        return;
    } catch (const CxxSockets::Error& err) {
        // Server aborted with an incomplete transmission
        LOG_WARN_MSG("Connection to bgmaster_server ended while sending start reply in method " << __FUNCTION__);
        // FIXME fall through?
    }

    int exit_status = 0;
    int child_pid = bid.get_pid();
    if (child_pid > 0)
        waitpid(child_pid, &exit_status, 0);

    bool dont_report = bin->stopping();
    if (_ending)
        dont_report = true;

    // Now we need to remove the binary because we're done.
    LOG_DEBUG_MSG("Removing binary " << bin);
    this->removeController( bin );

    if (!WIFEXITED(exit_status) || exit_status != EXIT_SUCCESS) {
        // Failed!  Let master know.
        LOG_INFO_MSG("Binary id " << bid.str() << " alias " << bin->get_alias_name() << " ended with " << bgq::utility::ExitStatus( exit_status ));
        if (dont_report) {
            return;
        }

        const BGMasterAgentProtocolSpec::FailedRequest::BinaryStatus binstat(bid.str(), exit_status);
        const BGMasterAgentProtocolSpec::FailedRequest failreq(binstat);
        BGMasterAgentProtocolSpec::FailedReply failrep;
        failrep._rc = exceptions::OK;

        try {
            LOG_DEBUG_MSG("Sending ending request for alias " << bin->get_alias_name() << " binary id " << bid.str());
            _prot->failed(failreq, failrep);
        } catch (const CxxSockets::SoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Connection to bgmaster_server interrupted while sending ending request for alias " << bin->get_alias_name() << " in method " <<  __FUNCTION__);
            return;
        } catch (const CxxSockets::Error& err) {
            // Server aborted with an incomplete transmission
            LOG_WARN_MSG("Connection to bgmaster_server ended while sending ending request for alias " << bin->get_alias_name() << " in method " <<  __FUNCTION__);
            const MsgBasePtr bp(new BGMasterAgentProtocolSpec::FailedRequest(binstat));
            boost::mutex::scoped_lock lock( _buffered_messages_mutex );
            _buffered_messages.push_back(bp);
        }
    } else {
        if (child_pid > 0 && !dont_report) {
            // Only send a complete message if we haven't already sent a start failure.
            const BGMasterAgentProtocolSpec::CompleteRequest::BinaryStatus binstat(bid.str(), "COMPLETED");
            const BGMasterAgentProtocolSpec::CompleteRequest exereq(binstat, exit_status);
            BGMasterAgentProtocolSpec::CompleteReply exerep;
            exerep._rc = exceptions::OK;

            LOG_INFO_MSG(
                    "Sending complete request for " << bin->get_alias_name() <<
                    " binary id " << bid.str() << " with exit status of " << exit_status
                    );
            try {
                _prot->complete(exereq, exerep);
            } catch (const CxxSockets::SoftError& err) {
                // For soft errors, we just back out and let it try again
                LOG_WARN_MSG("Connection to bgmaster_server interrupted while sending complete request for alias in method " <<  __FUNCTION__);
                return;
            } catch (const CxxSockets::Error& err) {
                // Server aborted with an incomplete transmission
                LOG_WARN_MSG("Connection to bgmaster_server ended while sending complete request for alias in method " <<  __FUNCTION__);
                const MsgBasePtr bp(new BGMasterAgentProtocolSpec::CompleteRequest(binstat, exit_status));
                boost::mutex::scoped_lock lock( _buffered_messages_mutex );
                _buffered_messages.push_back(bp);
            }
        }
    }
}

void
Agent::doStopRequest(
        const BGMasterAgentProtocolSpec::StopRequest& stopreq
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    // Find the binary and nuke it.
    const BinaryId bid(stopreq._binary_id);
    BGMasterAgentProtocolSpec::StopReply stoprep;
    stoprep._rc = exceptions::OK;

    LOG_INFO_MSG("Received stop request for binary id " << stopreq._binary_id);

    BinaryControllerPtr ptr;
    if (find_binary(bid, ptr)) {
        // Only one at a time because we are switching uids
        boost::mutex::scoped_lock lock(_uid_mutex);
        const int stop_sig = ptr->stop(stopreq._signal);
        if (stop_sig == 0) {
            LOG_INFO_MSG("Failed to stop binary id " << bid.str());
            stoprep._rc = exceptions::WARN;
            stoprep._rt = "failed to stop binary";
        }
        stoprep._status._binary_id = bid.str();
        if (ptr->get_exit_status() != 0) {
            stoprep._status._exit_status = ptr->get_exit_status();
        } else {
            stoprep._status._exit_status = stop_sig;
        }
        // Now take it out of the list
        this->removeController( ptr );
    } else {
        LOG_INFO_MSG("Binary id " << bid.str() << " not found, may have previously ended.");
        stoprep._rc = exceptions::INFO;
        stoprep._rt = "Binary " + bid.str() + " not found";
    }

    try {
        LOG_DEBUG_MSG("Sending stop reply");
        _prot->sendReply(stoprep.getClassName(), stoprep);
        LOG_DEBUG_MSG("Sent stop reply for binary id " << bid.str());
    } catch (const CxxSockets::SoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_WARN_MSG("Connection to bgmaster_server interrupted while sending stop reply for binary id " << bid.str()<< " in method " <<  __FUNCTION__);
        return;
    } catch (const CxxSockets::Error& err) {
        // Master aborted with an incomplete transmission
        LOG_WARN_MSG("Connection to bgmaster_server ended while sending stop reply for binary id " << bid.str()<< " in method " <<  __FUNCTION__);
    }
}

void
Agent::doEndAgentRequest(
        const int signal
        )
{
    LOG_TRACE_MSG(__FUNCTION__);

    _ending = true;
    // Kill all binaries.
    LOG_INFO_MSG("Ending bgagentd requested.");
    const Binaries binaries = this->get_binaries();
    for ( Binaries::const_iterator i = binaries.begin(); i != binaries.end(); ++i ) {
        // Only one at a time because we are switching uids
        boost::mutex::scoped_lock lock(_uid_mutex);
        const BinaryControllerPtr ptr = *i;
        ptr->stop(signal);
        this->removeController( ptr );
    }

    exit(0);
}

void
Agent::processRequest()
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::string request_name;

    try {
        // This will wait on a new request from bgmaster_server
        _prot->getName(request_name);
    } catch (const CxxSockets::SoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_WARN_MSG("Connection to bgmaster_server interrupted in method " <<  __FUNCTION__);
        return;
    } catch (const CxxSockets::Error& err) {
        // Server aborted with an incomplete transmission
        LOG_WARN_MSG("Connection to bgmaster_server ended in method " <<  __FUNCTION__ << ". Error is: " << err.what());
        _prot->getResponder().reset();
        return;
    }

    LOG_DEBUG_MSG("Request " << request_name << " received.");

    if (request_name == "StartRequest") {
        // Start request must be threaded because it does a wait on
        // a binary and we may later need to kill that binary.
        BGMasterAgentProtocolSpec::StartRequest startreq;
        try {
            _prot->getObject(&startreq);
        } catch (const CxxSockets::SoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Connection to bgmaster_server interrupted while handling StartRequest in method " <<  __FUNCTION__);
            return;
        } catch (const CxxSockets::Error& err) {
            // Server aborted with an incomplete transmission
            LOG_WARN_MSG("Connection to bgmaster_server ended while handling StartRequest in method " <<  __FUNCTION__);
            return;
        }
        boost::thread startthread(&Agent::processStartRequest, this, startreq);
    } else if (request_name == "StopRequest") {
        BGMasterAgentProtocolSpec::StopRequest stopreq;
        try {
            _prot->getObject(&stopreq);
        } catch (const CxxSockets::SoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Connection to bgmaster_server interrupted while handling StopRequest in method " <<  __FUNCTION__);
            return;
        } catch (const CxxSockets::Error& err) {
            // Server aborted with an incomplete transmission
            LOG_WARN_MSG("Connection to bgmaster_server ended while handling StopRequest in method " <<  __FUNCTION__);
            return;
        }

        doStopRequest(stopreq);
    } else {
        LOG_WARN_MSG("Unknown request: '" << request_name << "'");
    }
}
