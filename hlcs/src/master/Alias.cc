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

#include <ifaddrs.h>
#include <boost/scope_exit.hpp>
#include "Host.h"
#include "Policy.h"
#include "BGMaster.h"
#include "BGMasterClientProtocol.h"
#include "BGMasterExceptions.h"
#include "Host.h"
#include "Alias.h"
#include "MasterRasMetadata.h"
#include "BinaryController.h"

LOG_DECLARE_FILE( "master" );

const int RETRY_WINDOW = 60;
const unsigned MAX_PREFERRED_FAIL = 15;

bool isMe(Host& isme) {
    LOG_DEBUG_MSG(__FUNCTION__);
    std::vector<std::string> IPs;
    // Discover IP addresses on the machine for later use
    struct ifaddrs* ifa;
    struct ifaddrs* base_ifa;
    if(getifaddrs(&ifa) < 0) {
        LOG_WARN_MSG("No valid interface IPs");
    }
    
    base_ifa = ifa;
    BOOST_SCOPE_EXIT( ( &base_ifa ) ) {
        freeifaddrs(base_ifa);
    } BOOST_SCOPE_EXIT_END;

    char buff[128];
    while (ifa->ifa_next) {
        bzero(buff, 128);
        if(ifa) {
            if(ifa->ifa_addr->sa_family == AF_INET) {
                inet_ntop(ifa->ifa_addr->sa_family,
                          &((struct sockaddr_in*)(ifa->ifa_addr))->sin_addr.s_addr,
                          buff, 128);
            } else if(ifa->ifa_addr->sa_family == AF_INET6) {
                inet_ntop(ifa->ifa_addr->sa_family,
                          &((struct sockaddr_in6*)(ifa->ifa_addr))->sin6_addr.s6_addr,
                          buff, 128);
            }
            std::ostringstream addr;
            addr << buff;
            if(ifa->ifa_addr->sa_family == AF_INET6 && addr.str() != "::1") {
                addr << "%" << ifa->ifa_name;
            }
            Host curr_host(addr.str());
            if(curr_host == isme)
                return true;
        }
        ifa = ifa->ifa_next;
    }
    return false;
}

void Alias::remove_binary(BinaryId& id) {
    _halt_waiting_for_agent = true; // If anyone is waiting, STOP!
    LOG_DEBUG_MSG("Removing binary id " << id.str() << " from alias.");
    boost::mutex::scoped_lock scoped_lock(_mutex);
    _binaries.remove(id); 
}

// Find agent on which to run based on policy.
BGAgentRepPtr Alias::runPolicy(BGAgentId& agent_id, bool restart = false) {
    // Non-locking private method.  Must be called from locking method.
    LOGGING_DECLARE_ALIAS_MDC(_name);
    LOG_DEBUG_MSG("Policy check for alias " << _name);

    // Make SURE we are no longer waiting for an agent when
    // this function is done!
    BOOST_SCOPE_EXIT( ( &_waiting_for_agent ) ) {
        _waiting_for_agent = false;
    } BOOST_SCOPE_EXIT_END;

    // We have to check the following to see if we can start the job:
    // 1) That we will not exceed the maximum number of occurences
    // 2) That any specified agent is valid
    // 3) That any specified agent is associated with a host specified on config

    // See if we have room for another instance
    // Add one because we haven't removed the binary for
    // anything that has failed.
    if(_binaries.size() >= _my_policy.limit()) {
        std::ostringstream msg;
        msg << "Policy instance limit exceeded by binary for alias " << get_name();
        LOG_DEBUG_MSG(msg.str());
        BGMasterController::handleErrorMessage(msg.str());
        std::map<std::string, std::string> details;
        details["ALIAS"] = get_name();
        BGMasterController::putRAS(ALIAS_FAIL_RAS, details);
        throw BGMasterExceptions::InternalError(BGMasterExceptions::WARN, msg.str());
    }

    // If they specified an agent, pop it out and see if it is valid
    BGAgentRepPtr agent_to_run;
    if(agent_id.str() != "") {
        LOG_DEBUG_MSG("Agent " << agent_id.str() << " specified.");
        agent_to_run = BGMasterController::get_agent_manager().findAgentRep(agent_id);
        if(agent_to_run == 0) {
            // Bad agent specified!  Fail!
            std::ostringstream msg;
            msg <<"Bad agent " << agent_id.str() << " specified";
            std::map<std::string, std::string> details;
            details["ALIAS"] = get_name();
            BGMasterController::putRAS(ALIAS_FAIL_RAS, details);
            BGMasterController::handleErrorMessage(msg.str());
            if(restart) {
                // The selected agent doesn't exist.  Need to wait to see if another one comes up
                // with the same IP address.
                LOG_INFO_MSG("Agent " << agent_id.get_host().fqhn() << " not available.  Will wait for new one.");
                _waiting_for_agent = true;
                while(!BGMasterController::get_master_terminating()) {
                    // Wait forever, or until somebody stops bgmaster, for a valid agent to arrive.
                    if(_halt_waiting_for_agent == true) {
                        // Somebody wants us to stop.
                        LOG_DEBUG_MSG("Stopping the wait for a new Agent to run alias " << _name);
                        break;
                    }
                    Host h = agent_id.get_host();
                    BGAgentRepPtr new_agent = BGMasterController::get_agent_manager().findAgentRep(h);
                    if(new_agent != 0) {
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
            // See if there is a host policy for this alias and
            // make sure the specified agent agrees with it.
            if(find_host_internal(agent_to_run->get_host()) != true) {
                if(!restart) {
                    std::ostringstream msg;
                    msg << "Bad agent " << agent_id.str() << " specified.  "
                        << "Policy does not allow this alias to run"
                        << " on this agent.";

                    std::map<std::string, std::string> details;
                    details["ALIAS"] = get_name();
                    BGMasterController::putRAS(ALIAS_FAIL_RAS, details);
                    BGMasterController::handleErrorMessage(msg.str());
                    throw BGMasterExceptions::InternalError(BGMasterExceptions::WARN, msg.str());
                }
            }
        }
    } else {
        // No agent host is specified, see if our config
        // gives us direction.
        // Keep checking hosts until we get either the host we want (preferred or self) 
        // or a backup host with an attempt timeout.
        bool winner = false;
        bool preferred = false;
        BOOST_FOREACH(Host& curr_host, _hosts) {
            // Loop through hosts and the first one
            // that has a valid associated agent is our
            // winner.
            agent_to_run = BGMasterController::get_agent_manager().findAgentRep(curr_host);
            if(agent_to_run) {
                LOG_INFO_MSG("Agent " << agent_to_run->get_host().uhn() << " specified by config "
                             << curr_host.uhn() << " found");
                // Now we need to find out if we care whether it's a preferred host.
                if(curr_host.get_primary() == false && isMe(curr_host) == false) {
                    if(_preferred_fails >= MAX_PREFERRED_FAIL) {
                        winner = true;
                        LOG_INFO_MSG("Giving up waiting for the preferred host.  Starting on "
                                     << curr_host.uhn() << ".");
                    } else {
                        LOG_INFO_MSG("Host " << curr_host.uhn() << " is not the preferred host ");
                        ++_preferred_fails;
                    }
                } else {
                    preferred = true;
                    winner = true;
                }
            } else {
                LOG_DEBUG_MSG("Agent at " << curr_host.uhn() << " not checked in.");
                //                ++_preferred_fails;
                agent_to_run.reset();
            }
            if(winner) {
                break;
            }
        }

        // If we didn't find anybody.
        if(winner == false) {
            LOG_DEBUG_MSG("No running agent selected or defined by property.");
            if(_hosts.size() == 0) {
                LOG_DEBUG_MSG("No agent configured.  Pick any agent.");
                // We have nothing configured so...
                // Pick an agent manager and send out the request
                agent_to_run = BGMasterController::get_agent_manager().pickAgent();
            } else {
                std::ostringstream msg;
                std::ostringstream hosts;
                bool first = true;
                BOOST_FOREACH(Host& curr_host, _hosts) {
                    if(!first) { hosts << ", " << curr_host.uhn(); }
                    else { first = false; hosts << curr_host.uhn(); }
                }
                msg << "No agent " << hosts.str() << " specified by config for " << get_name() << " running";
                std::map<std::string, std::string> details;
                details["ALIAS"] = get_name();
                BGMasterController::putRAS(ALIAS_FAIL_RAS, details);
                BGMasterController::handleErrorMessage(msg.str());
                throw BGMasterExceptions::InternalError(BGMasterExceptions::WARN, msg.str());
            }
        }
    }
    _preferred_fails = 0;
    return agent_to_run;
}

BGAgentRepPtr Alias::validateStartAgent(BGAgentId& agent_id) {
    LOG_INFO_MSG(__FUNCTION__);
   if(_waiting_for_agent) {
        throw BGMasterExceptions::InternalError(BGMasterExceptions::INFO, "Cannot start alias until agent started");
    }
    LOGGING_DECLARE_ALIAS_MDC(_name);
    // This just locks and calls the nonlocking private method
    boost::mutex::scoped_lock scoped_lock(_mutex);
    return runPolicy(agent_id);
}

BGAgentRepPtr Alias::evaluatePolicy(Policy::Trigger trig, BGAgentId& agent, BinaryId& failed_bid, BinaryControllerPtr bptr) {
    LOGGING_DECLARE_ALIAS_MDC(_name);
    LOG_INFO_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_mutex);

    BGAgentRepPtr rep;

    // Check the failed binary id for the
    // start time to see if it is within the retry window.  If it is,
    // bump the retry count and we'll check the policy later.
    if(!bptr) {
        BinaryLocation bloc;
        if(BGMasterController::get_agent_manager().findBinary(failed_bid, bloc)) {
            bptr = bloc.first;
        }
    }

    if(!bptr) {  // Still no binary controller.  We'll construct a dummy one.
        BinaryControllerPtr p(new BinaryController());
        bptr = p;
    }

    boost::posix_time::ptime start = bptr->get_start_time();
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    LOG_DEBUG_MSG("Start time=" << boost::posix_time::to_simple_string(start)
                  << " Now=" << boost::posix_time::to_simple_string(now));
    boost::posix_time::time_duration td = now - start;
    if(td.total_seconds() < RETRY_WINDOW) {
        ++_retry_count;
    }

    Policy::Behavior::Behavior bvr;
    bool p = _my_policy.get_behavior(trig, bvr);
    if(p == false) {
        // No Behavior defined.  No policy to execute.
        LOG_INFO_MSG("No behavior defined for " << get_name()
                     << ".  ");
    } else if(bvr.get_action() == Policy::FAILOVER) {
        Host target = bvr.findFailoverTarget(agent.get_host());
        bool done = false;
        Host oldtarget = agent.get_host();
        while(!done) {  // We'll do retries, going back and forth between failover pairs.
            // We have to fail over.
            if(target.ip().length() == 0) {
                std::ostringstream errmsg;
                errmsg << "Invalid policy.  No failover target from " << oldtarget.fqhn()
                       << ".  Check configuration.";
                throw BGMasterExceptions::InternalError(BGMasterExceptions::WARN,
                                                        errmsg.str());
            }
            std::string failname = "";
            if(host_in(target) == false) {
                failname = target.fqhn();
            }
            if(host_in(oldtarget) == false) {
                failname = oldtarget.fqhn();
            }
            if(failname.length() > 0) {
                std::ostringstream errmsg;
                errmsg << "Host " << failname 
                       << " not in host list for "
                       << get_name();
                throw BGMasterExceptions::InternalError(BGMasterExceptions::WARN,
                                                        errmsg.str());
            }
            
            LOG_INFO_MSG("Failing over from " << oldtarget.fqhn()
                         << " to " << target.fqhn());

            // Now find the failover target.
            BGAgentId target_agent = BGMasterController::get_agent_manager().findAgentId(target);
            if(target_agent.str() == "") {
                // Whoops.  The target's not in our agent list.  Let's see if there's a failover
                // from the target.
                std::ostringstream no_agent_msg;
                no_agent_msg << "Failover target " << target.fqhn() << " is not connected to "
                             << "bgmaster_server.  Trying the next failover pair.";
                LOG_ERROR_MSG(no_agent_msg.str());
                oldtarget = target;
                target = bvr.findFailoverTarget(oldtarget);
                sleep(2);
                if(bvr.get_retries() < _retry_count) {
                    // Update database with ras message
                    std::ostringstream msg;
                    msg << "Retry count within retry window failed for alias "
                        << get_name() << " with a count of " << _retry_count - 1
                        << " and " << bvr.get_retries() << " allowed.";
                    LOG_INFO_MSG(msg.str());
                    BGMasterController::handleErrorMessage(msg.str());
                    std::map<std::string, std::string> details;
                    details["ALIAS"] = get_name();
                    BGMasterController::putRAS(ALIAS_FAIL_RAS, details);
                    _retry_count = 0;
                    done = true;
                }
                ++_retry_count;
                continue;
            }
            if(bvr.get_retries() > (_retry_count==0?_retry_count:_retry_count - 1)) {
                rep = runPolicy(target_agent, true);
                // Update database with ras message
                std::map<std::string, std::string> details;
                details["ALIAS"] = get_name();
                details["SOURCE"] = agent.get_host().fqhn();
                details["TARGET"] = target.fqhn();
                BGMasterController::putRAS(ALIAS_FAILOVER_RAS, details);
                done = true;
            } else {
                std::ostringstream msg;
                msg << "Retry count within retry window failed for alias "
                    << get_name() << " with a count of " << _retry_count - 1
                    << " and " << bvr.get_retries() << " allowed.";
                LOG_INFO_MSG(msg.str());
                BGMasterController::handleErrorMessage(msg.str());
                std::map<std::string, std::string> details;
                details["ALIAS"] = get_name();
                BGMasterController::putRAS(ALIAS_FAIL_RAS, details);
                _retry_count = 0;
                done = true;
            }
        }
        return rep;

    } else if(bvr.get_action() == Policy::RESTART) {
        if(bvr.get_retries() > (_retry_count==0?_retry_count:_retry_count - 1)) {
            LOG_INFO_MSG("Restarting on " << agent.get_host().fqhn());
            // Update database with ras message
            std::map<std::string, std::string> details;
            details["ALIAS"] = get_name();
            BGMasterController::putRAS(ALIAS_RESTART_RAS, details);
            rep = runPolicy(agent, true);
        } else {
            std::ostringstream msg;
            msg << "Retry count within retry window failed for alias "
                << get_name() << " with a count of " << _retry_count - 1
                << " and " << bvr.get_retries() << " allowed.";
            LOG_INFO_MSG(msg.str());
            BGMasterController::handleErrorMessage(msg.str());
            std::map<std::string, std::string> details;
            details["ALIAS"] = get_name();
            BGMasterController::putRAS(ALIAS_FAIL_RAS, details);
            _retry_count = 0;
        }
    } else if (bvr.get_action() == Policy::CLEANUP) {
        // just return, we're done here
        return rep;
    } else {
        // Invalid!  Throw exception!
        throw BGMasterExceptions::InternalError(BGMasterExceptions::WARN,
                                                "Invalid or nonexistent failover policy ");
    }
    return rep;
}

bool AliasList::find_alias(std::string& al, AliasPtr& alias) {
    LOG_TRACE_MSG(__FUNCTION__);
    BOOST_FOREACH(AliasPtr& curr_alias, _alias_list) {
        if(curr_alias->get_name() == al) {
            alias = curr_alias;
            return true;
        }
    }
    return false;
}

#if 0
bool AliasList::find_alias(std::string& al, AliasPtr& alias) {
    LOG_INFO_MSG(__FUNCTION__);
    BOOST_FOREACH(AliasPtr& curr_alias, _alias_list) {
        if(curr_alias->get_name() == al) {
            alias = &curr_alias;
            return true;
        }
    }
    return false;
}
#endif
