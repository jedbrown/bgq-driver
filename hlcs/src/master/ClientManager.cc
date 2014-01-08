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
#include <stdexcept>
#include <utility/include/Log.h>
#include <boost/foreach.hpp>
#include <xml/include/library/XML.h>
#include <utility/include/LoggingProgramOptions.h>
#include "Properties.h"
#include "BGAgentRep.h"
#include "BGMaster.h"
#include "Thread.h"
#include "Policy.h"
#include "Alias.h"
#include "BGMasterExceptions.h"
#include "MasterRasMetadata.h"
#include "BinaryController.h"

LOG_DECLARE_FILE( "master" );

BGMasterController::ClientManager BGMasterController::_client_manager;


BGMasterController::ClientManager::ClientManager() : _ending(false) {
}

void BGMasterController::ClientManager::cancel() {
    LOG_INFO_MSG(__FUNCTION__);
    _ending = true;
    // Loop through the clients and call the cancel for each
    std::vector<ClientControllerPtr> clients;
    for(std::vector<ClientControllerPtr>::iterator it = _clients.begin();
        it != _clients.end(); ++it) {
        ClientControllerPtr client = (*it);
        clients.push_back(client);
    }

    for(unsigned i = 0; i < clients.size(); ++i)
        clients[i]->cancel();
}

void BGMasterController::ClientManager::removeClient(ClientControllerPtr c) {
    LOG_DEBUG_MSG(__FUNCTION__);
    _clients.erase(remove(_clients.begin(),_clients.end(), c), _clients.end());
}

void BGMasterController::ClientManager::addClient(ClientControllerPtr c) {
    if(_ending == true) return;  // Don accept anybody new if we're going away.
    LOG_DEBUG_MSG(__FUNCTION__);
    _clients.push_back(c);
    LOG_INFO_MSG( _clients.size() << " clients managed");
}

BGMasterController::ClientManager::ClientController::~ClientController() {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_DEBUG_MSG(__FUNCTION__);
}

BGMasterController::ClientManager::ClientController::ClientController(BGMasterClientProtocolPtr& prot,
                                                                      std::string& ipaddr, int port) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_DEBUG_MSG(__FUNCTION__);
    std::ostringstream p;
    p << port;
    _prot = prot;
    ClientId id(port, ipaddr);
    _client_id = id;
    _ending = false;

    // Send a Join reply indicating that we are satisfied the handshaking is complete.
    BGMasterAgentProtocolSpec::JoinReply rep(BGMasterExceptions::OK, "join successful", "", "", "");
    _prot->sendReply(rep.getClassName(), rep);
    LOG_DEBUG_MSG("Sent join reply to " << ipaddr << ":" << port);
    startPoller();
}

void BGMasterController::ClientManager::ClientController::
doStartRequest(BGMasterClientProtocolSpec::StartRequest startreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    BinaryId id;
    BGMasterClientProtocolSpec::StartReply clientrep(BGMasterExceptions::OK, "started servers", id);
    if(_utype != CxxSockets::Administrator) {
        clientrep._rc = BGMasterExceptions::FATAL;
        clientrep._rt = "Authorization failure.  Client did not present Administrator certificate";
        BGMasterController::handleErrorMessage(clientrep._rt);
        std::map<std::string, std::string> details;
        details["USER"] = "unknown";
        details["COMMAND"] = "master_start";
        BGMasterController::putRAS(AUTHORITY_FAIL_RAS, details);
    } else {
        if(startreq._alias.empty()) {
            // Not an error!  This tells us to start 'em all up.
            std::map<std::string, std::string> failed_aliases;
            BGAgentRepPtr np;
            BGMasterController::startServers(failed_aliases, np);
            if(failed_aliases.size() !=0) {
                clientrep._rc = BGMasterExceptions::WARN;
                std::ostringstream failure;
                if(failed_aliases.size() == 1)
                    failure << "Alias ";
                else
                    failure << "Aliases ";

                for(std::map<std::string, std::string>::iterator i = failed_aliases.begin();
                    i != failed_aliases.end(); ++i) {
                    failure << i->first << " ";
                }

                failure << "did not start.";
                clientrep._rt = failure.str();
            }
            try {
                _prot->sendReply(clientrep.getClassName(), clientrep);
            } catch(CxxSockets::SockSoftError& err) {
                // For soft errors, we just back out and let it try again
                LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
            } catch(CxxSockets::CxxError& err) {
                // Client aborted with an incomplete transmission
                LOG_WARN_MSG("Client connection has abnormally ended during start reply: " << strerror(err.errcode));
            }

            return;
        }

        std::string alias = startreq._alias;

        BinaryId bid;

        // Find the alias
        BOOST_FOREACH(AliasPtr& al, _aliases) {
            if(al->get_name() == startreq._alias) {
                try {
                    BGAgentId aid(startreq._agent_id);
                    BGAgentRepPtr p;
                    p = al->validateStartAgent(aid);
                    if(p) {
                        BGMasterAgentProtocolSpec::StartRequest
                            agentreq(al->get_path(), al->get_args(),
                                     al->get_logdir(), al->get_name(), al->get_user());
                        BGMasterAgentProtocolSpec::StartReply agentrep;
                        agentrep._rc = BGMasterExceptions::OK;

                        al->resetRetries();

                        std::ostringstream logmsg;
                        logmsg << "start request path=" << agentreq._path << " "
                               << "arguments=" << agentreq._arguments << " "
                               << "logdir=" << al->get_logdir() << " "
                               << "user=" << agentreq._user << " ";
                        LOG_DEBUG_MSG(logmsg.str());

                        bid = p->startBin(agentreq, agentrep);
                        if(bid.str() != "0")
                            al->add_binary(bid);
                        else {
                            // We have a failure, note it and put it in the reply
                            clientrep._rc = agentrep._rc;
                            clientrep._rt = agentrep._rt;
                            break;  // Stop trying if we get a failure
                        }
                    }
                } catch (BGMasterExceptions::InternalError& e) {
                    // Policy error
                    std::ostringstream msg;
                    msg << "Policy error for "
                        << startreq._alias << ". " << e.what()
                        << " Check config, restart or refresh.";
                    handleErrorMessage(msg.str());
                    BinaryId id("0");
                    BGMasterClientProtocolSpec::StartReply failrep(e.errcode, msg.str(), id);
                    try {
                        _prot->sendReply(failrep.getClassName(), failrep);
                    } catch(CxxSockets::SockSoftError& err) {
                        // For soft errors, we just back out and let it try again
                        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
                    } catch(CxxSockets::CxxError& err) {
                        // Client aborted with an incomplete transmission
                        LOG_WARN_MSG("Client connection has abnormally ended during start reply: " << strerror(err.errcode));
                    }

                    return;
                }

                // If we got here, we're good.
                LOG_INFO_MSG(startreq._alias << " cleared against policy to run");
                break;
            }
        }

        // Return the reply
        clientrep._binary_id = bid.str();
    }
    try {
        _prot->sendReply(clientrep.getClassName(), clientrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_WARN_MSG("Client connection has abnormally ended during start reply: " << strerror(err.errcode));
    }

}

void BGMasterController::ClientManager::ClientController::
doAgentRequest(BGMasterClientProtocolSpec::AgentlistRequest agentreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    // Got an agent request.  We have to build up a reply which
    // means going through all of the agents and all of their
    // binaries, and shoving it in the message.
    // First, iterate through the agent manager:
    BGMasterClientProtocolSpec::AgentlistReply agentrep(BGMasterExceptions::OK, "success");
    BOOST_FOREACH(BGAgentRepPtr agent, _agent_manager.get_agent_list()) {
        // Build our corresponding protocol agent object
        BGMasterClientProtocolSpec::AgentlistReply::Agent reply_agent(agent->get_agent_id());

        // Now iterate through its binaries and add them.  This gets a COPY
        // of the binary list which could change, but that's OK here.
        std::vector<BinaryControllerPtr> binaries = agent->get_binaries();
        BOOST_FOREACH(BinaryControllerPtr& binary, binaries) {
            typedef BGMasterClientProtocolSpec::AgentlistReply::Agent::Binary AgentBin;
            LOG_DEBUG_MSG("Found binary " << binary);
            std::string t = boost::posix_time::to_simple_string(binary->get_start_time());
            AgentBin bin((int)(binary->get_status()),
                         binary->get_exit_status(),
                         binary->get_binary_bin_path(),
                         binary->get_alias_name(),
                         binary->get_user(),
                         binary->get_binid().str(),
                         t);
            reply_agent._binaries.push_back(bin);
        }

        // Now we've filled out an agent and binaries, push it
        // in the vector
        agentrep._agent.push_back(reply_agent);
    }

    // Our reply should be complete now.  Return it.
    try {
        _prot->sendReply(agentrep.getClassName(), agentrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during list agents reply: " << strerror(err.errcode));
    }

}

void BGMasterController::ClientManager::ClientController::
doClientsRequest(BGMasterClientProtocolSpec::ClientsRequest clientreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
     LOG_INFO_MSG(__FUNCTION__);
   // Spin through the client controllers and add them to the reply
    BGMasterClientProtocolSpec::ClientsReply clientrep(BGMasterExceptions::OK, "success");
    typedef std::vector<ClientControllerPtr> ClientVec;
    ClientVec cv = BGMasterController::get_client_manager().getClients();
    LOG_DEBUG_MSG("Found " << cv.size() << " clients.");
    for(ClientVec::iterator ci = cv.begin(); ci != cv.end(); ++ci) {
        LOG_DEBUG_MSG("Sending client " << (*ci)->get_client_id().str() << ".");
        BGMasterClientProtocolSpec::ClientsReply::Client ct;
        ct._client_id = (*ci)->get_client_id().str();
        ct._user_id = (*ci)->getUserId();
        clientrep._clients.push_back(ct);
    }
    try {
        _prot->sendReply(clientrep.getClassName(), clientrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during list clients reply: " << strerror(err.errcode));
    }

}

void BGMasterController::ClientManager::ClientController::
doWaitRequest(BGMasterClientProtocolSpec::WaitRequest waitreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__ << " " << waitreq._binary_id);
    // Get the binary id and then we'll loop until we can't
    // find the controller any more.
    BinaryId reqbid(waitreq._binary_id);

    // Now find the associated binary controller.  We'll have a smart
    // pointer so it can't go away, but the status can change
    BinaryLocation loc;
    bool found = _agent_manager.findBinary(reqbid, loc);

    unsigned estatus = 0;

    BGMasterClientProtocolSpec::WaitReply waitrep;
    waitrep._rc = BGMasterExceptions::OK;

    BGMasterClientProtocolSpec::WaitReply::BinaryStatus binstat;

    if(found == true) {
        BinaryControllerPtr bcptr = loc.first;

        boost::unique_lock<boost::mutex> ulock(bcptr->_status_lock);
        while(bcptr->get_status() == BinaryController::RUNNING && !_ending) {
            // As long as it is RUNNING, we wait
            bcptr->_status_notifier.wait(ulock);
        }

        if(!_ending) {
            estatus = bcptr->get_exit_status();
            // We're done with it
            bcptr.reset();
            BGMasterClientProtocolSpec::WaitReply::BinaryStatus bs(reqbid.str(), estatus);
            binstat = bs;
            waitrep._rc = BGMasterExceptions::OK;
            waitrep._rt = "success";
        } else {
            bcptr.reset();
            BGMasterClientProtocolSpec::WaitReply::BinaryStatus bs(reqbid.str(), 0);
            binstat = bs;
            waitrep._rc = BGMasterExceptions::WARN;
            waitrep._rt = "bgmaster_server ending!";
        }
    } else {
        BGMasterClientProtocolSpec::WaitReply::BinaryStatus bs("0",-1);
        binstat = bs;
        waitrep._rc = BGMasterExceptions::INFO;
        waitrep._rt = "Specified binary not found.";
    }

    // Out of the loop.  We're done now.  Send a reply.
    waitrep._status = binstat;
    try {
        _prot->sendReply(waitrep.getClassName(), waitrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during wait reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doStopRequest(BGMasterClientProtocolSpec::StopRequest stopreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::StopReply reply_to_client(BGMasterExceptions::OK, "stopped");

    LOG_INFO_MSG("Stop request for " << stopreq._binary_ids.size() << " ids  and "
                  << stopreq._aliases.size() << " binary names");
    bool binsfound = 0;
    if(_utype != CxxSockets::Administrator) {
        reply_to_client._rc = BGMasterExceptions::FATAL;
        reply_to_client._rt = "Authorization failure.  Client did not present Administrator certificate";
        BGMasterController::handleErrorMessage(reply_to_client._rt);
        std::map<std::string, std::string> details;
        details["USER"] = "unknown";
        details["COMMAND"] = "master_stop";
        BGMasterController::putRAS(AUTHORITY_FAIL_RAS, details);
    } else {
        // We can have a list of aliases or specific binary ids to stop.
        // Do both.
        if(stopreq._binary_ids.size() == 0 && stopreq._aliases.size() == 0) {
            // Stop them all if nothing is passed
            ++binsfound;  // Assume at least one is running.
            std::vector<BGAgentRepPtr> agents = 
                BGMasterController::get_agent_manager().get_agent_list();
            BOOST_FOREACH(BGAgentRepPtr& rep, agents) {
                if(rep) // The iterator may have gone stale.  Check the rep ptr.
                    rep->stopAllBins(reply_to_client, stopreq._signal);
            }
        }

        // First go through all of the binary ids specified for stoppage
        for(std::vector<std::string>::iterator idit = stopreq._binary_ids.begin();
            idit != stopreq._binary_ids.end(); ++idit) {
            BinaryId bid((*idit));

            LOG_INFO_MSG("Stop request for binary id " << bid.str() << " processing");

            // Find the binary in the agents
            BinaryLocation location;

            bool found = BGMasterController::get_agent_manager().findBinary(bid, location);

            if(!found) {
                LOG_DEBUG_MSG("No binary by id " << bid.str() << " found");
                //                break; // None here, we'll check by alias
            } else {
                ++binsfound;
                BGMasterAgentProtocolSpec::StopReply stop_from_agent;
                stop_from_agent._rc = BGMasterExceptions::OK;
                
                boost::static_pointer_cast<BGAgentRep>(location.second)->
                    stopBin(bid, location, stopreq._signal, stop_from_agent, false);
                
                // Now collect the response and add it to the reply
                BGMasterClientProtocolSpec::StopReply::BinaryStatus
                    binstat_to_return (stop_from_agent._status._binary_id,
                                       stop_from_agent._status._exit_status);
                
                reply_to_client._statuses.push_back(binstat_to_return);
                if(stop_from_agent._rc != BGMasterExceptions::OK) {
                    reply_to_client._rc = stop_from_agent._rc;
                    reply_to_client._rt = stop_from_agent._rt;
                }
            }
        }

        // Now go through all of the aliases and find and stop each binary
        // associated with the alias supplied.
        for(std::vector<std::string>::iterator alias_it = stopreq._aliases.begin();
            alias_it != stopreq._aliases.end(); ++alias_it) {

            // First, find all of the binaries associated with this alias
            std::vector<BinaryLocation> locations;
            bool found = BGMasterController::get_agent_manager().findBinary(*alias_it, locations);

            if(!found) {
                LOG_INFO_MSG("No binaries by alias " << *alias_it << " found");
                //                break;
            } else {
                ++binsfound;
                LOG_INFO_MSG("Found " << locations.size() << " binaries associated with alias "
                             << *alias_it);
                
                // Now loop through the vector of locations associated with this
                // alias and send a stop request to each agent
                for(std::vector<BinaryLocation>::iterator location_it = locations.begin();
                    location_it != locations.end(); ++location_it) {
                    
                    // Extract the agent and binary id from the location
                    BGAgentRepPtr rep = boost::static_pointer_cast<BGAgentRep>(location_it->second);
                    BinaryId bid = location_it->first->get_binid();
                    
                    // Build the stop request and reply that go to the agent
                    BGMasterAgentProtocolSpec::StopReply stop_from_agent;
                    stop_from_agent._rc = BGMasterExceptions::OK;
                    
                    LOG_INFO_MSG("Sending stop request for alias \"" << *alias_it
                                 << "\" to agent " << rep->get_agent_id().str());
                    
                    rep->stopBin(bid, *location_it, stopreq._signal, stop_from_agent, false);
                    
                    // Now collect the response and add it to the reply
                    BGMasterClientProtocolSpec::StopReply::BinaryStatus
                        binstat_to_return (
                                           stop_from_agent._status._binary_id,
                                           stop_from_agent._status._exit_status);
                    
                    reply_to_client._statuses.push_back(binstat_to_return);
                    if(stop_from_agent._rc != BGMasterExceptions::OK) {
                        reply_to_client._rc = stop_from_agent._rc;
                        reply_to_client._rt = stop_from_agent._rt;
                    }
                }
            }
        }
    }

    if(!binsfound) {
        reply_to_client._rc = BGMasterExceptions::INFO;
        reply_to_client._rt = "No matching binary found";
    }

    try {
        _prot->sendReply(reply_to_client.getClassName(), reply_to_client);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during stop reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doStatusRequest(BGMasterClientProtocolSpec::StatusRequest statusreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::StatusReply statusrep(BGMasterExceptions::OK, 
                                                      "success");

    typedef BGMasterClientProtocolSpec::StatusReply::BinaryController BinCont;

    // If binary ids are specified, must find them all and get their
    // status.  Else, we just need to get ALL of them.
    if(statusreq._binary_ids.size() > 0) {
        for(std::vector<std::string>::iterator idit = statusreq._binary_ids.begin();
            idit != statusreq._binary_ids.end(); ++idit) {
            BinaryId id(*idit);
            BinaryControllerPtr pbase;

            BinaryLocation location;
            bool found = get_agent_manager().findBinary(id, location);
            if(found) {
                pbase = location.first;
                if(pbase->valid() == true) {
                    std::string t = boost::posix_time::to_simple_string(pbase->get_start_time());
                    BinCont bin((unsigned)(pbase->get_exit_status()), pbase->get_binid().str(),
                                pbase->get_binary_bin_path(), pbase->get_alias_name(), pbase->get_user(),
                                (int)(pbase->get_status()), t);
                    statusrep._binaries.push_back(bin);
                }
            }
        }
    } else {
        // Need them ALL.  Loop through agents.
        std::vector<BGAgentRepPtr> agents = 
            BGMasterController::get_agent_manager().get_agent_list();
        BOOST_FOREACH(BGAgentRepPtr& rep, agents) {
            if(!rep) continue;
            std::vector<BinaryControllerPtr> binaries = rep->get_binaries();
            // Now loop through binaries
            BOOST_FOREACH(BinaryControllerPtr& pbase, binaries) {
                if(pbase->valid() == true) {
                    std::string t = boost::posix_time::to_simple_string(pbase->get_start_time());
                    BinCont bin((unsigned)(pbase->get_exit_status()), pbase->get_binid().str(),
                                pbase->get_binary_bin_path(), pbase->get_alias_name(), pbase->get_user(),
                                (int)(pbase->get_status()), t);
                    statusrep._binaries.push_back(bin);
                }
            }
        }
    }
    try {
        _prot->sendReply(statusrep.getClassName(), statusrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during status reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doExitRequest(BGMasterClientProtocolSpec::ExitStatusRequest exitreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::ExitStatusReply exitrep(BGMasterExceptions::OK, "success");

    // Iterate through all of the agents, getting last binary for each
    std::vector<BGAgentRepPtr> agents = 
        BGMasterController::get_agent_manager().get_agent_list();
    BOOST_FOREACH(BGAgentRepPtr agent, agents) {
        if(!agent) continue;
        BinaryControllerPtr bin = agent->get_last_bin();
        if(bin) {
            BGMasterClientProtocolSpec::ExitStatusReply::Agent::Binary
                reply_bin(BinaryController::status_to_string(bin->get_status()),
                          bin->get_exit_status(),
                          bin->get_binary_bin_path(),
                          bin->get_alias_name(),
                          bin->get_user(),
                          bin->get_binid().str());
            BGMasterClientProtocolSpec::ExitStatusReply::Agent
                reply_agent(reply_bin, agent->get_agent_id());

            exitrep._agent.push_back(reply_agent);
            exitrep._rc = BGMasterExceptions::OK;
            exitrep._rt = "success";
        }
    }
    try {
        _prot->sendReply(exitrep.getClassName(), exitrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during exit reply: " << strerror(err.errcode));
    }

}

void BGMasterController::ClientManager::ClientController::
doEnd_agentRequest(BGMasterClientProtocolSpec::End_agentRequest diereq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::End_agentReply dierep(BGMasterExceptions::OK, "success");
    dierep._rc = BGMasterExceptions::OK;
    dierep._rt = "success";
    BGMasterAgentProtocolSpec::End_agentReply agentdierep(BGMasterExceptions::OK, "success");

    // If there's only an "all" agent in the list, kill 'em all
    if(diereq._agent_ids[0] == "all") {
        // This causes all agents to be cleaned up.
        try {
            _agent_manager.cancel(true, true, 15);
        } catch(BGMasterExceptions::InternalError& e) {
            dierep._rc = e.errcode;
            dierep._rt = e.what();
        }
    } else {
        // Iterate through all agents in list then ask each
        // one to kill itself
        for(std::vector<std::string>::iterator it = diereq._agent_ids.begin();
            it != diereq._agent_ids.end(); ++it) {
            BGAgentId id(*it);
            BGAgentRepPtr agent_to_die;
            agent_to_die = _agent_manager.findAgentRep(id);
            if(agent_to_die == 0) {
                // Agent not registered.  Maybe dead, maybe never started
                std::ostringstream errstr;
                errstr << "Bad agent " << id.str() << " specified but not found.";
                LOG_INFO_MSG(errstr.str());
                dierep._rc = BGMasterExceptions::INFO;
                dierep._rt = errstr.str();
            } else {
                // Found the guy. Kill him
                if(agent_to_die->endAgent(agentdierep) == true) {

                    // Update database with ras message
                    std::map<std::string, std::string> details;
                    details["AGENT_ID"] = agent_to_die->get_agent_id().str();
                    BGMasterController::putRAS(AGENT_KILL_RAS, details);
                    std::ostringstream diemsg;
                    diemsg << "Agent " << agent_to_die->get_agent_id().str() << " killed";
                    BGMasterController::addHistoryMessage(diemsg.str());
                } else {
                    std::ostringstream errstr;
                    errstr << "bgmaster_server failed to end agent "
                           << id.str() << ".";
                    LOG_INFO_MSG(errstr.str());
                    dierep._rc = BGMasterExceptions::INFO;
                    dierep._rt = errstr.str();
                }
            }
        }
    }
    try {
        _prot->sendReply(dierep.getClassName(), dierep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during die reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doTermRequest(BGMasterClientProtocolSpec::TerminateRequest termreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_WARN_MSG("TERMINATE REQUEST RECEIVED.  BGMASTER_SERVER ENDING.");

    BGMasterClientProtocolSpec::TerminateReply termrep(BGMasterExceptions::OK, "success");
    termrep._rc = BGMasterExceptions::OK;
    termrep._rt = "terminate request started";
    try {
        _prot->sendReply(termrep.getClassName(), termrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during terminate reply: " << strerror(err.errcode));
    }

    bool end_binaries = true;
    if(termreq._master_only)
        end_binaries = false;
    _end_requested = true;
    BGMasterController::stopThreads(false, end_binaries, termreq._signal);  // Not ending agents
    BGMasterController::_master_terminating = true;
}

void BGMasterController::ClientManager::ClientController::
doReloadRequest(BGMasterClientProtocolSpec::ReloadRequest relreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::ReloadReply relrep(BGMasterExceptions::OK, "success! ");
    LOG_INFO_MSG("Reload request config file " << relreq._config_file);

    bool skip_policies = false;
    if(relreq._config_file.empty()) {
        LOG_INFO_MSG("No config file specified.  Reloading default.");
        _master_props->reload();
    } else {
        // New config file
        bgq::utility::Properties::Ptr newprops;
        try {
            if(!_master_props->reload(relreq._config_file)) {
                throw std::runtime_error("Properties file reload failed");
            }
        } catch(std::runtime_error& e) {
            std::ostringstream error;
            error << "Error reading properties: " << e.what();
            LOG_ERROR_MSG(error.str());
            handleErrorMessage(error.str());
            relrep._rc = -1;
            relrep._rt = error.str();
            skip_policies = true;
        }
    }

    // Now build it back up.
    if(skip_policies == false) {
        std::ostringstream failmsg;
        try {
            BGMasterController::buildPolicies(failmsg);
        } catch (BGMasterExceptions::ConfigError& e) {
            relrep._rc = e.errcode;
            relrep._rt = e.what();
        } catch (std::runtime_error& e) {
            // Likely one of Properties' exceptions
            relrep._rc = BGMasterExceptions::WARN;
            relrep._rt = e.what();
        }

        if(!failmsg.str().empty()) {
            if(relrep._rc == BGMasterExceptions::OK)
                relrep._rc = BGMasterExceptions::WARN;
            relrep._rt += failmsg.str();
        }
    }

    try {
        _prot->sendReply(relrep.getClassName(), relrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during reload reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doFailRequest(BGMasterClientProtocolSpec::FailoverRequest failreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__ << " " << failreq._binary_ids[0]);
    BGMasterClientProtocolSpec::FailoverReply failrep(BGMasterExceptions::OK, "success");
    std::string trigger = failreq._trigger;
    if(_utype != CxxSockets::Administrator) {
        failrep._rc = BGMasterExceptions::FATAL;
        failrep._rt = "Authorization failure.  Client did not present Administrator certificate";
        BGMasterController::handleErrorMessage(failrep._rt);
        std::map<std::string, std::string> details;
        details["USER"] = "unknown";
        details["COMMAND"] = "fail_over";
        BGMasterController::putRAS(AUTHORITY_FAIL_RAS, details);
    } else {
        BOOST_FOREACH(std::string& strbid, failreq._binary_ids) {
            // For every bin in the list, do the failover action, update
            // the status in the reply, and return it.
            // First, find out if we still have the binary.  If we do,
            // send a stop request.  Then, have the agent handle the policy.
            BinaryLocation location;
            BinaryId binid(strbid);
            if(BGMasterController::get_agent_manager().findBinary(binid, location) == true) {
                // Got it, kill it!
                BGMasterAgentProtocolSpec::StopReply stoprep;
                stoprep._rc = BGMasterExceptions::OK;

                BGAgentRepPtr rep = boost::static_pointer_cast<BGAgentRep>(location.second);
                try {
                    rep->stopBinaryAndExecutePolicy(binid, location, SIGUSR2, stoprep, location.first, trigger);
                    if(stoprep._rc == BGMasterExceptions::OK) {
                        failrep._rc = stoprep._rc;
                        failrep._rt = "success";
                    }
                } catch(BGMasterExceptions::ConfigError& e) {
                    LOG_WARN_MSG("Policy execution failed.  Check configuration");
                    BGMasterClientProtocolSpec::FailoverReply::BinaryStatus binstat(strbid, BinaryController::UNINITIALIZED);
                    failrep._statuses.push_back(binstat);
                    failrep._rc = BGMasterExceptions::WARN;
                    failrep._rt = e.what();
                }
            } else {
                // Binary no longer exists.  We're going to send a reply that
                // says so because we won't fail over something that wasn't there
                // in the first place.
                BGMasterClientProtocolSpec::FailoverReply::BinaryStatus binstat(strbid, BinaryController::UNINITIALIZED);
                failrep._statuses.push_back(binstat);
                failrep._rc = BGMasterExceptions::INFO;
                failrep._rt = "A specified binary does not exist";
            }
        }
    }
    try {
        _prot->sendReply(failrep.getClassName(), failrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during failover reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doMasterStatRequest(BGMasterClientProtocolSpec::MasterstatRequest statreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    std::string t = boost::posix_time::to_simple_string(BGMasterController::_start_time);
    BGMasterClientProtocolSpec::MasterstatReply statrep(BGMasterExceptions::OK, "", getpid(), t, BGMasterController::_version_string);
    try {
        _prot->sendReply(statrep.getClassName(), statrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during status reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doAliasWaitRequest(BGMasterClientProtocolSpec::Alias_waitRequest waitreq) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::Alias_waitReply waitrep;
    waitrep._rc = BGMasterExceptions::OK;

    // Find the matching alias in the alias list.
    AliasPtr al;
    bool found = _aliases.find_alias(waitreq._alias, al);
    unsigned timeout = waitreq._timeout;
    BinaryId id;

    if(!found) {
        LOG_INFO_MSG("Alias " << waitreq._alias << " not found.");
        waitrep._rc = BGMasterExceptions::WARN;
        waitrep._rt = "Specified alias not found";
        waitrep._binary_id = "";
        try {
            _prot->sendReply(waitrep.getClassName(), waitrep);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_INFO_MSG("Client connection has abnormally ended during wait reply: " << strerror(err.errcode));
        }
        return;
    }

    while(al->running(id) == false && timeout != 0 && !_ending) {
        sleep(1);
        --timeout;
    }

    if(timeout == 0) { // We timed out without a start
        LOG_WARN_MSG("Alias wait for " << waitreq._alias << " timed out.");
        waitrep._rc = BGMasterExceptions::INFO;
        waitrep._rt = "wait timed out without a start";
        waitrep._binary_id = "";
    } else {
        waitrep._rc = BGMasterExceptions::OK;
        waitrep._rt = "success";
        waitrep._binary_id = id.str();
    }
    try {
        _prot->sendReply(waitrep.getClassName(), waitrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during wait reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doErrorsRequest(BGMasterClientProtocolSpec::Get_errorsRequest& error_req) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::Get_errorsReply error_rep(BGMasterExceptions::OK, "success");
    BGMasterController::getErrorMessages(error_rep._errors);
    try {
        _prot->sendReply(error_rep.getClassName(), error_rep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during get errors reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doHistoryRequest(BGMasterClientProtocolSpec::Get_historyRequest& history_req) {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::Get_historyReply history_rep(BGMasterExceptions::OK, "success");
    BGMasterController::getHistoryMessages(history_rep._history);
    try {
        _prot->sendReply(history_rep.getClassName(), history_rep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during get history reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doEndmonitorRequest(BGMasterClientProtocolSpec::EndmonitorRequest& endmonreq) {
    BGMasterClientProtocolSpec::EndmonitorReply endmonrep;
   
    try {
        _prot->sendReply(endmonrep.getClassName(), endmonrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during end monitor reply: " << strerror(err.errcode));
    }
    BGMasterController::_monitor_prots.erase(std::remove(BGMasterController::_monitor_prots.begin(),
                                                         BGMasterController::_monitor_prots.end(),
                                                         _prot), BGMasterController::_monitor_prots.end());
}

void BGMasterController::ClientManager::ClientController::
doLoglevelRequest(BGMasterClientProtocolSpec::LoglevelRequest& loglevreq) {
    BGMasterClientProtocolSpec::LoglevelReply loglevrep;
    loglevrep._rc = BGMasterExceptions::OK;
    loglevrep._rt = "";

    // If there are any changes in the request, apply them.
    bgq::utility::LoggingProgramOptions::Strings logstrings;
    BOOST_FOREACH(BGMasterClientProtocolSpec::Logger& curr_logger, loglevreq._loggers) {
        logstrings.push_back(curr_logger._name + "=" + curr_logger._level);
    }
        // set logging level
    try {
        bgq::utility::LoggingProgramOptions lpo( "ibm.master" );
        lpo.notifier( logstrings );
        lpo.apply();
    } catch ( const std::invalid_argument& e ) {
        loglevrep._rc = BGMasterExceptions::FATAL;
        loglevrep._rt = e.what();
    }

    // Get the current levels and return them.
    using namespace log4cxx;
    // output all current loggers and their levels
    LoggerPtr root = Logger::getRootLogger();
    BOOST_FOREACH( LoggerPtr curr_loggerp, root->getLoggerRepository()->getCurrentLoggers() ) {
        BGMasterClientProtocolSpec::Logger logger;
        if ( curr_loggerp && curr_loggerp->getLevel() ) {
            logger._name = curr_loggerp->getName();
            logger._level = curr_loggerp->getLevel()->toString();
        }
        loglevrep._loggers.push_back(logger);
    }
    
    try {
        _prot->sendReply(loglevrep.getClassName(), loglevrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during log level reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doGetidleRequest(BGMasterClientProtocolSpec::GetidleRequest& idlereq) {
    // Make a local copy because we don't really care about updates
    // and we are going to destroy it when we're done.
    std::vector<AliasPtr> alist = BGMasterController::_aliases.get_list_copy();
    BGMasterClientProtocolSpec::GetidleReply idlerep;
    BOOST_FOREACH(AliasPtr& al, alist) {
        if(al->running() == false) {
            // Nothing running, put it in the reply.
            idlerep._aliases.push_back(al->get_name());
        }
    }

    try {
        _prot->sendReply(idlerep.getClassName(), idlerep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during idle reply: " << strerror(err.errcode));
    }
}

void BGMasterController::ClientManager::ClientController::
doMonitorRequest(BGMasterClientProtocolSpec::MonitorRequest& monreq) {
    // To do this, we first send a reply of all of the history and event messages in
    // our buffer.
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    BGMasterClientProtocolSpec::MonitorReply monrep(BGMasterExceptions::OK, "success");
    std::vector<std::string> history_messages;
    BGMasterController::getHistoryMessages(history_messages);
    BOOST_FOREACH(std::string& curr_message, history_messages) {
        BGMasterClientProtocolSpec::MonitorReply::EventMessage em(curr_message);
        monrep._eventmessages.push_back(em);
    }
    std::vector<std::string> error_messages;
    BGMasterController::getErrorMessages(error_messages);
    BOOST_FOREACH(std::string& curr_message, error_messages) {
        BGMasterClientProtocolSpec::MonitorReply::ErrorMessage em(curr_message);
        monrep._errormessages.push_back(em);
    }
    
    try {
        _prot->sendReply(monrep.getClassName(), monrep);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        LOG_INFO_MSG("Client connection has abnormally ended during get history reply: " << strerror(err.errcode));
    }
    _prot->setRequester(_prot->getResponder());
    // Now add the protocol object to the master controller
    BGMasterController::_monitor_prots.push_back(_prot);
}

void BGMasterController::ClientManager::ClientController::processRequest() {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_DEBUG_MSG(__FUNCTION__);
    // These will be requests coming from the bgagent.
    std::string request_name = "";

    try {
        _prot->getName(request_name);
    } catch(CxxSockets::SockSoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        return;
    } catch(CxxSockets::CxxError& err) {
        // Client aborted with an incomplete transmission
        if(err.errcode != 0) {
            LOG_WARN_MSG("Client connection has abnormally ended receiving class name: " << strerror(err.errcode));
        } else LOG_INFO_MSG("Client has ended connection.");
        _ending = true;
        return;
    }

    LOG_INFO_MSG("-*-Request " << request_name << " received.-*-");

    if(request_name == "StartRequest") {
        BGMasterClientProtocolSpec::StartRequest startreq;
        try {
            _prot->getObject(&startreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during start request: " << strerror(err.errcode));
        }

        doStartRequest(startreq);
    } else if(request_name == "StopRequest") {
        BGMasterClientProtocolSpec::StopRequest stopreq;
        try {
            _prot->getObject(&stopreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during stop request: " << strerror(err.errcode));
        }
        doStopRequest(stopreq);
    } else if(request_name == "AgentlistRequest") {
        BGMasterClientProtocolSpec::AgentlistRequest agentreq;
        try {
            _prot->getObject(&agentreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during agent list request: " << strerror(err.errcode));
        }
        doAgentRequest(agentreq);
    } else if(request_name == "ClientsRequest") {
        BGMasterClientProtocolSpec::ClientsRequest clientreq;
        try {
            _prot->getObject(&clientreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during clients request: " << strerror(err.errcode));
        }
        doClientsRequest(clientreq);
    } else if(request_name == "WaitRequest") {
        BGMasterClientProtocolSpec::WaitRequest waitreq;
        try {
            _prot->getObject(&waitreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during wait request: " << strerror(err.errcode));
        }
        doWaitRequest(waitreq);
    } else if(request_name == "StatusRequest") {
        BGMasterClientProtocolSpec::StatusRequest statreq;
        try {
            _prot->getObject(&statreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during status request: " << strerror(err.errcode));
        }
        doStatusRequest(statreq);
    } else if(request_name == "ExitStatusRequest") {
        BGMasterClientProtocolSpec::ExitStatusRequest exitreq;
        try {
            _prot->getObject(&exitreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during exit status request: " << strerror(err.errcode));
        }
        doExitRequest(exitreq);
    } else if(request_name == "End_agentRequest") {
        BGMasterClientProtocolSpec::End_agentRequest diereq;
        try {
            _prot->getObject(&diereq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during end agent request: " << strerror(err.errcode));
        }
        doEnd_agentRequest(diereq);
    } else if(request_name == "TerminateRequest") {
        BGMasterClientProtocolSpec::TerminateRequest termreq;
        try {
            _prot->getObject(&termreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during terminate request: " << strerror(err.errcode));
        }
        doTermRequest(termreq);
    } else if(request_name == "ReloadRequest") {
        BGMasterClientProtocolSpec::ReloadRequest relreq;
        try {
            _prot->getObject(&relreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during reload request: " << strerror(err.errcode));
        }
        doReloadRequest(relreq);
    } else if(request_name == "FailoverRequest") {
        BGMasterClientProtocolSpec::FailoverRequest failreq;
        try {
            _prot->getObject(&failreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during failover request: " << strerror(err.errcode));
        }
        doFailRequest(failreq);
    } else if(request_name == "MasterstatRequest") {
        BGMasterClientProtocolSpec::MasterstatRequest statreq;
        try {
            _prot->getObject(&statreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during master status request: " << strerror(err.errcode));
        }
        doMasterStatRequest(statreq);
    } else if(request_name == "Alias_waitRequest") {
        BGMasterClientProtocolSpec::Alias_waitRequest waitreq;
        try {
            _prot->getObject(&waitreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during alias wait request: " << strerror(err.errcode));
        }
        doAliasWaitRequest(waitreq);
    } else if(request_name == "Get_errorsRequest") {
        BGMasterClientProtocolSpec::Get_errorsRequest error_req;
        try {
            _prot->getObject(&error_req);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during get errors request: " << strerror(err.errcode));
        }
        doErrorsRequest(error_req);
    } else if(request_name == "Get_historyRequest") {
        BGMasterClientProtocolSpec::Get_historyRequest history_req;
        try {
            _prot->getObject(&history_req);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during get errors request: " << strerror(err.errcode));
        }
        doHistoryRequest(history_req);
    } else if(request_name == "MonitorRequest") {
        BGMasterClientProtocolSpec::MonitorRequest monreq;
        try {
            _prot->getObject(&monreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during get errors request: " << strerror(err.errcode));
        }
        doMonitorRequest(monreq);
    } else if(request_name == "EndmonitorRequest") {
        BGMasterClientProtocolSpec::EndmonitorRequest endmonreq;
        try {
            _prot->getObject(&endmonreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during get errors request: " << strerror(err.errcode));
        }
        doEndmonitorRequest(endmonreq);
    } else if(request_name == "LoglevelRequest") {
        BGMasterClientProtocolSpec::LoglevelRequest loglevreq;
        try {
            _prot->getObject(&loglevreq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during get errors request: " << strerror(err.errcode));
        }
        doLoglevelRequest(loglevreq);

    } else if(request_name == "GetidleRequest") {
        BGMasterClientProtocolSpec::GetidleRequest idlereq;
        try {
            _prot->getObject(&idlereq);
        } catch(CxxSockets::SockSoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_INFO_MSG("Client connection interrupted: " << strerror(err.errcode));
        } catch(CxxSockets::CxxError& err) {
            // Client aborted with an incomplete transmission
            LOG_WARN_MSG("Client connection has abnormally ended during get errors request: " << strerror(err.errcode));
        }
        doGetidleRequest(idlereq);
    } else {
        LOG_WARN_MSG("Received invalid message " << request_name << ". Ignoring."); 
    }
}

void BGMasterController::ClientManager::ClientController::waitMessages() {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    // Wait for requests and send responses
    ThreadLog tl("Client Controller Waiter");

    _my_tid = pthread_self();

    while(!_ending && BGMasterController::get_client_manager()._ending == false) {
        // process message
        processRequest();
    }

    // Client is gone, the controller must go too
    ClientControllerPtr self_ptr = shared_from_this();
    BGMasterController::get_client_manager().removeClient(self_ptr);
}

void BGMasterController::ClientManager::ClientController::startPoller() {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_DEBUG_MSG(__FUNCTION__);
    _client_socket_poller = boost::thread(&BGMasterController::ClientManager::ClientController::waitMessages, this);
}

void BGMasterController::ClientManager::ClientController::cancel() {
    LOGGING_DECLARE_ID_MDC(_client_id.str());
    LOG_INFO_MSG(__FUNCTION__);
    std::ostringstream os;
    os << std::setbase(16);
    os << _my_tid;

    LOG_INFO_MSG("Canceling client " << os.str());
    _ending = true;
    XML::Parser::setstopping(pthread_self());
    pthread_kill(_my_tid, SIGUSR1);
    if(_my_tid != pthread_self())  // Don't want to wait for myself!
        _client_socket_poller.join();
}
