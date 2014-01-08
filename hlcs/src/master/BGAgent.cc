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

#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include "BGMasterAgentProtocol.h"
#include "BGMasterExceptions.h"
#include "BGAgent.h"
#include <utility/include/Log.h>
#include <utility/include/Exec.h>
#include "Thread.h"
#include "Host.h"

LOG_DECLARE_FILE( "master" );

//! \brief static properties object
static bgq::utility::Properties::Ptr agentProps;

boost::mutex BGAgent::_uid_mutex;
std::string BGAgent::_user_list;
bgq::utility::PortConfiguration::Pairs BGAgent::_portpairs;

BGAgent::BGAgent() { _user_list = ""; }

BGAgent::~BGAgent() {
    LOG_INFO_MSG(__FUNCTION__);
    // Join all threads
    for(std::vector<ThreadPtr>::iterator thread_it = _all_threads.begin();
        thread_it != _all_threads.end(); ++thread_it) {
        pthread_kill((*thread_it)->native_handle(), SIGUSR1);
        (*thread_it)->join();
    }
}

void BGAgent::startup(bgq::utility::Properties::Ptr& props) {
    LOG_INFO_MSG(__FUNCTION__);
    // Get my host name
    char hostname[256];
    bzero(hostname, sizeof(hostname));
    // This will be temporary until we
    // get our actual host/ip
    int rc = ::gethostname(hostname, 32);
    if(rc == 0) {
        Host h(hostname);
        _host = h;
        BGAgentId id(0, _host.ip());
        _agent_id = id;
    }
    else {
        // Can't run this way.  Bail out.
        perror("Failed to construct BGAgent");
        exit(1);
    }

    agentProps = props;

    // Be who I should be!
    if(getuid() != 0) {
        LOG_WARN_MSG("Warning:  Not running as root.  Will not be able to change user ids.");
    } else {
        // I'm root, so find who I'm supposed to be!
        std::string my_name = "root";
        try {
            my_name = agentProps->getValue("master.agent", "agentuid");
        } catch(std::invalid_argument& e) {
            LOG_WARN_MSG("No agent userid" << e.what());
        }

        if(_user_list.empty())
            _user_list = "bgqadmin,bgqsysdb,bgws";

        if(_user_list.find(my_name) == std::string::npos) {
            LOG_ERROR_MSG("Invalid user ID " << my_name << ".  Not in list of valid users: "
                          << _user_list);
            exit(EXIT_FAILURE);
        }

        LOG_INFO_MSG("Will attempt to switch user ID to " << my_name << " in user list "
                     << _user_list);

        struct passwd* my_entry = getpwnam(my_name.c_str());
        if(my_entry == 0) {
            LOG_ERROR_MSG("Invalid user ID " << my_name << " specified for bgagentd.  Exiting.");
            exit(EXIT_FAILURE);
        }
        // And change my group for good measure
        if(setegid(my_entry->pw_gid) != 0) {
            LOG_ERROR_MSG("Cannot change effective gid to " << my_entry->pw_gid);
            exit(EXIT_FAILURE);
        }

        // Now, find all of the groups for the user (up to 25) and set them:
        gid_t groups[25];
        int groupcount = 25;
        rc = getgrouplist(my_name.c_str(), my_entry->pw_gid, groups, &groupcount);
        if(rc < 0) {
            LOG_INFO_MSG("User is a member of more than 25 groups.");
        }
        rc = setgroups(groupcount, groups);
        if(rc < 0) {
            LOG_ERROR_MSG("Could not set secondary groups of this process: " << strerror(errno));
        }

        std::ostringstream groupmsg;
        for(int i = 0; i < groupcount; ++i) {
            groupmsg << groups[i] << " ";
        }
        LOG_TRACE_MSG("Set the following supplementary groups " << groupmsg.str());
     
        // Finally, set the user id.
        if(seteuid(my_entry->pw_uid) != 0) { // Make the change
            LOG_ERROR_MSG("Cannot change effective uid to " << my_entry->pw_uid);
            exit(EXIT_FAILURE);
        }
    }

    // If WaitMessages returns, that means that the connection to bgmaster
    // failed.  We're going to retry joining
    while(true) {
        // Join the "cluster"
        try {
            join();
        } catch(CxxSockets::CxxError& e) {
            LOG_ERROR_MSG("Socket error detected: " << e.errcode << " " << e.what());

            // If we're spinning, do so slowly.
            sleep(1);
            continue;
        } catch (BGMasterExceptions::CommunicationError& e) {
            LOG_ERROR_MSG("Communication error detected: " << e.errcode << " " << e.what());
            sleep(1);
            continue;
        }

        // We're joined to the agent cluster...
        _ending = false; // so we aren't ending...
        waitMessages();  // and we should wait for new messages.
    }
}

BGMasterAgentProtocolSpec::JoinRequest
BGAgent::build_join_request(std::string hostaddr, int servname) {
    LOG_INFO_MSG(__FUNCTION__);
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

void BGAgent::join() {
    LOG_INFO_MSG(__FUNCTION__);
    // bgq::utility::ClientPortConfiguration port_config(32041, bgq::utility::ClientPortConfiguration::ConnectionType::Administrative);
    // port_config.setProperties( agentProps, "master.agent");
    // port_config.notifyComplete();
    // bgq::utility::PortConfiguration::Pairs portpairs = port_config.getPairs();

    typedef CxxSockets::ListeningSocketPtr Listener;

    BGMasterAgentProtocolPtr p(new BGMasterAgentProtocol(agentProps));
    _prot = p;

    std::string connected_host = "";
    std::string connected_port = "";
    bool failed = true;
    std::ostringstream portstrings;

    while(failed) {
        BOOST_FOREACH(bgq::utility::PortConfiguration::Pair portpair, _portpairs) {
            portstrings << portpair.first << ":" << portpair.second;
            LOG_INFO_MSG("Attempting to connect to " << portpair.first << ":" << portpair.second);
            try {
                _prot->initializeRequester(AF_UNSPEC, portpair.first, portpair.second, "secure", 1);
                connected_host = portpair.first;
                connected_port = portpair.second;
                LOG_INFO_MSG("Connected to " << portpair.first << ":" << portpair.second);
                failed = false;
                break;
            } catch (CxxSockets::SockSoftError& err) {
                std::ostringstream msg;
                msg << "Master connection failed (Soft Join): " << strerror(err.errcode);
                LOG_WARN_MSG(msg.str());
            } catch (CxxSockets::CxxError& err) {
                std::ostringstream msg;
                msg << "Master connection failed (Hard Join): " << strerror(err.errcode);
                LOG_WARN_MSG(msg.str());
            }
            sleep(1);
        }
    }

    CxxSockets::SockAddr localsockaddr;
    _prot->getRequester()->getSockName(localsockaddr);

    // This socket will listen for a connection back from bgmaster.
    // It'll be on the same IP address as the requester.
    CxxSockets::SockAddr sa(localsockaddr.family(), "", "");
    Listener ln(new CxxSockets::ListeningSocket(sa, 1, CxxSockets::SECURE));
    _masterListener = ln;

    // Need to send back the info for our listener
    CxxSockets::SockAddr requestersockaddr;
    CxxSockets::SockAddr listenersockaddr;
    _prot->getRequester()->getSockName(requestersockaddr);
    _masterListener->getSockName(listenersockaddr);

    // Note, this can throw an exception and dump us out of join
    // processing if the hostname is unroutable.
    Host h(requestersockaddr.getHostAddr());
    _host = h;
        
    // The join request will be formulated with the local IP address of the requester
    // and the local port of the listener.  This is to ensure that the master has a
    // routable IP address (not loopback) and the correct port to connect back to.
    BGMasterAgentProtocolSpec::JoinRequest joinreq =
        build_join_request(requestersockaddr.getHostAddr(), listenersockaddr.getServicePort());

    BGMasterAgentProtocolSpec::JoinReply joinrep;
    joinrep._rc = BGMasterExceptions::OK;

    LOG_INFO_MSG("Sending join request from " << requestersockaddr.getHostAddr() << ":" << listenersockaddr.getServicePort()
                 << " to " << connected_host << ":" << connected_port);
    _prot->join(joinreq, joinrep);

    if(joinrep._rc != 0) {
        LOG_ERROR_MSG("Join request denied by bgmaster_server.  " << joinrep._rt);
        exit(EXIT_FAILURE);
    }

    LOG_INFO_MSG("Group joined");

    // Now look at the join reply to see if we need to kill any binaries
    BOOST_FOREACH(std::string badbin, joinrep._bad_bins) {
        BinaryId bid(badbin);
        BinaryControllerPtr ptr;
        if(find_binary(bid, ptr)) {
            BinaryControllerPtr p = boost::static_pointer_cast<BinaryController>(ptr);
            // Only one at a time because we are switching uids
            boost::mutex::scoped_lock(_uid_mutex);
            int rc = p->stopBinary(SIGTERM);
            LOG_INFO_MSG("Stopped bad binary " << p << " with signal " << rc);
            // Now take it out of the list
            _binaries.erase(std::remove(_binaries.begin(),_binaries.end(), ptr), _binaries.end());
        } else {
            LOG_INFO_MSG("Binary " << bid.str() << " not found");
        }
    }

    // We've joined the group.  Now set up our responder.  We've created
    // the listener, so we need to sit on an accept until the master
    // connects back.  At that point, we are completely part of the cluster.
    CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(CxxSockets::SECURE, CxxSockets::CRYPTALL));
    bool accepted = false;
    while(!accepted) {
        try {
            bgq::utility::ServerPortConfiguration port_config(0, bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeCommand);
            port_config.setProperties(agentProps, "");
            port_config.notifyComplete();
            accepted  = _masterListener->AcceptNew(sock, port_config);
        } catch (std::runtime_error& e) {
            if(errno != EINTR && errno != EAGAIN) {
                LOG_FATAL_MSG( e.what() << " errno: " << errno);
                exit(1);
            }
        }
        if(accepted) {
            LOG_INFO_MSG( "got a new connection from master");
            _prot->initializeResponder(sock);
            break;
        }
    }

    // Spawn a thread to send any old buffered up messages
    // so that we can immediately handle incoming requests
    // without deadlock.
    boost::thread startthread(&BGAgent::sendBuffered, this);
}

void BGAgent::sendBuffered() {
    ThreadLog tl(__FUNCTION__);
    // Send buffered messages
    LOG_INFO_MSG("Sending " << _buffered_messages.size() << " buffered messages to bgmaster_server.");
    BOOST_FOREACH(MsgBasePtr& curr_msg, _buffered_messages) {
        if(_buffered_messages.size() <= 0) break;
        // Inelegant, but should work.
        std::string classname;
        classname = boost::static_pointer_cast<BGMasterAgentProtocolSpec::FailedRequest>(curr_msg)->getClassName();
        if(classname == "FailedRequest") {
            BGMasterAgentProtocolSpec::FailedReply failrep;
            failrep._rc = BGMasterExceptions::OK;

            try {
                _prot->failed(*(boost::static_pointer_cast
                                <BGMasterAgentProtocolSpec::FailedRequest>(curr_msg).get()), failrep);
            } catch (CxxSockets::SockSoftError& err) {
                std::ostringstream msg;
                msg << "Master connection failed (FailedRequest Soft): " << strerror(err.errcode);
                throw BGMasterExceptions::CommunicationError(BGMasterExceptions::INFO, msg.str());
            } catch (CxxSockets::CxxError& err) {
                std::ostringstream msg;
                msg << "Master connection failed (FailedRequest Hard): " << strerror(err.errcode);
                throw BGMasterExceptions::CommunicationError(BGMasterExceptions::WARN, msg.str());
            }
            LOG_INFO_MSG("Sent buffered failed request.");
        } else if(classname == "CompleteRequest") {
            BGMasterAgentProtocolSpec::CompleteReply comprep;
            comprep._rc = BGMasterExceptions::OK;

            try {
                _prot->complete(*(boost::static_pointer_cast
                                  <BGMasterAgentProtocolSpec::CompleteRequest>(curr_msg).get()), comprep);
            } catch (CxxSockets::SockSoftError& err) {
                std::ostringstream msg;
                msg << "Master connection failed (CompleteRequest Soft): " << strerror(err.errcode);
                throw BGMasterExceptions::CommunicationError(BGMasterExceptions::INFO, msg.str());
            } catch (CxxSockets::CxxError& err) {
                std::ostringstream msg;
                msg << "Master connection failed (CompleteRequest Hard): " << strerror(err.errcode);
                throw BGMasterExceptions::CommunicationError(BGMasterExceptions::WARN, msg.str());
            }
            LOG_INFO_MSG("Sent buffered complete request. ");
        }
        // Must remove this message from the list
        _buffered_messages.erase(std::remove(_buffered_messages.begin(),_buffered_messages.end(), curr_msg), _buffered_messages.end());
    }
}

void BGAgent::processStartRequest(BGMasterAgentProtocolSpec::StartRequest& startreq) {
    LOG_INFO_MSG(__FUNCTION__);
    // We're happy to start as many instances of this binary
    // as we're asked.  If only one is wanted, the client needs
    // to enforce that.
    ThreadLog tl("Start Request");
    boost::mutex::scoped_lock(_uid_mutex);

    LOG_INFO_MSG("Received start request for " << startreq._alias << " as " << startreq._user);
    // Get my host name to put in the log file name.
    char hostname[256];
    bzero(hostname, sizeof(hostname));
    int rc = ::gethostname(hostname, 32);
    std::string hname = "unknown";
    if(rc == 0) {
        hname = hostname;
    }
    else {
        // Can't run this way.  Bail out.
        perror("Cannot get host name!");
    }

    std::ostringstream filearg;
    filearg << startreq._logdir <<
        "/" << hname << "-" << startreq._alias << ".log";
    std::string log = filearg.str();
    std::ostringstream logmsg;
    logmsg << "start request path=" << startreq._path << " "
           << "arguments=" << startreq._arguments << " "
           << "logdir=" << log << " "
           << "user=" << startreq._user << " ";
    LOG_TRACE_MSG(logmsg.str());

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
    rep._rc = BGMasterExceptions::OK;

    try {
        bid = bin->startBinary();
        if(bid.get_pid() == -1) {
            // There's an error, we've returned once and handled it.
            // This time we just bail.
            // sendreply = false;
        }
    } catch (BGMasterExceptions::FileError& e) {
        LOG_ERROR_MSG("Exec failed with a file error: " << e.what());
        rep._rc = e.errcode;
        rep._rt = e.what();
    } catch (Exec::ExecException& e) {
        LOG_ERROR_MSG("Exec failed: " << e.what());
        rep._rc = e.errcode;
        rep._rt = e.what();
    }

    rep._status._binary_id = bid.str();

    if(bid.get_pid() > 0) {
        // Don't create the current symlink now that we're using logrotate.
    } else {
        // Failed to start
        if(std::find(_binaries.begin(), _binaries.end(), bin) == _binaries.end()) {
            // Binary controller has been removed, which means we told the server
            // about the failure the last time execv returned.
            //            return;
        }
        LOG_ERROR_MSG("Failed to start binary" << bin->get_error_text());
        rep._rc = BGMasterExceptions::WARN;
        rep._rt = bin->get_error_text();
    }

    try {
        LOG_DEBUG_MSG("Sending start reply");
        _prot->sendReply(rep.getClassName(), rep);
        LOG_INFO_MSG("Start reply sent for " << bid.str() << "|" << bin->get_binary_bin_path() << " " << rep._rt);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_WARN_MSG("Master connection interrupted sending start reply: " << strerror(err.errcode));
        return;
    } catch(CxxSockets::CxxError& err) {
        // Server aborted with an incomplete transmission
        LOG_INFO_MSG("Master connection has abnormally ended during agent start reply send: " << strerror(err.errcode));
        if(err.errcode >= 0)
            _ending = true;
    }

    int exit_status = 0;
    int child_pid = bid.get_pid();
    if(child_pid > 0)
        waitpid(child_pid, &exit_status, 0);

    bool dont_report = bin->stopping();
    if(_ending) dont_report = true;

    // Now we need to remove the binary because we're done.
    if(std::find(_binaries.begin(), _binaries.end(), bin) != _binaries.end()) {
        LOG_INFO_MSG("Removing binary " << bin);
        _binaries.erase(std::remove(_binaries.begin(),_binaries.end(), bin), _binaries.end());
    }

    if(!WIFEXITED(exit_status) || exit_status != EXIT_SUCCESS) {

        // Failed!  Let master know.
        LOG_ERROR_MSG("Binary " << bin << " failed with status " << exit_status);
        if(dont_report) return;

        BGMasterAgentProtocolSpec::FailedRequest::BinaryStatus binstat(bid.str(), exit_status);
        BGMasterAgentProtocolSpec::FailedRequest failreq(binstat);
        BGMasterAgentProtocolSpec::FailedReply failrep;
        failrep._rc = BGMasterExceptions::OK;

        try {
            LOG_INFO_MSG("Sending fail request for " << bin->get_alias_name() << " id: " << bid.str());
            _prot->failed(failreq, failrep);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Master connection interrupted sending failed request: " << strerror(err.errcode));
            return;
        } catch(CxxSockets::CxxError& err) {
            // Server aborted with an incomplete transmission
            LOG_INFO_MSG("Master connection has abnormally ended during failed send: " << strerror(err.errcode));
            MsgBasePtr bp(new BGMasterAgentProtocolSpec::FailedRequest(binstat));
            _buffered_messages.push_back(bp);
            if(err.errcode >= 0)
                _ending = true;
        }
    } else {
        if(child_pid > 0 && !dont_report) {
            // Only send a complete message if we haven't already sent a start failure.
            BGMasterAgentProtocolSpec::CompleteRequest::BinaryStatus binstat(bid.str(), "COMPLETED");
            BGMasterAgentProtocolSpec::CompleteRequest exereq(binstat, exit_status);
            BGMasterAgentProtocolSpec::CompleteReply exerep;
            exerep._rc = BGMasterExceptions::OK;

            LOG_INFO_MSG("Sending complete request for " << bin->get_alias_name() 
                         << " id: " << bid.str() << " and exit status of " << exit_status);
            try {
                _prot->complete(exereq, exerep);
            } catch(CxxSockets::SockSoftError& err) {
                // For soft errors, we just back out and let it try again
                LOG_WARN_MSG("Master connection interrupted sending complete message: " << strerror(err.errcode));
                return;
            } catch(CxxSockets::CxxError& err) {
                // Server aborted with an incomplete transmission
                LOG_WARN_MSG("Master connection has abnormally ended during 'complete' send: " << strerror(err.errcode));
                MsgBasePtr bp(new BGMasterAgentProtocolSpec::CompleteRequest(binstat, exit_status));
                _buffered_messages.push_back(bp);
                if(err.errcode >= 0)
                    _ending = true;
            }
        }
    }
}

void BGAgent::doStopRequest(BGMasterAgentProtocolSpec::StopRequest& stopreq) {
    LOG_INFO_MSG(__FUNCTION__);
    boost::mutex::scoped_lock(_uid_mutex);
    // Find the binary and nuke it.
    BinaryId bid(stopreq._binary_id);
    BinaryControllerPtr ptr;
    BGMasterAgentProtocolSpec::StopReply stoprep;
    stoprep._rc = BGMasterExceptions::OK;

    LOG_INFO_MSG("Received stop request for id: " << stopreq._binary_id);

    if(find_binary(bid, ptr)) {
        BinaryControllerPtr p = boost::static_pointer_cast<BinaryController>(ptr);
        // Only one at a time because we are switching uids
        int stop_sig = p->stopBinary(stopreq._signal);
        if(stop_sig == 0) {
            LOG_INFO_MSG("Failed to stop binary" << bid.str());
            stoprep._rc = BGMasterExceptions::WARN;
            stoprep._rt = "failed to stop binary";
        }
        stoprep._status._binary_id = bid.str();
        if(ptr->get_exit_status() != 0)
            stoprep._status._exit_status = ptr->get_exit_status();
        else 
            stoprep._status._exit_status = stop_sig;
        // Now take it out of the list
        _binaries.erase(std::remove(_binaries.begin(),_binaries.end(), ptr), _binaries.end());
    } else {
        LOG_INFO_MSG("Binary " << bid.str() << " not found");
        stoprep._rc = BGMasterExceptions::INFO;
        stoprep._rt = "Binary " + bid.str() + " not found";
    }

    try {
        LOG_DEBUG_MSG("Sending stop reply");
        _prot->sendReply(stoprep.getClassName(), stoprep);
        LOG_INFO_MSG("Sent stop reply for " << bid.str());
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_WARN_MSG("Master connection interrupted sending stop reply: " << strerror(err.errcode));
        return;
    } catch(CxxSockets::CxxError& err) {
        // Master aborted with an incomplete transmission
        LOG_INFO_MSG("Master connection has abnormally ended during send: " << strerror(err.errcode));
        if(err.errcode >= 0)
            _ending = true;
    }
}

void BGAgent::doEndAgentRequest(BGMasterAgentProtocolSpec::End_agentRequest& diereq, bool reply_to_master) {
    LOG_INFO_MSG(__FUNCTION__);
    boost::mutex::scoped_lock(_uid_mutex);
    BGMasterAgentProtocolSpec::End_agentReply dierep;
    dierep._rc = BGMasterExceptions::OK;

    _ending = true;
    // Kill all binaries.
    LOG_INFO_MSG("Ending bgagentd requested");
    int max = _binaries.size();
    for(int i = 0; i < max; ++i) {
        if(_binaries.begin() != _binaries.end()) {
            // Only one at a time because we are switching uids
            BinaryControllerPtr ptr = _binaries.front();
            ptr->stopBinary(SIGTERM);
            _binaries.erase(std::remove(_binaries.begin(),_binaries.end(), ptr), _binaries.end());
        }
    }

    dierep._rc = BGMasterExceptions::OK;
    dierep._rt = "success";
    if(reply_to_master) {
        try {
            _prot->sendReply(dierep.getClassName(), dierep);
            LOG_INFO_MSG("Sent die reply");
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Master connection interrupted sending die reply: " << strerror(err.errcode));
            return;
        } catch(CxxSockets::CxxError& err) {
            // Server aborted with an incomplete transmission
            LOG_INFO_MSG("Master connection has abnormally ended during die reply: " << strerror(err.errcode));
            if(err.errcode >= 0)
                _ending = true;
        }
    }
    exit(0);
}

void BGAgent::processRequest() {
    LOG_INFO_MSG(__FUNCTION__);
    std::string request_name = "";

    try {
        _prot->getName(request_name);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_WARN_MSG("Master connection interrupted getting request name: " << strerror(err.errcode));
        return;
    } catch(CxxSockets::CxxError& err) {
        // Server aborted with an incomplete transmission
        LOG_INFO_MSG("Master connection has abnormally ended receiving request name: " << strerror(err.errcode));
        if(err.errcode >= 0)
            _ending = true;
        return;
    }

    LOG_INFO_MSG("-*-Request " << request_name << " received.-*-");

    if(request_name == "StartRequest") {
        // Start request must be threaded because it does a wait on
        // a binary and we may later need to kill that binary.
        BGMasterAgentProtocolSpec::StartRequest startreq;
        try {
            _prot->getObject(&startreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Master connection interrupted getting start request: " << strerror(err.errcode));
            return;
        } catch(CxxSockets::CxxError& err) {
            // Server aborted with an incomplete transmission
            LOG_INFO_MSG("Master connection has abnormally ended receiving start request: " << strerror(err.errcode));
            if(err.errcode >= 0)
                _ending = true;
            return;
        }
        boost::thread startthread(&BGAgent::processStartRequest, this, startreq);
    }
    else if(request_name == "StopRequest") {
        BGMasterAgentProtocolSpec::StopRequest stopreq;
        try {
            _prot->getObject(&stopreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Master connection interrupted getting stop request: " << strerror(err.errcode));
            return;
        } catch(CxxSockets::CxxError& err) {
            // Server aborted with an incomplete transmission
            LOG_INFO_MSG("Master connection has abnormally ended receiving stop request: " << strerror(err.errcode));
            if(err.errcode >= 0)
                _ending = true;
            return;
        }

        doStopRequest(stopreq);
    }
    else if(request_name == "End_agentRequest") {
        BGMasterAgentProtocolSpec::End_agentRequest diereq;
        try {
            _prot->getObject(&diereq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_WARN_MSG("Master connection interrupted getting end agent request: " << strerror(err.errcode));
            return;
        } catch(CxxSockets::CxxError& err) {
            // Server aborted with an incomplete transmission
            LOG_INFO_MSG("Master connection has abnormally ended receiving end agent request: " << strerror(err.errcode));
            if(err.errcode >= 0)
                _ending = true;
            return;
        }
        doEndAgentRequest(diereq, true);
    } else sleep(1); // Just do this to because LNs somehow get here and spin.
}

void BGAgent::waitMessages() {
    LOG_INFO_MSG(__FUNCTION__);
    // Wait for requests and send responses
    LOG_INFO_MSG("bgmaster responder waiting for messages");
    while(!_ending) {
        // process message.
        processRequest();
    }
    LOG_TRACE_MSG(__FUNCTION__ << " ending.");
    _ending = false;
}

void BGAgent::cleanup() {
    LOG_INFO_MSG(__FUNCTION__);
    int rc = seteuid(0); // Go back to root while we do this.
    // Be root for this.
    if(rc != 0) {
        LOG_WARN_MSG("Cannot change uid back to root.  Current uid is " << geteuid() << ". " << strerror(errno));
    }

    rc = setegid(0);
    if(rc != 0) {
        LOG_ERROR_MSG("Could not set effective gid back to root: " << strerror(errno));
    }
    BGMasterAgentProtocolSpec::End_agentRequest diereq;
    doEndAgentRequest(diereq, false);
}
