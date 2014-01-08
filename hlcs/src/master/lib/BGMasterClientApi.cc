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

#include "BGMasterClientApi.h"

#include "exceptions.h"

#include "common/BinaryController.h"
#include "common/ClientProtocol.h"
#include "common/Ids.h"

#include <utility/include/Log.h>

#include <utility/include/cxxsockets/SocketTypes.h>

#include <utility/include/portConfiguration/PortConfiguration.h>

#include <xml/include/library/XML.h>

#include <boost/foreach.hpp>
#include <boost/date_time.hpp>
#include <boost/scope_exit.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <errno.h>

LOG_DECLARE_FILE( "master" );

ClientId
BGMasterClient::connectMaster(
        const bgq::utility::PortConfiguration::Pairs& portpairs
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    ClientProtocolPtr p(new ClientProtocol(_client_props));
    _prot = p;
    std::string connected_host = "";
    std::string connected_port = "";
    bool failed = true;
    std::ostringstream portstrings;

    BOOST_FOREACH(bgq::utility::PortConfiguration::Pair portpair, portpairs) {
        portstrings << portpair.first << ":" << portpair.second;
        try {
            _prot->initializeRequester(AF_UNSPEC, portpair.first, portpair.second, 2);
            connected_host = portpair.first;
            connected_port = portpair.second;
            LOG_DEBUG_MSG("Connected to " << portpair.first << ":" << portpair.second);
            failed = false;
            break;
        } catch (CxxSockets::SockSoftError& err) {
            LOG_WARN_MSG("Connection to bgmaster_server failed.");
        } catch (CxxSockets::CxxError& err) {
            LOG_WARN_MSG("Connection to bgmaster_server failed.");
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
    int local_port = la.getServicePort();
    BGMasterClientProtocolSpec::JoinRequest joinreq(la.getHostAddr(), local_port, "client", connected_host);
    BGMasterClientProtocolSpec::JoinReply joinrep;
    joinrep._rc = exceptions::OK;

    LOG_DEBUG_MSG("Sending join to " << connected_host << ":" << connected_port);
    try {
        _prot->join(joinreq, joinrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    LOG_DEBUG_MSG("Group joined");
    CxxSockets::SockAddr sa;
    _prot->getRequester()->getSockName(sa);
    ClientId id(sa.getServicePort(), sa.getHostName());
    return id;
}

BinaryId
BGMasterClient::start(
        const std::string& alias,
        const BGAgentId* loc
        )
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
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    BinaryId id(startrep._binary_id);
    if (startrep._rc != 0) { // Could not start!
        throw exceptions::InternalError(exceptions::FATAL, startrep._rt);
    }
    LOG_DEBUG_MSG("Received start reply, binary id " << startrep._binary_id);
    return id;
}

int
BGMasterClient::wait_for_terminate(
        BinaryId bid
        )
{
    LOG_TRACE_MSG(__FUNCTION__);

    BGMasterClientProtocolSpec::WaitRequest waitreq(bid);
    BGMasterClientProtocolSpec::WaitReply waitrep;
    waitrep._rc = exceptions::OK;

    LOG_DEBUG_MSG("Sending wait request " << bid.str());
    try {
        _prot->wait(waitreq, waitrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
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
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::vector<BinaryId> bv;
    std::vector<std::string> sv;
    sv.push_back(alias);
    return stop(bv, sv, signal, errormsg);
}

void
BGMasterClient::stop(
        const BinaryId& id,
        const int signal,
        std::string& errormsg
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::vector<std::string> sv;
    std::vector<BinaryId> bv;
    if (id.str() != "")
        bv.push_back(id);
    return stop(bv, sv, signal, errormsg);
}

void
BGMasterClient::stop(
        const std::vector<std::string>& aliases,
        const int signal,
        std::string& errormsg)
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::vector<BinaryId> bv;
    return stop(bv, aliases, signal, errormsg);
}

void
BGMasterClient::stop(
        const std::vector<BinaryId>& ids,
        const int signal,
        std::string& errormsg
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::vector<std::string> sv;
    return stop(ids, sv, signal, errormsg);
}

void
BGMasterClient::stop(
        const std::vector<BinaryId>& ids,
        const std::vector<std::string>& aliases,
        const int signal,
        std::string& errormsg
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::StopRequest stopreq(signal);
    stopreq._aliases = aliases;

    std::vector<BinaryId> idvec = ids;
    for (std::vector<BinaryId>::iterator it = idvec.begin();
        it != idvec.end(); ++it) {
        stopreq._binary_ids.push_back(it->str());
    }

    BGMasterClientProtocolSpec::StopReply stoprep(exceptions::OK, "success");
    try {
        _prot->stop(stopreq, stoprep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
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
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::Get_errorsRequest error_req;
    BGMasterClientProtocolSpec::Get_errorsReply error_rep(exceptions::OK, "success");
    try {
        _prot->get_errors(error_req, error_rep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }
    for(std::vector<std::string>::iterator it = error_rep._errors.begin();
        it != error_rep._errors.end(); ++it) {
        std::string es = *it;
        error_vec.push_back(es);
    }
}

void
BGMasterClient::get_history(
        std::vector<std::string>& history_vec
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::Get_historyRequest history_req;
    BGMasterClientProtocolSpec::Get_historyReply history_rep(exceptions::OK, "success");
    try {
        _prot->get_history(history_req, history_rep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }
    for (std::vector<std::string>::iterator it = history_rep._history.begin();
        it != history_rep._history.end(); ++it) {
        std::string es = *it;
        history_vec.push_back(es);
    }
}

int
BGMasterClient::master_status(
        std::string& start_time,
        std::string& version
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::MasterstatRequest statreq;
    BGMasterClientProtocolSpec::MasterstatReply statrep;
    statrep._rc = exceptions::OK;

    try {
        _prot->masterstat(statreq, statrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    int retval = -1;
    if (statrep._rc == exceptions::OK) {
        retval = statrep._pid;
        start_time = statrep._master_start_time;
        version = statrep._version;
    }
    return retval;
}

void
BGMasterClient::status(
        std::map<BinaryId,
        BinaryControllerPtr,
        Id::Comp >& stats
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::StatusRequest statreq;
    for (std::map<BinaryId, BinaryControllerPtr, Id::Comp >::iterator it = stats.begin();
        it != stats.end(); ++it) {
        BinaryId bid = it->first;
        statreq._binary_ids.push_back(bid.str());
    }

    BGMasterClientProtocolSpec::StatusReply statrep(exceptions::OK, "success");
    try {
        _prot->status(statreq, statrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    stats.clear();

    typedef BGMasterClientProtocolSpec::StatusReply::BinaryController BinCont;
    for (std::vector<BinCont>::iterator binit = statrep._binaries.begin();
        binit != statrep._binaries.end(); ++binit) {
        BinCont bincont = *binit;
        BinaryId bid(bincont._binary_id);
        BinaryController::Status stat = (BinaryController::Status)(bincont._status);
        BinaryControllerPtr binary(new BinaryController(bid,
                                                        bincont._binary_name,
                                                        bincont._alias,
                                                        bincont._user,
                                                        bincont._exit_status,
                                                        stat,
                                                        bincont._start_time));

        stats[bid] = binary;
    }
}

void
BGMasterClient::exit_status(
        std::map<BGAgentId,
        BinaryControllerPtr,
        Id::Comp >& statuses
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::ExitStatusRequest exitreq;
    BGMasterClientProtocolSpec::ExitStatusReply exitrep(exceptions::OK, "success");

    // If some agent ids are passed in, we need to send them
    if (!statuses.empty()) {
        typedef std::pair<const BGAgentId, BinaryControllerPtr>& agent_pair;
        BOOST_FOREACH(agent_pair mypair, statuses) {
            BGAgentId id = mypair.first;
            exitreq._agent_id.push_back( id );
        }
    }

    statuses.clear();  // Get rid of anything currently in the map

    // Send the request
    try {
        _prot->exit_status(exitreq, exitrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    // Pull out the agents and statuses and put them in the map
    typedef BGMasterClientProtocolSpec::ExitStatusReply::Agent ReplyAgent;
    for (std::vector<ReplyAgent>::iterator agent_it = exitrep._agent.begin();
        agent_it != exitrep._agent.end(); ++agent_it) {
        ReplyAgent agent = (*agent_it);
        BGAgentId id = agent._agent_id;

        typedef BGMasterClientProtocolSpec::ExitStatusReply::Agent::Binary ReplyBinary;
        ReplyBinary rep_bin = agent._binary;
        BinaryController::Status stat = BinaryController::string_to_status(rep_bin._status);

        BinaryId bid(rep_bin._binary_id);

        BinaryControllerPtr
            binary(new BinaryController(bid,
                                        rep_bin._name,
                                        rep_bin._alias,
                                        rep_bin._user,
                                        rep_bin._exit_status,
                                        stat));
        statuses[id] = binary;
    }
}

void
BGMasterClient::idle_aliases(
        std::vector<std::string>& aliases
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::GetidleRequest idlereq;
    BGMasterClientProtocolSpec::GetidleReply idlerep(exceptions::OK, "success");
    try {
        _prot->getidle(idlereq, idlerep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }
    BOOST_FOREACH(std::string al, idlerep._aliases) {
        aliases.push_back(al);
    }
}

void
BGMasterClient::get_agents(
        std::map<BGAgentId,
        std::vector<BinaryControllerPtr>,
        Id::Comp >& amap
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::AgentlistRequest agentreq;
    BGMasterClientProtocolSpec::AgentlistReply agentrep(exceptions::OK, "success");
    try {
        _prot->agentlist(agentreq, agentrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    // Must iterate through this mess and create the map.
    // First, iterate through the agents.
    typedef BGMasterClientProtocolSpec::AgentlistReply::Agent ReplyAgent;
    for (std::vector<ReplyAgent>::iterator agent_it = agentrep._agent.begin();
        agent_it != agentrep._agent.end(); ++agent_it) {
        // Get the id
        BGAgentId id = (*agent_it)._agent_id;
        ReplyAgent agent = (*agent_it);
        // Now build the vector of binaries
        typedef BGMasterClientProtocolSpec::AgentlistReply::Agent::Binary ReplyBinary;
        std::vector<BinaryControllerPtr> binvec;
        for (std::vector<ReplyBinary>::iterator bin_it = agent._binaries.begin();
            bin_it != agent._binaries.end(); ++bin_it) {
            // Now build our binary type from the reply's
            ReplyBinary rep_bin = (*bin_it);
            LOG_DEBUG_MSG("Found binary " << rep_bin._binary_id << "|" << rep_bin._name << "|" << rep_bin._status);
            BinaryControllerPtr  base(new BinaryController(rep_bin._binary_id,
                                          rep_bin._name,
                                          rep_bin._alias,
                                          rep_bin._user,
                                          rep_bin._exit_status,
                                          rep_bin._status,
                                          rep_bin._start_time));
            // Stick it in our vector
            binvec.push_back(base);
        }
        // Now stick our vector in our map
        amap[id] = binvec;
    }
}

void
BGMasterClient::get_clients(
        ClientAndUserMap& clients
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::ClientsRequest clientreq;
    BGMasterClientProtocolSpec::ClientsReply clientrep(exceptions::OK, "success");
    try {
        _prot->clients(clientreq, clientrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    typedef std::vector<BGMasterClientProtocolSpec::ClientsReply::Client>::iterator ClientIterator;
    for (ClientIterator it = clientrep._clients.begin(); it != clientrep._clients.end(); ++it) {
        ClientId id(it->_client_id);
        ClientUID uid(it->_user_id);
        clients[id] = uid;
    }
}

void
BGMasterClient::end_agent(
        BGAgentId& agent
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::vector<BGAgentId> agents;
    agents.push_back(agent);
    end_agent(agents);
}

void
BGMasterClient::end_agent(
        std::vector<BGAgentId>& agents
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::End_agentRequest diereq;
    BGMasterClientProtocolSpec::End_agentReply dierep(exceptions::OK, "success");
    for (std::vector<BGAgentId>::iterator it = agents.begin();
        it != agents.end(); ++it) {
        diereq._agent_ids.push_back(*it);
    }

    try {
        _prot->end_agent(diereq, dierep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    agents.clear();

    for (std::vector<std::string>::iterator it = dierep._agent_ids.begin(); it != dierep._agent_ids.end(); ++it) {
        BGAgentId a(*it);
        agents.push_back(a);
    }

    if (dierep._rc != exceptions::OK) {
        LOG_DEBUG_MSG("Bad return code from bgmaster_server.");
        throw exceptions::APICommandError(exceptions::INFO, dierep._rt);
    }
}

void
BGMasterClient::end_master(
        bool master_only,
        unsigned signal
        )
{
    // Note that this just requests a graceful end to the bgmaster_server
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::TerminateRequest termreq(master_only, signal);
    BGMasterClientProtocolSpec::TerminateReply termrep(exceptions::OK, "success");
    try {
        _prot->terminate(termreq, termrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }
}

void
BGMasterClient::fail_over(
        std::vector<BinaryId>& bins,
        const std::string& trigger
        )
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
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    // Empty the vector
    bins.clear();
    // Now go through the fail replies and put all of the failed failovers in the vector
    typedef BGMasterClientProtocolSpec::FailoverReply::BinaryStatus BinStat;
    BOOST_FOREACH(BinStat& bs, failrep._statuses) {
        BinaryId bid(bs._binary_id);
        bins.push_back(bid);
    }

    if (failrep._rc != exceptions::OK) {
        throw exceptions::ConfigError(exceptions::WARN, failrep._rt);
    }
}

void
BGMasterClient::reload_config(
        const std::string& config_file
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::ReloadRequest relreq;
    if (!config_file.empty())
        relreq._config_file = config_file;

    BGMasterClientProtocolSpec::ReloadReply relrep(exceptions::OK, "success");

    try {
        _prot->reload(relreq, relrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::FATAL, "Connection to bgmaster_server ended.");
    }

    if (relrep._rc != exceptions::OK) {
        exceptions::Severity s;
        if (relrep._rc == exceptions::INFO)
            s = exceptions::INFO;
        else if(relrep._rc == exceptions::WARN)
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
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BinaryId id;
    BGMasterClientProtocolSpec::Alias_waitRequest waitreq(alias, timeout);
    BGMasterClientProtocolSpec::Alias_waitReply waitrep;
    waitrep._rc = exceptions::OK;

    try {
        _prot->alias_wait(waitreq, waitrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    if (waitrep._rc != exceptions::OK) {
        throw exceptions::BGMasterError(exceptions::INFO, waitrep._rt);
    } else {
        id = waitrep._binary_id;
    }
    return id;
}

void
BGMasterClient::event_monitor()
{
    BGMasterClientProtocolSpec::MonitorRequest monreq;
    BGMasterClientProtocolSpec::MonitorReply monrep;
    try {
        _prot->monitor(monreq, monrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::WARN, "Connection to bgmaster_server failed.");
    }

    if (monrep._rc != exceptions::OK) {
        throw exceptions::BGMasterError(exceptions::INFO, monrep._rt);
    } else {
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
        BOOST_FOREACH(std::string& curr_msg, all_messages) {
            using namespace boost::posix_time;
            ptime curr_msg_time(time_from_string(curr_msg.substr(7,20)));
            bool inserted = false;
            BOOST_FOREACH(std::string& inner_msg, sorted_msgs) {
                ptime inner_msg_time(time_from_string(inner_msg.substr(7,20)));
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

        BGMasterClient* this_p = this;
        BOOST_SCOPE_EXIT( (&this_p) ) {
            try {
                this_p->end_monitor();
            } catch(std::runtime_error& e) {
                std::cerr << e.what() << std::endl;
            }
        } BOOST_SCOPE_EXIT_END;

        // Now just monitor the socket forever.
        while (!_ending) {
            // Now just sit on the socket forever.
            std::string message_name = "";
            try {
                _prot->getName(message_name);
            } catch(CxxSockets::SockSoftError& err) {
                // For soft errors, we just back out and let it try again
                LOG_INFO_MSG("Connection ended by server, will retry connection to bgmaster_server.");
                throw exceptions::BGMasterError(exceptions::FATAL, "bgmaster_server ended connection.");
            } catch(CxxSockets::CxxError& err) {
                // Server aborted with an incomplete transmission
                if (err.errcode != 0) {
                    LOG_WARN_MSG("Connection ended by bgmaster_server.");
                    throw exceptions::BGMasterError(exceptions::FATAL, "bgmaster_server ended connection.");
                } else {
                    LOG_INFO_MSG("Connection ended by bgmaster_server.");
                    throw exceptions::BGMasterError(exceptions::FATAL, "bgmaster_server ended connection.");
                }
            }
            LOG_DEBUG_MSG("-*-Message " << message_name << " received.-*-");
            if (message_name == "EventMessage") {
                BGMasterClientProtocolSpec::EventMessage eventmsg;
                try {
                    _prot->getObject(&eventmsg);
                } catch(CxxSockets::SockSoftError& err) {
                    // For soft errors, we just back out and let it try again
                    LOG_INFO_MSG("Connection ended by server, will retry connection to bgmaster_server.");
                } catch(CxxSockets::CxxError& err) {
                    // Server aborted with an incomplete transmission
                    LOG_WARN_MSG("Connection ended by bgmaster_server.");
                }
                std::cout << "Event: " << eventmsg._eventmsg << std::endl;
            } else if (message_name == "ErrorMessage") {
                BGMasterClientProtocolSpec::ErrorMessage errormsg;
                try {
                    _prot->getObject(&errormsg);
                } catch(CxxSockets::SockSoftError& err) {
                    // For soft errors, we just back out and let it try again
                    LOG_INFO_MSG("Connection ended by server, will retry connection to bgmaster_server.");
                } catch(CxxSockets::CxxError& err) {
                    // Server aborted with an incomplete transmission
                    LOG_WARN_MSG("Connection ended by bgmaster_server.");
                }
                std::cout << "Error: " << errormsg._errormsg << std::endl;

            } else {
                throw exceptions::BGMasterError(exceptions::FATAL, "Invalid event type: " + message_name);
            }
        }
    }
}

void
BGMasterClient::end_monitor()
{
    BGMasterClientProtocolSpec::EndmonitorRequest endmonreq;
    BGMasterClientProtocolSpec::EndmonitorReply endmonrep;
    try {
        _prot->endmonitor(endmonreq, endmonrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    }

    if (endmonrep._rc != exceptions::OK) {
        throw exceptions::BGMasterError(exceptions::INFO, endmonrep._rt);
    }
}

void
BGMasterClient::log_level(
        std::map<LogName,
        LogLevel>& loglevels
        )
{
    BGMasterClientProtocolSpec::LoglevelRequest loglevreq;
    BGMasterClientProtocolSpec::LoglevelReply loglevrep;
    typedef std::pair<LogName, LogLevel> logpair;
    // Load up the new log levels for the request.
    BOOST_FOREACH(logpair curr_pair, loglevels) {
        BGMasterClientProtocolSpec::Logger logger;
        logger._name = curr_pair.first;
        logger._level = curr_pair.second;
        loglevreq._loggers.push_back(logger);
    }
    try {
        _prot->loglevel(loglevreq, loglevrep);
    } catch (CxxSockets::SockSoftError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    } catch (CxxSockets::CxxError& err) {
        throw exceptions::CommunicationError(exceptions::INFO, "Connection to bgmaster_server failed.");
    }
    loglevels.clear(); // Clear out the ones they sent us so we can return the new ones.
    BOOST_FOREACH(BGMasterClientProtocolSpec::Logger& curr_logger, loglevrep._loggers) {
        loglevels[curr_logger._name] = curr_logger._level;
    }
}

void
BGMasterClient::set_ending(
        bool tf
        )
{
    _ending = tf;
    XML::Parser::setstopping(pthread_self());
}
