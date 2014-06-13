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

#include "Alias.h"
#include "AgentManager.h"
#include "AgentRep.h"
#include "MasterController.h"
#include "ras.h"

#include "common/BinaryController.h"

#include "../lib/exceptions.h"


#include <boost/scope_exit.hpp>


#include <unistd.h>

LOG_DECLARE_FILE( "master" );

#define LOGGING_DECLARE_ALIAS_MDC(name) \
    log4cxx::MDC _location_mdc( "ALIAS", std::string("{") + name + "} " );

const int RETRY_WINDOW = 60;

Alias::Alias(
        const std::string& name, 
        const std::string& path, 
        const Policy& p, 
        const std::string& user,
        const std::string& logdir,
        const int preferredHostWait
        ) :
    _name(name),
    _path(path),
    _user(user),
    _logdir(logdir),
    _preferredHostWait(preferredHostWait),
    _my_policy(p),
    _retry_count(0),
    _waiting_for_agent(false),
    _halt_waiting_for_agent(false),
    _preferred_start_time()
{

}

void
Alias::remove_binary(
        const BinaryId& id
        )
{
    _halt_waiting_for_agent = true; // If anyone is waiting, STOP!
    LOG_DEBUG_MSG("Removing binary id " << id.str() << " from alias.");
    boost::mutex::scoped_lock scoped_lock(_mutex);
    _binaries.remove(id);
}

// Find agent on which to run based on policy.
AgentRepPtr
Alias::runPolicy(
        const BGAgentId& agent_id,
        bool restart = false
        )
{
    // Non-locking private method.  Must be called from locking method.
    LOGGING_DECLARE_ALIAS_MDC(_name);
    LOG_DEBUG_MSG("Policy check for alias " << _name);

    // Make SURE we are no longer waiting for an agent when this function is done!
    BOOST_SCOPE_EXIT( ( &_waiting_for_agent ) ) {
        _waiting_for_agent = false;
    } BOOST_SCOPE_EXIT_END;

    // We have to check the following to see if we can start the job:
    // 1) That we will not exceed the maximum number of occurrences
    // 2) That any specified agent is valid
    // 3) That any specified agent is associated with a host specified on config

    // See if we have room for another instance.
    // Add one because we haven't removed the binary for anything that has failed.
    if (_binaries.size() >= _my_policy.limit()) {
        std::ostringstream msg;
        msg << "Policy instance limit exceeded by binary for alias " << get_name();
        LOG_ERROR_MSG(msg.str());
        MasterController::handleErrorMessage(msg.str());
        std::map<std::string, std::string> details;
        details["ALIAS"] = get_name();
        MasterController::putRAS(ALIAS_FAIL_RAS, details);
        throw exceptions::InternalError(exceptions::WARN, msg.str());
    }

    // If they specified an agent, pop it out and see if it is valid
    AgentRepPtr agent_to_run;
    if (agent_id.str() != "") {
        LOG_DEBUG_MSG("Agent " << agent_id.str() << " specified.");
        agent_to_run = MasterController::get_agent_manager().findAgentRep(agent_id);
        if (agent_to_run == 0) {
            // Bad agent specified! Fail!
            std::ostringstream msg;
            msg <<"Bad agent " << agent_id.str() << " specified";
            std::map<std::string, std::string> details;
            details["ALIAS"] = get_name();
            MasterController::putRAS(ALIAS_FAIL_RAS, details);
            MasterController::handleErrorMessage(msg.str());
            if (restart) {
                // The selected agent doesn't exist. Need to wait to see if another one comes up with the same IP address.
                LOG_INFO_MSG("Agent " << agent_id.get_host().fqhn() << " not available. Will wait for new agent.");
                _waiting_for_agent = true;
                while (!MasterController::get_master_terminating()) {
                    // Wait forever, or until somebody stops bgmaster, for a valid agent to arrive.
                    if (_halt_waiting_for_agent) {
                        // Somebody wants us to stop.
                        LOG_DEBUG_MSG("Stopped waiting for new agent to run alias " << _name);
                        break;
                    }
                    const CxxSockets::Host h = agent_id.get_host();
                    const AgentRepPtr new_agent = MasterController::get_agent_manager().findAgentRep(h);
                    if (new_agent != 0) {
                        agent_to_run = new_agent;
                        break;
                    } else {
                        LOG_DEBUG_MSG("No agent on host " << h.fqhn() << " yet.");
                    }
                    sleep(1);
                }
                _waiting_for_agent = false;
            }
        } else {
            // See if there is a host policy for this alias and make sure the specified agent agrees with it.
            if (find_host_internal(agent_to_run->get_host()) != true) {
                if (!restart) {
                    std::ostringstream msg;
                    msg << "Bad agent " << agent_id.str() << " specified. Policy does not allow alias to run on this agent.";
                    std::map<std::string, std::string> details;
                    details["ALIAS"] = get_name();
                    MasterController::putRAS(ALIAS_FAIL_RAS, details);
                    MasterController::handleErrorMessage(msg.str());
                    throw exceptions::InternalError(exceptions::WARN, msg.str());
                }
            }
        }
    } else {
        // No agent host is specified, see if our config gives us direction.
        // Keep checking hosts until we get either the host we want (preferred or self)
        // or a backup host with an attempt timeout.
        bool winner = false;
        BOOST_FOREACH(const CxxSockets::Host& curr_host, _hosts) {
            // Loop through hosts and the first one that has a valid associated agent is our winner.
            agent_to_run = MasterController::get_agent_manager().findAgentRep(curr_host);
            if (agent_to_run) {
                LOG_DEBUG_MSG("Agent " << agent_to_run->get_host().uhn() << " specified by configuration "
                        << curr_host.uhn() << " found.");
                // Now we need to find out if we care whether it's a preferred host.
                if (curr_host.get_primary() == false) {
                    if ( _preferred_start_time.is_not_a_date_time() ) {
                        LOG_INFO_MSG( "waiting " << _preferredHostWait << " seconds for primary host" );
                        _preferred_start_time = boost::posix_time::microsec_clock::local_time();
                    }

                    const boost::posix_time::ptime now( boost::posix_time::microsec_clock::local_time() );
                    const boost::posix_time::time_duration duration( now - _preferred_start_time );
                    if ( duration.total_seconds() > _preferredHostWait ) {
                        winner = true;
                        LOG_INFO_MSG("Giving up after " << _preferredHostWait << " seconds waiting for the preferred agent host.");
                        LOG_INFO_MSG("Starting on agent " << curr_host.uhn() << ".");
                    } else {
                        LOG_INFO_MSG("Agent " << curr_host.uhn() << " is not the preferred agent host.");
                        LOG_INFO_MSG("waiting " << _preferredHostWait - duration.total_seconds() << " seconds");
                    }
                } else {
                    winner = true;
                }
            } else {
                LOG_DEBUG_MSG("Agent at " << curr_host.uhn() << " not connected in.");
                agent_to_run.reset();
            }
            if (winner) {
                break;
            }
        }

        // If we didn't find anybody.
        if (winner == false) {
            LOG_DEBUG_MSG("No running agent selected or defined by property.");
            if (_hosts.empty()) {
                LOG_DEBUG_MSG("No agent configured. Pick any agent.");
                // We have nothing configured so... pick an agent manager and send out the request.
                agent_to_run = MasterController::get_agent_manager().pickAgent();
            } else {
                std::ostringstream msg;
                std::ostringstream hosts;
                bool first = true;
                BOOST_FOREACH(const CxxSockets::Host& curr_host, _hosts) {
                    if (!first) {
                        hosts << ", " << curr_host.uhn();
                    } else {
                        first = false;
                        hosts << curr_host.uhn();
                    }
                }
                msg << "No agent " << hosts.str() << " specified by configuration for " << get_name() << " is running.";
                MasterController::handleErrorMessage(msg.str());
                throw exceptions::InternalError(exceptions::WARN, msg.str());
            }
        }
    }
    _preferred_start_time = boost::posix_time::ptime();
    return agent_to_run;
}

AgentRepPtr
Alias::validateStartAgent(
        const BGAgentId& agent_id
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    if (_waiting_for_agent) {
        throw exceptions::InternalError(exceptions::INFO, "Cannot start alias until agent started.");
    }
    LOGGING_DECLARE_ALIAS_MDC(_name);
    // This just locks and calls the non-locking private method
    boost::mutex::scoped_lock scoped_lock(_mutex);
    return runPolicy(agent_id);
}

AgentRepPtr
Alias::evaluatePolicy(
        Policy::Trigger trig,
        BGAgentId& agent,
        const BinaryId& failed_bid,
        BinaryControllerPtr bptr
        )
{
    LOGGING_DECLARE_ALIAS_MDC(_name);
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_mutex);

    AgentRepPtr rep;

    // Check the failed binary id for the start time to see if it is within the retry window.
    // If it is, bump the retry count and we'll check the policy later.
    if (!bptr) {
        BinaryLocation bloc;
        if (MasterController::get_agent_manager().findBinary(failed_bid, bloc)) {
            bptr = bloc.first;
        }
    }

    if (!bptr) {  // Still no binary controller. We'll construct a dummy one.
        const BinaryControllerPtr p(new BinaryController());
        bptr = p;
    }

    const boost::posix_time::ptime start = bptr->get_start_time();
    const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    LOG_DEBUG_MSG("Start time=" << boost::posix_time::to_simple_string(start)
                  << " Now=" << boost::posix_time::to_simple_string(now));
    const boost::posix_time::time_duration td = now - start;
    if (td.total_seconds() < RETRY_WINDOW) {
        ++_retry_count;
        LOG_DEBUG_MSG( "retry count: " << _retry_count );
    }

    Behavior bvr;
    bool p = _my_policy.get_behavior(trig, bvr);
    if (p == false) {
        // No Behavior defined.  No policy to execute.
        LOG_INFO_MSG("No behavior defined for " << get_name() << ".");
    } else if (bvr.get_action() == Behavior::FAILOVER) {
        CxxSockets::Host target = bvr.findFailoverTarget(agent.get_host());
        bool done = false;
        CxxSockets::Host oldtarget = agent.get_host();
        while (!done) {  // We'll do retries, going back and forth between fail-over pairs.
            // We have to fail over.
            if (target.ip().length() == 0) {
                std::ostringstream errmsg;
                errmsg << "Invalid policy, no fail-over target from " << oldtarget.fqhn() << ". Check the configuration.";
                throw exceptions::InternalError(exceptions::WARN, errmsg.str());
            }
            std::string failname = "";
            if (find_host_internal(target) == false) {
                failname = target.fqhn();
            }
            if (find_host_internal(oldtarget) == false) {
                failname = oldtarget.fqhn();
            }
            if (failname.length() > 0) {
                std::ostringstream errmsg;
                errmsg << "Host " << failname << " not in host list for "  << get_name();
                throw exceptions::InternalError(exceptions::WARN, errmsg.str());
            }

            LOG_INFO_MSG("Failing over from agent " << oldtarget.fqhn() << " to agent " << target.fqhn() << ".");

            // Now find the fail-over agent.
            BGAgentId target_agent = MasterController::get_agent_manager().findAgentId(target);
            if (target_agent.str() == "") {
                // Whoops. The target's not in our agent list. Let's see if there's a fail-over from the target.
                std::ostringstream no_agent_msg;
                no_agent_msg << "Fail-over agent " << target.fqhn() << " is not connected to bgmaster_server. Trying the next fail-over agent.";
                LOG_ERROR_MSG(no_agent_msg.str());
                oldtarget = target;
                target = bvr.findFailoverTarget(oldtarget);
                if (bvr.get_retries() < _retry_count) {
                    // Update database with RAS message
                    std::ostringstream msg;
                    msg << "Retry count within " << RETRY_WINDOW << " second window failed for alias " << get_name() << " with a count of "
                        << _retry_count - 1 << " and " << bvr.get_retries() << " allowed.";
                    LOG_INFO_MSG(msg.str());
                    MasterController::handleErrorMessage(msg.str());
                    std::map<std::string, std::string> details;
                    details["ALIAS"] = get_name();
                    MasterController::putRAS(ALIAS_FAIL_RAS, details);
                    _retry_count = 0;
                    done = true;
                }
                ++_retry_count;
                continue;
            }
            if (bvr.get_retries() > (_retry_count==0?_retry_count:_retry_count - 1)) {
                rep = runPolicy(target_agent, true);
                // Update database with RAS message
                std::map<std::string, std::string> details;
                details["ALIAS"] = get_name();
                details["SOURCE"] = agent.get_host().fqhn();
                details["TARGET"] = target.fqhn();
                MasterController::putRAS(ALIAS_FAILOVER_RAS, details);
                done = true;
            } else {
                std::ostringstream msg;
                msg << "Retry count within retry window failed for alias " << get_name() << " with a count of "
                    << _retry_count - 1 << " and " << bvr.get_retries() << " allowed.";
                LOG_INFO_MSG(msg.str());
                MasterController::handleErrorMessage(msg.str());
                std::map<std::string, std::string> details;
                details["ALIAS"] = get_name();
                MasterController::putRAS(ALIAS_FAIL_RAS, details);
                _retry_count = 0;
                done = true;
            }
        }
        return rep;

    } else if (bvr.get_action() == Behavior::RESTART) {
        if (bvr.get_retries() > (_retry_count==0?_retry_count:_retry_count - 1)) {
            LOG_INFO_MSG(
                    "Restarting alias " << get_name() << " on agent " << agent.get_host().fqhn() << " " <<
                    _retry_count << " of " << bvr.get_retries() 
                    );
            // Update database with RAS message
            std::map<std::string, std::string> details;
            details["ALIAS"] = get_name();
            MasterController::putRAS(ALIAS_RESTART_RAS, details);
            rep = runPolicy(agent, true);
        } else {
            std::ostringstream msg;
            msg << "Retry count within retry window failed for alias " << get_name() << " with a count of "
                << _retry_count - 1 << " and " << bvr.get_retries() << " allowed.";
            LOG_INFO_MSG(msg.str());
            MasterController::handleErrorMessage(msg.str());
            std::map<std::string, std::string> details;
            details["ALIAS"] = get_name();
            MasterController::putRAS(ALIAS_FAIL_RAS, details);
            _retry_count = 0;
        }
    } else if (bvr.get_action() == Behavior::CLEANUP) {
        // just return, we're done here
        return rep;
    } else {
        // Invalid! Throw exception!
        throw exceptions::InternalError(exceptions::WARN, "Invalid or nonexistent fail-over policy.");
    }
    return rep;
}
