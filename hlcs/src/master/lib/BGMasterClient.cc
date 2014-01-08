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

#include "BGMasterClient.h"

#include "exceptions.h"

#include "common/BinaryController.h"
#include "common/ClientProtocol.h"

#include <boost/foreach.hpp>
#include <boost/scope_exit.hpp>

LOG_DECLARE_FILE( "master" );

BGMasterClient::BGMasterClient() :
    _prot( new ClientProtocol )
{

}

ClientId
BGMasterClient::connectMaster(
        const bgq::utility::Properties::ConstPtr& props,
        const bgq::utility::PortConfiguration::Pairs& portpairs
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::string connected_host;
    std::string connected_port;
    bool failed = true;
    std::ostringstream portstrings;

    BOOST_FOREACH(const bgq::utility::PortConfiguration::Pair& portpair, portpairs) {
        portstrings << portpair.first << ":" << portpair.second;
        try {
            _prot->initializeRequester(props, AF_UNSPEC, portpair.first, portpair.second, 2);
            connected_host = portpair.first;
            connected_port = portpair.second;
            LOG_DEBUG_MSG("Connected to " << portpair.first << ":" << portpair.second);
            failed = false;
            break;
        } catch (const CxxSockets::Error& e) {
            if ( e.errcode == -1 ) {
                throw exceptions::CommunicationError(exceptions::FATAL, e.what());
            }
            LOG_WARN_MSG("Connection to bgmaster_server failed: " << e.what());
        } catch (const std::exception& e) {
            LOG_FATAL_MSG(e.what());
        }
        portstrings << " ";
    }

    if (failed) {
        std::ostringstream msg;
        msg << "Failed to connect to bgmaster_server on host pairs: " << portstrings.str();
        LOG_ERROR_MSG(msg.str());
        throw exceptions::CommunicationError(exceptions::FATAL, msg.str());
    }

    CxxSockets::SockAddr la;
    _prot->getRequester()->getSockName(la);
    const int local_port = la.getServicePort();
    BGMasterClientProtocolSpec::JoinRequest joinreq(la.getHostAddr(), local_port, "client", connected_host);
    BGMasterClientProtocolSpec::JoinReply joinrep;
    joinrep._rc = exceptions::OK;

    LOG_DEBUG_MSG("Sending join to " << connected_host << ":" << connected_port);
    try {
        _prot->join(joinreq, joinrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    LOG_DEBUG_MSG("Group joined");
    CxxSockets::SockAddr sa;
    _prot->getRequester()->getSockName(sa);
    const ClientId id(sa.getServicePort(), sa.getHostName());
    return id;
}

BinaryId
BGMasterClient::start(
        const std::string& alias,
        const BGAgentId* loc
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGAgentId location;
    if (loc) {
        location = *loc;
    }

    BGMasterClientProtocolSpec::StartRequest startreq(alias, location.str());
    BGMasterClientProtocolSpec::StartReply startrep;
    startrep._rc = exceptions::OK;

    LOG_DEBUG_MSG("Sending start request " << alias << " " << location.str());
    try {
        _prot->start(startreq, startrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    const BinaryId id(startrep._binary_id);
    if (startrep._rc != 0) { // Could not start!
        throw exceptions::InternalError(exceptions::FATAL, startrep._rt);
    }
    LOG_DEBUG_MSG("Received start reply, binary id " << startrep._binary_id);
    return id;
}

int
BGMasterClient::wait_for_terminate(
        BinaryId bid
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);

    BGMasterClientProtocolSpec::WaitRequest waitreq(bid);
    BGMasterClientProtocolSpec::WaitReply waitrep;
    waitrep._rc = exceptions::OK;

    LOG_DEBUG_MSG("Sending wait request " << bid.str());
    try {
        _prot->wait(waitreq, waitrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    LOG_DEBUG_MSG("Received terminate reply, binary id " << waitrep._status._binary_id);
    if (waitrep._rc != exceptions::OK)
        return -1; // Abnormal end.  Usually, bgmaster_server was ended rather than the subnet going down.
    return waitrep._status._status;
}

void
BGMasterClient::stop(
        const std::string& alias,
        const int signal,
        std::string& errormsg
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const std::vector<BinaryId> bv;
    std::vector<std::string> sv;
    sv.push_back(alias);
    return stop(bv, sv, signal, errormsg);
}

void
BGMasterClient::stop(
        const BinaryId& id,
        const int signal,
        std::string& errormsg
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const std::vector<std::string> sv;
    std::vector<BinaryId> bv;
    if (id.str() != "")
        bv.push_back(id);
    return stop(bv, sv, signal, errormsg);
}

void
BGMasterClient::stop(
        const std::vector<std::string>& aliases,
        const int signal,
        std::string& errormsg
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const std::vector<BinaryId> bv;
    return stop(bv, aliases, signal, errormsg);
}

void
BGMasterClient::stop(
        const std::vector<BinaryId>& ids,
        const int signal,
        std::string& errormsg
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const std::vector<std::string> sv;
    return stop(ids, sv, signal, errormsg);
}

void
BGMasterClient::stop(
        const std::vector<BinaryId>& ids,
        const std::vector<std::string>& aliases,
        const int signal,
        std::string& errormsg
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::StopRequest stopreq(signal);
    stopreq._aliases = aliases;

    std::vector<BinaryId> idvec = ids;
    for (std::vector<BinaryId>::const_iterator it = idvec.begin(); it != idvec.end(); ++it) {
        stopreq._binary_ids.push_back(it->str());
    }

    BGMasterClientProtocolSpec::StopReply stoprep(exceptions::OK, "success");
    try {
        _prot->stop(stopreq, stoprep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    errormsg = stoprep._rt;
    if (stoprep._rc != exceptions::OK) {
        throw exceptions::APICommandError(exceptions::INFO, stoprep._rt);
    }
    return;
}

void
BGMasterClient::get_errors(
        std::vector<std::string>& error_vec
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const BGMasterClientProtocolSpec::Get_errorsRequest error_req;
    BGMasterClientProtocolSpec::Get_errorsReply error_rep(exceptions::OK, "success");
    try {
        _prot->get_errors(error_req, error_rep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }
    for (std::vector<std::string>::const_iterator it = error_rep._errors.begin(); it != error_rep._errors.end(); ++it) {
        error_vec.push_back(*it);
    }
}

void
BGMasterClient::get_history(
        std::vector<std::string>& history_vec
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const BGMasterClientProtocolSpec::Get_historyRequest history_req;
    BGMasterClientProtocolSpec::Get_historyReply history_rep(exceptions::OK, "success");
    try {
        _prot->get_history(history_req, history_rep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }
    for (std::vector<std::string>::const_iterator it = history_rep._history.begin(); it != history_rep._history.end(); ++it) {
        history_vec.push_back(*it);
    }
}

int
BGMasterClient::master_status(
        std::string& start_time,
        std::string& version,
        std::string& properties
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const BGMasterClientProtocolSpec::MasterstatRequest statreq;
    BGMasterClientProtocolSpec::MasterstatReply statrep;
    statrep._rc = exceptions::OK;

    try {
        _prot->masterstat(statreq, statrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    int retval = -1;
    if (statrep._rc == exceptions::OK) {
        retval = statrep._pid;
        start_time = statrep._master_start_time;
        version = statrep._version;
        properties = statrep._properties;
    }
    return retval;
}

void
BGMasterClient::status(
        std::map<BinaryId, BinaryControllerPtr, Id::Comp>& stats
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::StatusRequest statreq;
    for (std::map<BinaryId, BinaryControllerPtr, Id::Comp>::const_iterator it = stats.begin(); it != stats.end(); ++it) {
        const BinaryId bid = it->first;
        statreq._binary_ids.push_back(bid.str());
    }

    BGMasterClientProtocolSpec::StatusReply statrep(exceptions::OK, "success");
    try {
        _prot->status(statreq, statrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    stats.clear();

    typedef BGMasterClientProtocolSpec::StatusReply::BinaryController BinCont;
    for (std::vector<BinCont>::const_iterator binit = statrep._binaries.begin(); binit != statrep._binaries.end(); ++binit) {
        const BinCont bincont = *binit;
        const BinaryId bid(bincont._binary_id);
        const BinaryController::Status stat = (BinaryController::Status)(bincont._status);
        const BinaryControllerPtr binary(
                new BinaryController(
                    bid,
                    bincont._binary_name,
                    bincont._alias,
                    bincont._user,
                    bincont._exit_status,
                    stat,
                    bincont._start_time
                    )
                );

        stats[bid] = binary;
    }
}

void
BGMasterClient::idle_aliases(
        std::vector<std::string>& aliases
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const BGMasterClientProtocolSpec::GetidleRequest idlereq;
    BGMasterClientProtocolSpec::GetidleReply idlerep(exceptions::OK, "success");
    try {
        _prot->getidle(idlereq, idlerep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }
    BOOST_FOREACH(const std::string& al, idlerep._aliases) {
        aliases.push_back(al);
    }
}

void
BGMasterClient::get_agents(
        std::map<BGAgentId, std::vector<BinaryControllerPtr>, Id::Comp>& amap
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const BGMasterClientProtocolSpec::AgentlistRequest agentreq;
    BGMasterClientProtocolSpec::AgentlistReply agentrep(exceptions::OK, "success");
    try {
        _prot->agentlist(agentreq, agentrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    // Must iterate through this mess and create the map.
    // First, iterate through the agents.
    typedef BGMasterClientProtocolSpec::AgentlistReply::Agent ReplyAgent;
    for (std::vector<ReplyAgent>::const_iterator agent_it = agentrep._agent.begin(); agent_it != agentrep._agent.end(); ++agent_it) {
        // Get the id
        const BGAgentId id = agent_it->_agent_id;
        const ReplyAgent agent = *agent_it;
        // Now build the vector of binaries
        typedef BGMasterClientProtocolSpec::AgentlistReply::Agent::Binary ReplyBinary;
        std::vector<BinaryControllerPtr> binvec;
        for (std::vector<ReplyBinary>::const_iterator bin_it = agent._binaries.begin(); bin_it != agent._binaries.end(); ++bin_it) {
            // Now build our binary type from the reply's
            const ReplyBinary rep_bin = *bin_it;
            LOG_DEBUG_MSG("Found binary " << rep_bin._binary_id << "|" << rep_bin._name << "|" << rep_bin._status);
            const BinaryControllerPtr base(
                    new BinaryController(rep_bin._binary_id,
                        rep_bin._name,
                        rep_bin._alias,
                        rep_bin._user,
                        rep_bin._exit_status,
                        rep_bin._status,
                        rep_bin._start_time
                        )
                    );
            binvec.push_back(base);
        }
        // Now stick our vector in our map
        amap[id] = binvec;
    }
}

void
BGMasterClient::get_clients(
        ClientAndUserMap& clients
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const BGMasterClientProtocolSpec::ClientsRequest clientreq;
    BGMasterClientProtocolSpec::ClientsReply clientrep(exceptions::OK, "success");
    try {
        _prot->clients(clientreq, clientrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    typedef std::vector<BGMasterClientProtocolSpec::ClientsReply::Client>::const_iterator ClientIterator;
    for (ClientIterator it = clientrep._clients.begin(); it != clientrep._clients.end(); ++it) {
        const ClientId id(it->_client_id);
        const ClientUID uid(it->_user_id);
        clients[id] = uid;
    }
}

void
BGMasterClient::end_master(
        bool master_only,
        int signal
        ) const
{
    // Note that this just requests a graceful end to the bgmaster_server
    LOG_TRACE_MSG(__FUNCTION__);
    const BGMasterClientProtocolSpec::TerminateRequest termreq(master_only, signal);
    BGMasterClientProtocolSpec::TerminateReply termrep(exceptions::OK, "success");
    try {
        _prot->terminate(termreq, termrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    if (termrep._rc != exceptions::OK) {
        throw exceptions::APICommandError(exceptions::Severity(termrep._rc), termrep._rt);
    }
}

void
BGMasterClient::fail_over(
        std::vector<BinaryId>& bins,
        const std::string& trigger
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::FailoverRequest failreq;
    BGMasterClientProtocolSpec::FailoverReply failrep(exceptions::OK, "success");
    BOOST_FOREACH(const BinaryId& curr_id, bins) {
        failreq._binary_ids.push_back(curr_id.str());
    }
    failreq._trigger = trigger;

    try {
        _prot->failover(failreq, failrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    // Empty the vector
    bins.clear();
    // Now go through the fail replies and put all of the failed failovers in the vector
    typedef BGMasterClientProtocolSpec::FailoverReply::BinaryStatus BinStat;
    BOOST_FOREACH(BinStat& bs, failrep._statuses) {
        const BinaryId bid(bs._binary_id);
        bins.push_back(bid);
    }

    if (failrep._rc != exceptions::OK) {
        throw exceptions::ConfigError(exceptions::WARN, failrep._rt);
    }
}

void
BGMasterClient::reload_config(
        const std::string& config_file
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::ReloadRequest relreq;
    if (!config_file.empty())
        relreq._config_file = config_file;

    BGMasterClientProtocolSpec::ReloadReply relrep(exceptions::OK, "success");

    try {
        _prot->reload(relreq, relrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::FATAL, "Connection to bgmaster_server ended.");
    }

    if (relrep._rc != exceptions::OK) {
        exceptions::Severity s;
        if (relrep._rc == exceptions::INFO)
            s = exceptions::INFO;
        else if (relrep._rc == exceptions::WARN)
            s = exceptions::WARN;
        else
            s = exceptions::FATAL;

        throw exceptions::ConfigError(s, relrep._rt);
    }
}

BinaryId
BGMasterClient::alias_wait(
        const std::string& alias,
        unsigned timeout
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    const BGMasterClientProtocolSpec::Alias_waitRequest waitreq(alias, timeout);
    BGMasterClientProtocolSpec::Alias_waitReply waitrep;
    waitrep._rc = exceptions::OK;

    try {
        _prot->alias_wait(waitreq, waitrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    if (waitrep._rc != exceptions::OK) {
        throw exceptions::BGMasterError(exceptions::INFO, waitrep._rt);
    }

    return waitrep._binary_id;
}

void
BGMasterClient::event_monitor() const
{
    const BGMasterClientProtocolSpec::MonitorRequest monreq;
    BGMasterClientProtocolSpec::MonitorReply monrep;
    try {
        _prot->monitor(monreq, monrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    if (monrep._rc != exceptions::OK) {
        throw exceptions::BGMasterError(exceptions::INFO, monrep._rt);
    }

    // First, check the time stamps and interlace the messages from the reply
    // and then print them out.
    std::vector<std::string> all_messages;
    std::vector<std::string> sorted_msgs;
    // Note: The format of the messages is important. The "Event: " and "Error: " needs
    // to be there and be that length or the string parsification that happens later gets broken.
    BOOST_FOREACH(BGMasterClientProtocolSpec::MonitorReply::EventMessage curr_msg, monrep._eventmessages) {
        all_messages.push_back("Event: " + curr_msg._eventmsg);
    }
    BOOST_FOREACH(BGMasterClientProtocolSpec::MonitorReply::ErrorMessage curr_msg, monrep._errormessages) {
        all_messages.push_back("Error: " + curr_msg._errormsg);
    }

    // For each message, put it after the last message <= its time.
    BOOST_FOREACH(const std::string& curr_msg, all_messages) {
        using namespace boost::posix_time;
        const ptime curr_msg_time(time_from_string(curr_msg.substr(7,20)));
        bool inserted = false;
        BOOST_FOREACH(const std::string& inner_msg, sorted_msgs) {
            const ptime inner_msg_time(time_from_string(inner_msg.substr(7,20)));
            if (curr_msg_time > inner_msg_time) {
                // Stick it in the sorted vector after the inner message.
                sorted_msgs.insert(std::find(sorted_msgs.begin(), sorted_msgs.end(), inner_msg), curr_msg);
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            sorted_msgs.push_back(curr_msg);
        }
    }

    BOOST_REVERSE_FOREACH(std::string& curr_msg, sorted_msgs) {
        std::cout << curr_msg << std::endl;
    }

    // We'll use the same socket for getting the events that we used for the monitor message.
    // That means that we'll need to stop monitoring before we send an end message so we
    // don't confuse the end message reply with an event message.
    _prot->initializeResponder(_prot->getRequester());

    const BGMasterClient* this_p = this;
    BOOST_SCOPE_EXIT( (&this_p) ) {
        try {
            this_p->end_monitor();
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
    } BOOST_SCOPE_EXIT_END;

    // Now just monitor the socket forever.
    while (1) {
        // Now just sit on the socket forever.
        std::string message_name;
        try {
            _prot->getName(message_name);
        } catch (const CxxSockets::SoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Connection ended by server, will retry connection to bgmaster_server.");
            throw exceptions::BGMasterError(exceptions::FATAL, "bgmaster_server ended connection.");
        } catch (const CxxSockets::Error& err) {
            // Server aborted with an incomplete transmission
            if (err.errcode != 0) {
                LOG_WARN_MSG("Connection ended by bgmaster_server.");
                throw exceptions::BGMasterError(exceptions::FATAL, "bgmaster_server ended connection.");
            } else {
                LOG_INFO_MSG("Connection ended by bgmaster_server.");
                throw exceptions::BGMasterError(exceptions::FATAL, "bgmaster_server ended connection.");
            }
        }
        LOG_DEBUG_MSG("Message " << message_name << " received.");
        if (message_name == "EventMessage") {
            BGMasterClientProtocolSpec::EventMessage eventmsg;
            try {
                _prot->getObject(&eventmsg);
            } catch (const CxxSockets::SoftError& err) {
                // For soft errors, we just back out and let it try again
                LOG_INFO_MSG("Connection ended by server, will retry connection to bgmaster_server.");
            } catch (const CxxSockets::Error& err) {
                // Server aborted with an incomplete transmission
                LOG_WARN_MSG("Connection ended by bgmaster_server.");
            }
            std::cout << "Event: " << eventmsg._eventmsg << std::endl;
        } else if (message_name == "ErrorMessage") {
            BGMasterClientProtocolSpec::ErrorMessage errormsg;
            try {
                _prot->getObject(&errormsg);
            } catch (const CxxSockets::SoftError& err) {
                // For soft errors, we just back out and let it try again
                LOG_INFO_MSG("Connection ended by server, will retry connection to bgmaster_server.");
            } catch (const CxxSockets::Error& err) {
                // Server aborted with an incomplete transmission
                LOG_WARN_MSG("Connection ended by bgmaster_server.");
            }
            std::cout << "Error: " << errormsg._errormsg << std::endl;

        } else {
            throw exceptions::BGMasterError(exceptions::FATAL, "Invalid event type: " + message_name);
        }
    }
}

void
BGMasterClient::end_monitor() const
{
    const BGMasterClientProtocolSpec::EndmonitorRequest endmonreq;
    BGMasterClientProtocolSpec::EndmonitorReply endmonrep;
    try {
        _prot->endmonitor(endmonreq, endmonrep);
    } catch (const CxxSockets::SoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    }

    if (endmonrep._rc != exceptions::OK) {
        throw exceptions::BGMasterError(exceptions::INFO, endmonrep._rt);
    }
}

void
BGMasterClient::log_level(
        const std::vector<std::string>& input,
        std::map<LogName, LogLevel>& output
        ) const
{
    BGMasterClientProtocolSpec::LoglevelRequest loglevreq;
    BGMasterClientProtocolSpec::LoglevelReply loglevrep;
    // Load up the new log levels for the request.
    BOOST_FOREACH(const std::string& i, input) {
        loglevreq._loggers.push_back(i);
    }
    try {
        _prot->loglevel(loglevreq, loglevrep);
    } catch (const CxxSockets::Error& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    }

    if (loglevrep._rc != 0) {
        throw exceptions::InternalError(exceptions::FATAL, loglevrep._rt);
    }

    output.clear();
    BOOST_FOREACH(const BGMasterClientProtocolSpec::Logger& curr_logger, loglevrep._loggers) {
        output[curr_logger._name] = curr_logger._level;
    }
}
