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
#include "MasterController.h"

#include "common/BinaryController.h"

#include "../lib/exceptions.h"

LOG_DECLARE_FILE( "master" );

AgentManager::AgentManager():
    _ending_agents(false),
    _agents_per_host( 1 )
{
    // Nothing to do
}

void
AgentManager::setCount(
        const unsigned count
        )
{
    _agents_per_host = count;
    LOG_DEBUG_MSG( "Set maximum agents per host to " << _agents_per_host );
}

bool
AgentManager::addNew(
        AgentRepPtr agent
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_agent_manager_mutex);
    LOG_DEBUG_MSG("Adding agent " << agent->get_agent_id().str() << " from list.");
    unsigned agent_count = 0;
    BOOST_FOREACH(const AgentRepPtr& ptr, _agents) {
        if (ptr->get_host().ip() == agent->get_host().ip()) {
            // We've got one
            ++agent_count;
        }
    }

    if (agent_count < _agents_per_host) {
        // Put it in the agent list
        _agents.push_back(agent);
    } else {
        return false;
    }
    return true;
}

void
AgentManager::removeAgent(
        AgentRepPtr agent
        )
{
    _agent_manager_mutex.lock();
    LOG_DEBUG_MSG("Removing agent " << agent->get_agent_id().str() << " from list.");
    // Get copies of the binaries and the agent id
    BGAgentId failing = agent->get_agent_id();
    std::vector<BinaryControllerPtr> binlist = agent->get_binaries();
    _agents.erase(remove(_agents.begin(),_agents.end(), agent), _agents.end());
    // Don't need the lock any more.  Won't be using local resources.
    _agent_manager_mutex.unlock();

    // For each binary, run through the policy checks and restart.
    BOOST_FOREACH(const BinaryControllerPtr& binptr, binlist) {
        // Mark the binary done!  If the agent is gone, so is the binary.
        binptr->set_status(BinaryController::COMPLETED);
        LOG_DEBUG_MSG("Checking for policy for " << binptr->get_alias_name());
        BinaryId reqbid = binptr->get_binid();
        if (binptr->stopping() != true) {
            // We haven't explicitly stopped, so we have to check our policy
            BOOST_FOREACH(const AliasPtr& al, MasterController::_aliases) {
                if (al->find_binary(reqbid)) {
                    // This alias has my binary id, so remove my id and execute the policy
                    LOG_TRACE_MSG("Found alias " << al->get_name());
                    al->remove_binary(reqbid);
                    try {
                        Policy::Trigger t = Policy::AGENT_ABEND;
                        AgentRepPtr rep_p = al->evaluatePolicy(t, failing, reqbid, binptr);
                        if (rep_p && !rep_p->runningAlias(al->get_name())) {
                            // Now we've got a new agent that isn't already running this alias.
                            const BGMasterAgentProtocolSpec::StartRequest agentreq(
                                    al->get_path(),
                                    al->get_args(),
                                    al->get_logdir(),
                                    al->get_name(),
                                    al->get_user()
                                    );
                            std::ostringstream logmsg;
                            logmsg << "start request path=" << agentreq._path << " "
                                   << "arguments=" << agentreq._arguments << " "
                                   << "logdir=" << al->get_logdir() << " "
                                   << "user=" << agentreq._user << " ";
                            LOG_TRACE_MSG(logmsg.str());

                            BGMasterAgentProtocolSpec::StartReply agentrep;
                            agentrep._rc = exceptions::OK;

                            BinaryId b = rep_p->startBin(agentreq, agentrep);
                            if (b.str() != "0") {
                                al->add_binary(b);
                            }
                        }
                    } catch (const exceptions::InternalError& e) {
                        std::ostringstream msg;
                        msg << "Unable to execute policy for " << reqbid.str() << "|" << al->get_name() << " " << e.what();
                        MasterController::handleErrorMessage(msg.str());
                    }
                }
            }
        }
    }
}

AgentRepPtr
AgentManager::findAgentRep(
        const BGAgentId& aid
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_agent_manager_mutex);
    AgentRepPtr p;
    BOOST_FOREACH(const AgentRepPtr& agent, _agents) {
        if (agent->get_agent_id() == aid) {
            p = agent;
        }
    }
    return p;
}

BGAgentId
AgentManager::findAgentId(
        const CxxSockets::Host& host
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_agent_manager_mutex);
    BGAgentId p;

    // Find and return the =first= active agent on this host
    BOOST_FOREACH(const AgentRepPtr& agent, _agents) {
        if (agent->get_host() == host) {
            p = agent->get_agent_id();
            break;
        }
    }
    return p;
}

AgentRepPtr
AgentManager::findAgentRep(
        const CxxSockets::Host& host
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_agent_manager_mutex);
    AgentRepPtr p;
    BOOST_FOREACH(const AgentRepPtr& agent, _agents) {
        const CxxSockets::Host lname = agent->get_host();
        if (lname == host) {
            p = agent;
        }
    }
    return p;
}

AgentRepPtr
AgentManager::pickAgent()
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_agent_manager_mutex);
    // We'll start simple and use either the first one with zero
    // binaries or the one with the smallest number of binaries.
    std::vector<AgentRepPtr>::const_iterator smallest = _agents.begin();
    for (std::vector<AgentRepPtr>::const_iterator it = _agents.begin(); it != _agents.end(); ++it) {
        if ((*smallest)->binCount() > (*it)->binCount()) {
            smallest = it;
        }
    }
    AgentRepPtr p;
    if (!_agents.empty()) { // There has to be at least one!
        p = *smallest;
    }
    return p;
}

bool
AgentManager::findBinary(
        const BinaryId& id,
        BinaryLocation& loc
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_agent_manager_mutex);
    BinaryControllerPtr ptr;
    AgentRepPtr foundagent;

    bool found = false;

    // Loop through agents
    BOOST_FOREACH(const AgentRepPtr& agent, _agents) {
        // Now loop through the binaries it controls
        const std::vector<BinaryControllerPtr> binaries = agent->get_binaries();
        BOOST_FOREACH(const BinaryControllerPtr& binary, binaries) {
            if (binary->get_binid() == id) {
                // This is the one
                ptr = binary;
                foundagent = agent;
                found = true;
            }
        }
    }
    loc = BinaryLocation(ptr, foundagent);
    return found;
}

bool
AgentManager::findBinary(
        const std::string& alias,
        std::vector<BinaryLocation>& loc
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_agent_manager_mutex);
    bool found = false;

    BOOST_FOREACH(const AgentRepPtr& agent, _agents) {
        // Now loop through the binaries it controls
        const std::vector<BinaryControllerPtr> binaries = agent->get_binaries();
        BOOST_FOREACH(const BinaryControllerPtr& binary, binaries) {
            if (binary->get_alias_name() == alias) {
                // Got one
                BinaryLocation foundling(binary, agent);
                loc.push_back(foundling);
                found = true;
            }
        }
    }
    return found;
}

void
AgentManager::cancel(
        const bool end_binaries,
        const int signal
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    _ending_agents = true;
    // Loop through the agents and call the cancel for each
    std::vector<AgentRepPtr> agents;
    {
        boost::mutex::scoped_lock lock(_agent_manager_mutex);
        BOOST_FOREACH(const AgentRepPtr& agent, _agents) {
            agents.push_back(agent);
        }
    }

    for (unsigned i = 0; i < agents.size(); ++i) {
        agents[i]->cancel(end_binaries, signal);
    }
}
