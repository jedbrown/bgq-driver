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
#include "Alias.h"
#include "AliasList.h"
#include "ClientController.h"
#include "ClientManager.h"
#include "MasterController.h"
#include "ras.h"

#include "../lib/exceptions.h"

#include "common/BinaryController.h"

#include <utility/include/LoggingProgramOptions.h>




#include <pthread.h>
#include <signal.h>
#include <unistd.h>

LOG_DECLARE_FILE( "master" );

ClientController::~ClientController()
{
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
}

ClientController::ClientController(
        const ClientProtocolPtr& prot,
        const std::string& ipaddr,
        const int port,
        const CxxSockets::UserType utype
        ) :
    _prot( prot ),
    _ending( false ),
    _client_id( port, ipaddr ),
    _utype( utype )
{
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_TRACE_MSG(__FUNCTION__);

    // Send a Join reply indicating that we are satisfied the handshaking is complete.
    const BGMasterAgentProtocolSpec::JoinReply rep(exceptions::OK, "join successful", "", "", "");
    _prot->sendReply(rep.getClassName(), rep);
    LOG_DEBUG_MSG("Sent join reply to " << ipaddr << ":" << port);
}

void
ClientController::doStartRequest(
        const BGMasterClientProtocolSpec::StartRequest& startreq
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BinaryId id;
    BGMasterClientProtocolSpec::StartReply clientrep(exceptions::OK, "started servers", id);
    if (_utype != CxxSockets::Administrator) {
        clientrep._rc = exceptions::FATAL;
        clientrep._rt = "Authorization failure. Client did not present Administrator certificate.";
        MasterController::handleErrorMessage(clientrep._rt);
        std::map<std::string, std::string> details;
        details["USER"] = "unknown";
        details["COMMAND"] = "master_start";
        MasterController::putRAS(AUTHORITY_FAIL_RAS, details);
    } else {
        if (startreq._alias.empty()) {
            // Not an error! This tells us to start 'em all up.
            std::map<std::string, std::string> failed_aliases;
            AgentRepPtr np;
            MasterController::startServers(failed_aliases, np);
            if (!failed_aliases.empty()) {
                clientrep._rc = exceptions::WARN;
                std::ostringstream failure;
                if (failed_aliases.size() == 1) {
                    failure << "Alias ";
                } else {
                    failure << "Aliases ";
                }
                for (std::map<std::string, std::string>::const_iterator i = failed_aliases.begin(); i != failed_aliases.end(); ++i) {
                    failure << i->first << " ";
                }

                failure << "did not start.";
                clientrep._rt = failure.str();
            }
            try {
                _prot->sendReply(clientrep.getClassName(), clientrep);
            } catch (const CxxSockets::SoftError& err) {
                LOG_ERROR_MSG("Client connection error during start reply.");
            } catch (const CxxSockets::Error& err) {
                // Client aborted with an incomplete transmission
                LOG_ERROR_MSG("Client connection ended during start reply.");
            }

            return;
        }

        BinaryId bid;

        // Find the alias
        AliasPtr al;
        if (MasterController::_aliases.find_alias(startreq._alias, al)) {
            try {
                const BGAgentId aid(startreq._agent_id);
                const AgentRepPtr p = al->validateStartAgent(aid);
                if (p) {
                    const BGMasterAgentProtocolSpec::StartRequest agentreq(
                            al->get_path(), 
                            al->get_args(), 
                            al->get_logdir(), 
                            al->get_name(), 
                            al->get_user()
                            );
                    BGMasterAgentProtocolSpec::StartReply agentrep;
                    agentrep._rc = exceptions::OK;

                    al->resetRetries();

                    std::ostringstream logmsg;
                    logmsg << "start request path=" << agentreq._path << " "
                        << "arguments=" << agentreq._arguments << " "
                        << "logdir=" << al->get_logdir() << " "
                        << "user=" << agentreq._user << " ";
                    LOG_TRACE_MSG(logmsg.str());

                    bid = p->startBin(agentreq, agentrep);
                    if (bid.str() != "0") {
                        al->add_binary(bid);
                    } else {
                        // We have a failure, note it and put it in the reply
                        clientrep._rc = agentrep._rc;
                        clientrep._rt = agentrep._rt;
                    }
                }
            } catch (const exceptions::InternalError& e) {
                // Policy error
                std::ostringstream msg;
                msg << "Policy error for alias " << startreq._alias << ". " << e.what() << " Correct configuration and restart or refresh.";
                MasterController::handleErrorMessage(msg.str());
                const BinaryId id("0");
                const BGMasterClientProtocolSpec::StartReply failrep(e.errcode, msg.str(), id);
                try {
                    _prot->sendReply(failrep.getClassName(), failrep);
                } catch (const CxxSockets::SoftError& err) {
                    LOG_ERROR_MSG("Client connection error during start reply.");
                } catch (const CxxSockets::Error& err) {
                    // Client aborted with an incomplete transmission
                    LOG_ERROR_MSG("Client connection ended during start reply.");
                }

                return;
            }

            // If we got here, we're good.
            LOG_DEBUG_MSG("Alias " << startreq._alias << " cleared against policy to run.");
            clientrep._binary_id = bid.str();
        } else {
            // alias not found
            clientrep._rc = exceptions::WARN;
            clientrep._rt = "Specified alias not found";
        }
    }
    try {
        _prot->sendReply(clientrep.getClassName(), clientrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during start reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during start reply.");
    }
}

void
ClientController::doAgentRequest(
        const BGMasterClientProtocolSpec::AgentlistRequest& /* agentreq */
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    // Got an agent request. Build up a reply which means going through all of the agents and all of their
    // binaries, and shoving it in the message. First, iterate through the agent manager:
    BGMasterClientProtocolSpec::AgentlistReply agentrep(exceptions::OK, "success");
    BOOST_FOREACH(const AgentRepPtr& agent, MasterController::get_agent_manager().get_agent_list()) {
        // Build our corresponding protocol agent object
        BGMasterClientProtocolSpec::AgentlistReply::Agent reply_agent(agent->get_agent_id());

        // Now iterate through binaries and add them. This gets a COPY of the binary list which could change, but that's OK.
        std::vector<BinaryControllerPtr> binaries = agent->get_binaries();
        BOOST_FOREACH(const BinaryControllerPtr& binary, binaries) {
            typedef BGMasterClientProtocolSpec::AgentlistReply::Agent::Binary AgentBin;
            LOG_DEBUG_MSG("Found binary " << binary);
            const std::string t = boost::posix_time::to_simple_string(binary->get_start_time());
            const AgentBin bin(
                    binary->get_status(),
                    binary->get_exit_status(),
                    binary->get_binary_bin_path(),
                    binary->get_alias_name(),
                    binary->get_user(),
                    binary->get_binid().str(),
                    t
                    );
            reply_agent._binaries.push_back(bin);
        }

        // Now we've filled out an agent and binaries, push it on the vector
        agentrep._agent.push_back(reply_agent);
    }

    // Our reply should be complete now.  Return it.
    try {
        _prot->sendReply(agentrep.getClassName(), agentrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during list agents reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during list agents reply.");
    }
}

void
ClientController::doClientsRequest(
        const BGMasterClientProtocolSpec::ClientsRequest& /* clientreq */
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    // Spin through the client controllers and add them to the reply
    BGMasterClientProtocolSpec::ClientsReply clientrep(exceptions::OK, "success");
    typedef std::vector<ClientControllerPtr> ClientVec;
    const ClientVec cv = MasterController::get_client_manager().getClients();
    LOG_DEBUG_MSG("Found " << cv.size() << " clients.");
    for (ClientVec::const_iterator ci = cv.begin(); ci != cv.end(); ++ci) {
        LOG_DEBUG_MSG("Sending client " << (*ci)->get_client_id().str() << ".");
        BGMasterClientProtocolSpec::ClientsReply::Client ct;
        ct._client_id = (*ci)->get_client_id().str();
        ct._user_id = (*ci)->getUserId();
        clientrep._clients.push_back(ct);
    }
    try {
        _prot->sendReply(clientrep.getClassName(), clientrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during list clients reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during list clients reply.");
    }
}

void
ClientController::doWaitRequest(
        const BGMasterClientProtocolSpec::WaitRequest& waitreq
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " " << waitreq._binary_id);
    // Get the binary id and then we'll loop until we can't find the controller any more.
    const BinaryId reqbid(waitreq._binary_id);

    // Now find the associated binary controller. We'll have a smart
    // pointer so it can't go away, but the status can change.
    BinaryLocation loc;
    bool found = MasterController::get_agent_manager().findBinary(reqbid, loc);

    BGMasterClientProtocolSpec::WaitReply waitrep;
    waitrep._rc = exceptions::OK;

    BGMasterClientProtocolSpec::WaitReply::BinaryStatus binstat;

    if (found) {
        const BinaryControllerPtr bcptr = loc.first;

        boost::unique_lock<boost::mutex> ulock(bcptr->_status_lock);
        while (bcptr->get_status() == BinaryController::RUNNING && !_ending) {
            // As long as it is RUNNING, we wait
            bcptr->_status_notifier.wait(ulock);
        }

        if (!_ending) {
            const int estatus = bcptr->get_exit_status();
            // We're done with it
            binstat = BGMasterClientProtocolSpec::WaitReply::BinaryStatus(reqbid.str(), estatus);
            waitrep._rc = exceptions::OK;
            waitrep._rt = "success";
        } else {
            binstat = BGMasterClientProtocolSpec::WaitReply::BinaryStatus(reqbid.str(), 0);
            waitrep._rc = exceptions::WARN;
            waitrep._rt = "bgmaster_server ending";
        }
    } else {
        binstat = BGMasterClientProtocolSpec::WaitReply::BinaryStatus("0", 0);
        waitrep._rc = exceptions::INFO;
        waitrep._rt = "Specified binary not found.";
    }

    // Out of the loop. We're done now. Send a reply.
    waitrep._status = binstat;
    try {
        _prot->sendReply(waitrep.getClassName(), waitrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during wait reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during wait reply.");
    }
}

void
ClientController::doStopRequest(
        const BGMasterClientProtocolSpec::StopRequest& stopreq
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::StopReply reply_to_client(exceptions::OK, "stopped");

    LOG_INFO_MSG(
            "Stop request for " << stopreq._binary_ids.size() << " ids and " << stopreq._aliases.size() <<
            " binary names with signal " << stopreq._signal
            );
    unsigned binsfound = 0;
    if (_utype != CxxSockets::Administrator) {
        reply_to_client._rc = exceptions::FATAL;
        reply_to_client._rt = "Authorization failure. Client did not present Administrator certificate.";
        MasterController::handleErrorMessage(reply_to_client._rt);
        std::map<std::string, std::string> details;
        details["USER"] = "unknown";
        details["COMMAND"] = "master_stop";
        MasterController::putRAS(AUTHORITY_FAIL_RAS, details);
    } else {
        // We can have a list of aliases or specific binary ids to stop. Do both.
        if (stopreq._binary_ids.size() == 0 && stopreq._aliases.size() == 0) {
            reply_to_client._rt = "stopped all binaries";
            // Stop them all if nothing is passed
            ++binsfound;  // Assume at least one is running.
            std::vector<AgentRepPtr> agents = MasterController::get_agent_manager().get_agent_list();
            BOOST_FOREACH(const AgentRepPtr& rep, agents) {
                if (rep) // The iterator may have gone stale.  Check the rep ptr.
                    rep->stopAllBins(reply_to_client, stopreq._signal);
            }
        }

        // First go through all of the binary ids specified for stoppage
        for (std::vector<std::string>::const_iterator idit = stopreq._binary_ids.begin(); idit != stopreq._binary_ids.end(); ++idit) {
            const BinaryId bid((*idit));

            LOG_INFO_MSG("Processing stop request for binary id " << bid.str());

            // Find the binary in the agents
            BinaryLocation location;

            bool found = MasterController::get_agent_manager().findBinary(bid, location);

            if (!found) {
                LOG_DEBUG_MSG("No binary by id " << bid.str() << " found.");
                // break; // None here, we'll check by alias
            } else {
                ++binsfound;
                BGMasterAgentProtocolSpec::StopReply stop_from_agent;
                stop_from_agent._rc = exceptions::OK;

                location.second->stopBin(bid, location, stopreq._signal, stop_from_agent, false);

                // Now collect the response and add it to the reply
                const BGMasterClientProtocolSpec::StopReply::BinaryStatus binstat_to_return(
                        stop_from_agent._status._binary_id,
                        stop_from_agent._status._exit_status
                        );

                reply_to_client._statuses.push_back(binstat_to_return);
                if (stop_from_agent._rc != exceptions::OK) {
                    reply_to_client._rc = stop_from_agent._rc;
                    reply_to_client._rt = stop_from_agent._rt;
                }
            }
        }

        // Now go through all of the aliases and find and stop each binary associated with the alias supplied.
        for (std::vector<std::string>::const_iterator ai = stopreq._aliases.begin(); ai != stopreq._aliases.end(); ++ai) {
            // First, find all of the binaries associated with this alias
            std::vector<BinaryLocation> locations;
            const bool found = MasterController::get_agent_manager().findBinary(*ai, locations);

            if (!found) {
                LOG_INFO_MSG("No binaries for alias " << *ai << " found.");
                // break;
            } else {
                ++binsfound;
                LOG_INFO_MSG("Found " << locations.size() << " binaries associated with alias " << *ai);

                // Now loop through the vector of locations associated with this alias and send a stop request to each agent.
                for (std::vector<BinaryLocation>::const_iterator li = locations.begin(); li != locations.end(); ++li) {
                    // Extract the agent and binary id from the location
                    const AgentRepPtr rep = li->second;
                    const BinaryId bid = li->first->get_binid();

                    // Build the stop request and reply that go to the agent
                    BGMasterAgentProtocolSpec::StopReply stop_from_agent;
                    stop_from_agent._rc = exceptions::OK;

                    LOG_INFO_MSG("Sending stop request for alias " << *ai << " to agent " << rep->get_agent_id().str());

                    rep->stopBin(bid, *li, stopreq._signal, stop_from_agent, false);

                    // Now collect the response and add it to the reply
                    const BGMasterClientProtocolSpec::StopReply::BinaryStatus binstat_to_return(
                            stop_from_agent._status._binary_id,
                            stop_from_agent._status._exit_status
                            );

                    reply_to_client._statuses.push_back(binstat_to_return);
                    if (stop_from_agent._rc != exceptions::OK) {
                        reply_to_client._rc = stop_from_agent._rc;
                        reply_to_client._rt = stop_from_agent._rt;
                    }
                }
            }
        }
    }

    if (!binsfound) {
        reply_to_client._rc = exceptions::INFO;
        reply_to_client._rt = "No matching binary found";
    }

    try {
        _prot->sendReply(reply_to_client.getClassName(), reply_to_client);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during stop reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during stop reply.");
    }
}

void
ClientController::doStatusRequest(
        const BGMasterClientProtocolSpec::StatusRequest& statusreq
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::StatusReply statusrep(exceptions::OK, "success");

    typedef BGMasterClientProtocolSpec::StatusReply::BinaryController BinCont;

    // If binary ids are specified, must find them all and get their status. Else, we just need to get ALL of them.
    if (statusreq._binary_ids.size() > 0) {
        for (std::vector<std::string>::const_iterator idit = statusreq._binary_ids.begin(); idit != statusreq._binary_ids.end(); ++idit) {
            const BinaryId id(*idit);

            BinaryLocation location;
            const bool found = MasterController::get_agent_manager().findBinary(id, location);
            if (found) {
                const BinaryControllerPtr pbase = location.first;
                if (pbase->valid()) {
                    const std::string t = boost::posix_time::to_simple_string(pbase->get_start_time());
                    const BinCont bin(
                            pbase->get_exit_status(),
                            pbase->get_binid().str(),
                            pbase->get_binary_bin_path(), 
                            pbase->get_alias_name(), 
                            pbase->get_user(),
                            pbase->get_status(),
                            t
                            );
                    statusrep._binaries.push_back(bin);
                }
            }
        }
    } else {
        // Need them ALL. Loop through agents.
        const std::vector<AgentRepPtr> agents = MasterController::get_agent_manager().get_agent_list();
        BOOST_FOREACH(const AgentRepPtr& rep, agents) {
            if (!rep) {
                continue;
            }
            const std::vector<BinaryControllerPtr> binaries = rep->get_binaries();
            // Now loop through binaries
            BOOST_FOREACH(const BinaryControllerPtr& pbase, binaries) {
                if (pbase->valid()) {
                    const std::string t = boost::posix_time::to_simple_string(pbase->get_start_time());
                    const BinCont bin(
                            pbase->get_exit_status(),
                            pbase->get_binid().str(),
                            pbase->get_binary_bin_path(), 
                            pbase->get_alias_name(), 
                            pbase->get_user(),
                            pbase->get_status(),
                            t
                            );
                    statusrep._binaries.push_back(bin);
                }
            }
        }
    }
    try {
        _prot->sendReply(statusrep.getClassName(), statusrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during status reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during status reply.");
    }
}

void
ClientController::doTermRequest(
        const BGMasterClientProtocolSpec::TerminateRequest& termreq
        )
{
    LOG_TRACE_MSG(__FUNCTION__);

    BGMasterClientProtocolSpec::TerminateReply termrep(exceptions::OK, "success");
    
    // If we are already in the process of terminating we don't want to start the termination process again.
    if ( MasterController::get_end_requested() ) {
        termrep._rc = exceptions::WARN;
        termrep._rt = "bgmaster_server already terminating from previous request";
    } else {
        termrep._rc = exceptions::OK;
        termrep._rt = "terminate request started";
    }

    try {
        LOG_INFO_MSG(termrep._rt);
        _prot->sendReply(termrep.getClassName(), termrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during terminate reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during terminate reply.");
    }

    // Go no further if a secondary termination request to avoid undefined behavior.
    if ( MasterController::get_end_requested() ) return;

    bool end_binaries = true;
    if (termreq._master_only) {
        end_binaries = false;
    }
    MasterController::set_end_requested();
    MasterController::stopThreads(end_binaries, termreq._signal);  // Not ending agents
    MasterController::set_master_terminating();
}

void
ClientController::doReloadRequest(
        const BGMasterClientProtocolSpec::ReloadRequest& relreq
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::ReloadReply relrep(exceptions::OK, "Successfully reloaded configuration. ");

    bool skip_policies = false;
    if (relreq._config_file.empty()) {
        LOG_INFO_MSG("No configuration file specified, reloading default.");
        MasterController::getProps()->reload();
    } else {
        // New config file
        LOG_INFO_MSG("Reload request for configuration file " << relreq._config_file);
        bgq::utility::Properties::Ptr newprops;
        try {
            if (!MasterController::getProps()->reload(relreq._config_file)) {
                throw std::runtime_error("Properties file reload failed.");
            }
        } catch (const std::runtime_error& e) {
            std::ostringstream error;
            error << "Error reloading configuration file: " << e.what();
            MasterController::handleErrorMessage(error.str());
            relrep._rc = -1;
            relrep._rt = error.str();
            skip_policies = true;
        } catch (const std::invalid_argument& e) {
            std::ostringstream error;
            error << "Error reading configuration file, verify file exists.";
            MasterController::handleErrorMessage(error.str());
            relrep._rc = -1;
            relrep._rt = error.str();
            skip_policies = true;
        }
    }

    // Now build it back up.
    if (skip_policies == false) {
        std::ostringstream failmsg;
        try {
            MasterController::buildPolicies(failmsg);
        } catch (const exceptions::ConfigError& e) {
            relrep._rc = e.errcode;
            relrep._rt = e.what();
        } catch (const std::runtime_error& e) {
            // Likely one of Properties' exceptions
            relrep._rc = exceptions::WARN;
            relrep._rt = e.what();
        }

        if (!failmsg.str().empty()) {
            if (relrep._rc == exceptions::OK)
                relrep._rc = exceptions::WARN;
            relrep._rt += failmsg.str();
        }
    }

    try {
        _prot->sendReply(relrep.getClassName(), relrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during reload reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during reload reply.");
    }
}

void
ClientController::doFailRequest(
        const BGMasterClientProtocolSpec::FailoverRequest& failreq
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " " << failreq._binary_ids[0]);
    BGMasterClientProtocolSpec::FailoverReply failrep(exceptions::OK, "success");
    const std::string trigger = failreq._trigger;
    if (_utype != CxxSockets::Administrator) {
        failrep._rc = exceptions::FATAL;
        failrep._rt = "Authorization failure. Client did not present Administrator certificate.";
        MasterController::handleErrorMessage(failrep._rt);
        std::map<std::string, std::string> details;
        details["USER"] = "unknown";
        details["COMMAND"] = "fail_over";
        MasterController::putRAS(AUTHORITY_FAIL_RAS, details);
    } else {
        BOOST_FOREACH(const std::string& strbid, failreq._binary_ids) {
            // For every bin in the list, do the failover action, update
            // the status in the reply, and return it.
            // First, find out if we still have the binary.  If we do,
            // send a stop request.  Then, have the agent handle the policy.
            BinaryLocation location;
            BinaryId binid(strbid);
            if (MasterController::get_agent_manager().findBinary(binid, location)) {
                // Got it, kill it!
                BGMasterAgentProtocolSpec::StopReply stoprep;
                stoprep._rc = exceptions::OK;

                AgentRepPtr rep = location.second;
                try {
                    rep->stopBinaryAndExecutePolicy(binid, location, SIGUSR2, stoprep, trigger);
                    if (stoprep._rc == exceptions::OK) {
                        failrep._rc = stoprep._rc;
                        failrep._rt = "success";
                    }
                } catch (const exceptions::ConfigError& e) {
                    LOG_WARN_MSG("Policy execution failed. Check configuration");
                    BGMasterClientProtocolSpec::FailoverReply::BinaryStatus binstat(strbid, BinaryController::UNINITIALIZED);
                    failrep._statuses.push_back(binstat);
                    failrep._rc = exceptions::WARN;
                    failrep._rt = e.what();
                }
            } else {
                // Binary no longer exists.  We're going to send a reply that
                // says so because we won't fail over something that wasn't there
                // in the first place.
                BGMasterClientProtocolSpec::FailoverReply::BinaryStatus binstat(strbid, BinaryController::UNINITIALIZED);
                failrep._statuses.push_back(binstat);
                failrep._rc = exceptions::INFO;
                failrep._rt = "A specified binary does not exist.";
            }
        }
    }
    try {
        _prot->sendReply(failrep.getClassName(), failrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during fail-over reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during fail-over reply.");
    }
}

void
ClientController::doMasterStatRequest(
        const BGMasterClientProtocolSpec::MasterstatRequest& /* statreq */
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    const BGMasterClientProtocolSpec::MasterstatReply statrep(
            exceptions::OK,
            std::string(),
            getpid(),
            boost::posix_time::to_simple_string(MasterController::get_start_time()),
            MasterController::_version_string,
            MasterController::getProps()->getFilename()
            );
    try {
        _prot->sendReply(statrep.getClassName(), statrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during status reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during status reply.");
    }
}

void
ClientController::doAliasWaitRequest(
        const BGMasterClientProtocolSpec::Alias_waitRequest& waitreq
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::Alias_waitReply waitrep;
    waitrep._rc = exceptions::OK;

    // Find the matching alias in the alias list.
    AliasPtr al;
    const bool found = MasterController::_aliases.find_alias(waitreq._alias, al);
    unsigned timeout = waitreq._timeout;
    BinaryId id;

    if (!found) {
        LOG_INFO_MSG("Alias " << waitreq._alias << " was not found.");
        waitrep._rc = exceptions::WARN;
        waitrep._rt = "Specified alias not found";
        waitrep._binary_id = "";
        try {
            _prot->sendReply(waitrep.getClassName(), waitrep);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during wait reply.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during wait reply.");
        }
        return;
    }

    while (al->running(id) == false && timeout != 0 && !_ending) {
        sleep(1);
        --timeout;
    }

    if (timeout == 0) { // We timed out without a start
        LOG_WARN_MSG("Alias wait for " << waitreq._alias << " timed out.");
        waitrep._rc = exceptions::INFO;
        waitrep._rt = "wait timed out without a start";
        waitrep._binary_id = "";
    } else {
        waitrep._rc = exceptions::OK;
        waitrep._rt = "success";
        waitrep._binary_id = id.str();
    }
    try {
        _prot->sendReply(waitrep.getClassName(), waitrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during wait reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during wait reply.");
    }
}

void
ClientController::doErrorsRequest(
        const BGMasterClientProtocolSpec::Get_errorsRequest& /* error_req */
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::Get_errorsReply error_rep(exceptions::OK, "success");
    MasterController::getErrorMessages(error_rep._errors);
    try {
        _prot->sendReply(error_rep.getClassName(), error_rep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during get errors reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during get errors reply.");
    }
}

void
ClientController::doHistoryRequest(
        const BGMasterClientProtocolSpec::Get_historyRequest& /* history_req */
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::Get_historyReply history_rep(exceptions::OK, "success");
    MasterController::getHistoryMessages(history_rep._history);
    try {
        _prot->sendReply(history_rep.getClassName(), history_rep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during get history reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during get history reply.");
    }
}

void
ClientController::doEndmonitorRequest(
        const BGMasterClientProtocolSpec::EndmonitorRequest& /* endmonreq */
        )
{
    LOG_TRACE_MSG(__FUNCTION__);

    const BGMasterClientProtocolSpec::EndmonitorReply endmonrep;

    try {
        _prot->sendReply(endmonrep.getClassName(), endmonrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during end monitor reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during end monitor reply.");
    }
    boost::mutex::scoped_lock scoped_lock(MasterController::_monitor_prots_mutex);
    MasterController::get_monitor_prots().erase(
            std::remove(
                MasterController::get_monitor_prots().begin(),
                MasterController::get_monitor_prots().end(),
                _prot
                ),
            MasterController::get_monitor_prots().end()
            );
}

void
ClientController::doLoglevelRequest(
        const BGMasterClientProtocolSpec::LoglevelRequest& loglevreq
        )
{
    BGMasterClientProtocolSpec::LoglevelReply loglevrep;
    loglevrep._rc = exceptions::OK;
    loglevrep._rt = "";

    // If there are any changes in the request, apply them.
    bgq::utility::LoggingProgramOptions::Strings logstrings;
    BOOST_FOREACH(const std::string& logger, loglevreq._loggers) {
        logstrings.push_back( logger );
    }
    // Set logging level
    try {
        bgq::utility::LoggingProgramOptions lpo( "ibm.master" );
        lpo.notifier( logstrings );
        lpo.apply();
    } catch ( const std::invalid_argument& e ) {
        loglevrep._rc = exceptions::FATAL;
        loglevrep._rt = e.what();
        LOG_WARN_MSG( e.what() );
    }

    // Get the current levels and return them.
    using namespace log4cxx;
    // Output all current loggers and their levels
    const LoggerPtr root = Logger::getRootLogger();
    BOOST_FOREACH(const LoggerPtr& curr_loggerp, root->getLoggerRepository()->getCurrentLoggers() ) {
        if ( !curr_loggerp ) continue;
        if ( !curr_loggerp->getLevel() ) continue;

        loglevrep._loggers.push_back(
                BGMasterClientProtocolSpec::Logger(
                    curr_loggerp->getName(),
                    curr_loggerp->getLevel()->toString()
                    )
                );
    }

    try {
        _prot->sendReply(loglevrep.getClassName(), loglevrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during log level reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during log level reply.");
    }
}

void
ClientController::doGetidleRequest(
        const BGMasterClientProtocolSpec::GetidleRequest& /* idlereq */
        )
{
    // Make a local copy because we don't really care about updates and we are going to destroy it when we're done.
    const std::vector<AliasPtr> alist = MasterController::_aliases.get_list_copy();
    BGMasterClientProtocolSpec::GetidleReply idlerep;
    BOOST_FOREACH(const AliasPtr& al, alist) {
        if (!al->running()) {
            // Nothing running, put it in the reply.
            idlerep._aliases.push_back(al->get_name());
        }
    }

    try {
        _prot->sendReply(idlerep.getClassName(), idlerep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during idle reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during idle reply.");
    }
}

void
ClientController::doMonitorRequest(
        const BGMasterClientProtocolSpec::MonitorRequest& /* monreq */
        )
{
    // To do this, we first send a reply of all of the history and event messages in our buffer.
    LOG_TRACE_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::MonitorReply monrep(exceptions::OK, "success");
    std::vector<std::string> history_messages;
    MasterController::getHistoryMessages(history_messages);
    BOOST_FOREACH(const std::string& curr_message, history_messages) {
        BGMasterClientProtocolSpec::MonitorReply::EventMessage em(curr_message);
        monrep._eventmessages.push_back(em);
    }
    std::vector<std::string> error_messages;
    MasterController::getErrorMessages(error_messages);
    BOOST_FOREACH(const std::string& curr_message, error_messages) {
        BGMasterClientProtocolSpec::MonitorReply::ErrorMessage em(curr_message);
        monrep._errormessages.push_back(em);
    }

    try {
        _prot->sendReply(monrep.getClassName(), monrep);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during get history reply.");
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        LOG_ERROR_MSG("Client connection ended during get history reply.");
    }
    _prot->setRequester(_prot->getResponder());
    // Now add the protocol object to the master controller
    boost::mutex::scoped_lock scoped_lock(MasterController::_monitor_prots_mutex);
    MasterController::get_monitor_prots().push_back(_prot);
}

void
ClientController::processRequest()
{
    LOG_TRACE_MSG(__FUNCTION__);
    // These will be requests coming from the bgagent.
    std::string request_name;

    try {
        _prot->getName(request_name);
    } catch (const CxxSockets::SoftError& err) {
        LOG_ERROR_MSG("Client connection error during receive of class name.");
        return;
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        if (err.errcode != 0) {
            LOG_ERROR_MSG("Client connection ended during receive of class name.");
        } else {
            LOG_DEBUG_MSG("Client has ended connection.");
        }
        _ending = true;
        return;
    }

    LOG_DEBUG_MSG("Request " << request_name << " received.");

    if (request_name == "StartRequest") {
        BGMasterClientProtocolSpec::StartRequest startreq;
        try {
            _prot->getObject(&startreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during start request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during start request.");
        }

        doStartRequest(startreq);
    } else if (request_name == "StopRequest") {
        BGMasterClientProtocolSpec::StopRequest stopreq;
        try {
            _prot->getObject(&stopreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during stop request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during stop request.");
        }
        doStopRequest(stopreq);
    } else if (request_name == "AgentlistRequest") {
        BGMasterClientProtocolSpec::AgentlistRequest agentreq;
        try {
            _prot->getObject(&agentreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during agent list request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during agent list request.");
        }
        doAgentRequest(agentreq);
    } else if (request_name == "ClientsRequest") {
        BGMasterClientProtocolSpec::ClientsRequest clientreq;
        try {
            _prot->getObject(&clientreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during clients request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during clients request.");
        }
        doClientsRequest(clientreq);
    } else if (request_name == "WaitRequest") {
        BGMasterClientProtocolSpec::WaitRequest waitreq;
        try {
            _prot->getObject(&waitreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during wait request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during wait request.");
        }
        doWaitRequest(waitreq);
    } else if (request_name == "StatusRequest") {
        BGMasterClientProtocolSpec::StatusRequest statreq;
        try {
            _prot->getObject(&statreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during status request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during status request.");
        }
        doStatusRequest(statreq);
    } else if (request_name == "TerminateRequest") {
        BGMasterClientProtocolSpec::TerminateRequest termreq;
        try {
            _prot->getObject(&termreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during terminate request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during terminate request.");
        }
        doTermRequest(termreq);
    } else if (request_name == "ReloadRequest") {
        BGMasterClientProtocolSpec::ReloadRequest relreq;
        try {
            _prot->getObject(&relreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during reload request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during reload request.");
        }
        doReloadRequest(relreq);
    } else if (request_name == "FailoverRequest") {
        BGMasterClientProtocolSpec::FailoverRequest failreq;
        try {
            _prot->getObject(&failreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during fail-over request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during fail-over request.");
        }
        doFailRequest(failreq);
    } else if (request_name == "MasterstatRequest") {
        BGMasterClientProtocolSpec::MasterstatRequest statreq;
        try {
            _prot->getObject(&statreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during master status request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during master status request.");
        }
        doMasterStatRequest(statreq);
    } else if (request_name == "Alias_waitRequest") {
        BGMasterClientProtocolSpec::Alias_waitRequest waitreq;
        try {
            _prot->getObject(&waitreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during alias wait request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during alias wait request.");
        }
        doAliasWaitRequest(waitreq);
    } else if (request_name == "Get_errorsRequest") {
        BGMasterClientProtocolSpec::Get_errorsRequest error_req;
        try {
            _prot->getObject(&error_req);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during get errors request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during get errors request.");
        }
        doErrorsRequest(error_req);
    } else if (request_name == "Get_historyRequest") {
        BGMasterClientProtocolSpec::Get_historyRequest history_req;
        try {
            _prot->getObject(&history_req);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during get history request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during get history request.");
        }
        doHistoryRequest(history_req);
    } else if (request_name == "MonitorRequest") {
        BGMasterClientProtocolSpec::MonitorRequest monreq;
        try {
            _prot->getObject(&monreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during monitor request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during monitor request.");
        }
        doMonitorRequest(monreq);
    } else if (request_name == "EndmonitorRequest") {
        BGMasterClientProtocolSpec::EndmonitorRequest endmonreq;
        try {
            _prot->getObject(&endmonreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during end monitor request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during get monitor request.");
        }
        doEndmonitorRequest(endmonreq);
    } else if (request_name == "LoglevelRequest") {
        BGMasterClientProtocolSpec::LoglevelRequest loglevreq;
        try {
            _prot->getObject(&loglevreq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during log level request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during log level request.");
        }
        doLoglevelRequest(loglevreq);
    } else if (request_name == "GetidleRequest") {
        BGMasterClientProtocolSpec::GetidleRequest idlereq;
        try {
            _prot->getObject(&idlereq);
        } catch (const CxxSockets::SoftError& err) {
            LOG_ERROR_MSG("Client connection error during get idle request.");
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission
            LOG_ERROR_MSG("Client connection ended during get idle request.");
        }
        doGetidleRequest(idlereq);
    } else {
        LOG_WARN_MSG("Received invalid request " << request_name << ", ignoring.");
    }
}

void
ClientController::waitMessages()
{
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    // Wait for requests and send responses

    _my_tid = pthread_self();

    while (!_ending && MasterController::get_client_manager()._ending == false) {
        // Process message
        processRequest();
    }

    // Client is gone, the controller must go too
    const ClientControllerPtr self_ptr = shared_from_this();
    MasterController::get_client_manager().removeClient(self_ptr);
}

void
ClientController::startPoller()
{
    _client_socket_poller = boost::thread(&ClientController::waitMessages, this);
}

void
ClientController::cancel()
{
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    std::ostringstream os;
    os << std::setbase(16);
    os << _my_tid;

    LOG_DEBUG_MSG("Canceling client " << os.str());
    _ending = true;
    pthread_kill(_my_tid, SIGUSR1);
    if (_my_tid != pthread_self()) {  // Don't want to wait for myself!
        _client_socket_poller.join();
    }
}
