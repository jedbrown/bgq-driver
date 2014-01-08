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

#include "common/BinaryController.h"
#include "common/Ids.h"
#include "common/ThreadLog.h"

#include "lib/exceptions.h"

#include <utility/include/Exec.h>
#include <utility/include/ExitStatus.h>
#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include <utility/include/cxxsockets/Host.h>

#include <utility/include/portConfiguration/ServerPortConfiguration.h>

#include <boost/asio/ip/host_name.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>

#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

#include <errno.h>
#include <grp.h>
#include <pthread.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


LOG_DECLARE_FILE( "master" );

Agent::Agent() :
    _uid_mutex(),
    _user_list(),
    _portpairs(),
    _hostname( boost::asio::ip::host_name() ),
    _properties()
{
    // Nothing to do
}

void
Agent::startup(
       bgq::utility::Properties::Ptr props
       )
{
    LOG_TRACE_MSG(__FUNCTION__);
    _host = _hostname;
    LOG_INFO_MSG("Agent starting on " << _hostname.uhn() << ".");
    BGAgentId id(0, _host.ip());
    _agent_id = id;

    _properties = props;

    // Be who I should be!
    if (getuid() != 0) {
        LOG_WARN_MSG("Not running as user id 'root'. Will not be able to change user ids.");
    } else {
        // I'm root, so find who I'm supposed to be!
        std::string my_name = "root";
        try {
            my_name = _properties->getValue("master.agent", "agentuid");
        } catch(std::invalid_argument& e) {
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

    // If WaitMessages returns, that means that the connection to bgmaster failed.  We're going to retry joining
    while (true) {
        // Join the "cluster"
        try {
            join();
        } catch (const std::exception& e) {
            LOG_WARN_MSG("Problem joining the cluster, will retry, error is: " << e.what());
            sleep(1);
            continue;
        }

        // We're joined to the agent cluster...
        _ending = false; // so we aren't ending...
        waitMessages();  // and we should wait for new messages.
    }
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
    BOOST_FOREACH( BinaryControllerPtr pbin, _binaries ) {
        BGMasterAgentProtocolSpec::JoinRequest::WorkingBins
            bin(pbin->get_binid().str(),
                pbin->get_binary_bin_path(),
                pbin->get_alias_name(),
                pbin->get_user(),
                BinaryController::status_to_string(pbin->get_status()));
        joinreq._running_binaries.push_back(bin);
    }
    return joinreq;
}

void
Agent::join()
{
    LOG_TRACE_MSG(__FUNCTION__);

    typedef CxxSockets::ListeningSocketPtr Listener;

    AgentProtocolPtr p(new AgentProtocol(_properties));
    _prot = p;

    std::string connected_host = "";
    std::string connected_port = "";
    bool failed = true;

    // If [master.agent] section has multiple hosts defined we need to round-robin trying to connect,
    // otherwise if only one host is defined try to connect forever.
    int connectAttempts = 9; // This will try and connect to a host for about 30 secs before trying next host
    if (_portpairs.size() == 1) {
        connectAttempts = 0; // Only one bgmaster_server host listener defined so try to connect to it forever
    }

    while (failed) {
        BOOST_FOREACH(bgq::utility::PortConfiguration::Pair portpair, _portpairs) {
            LOG_INFO_MSG("Attempting to connect to bgmaster_server on " << portpair.first << ":" << portpair.second);
            try {
                // If [master.agent] section has multiple hosts defined we need to round-robin trying to connect,
                // otherwise if only one host is defined try to connect indefinitely

                _prot->initializeRequester(AF_UNSPEC, portpair.first, portpair.second, connectAttempts);
                connected_host = portpair.first;
                connected_port = portpair.second;
                LOG_INFO_MSG("Connected to bgmaster_server on " << portpair.first << ":" << portpair.second);
                failed = false;
                break;
            } catch (CxxSockets::SockSoftError& err) {
                char errorText[256];
                LOG_WARN_MSG("Connection to bgmaster_server failed on " << portpair.first << ":" << portpair.second << " - " << std::string(strerror_r(errno, errorText, 256)));
            } catch (CxxSockets::CxxError& err) {
                char errorText[256];
                LOG_WARN_MSG("Connection to bgmaster_server failed on " << portpair.first << ":" << portpair.second << " - " << std::string(strerror_r(errno, errorText, 256)));
            }
            sleep(1);
        }
    }

    CxxSockets::SockAddr localsockaddr;
    _prot->getRequester()->getSockName(localsockaddr);

    // This socket will listen for a connection back from bgmaster.
    // It'll be on the same IP address as the requester.
    CxxSockets::SockAddr sa(static_cast<unsigned short>(localsockaddr.family()), "", "");
    Listener ln(new CxxSockets::ListeningSocket(sa, 1));
    _masterListener = ln;

    // Need to send back the info for our listener
    CxxSockets::SockAddr requestersockaddr;
    CxxSockets::SockAddr listenersockaddr;
    _prot->getRequester()->getSockName(requestersockaddr);
    _masterListener->getSockName(listenersockaddr);

    // Note, this can throw an exception and dump us out of join processing if the hostname is unroutable.
    CxxSockets::Host h(requestersockaddr.getHostAddr());
    _host = h;

    // The join request will be formulated with the local IP address of the requester
    // and the local port of the listener.  This is to ensure that the master has a
    // routable IP address (not loopback) and the correct port to connect back to.
    BGMasterAgentProtocolSpec::JoinRequest joinreq =
        build_join_request(requestersockaddr.getHostAddr(), listenersockaddr.getServicePort());

    BGMasterAgentProtocolSpec::JoinReply joinrep;
    joinrep._rc = exceptions::OK;

    LOG_DEBUG_MSG("Sending join request from " << requestersockaddr.getHostAddr() << ":" << listenersockaddr.getServicePort()
                 << " to " << connected_host << ":" << connected_port);
    _prot->join(joinreq, joinrep);

    if (joinrep._rc != 0) {
        LOG_ERROR_MSG("Join request denied by bgmaster_server. " << joinrep._rt);
        exit(EXIT_FAILURE);
    }

    LOG_DEBUG_MSG("Group joined");

    // Now look at the join reply to see if we need to kill any binaries
    BOOST_FOREACH(std::string badbin, joinrep._bad_bins) {
        BinaryId bid(badbin);
        BinaryControllerPtr ptr;
        if (find_binary(bid, ptr)) {
            // Only one at a time because we are switching uids
            boost::mutex::scoped_lock lock(_uid_mutex);
            ptr->stopBinary(SIGTERM);
            LOG_INFO_MSG("Stopped binary id " << bid.str());
            // Now take it out of the list
            _binaries.erase(std::remove(_binaries.begin(),_binaries.end(), ptr), _binaries.end());
        } else {
            LOG_WARN_MSG("Binary id " << bid.str() << " not found, may have already ended.");
        }
    }

    // We've joined the group.  Now set up our responder.  We've created
    // the listener, so we need to sit on an accept until the master
    // connects back.  At that point, we are completely part of the cluster.
    CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket);
    bool accepted = false;
    while (!accepted) {
        try {
            bgq::utility::ServerPortConfiguration port_config(0, bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeOnly);
            port_config.setProperties(_properties, "");
            port_config.notifyComplete();
            accepted  = _masterListener->AcceptNew(sock, port_config);
        } catch (std::runtime_error& e) {
            // punt and try joining again
            throw std::runtime_error(
                    std::string(e.what()) + " errno: " + boost::lexical_cast<std::string>(errno)
                    );
        }
        if (accepted) {
            LOG_INFO_MSG("Got a new connection from bgmaster_server.");
            _prot->initializeResponder(sock);
            break;
        }
    }

    // Spawn a thread to send any old buffered up messages so that we can immediately handle
    // incoming requests without deadlock.
    boost::thread startthread(&Agent::sendBuffered, this);
}

void
Agent::sendBuffered()
{
    ThreadLog tl(__FUNCTION__);
    // Send buffered messages
    LOG_DEBUG_MSG("Sending " << _buffered_messages.size() << " buffered messages to bgmaster_server.");
    BOOST_FOREACH(MsgBasePtr& curr_msg, _buffered_messages) {
        if (_buffered_messages.empty()) break;
        // Inelegant, but should work.
        std::string classname;
        classname = boost::static_pointer_cast<BGMasterAgentProtocolSpec::FailedRequest>(curr_msg)->getClassName();
        if (classname == "FailedRequest") {
            BGMasterAgentProtocolSpec::FailedReply failrep;
            failrep._rc = exceptions::OK;

            try {
                _prot->failed(*(boost::static_pointer_cast
                                <BGMasterAgentProtocolSpec::FailedRequest>(curr_msg).get()), failrep);
            } catch (CxxSockets::SockSoftError& err) {
                char errorText[256];
                std::ostringstream msg;
                msg << "Connection to bgmaster_server failed: " << std::string(strerror_r(errno, errorText, 256));
                throw exceptions::CommunicationError(exceptions::INFO, msg.str());
            } catch (CxxSockets::CxxError& err) {
                char errorText[256];
                std::ostringstream msg;
                msg << "Connection to bgmaster_server ended: " << std::string(strerror_r(errno, errorText, 256));
                throw exceptions::CommunicationError(exceptions::WARN, msg.str());
            }
            LOG_DEBUG_MSG("Sent buffered failed request.");
        } else if (classname == "CompleteRequest") {
            BGMasterAgentProtocolSpec::CompleteReply comprep;
            comprep._rc = exceptions::OK;

            try {
                _prot->complete(*(boost::static_pointer_cast
                                  <BGMasterAgentProtocolSpec::CompleteRequest>(curr_msg).get()), comprep);
            } catch (CxxSockets::SockSoftError& err) {
                char errorText[256];
                std::ostringstream msg;
                msg << "Connection to bgmaster_server failed: " << std::string(strerror_r(errno, errorText, 256));
                throw exceptions::CommunicationError(exceptions::INFO, msg.str());
            } catch (CxxSockets::CxxError& err) {
                char errorText[256];
                std::ostringstream msg;
                msg << "Connection to bgmaster_server ended: " << std::string(strerror_r(errno, errorText, 256));
                throw exceptions::CommunicationError(exceptions::WARN, msg.str());
            }
            LOG_DEBUG_MSG("Sent buffered complete request.");
        }
        // Must remove this message from the list
        _buffered_messages.erase(std::remove(_buffered_messages.begin(),_buffered_messages.end(), curr_msg), _buffered_messages.end());
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
    ThreadLog tl("Start Request");

    LOG_INFO_MSG("Received start request for alias " << startreq._alias << ".");

    std::ostringstream filearg;
    filearg << startreq._logdir <<
        "/" << _hostname.uhn() << "-" << startreq._alias << ".log";
    std::string log = filearg.str();
    std::ostringstream logmsg;
    logmsg << "Start request path=" << startreq._path << " "
           << "arguments=" << startreq._arguments << " "
           << "logdir=" << log << " "
           << "user=" << startreq._user << " ";
    LOG_DEBUG_MSG(logmsg.str());

    BinaryControllerPtr bin(new BinaryController(startreq._path,
                                                 startreq._arguments,
                                                 log,
                                                 startreq._alias,
                                                 _host,
                                                 startreq._user));
    _binaries.push_back(bin);

    BinaryId bid;
    BGMasterAgentProtocolSpec::StartReply::BinaryStatus binstat(bid, 0);
    BGMasterAgentProtocolSpec::StartReply rep(0, "start successful", binstat);
    rep._status = binstat;
    rep._rc = exceptions::OK;

    try {
        boost::mutex::scoped_lock lock(_uid_mutex);
        bid = bin->startBinary(_user_list);
        if (bid.get_pid() == -1) {
            // There's an error, we've returned once and handled it.
            // This time we just bail.
            // sendreply = false;
        }
    } catch (exceptions::FileError& e) {
        LOG_ERROR_MSG("Exec failed with a file error: " << e.what());
        rep._rc = e.errcode;
        rep._rt = e.what();
    }

    rep._status._binary_id = bid.str();

    if (bid.get_pid() > 0) {
        // Don't create the current symlink now that we're using logrotate.
    } else {
        // Failed to start
        if (std::find(_binaries.begin(), _binaries.end(), bin) == _binaries.end()) {
            // Binary controller has been removed, which means we told the server
            // about the failure the last time execv returned.
        }
        LOG_ERROR_MSG("Failed to start binary: " << bin->get_error_text());
        rep._rc = exceptions::WARN;
        rep._rt = bin->get_error_text();
    }

    try {
        LOG_DEBUG_MSG("Sending start reply");
        _prot->sendReply(rep.getClassName(), rep);
        LOG_DEBUG_MSG("Start reply sent for " << bid.str() << "|" << bin->get_binary_bin_path() << " " << rep._rt);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_WARN_MSG("Connection to bgmaster_server interrupted.");
        return;
    } catch(CxxSockets::CxxError& err) {
        // Server aborted with an incomplete transmission
        LOG_WARN_MSG("Connection to bgmaster_server ended.");
        if (err.errcode >= 0)
            _ending = true;
    }

    int exit_status = 0;
    int child_pid = bid.get_pid();
    if (child_pid > 0)
        waitpid(child_pid, &exit_status, 0);

    bool dont_report = bin->stopping();
    if (_ending)
        dont_report = true;

    // Now we need to remove the binary because we're done.
    if (std::find(_binaries.begin(), _binaries.end(), bin) != _binaries.end()) {
        LOG_DEBUG_MSG("Removing binary " << bin);
        _binaries.erase(std::remove(_binaries.begin(),_binaries.end(), bin), _binaries.end());
    }

    if (!WIFEXITED(exit_status) || exit_status != EXIT_SUCCESS) {
        // Failed!  Let master know.
        LOG_INFO_MSG("Binary id " << bid.str() << " alias " << bin->get_alias_name() << " ended with " << bgq::utility::ExitStatus( exit_status ));
        if (dont_report) {
            return;
        }

        BGMasterAgentProtocolSpec::FailedRequest::BinaryStatus binstat(bid.str(), exit_status);
        BGMasterAgentProtocolSpec::FailedRequest failreq(binstat);
        BGMasterAgentProtocolSpec::FailedReply failrep;
        failrep._rc = exceptions::OK;

        try {
            LOG_DEBUG_MSG("Sending ending request for alias " << bin->get_alias_name() << " binary id " << bid.str());
            _prot->failed(failreq, failrep);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Connection to bgmaster_server interrupted.");
            return;
        } catch(CxxSockets::CxxError& err) {
            // Server aborted with an incomplete transmission
            LOG_WARN_MSG("Connection to bgmaster_server ended.");
            MsgBasePtr bp(new BGMasterAgentProtocolSpec::FailedRequest(binstat));
            _buffered_messages.push_back(bp);
            if (err.errcode >= 0)
                _ending = true;
        }
    } else {
        if (child_pid > 0 && !dont_report) {
            // Only send a complete message if we haven't already sent a start failure.
            BGMasterAgentProtocolSpec::CompleteRequest::BinaryStatus binstat(bid.str(), "COMPLETED");
            BGMasterAgentProtocolSpec::CompleteRequest exereq(binstat, exit_status);
            BGMasterAgentProtocolSpec::CompleteReply exerep;
            exerep._rc = exceptions::OK;

            LOG_INFO_MSG("Sending complete request for " << bin->get_alias_name()
                         << " binary id " << bid.str() << " with exit status of " << exit_status);
            try {
                _prot->complete(exereq, exerep);
            } catch(CxxSockets::SockSoftError& err) {
                // For soft errors, we just back out and let it try again
                LOG_WARN_MSG("Connection to bgmaster_server interrupted.");
                return;
            } catch(CxxSockets::CxxError& err) {
                // Server aborted with an incomplete transmission
                LOG_WARN_MSG("Connection to bgmaster_server ended.");
                MsgBasePtr bp(new BGMasterAgentProtocolSpec::CompleteRequest(binstat, exit_status));
                _buffered_messages.push_back(bp);
                if (err.errcode >= 0)
                    _ending = true;
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
    BinaryId bid(stopreq._binary_id);
    BGMasterAgentProtocolSpec::StopReply stoprep;
    stoprep._rc = exceptions::OK;

    LOG_INFO_MSG("Received stop request for binary id " << stopreq._binary_id);

    BinaryControllerPtr ptr;
    if (find_binary(bid, ptr)) {
        // Only one at a time because we are switching uids
        boost::mutex::scoped_lock lock(_uid_mutex);
        int stop_sig = ptr->stopBinary(stopreq._signal);
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
        _binaries.erase(std::remove(_binaries.begin(),_binaries.end(), ptr), _binaries.end());
    } else {
        LOG_INFO_MSG("Binary id " << bid.str() << " not found, may have previously ended.");
        stoprep._rc = exceptions::INFO;
        stoprep._rt = "Binary " + bid.str() + " not found";
    }

    try {
        LOG_DEBUG_MSG("Sending stop reply");
        _prot->sendReply(stoprep.getClassName(), stoprep);
        LOG_DEBUG_MSG("Sent stop reply for binary id " << bid.str());
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_WARN_MSG("Connection to bgmaster_server interrupted.");
        return;
    } catch(CxxSockets::CxxError& err) {
        // Master aborted with an incomplete transmission
        LOG_WARN_MSG("Connection to bgmaster_server ended.");
        if (err.errcode >= 0)
            _ending = true;
    }
}

void
Agent::doEndAgentRequest(
        const BGMasterAgentProtocolSpec::End_agentRequest& /* diereq */,
        bool reply_to_master
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterAgentProtocolSpec::End_agentReply dierep;
    dierep._rc = exceptions::OK;

    _ending = true;
    // Kill all binaries.
    LOG_INFO_MSG("Ending bgagentd requested.");
    const size_t max = _binaries.size();
    for (size_t i = 0; i < max; ++i) {
        if (_binaries.begin() != _binaries.end()) {
            // Only one at a time because we are switching uids
            boost::mutex::scoped_lock lock(_uid_mutex);
            BinaryControllerPtr ptr = _binaries.front();
            ptr->stopBinary(SIGTERM);
            _binaries.erase(std::remove(_binaries.begin(),_binaries.end(), ptr), _binaries.end());
        }
    }

    dierep._rc = exceptions::OK;
    dierep._rt = "success";
    if (reply_to_master) {
        try {
            _prot->sendReply(dierep.getClassName(), dierep);
            LOG_DEBUG_MSG("Sent die reply");
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Connection to bgmaster_server interrupted.");
            return;
        } catch(CxxSockets::CxxError& err) {
            // Server aborted with an incomplete transmission
            LOG_WARN_MSG("Connection to bgmaster_server ended.");
            if (err.errcode >= 0)
                _ending = true;
        }
    }
    exit(0);
}

void
Agent::processRequest() {
    LOG_TRACE_MSG(__FUNCTION__);
    std::string request_name = "";

    try {
        _prot->getName(request_name);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_WARN_MSG("Connection to bgmaster_server interrupted.");
        return;
    } catch(CxxSockets::CxxError& err) {
        // Server aborted with an incomplete transmission
        LOG_WARN_MSG("Connection to bgmaster_server ended.");
        if (err.errcode >= 0)
            _ending = true;
        return;
    }

    LOG_DEBUG_MSG("-*-Request " << request_name << " received.-*-");

    if (request_name == "StartRequest") {
        // Start request must be threaded because it does a wait on
        // a binary and we may later need to kill that binary.
        BGMasterAgentProtocolSpec::StartRequest startreq;
        try {
            _prot->getObject(&startreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Connection to bgmaster_server interrupted.");
            return;
        } catch(CxxSockets::CxxError& err) {
            // Server aborted with an incomplete transmission
            LOG_WARN_MSG("Connection to bgmaster_server ended.");
            if (err.errcode >= 0)
                _ending = true;
            return;
        }
        boost::thread startthread(&Agent::processStartRequest, this, startreq);
    } else if (request_name == "StopRequest") {
        BGMasterAgentProtocolSpec::StopRequest stopreq;
        try {
            _prot->getObject(&stopreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Connection to bgmaster_server interrupted.");
            return;
        } catch(CxxSockets::CxxError& err) {
            // Server aborted with an incomplete transmission
            LOG_WARN_MSG("Connection to bgmaster_server ended.");
            if (err.errcode >= 0)
                _ending = true;
            return;
        }

        doStopRequest(stopreq);
    } else if (request_name == "End_agentRequest") {
        BGMasterAgentProtocolSpec::End_agentRequest diereq;
        try {
            _prot->getObject(&diereq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Connection to bgmaster_server interrupted.");
            return;
        } catch(CxxSockets::CxxError& err) {
            // Server aborted with an incomplete transmission
            LOG_WARN_MSG("Connection to bgmaster_server ended.");
            if (err.errcode >= 0)
                _ending = true;
            return;
        }
        doEndAgentRequest(diereq, true);
    } else {
        LOG_WARN_MSG("Unknown request: '" << request_name << "'");
        sleep(5); // Just do this because LNs somehow get here and spin.
    }
}

void
Agent::waitMessages() {
    LOG_TRACE_MSG(__FUNCTION__);
    // Wait for requests and send responses
    LOG_DEBUG_MSG("Waiting for messages.");
    while (!_ending) {
        // Process message.
        processRequest();
    }
    LOG_TRACE_MSG(__FUNCTION__ << " ending.");
    _ending = false;
}

void
Agent::cleanup() {
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterAgentProtocolSpec::End_agentRequest diereq;
    doEndAgentRequest(diereq, false);
}
