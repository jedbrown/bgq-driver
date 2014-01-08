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
#include "ClientManager.h"
#include "LockFile.h"
#include "MasterController.h"
#include "ras.h"
#include "Registrar.h"

#include "../lib/exceptions.h"


#include <db/include/api/tableapi/DBConnectionPool.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/version.h>




#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

LOG_DECLARE_FILE( "master" );

const int CIRC_BUFFER_SIZE = 50;

extern LockFile* lock_file;

//! \brief static member instantiations
boost::mutex MasterController::_policy_build_mutex;
bgq::utility::Properties::Ptr MasterController::_props;
bool MasterController::_master_terminating = false;
bool MasterController::_end_requested = false;
bool MasterController::_start_servers = false;
AliasList MasterController::_aliases;
boost::barrier MasterController::_start_barrier(3);
std::string MasterController::_master_logdir;
bool MasterController::_master_db;
bool MasterController::_stop_once = false;
bool MasterController::_start_once = true;
boost::posix_time::ptime MasterController::_start_time;
std::string MasterController::_version_string;
LockingStringRingBuffer MasterController::_err_buff(CIRC_BUFFER_SIZE);
LockingStringRingBuffer MasterController::_history_buff(CIRC_BUFFER_SIZE);
std::vector<ClientProtocolPtr> MasterController::_monitor_prots;
boost::mutex MasterController::_monitor_prots_mutex;

AgentManager MasterController::_agent_manager;
ClientManager MasterController::_client_manager;

DBUpdater MasterController::_updater;

Registrar MasterController::_agent_registrar;
Registrar MasterController::_client_registrar;

MasterController::MasterController(
        const bgq::utility::Properties::Ptr& properties
        ) :
    _pid( getpid() )
{
    _props = properties;
}

void
MasterController::putRAS(
        const unsigned int id,
        const std::map<std::string, std::string>& details
        )
{
    if (_master_db) { // Only bother with RAS if we've got a DB
        RasEventImpl event(id);
        for (std::map<std::string, std::string>::const_iterator it = details.begin(); it != details.end(); ++it) {
            event.setDetail(it->first, it->second);
        }

        RasEventHandlerChain::handle(event);

        std::string rasMessage("insert into tbgqeventlog (msg_id,component,category,severity,message) values(");
        rasMessage.append("'").append(event.getDetail(RasEvent::MSG_ID)).append("'");
        rasMessage.append(",'").append(event.getDetail(RasEvent::COMPONENT)).append("'");
        rasMessage.append(",'").append(event.getDetail(RasEvent::CATEGORY)).append("'");
        rasMessage.append(",'").append(event.getDetail(RasEvent::SEVERITY)).append("'");
        rasMessage.append(",'").append(event.getDetail(RasEvent::MESSAGE)).append("'");
        rasMessage.append(")");

        LOG_DEBUG_MSG("Sending RAS request to DB updater: " << rasMessage);
        _updater.addMsg(rasMessage);
    }
}

void
MasterController::stopThreads(
        const bool end_binaries,
        const int signal
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    if (_stop_once) {
        return;
    } else {
        _stop_once = true;
    }

    LOG_INFO_MSG("Stopping all threads.");

    // Stop the agent registrar.
    _agent_registrar.cancel();

    // Stop agent threads.
    _agent_manager.cancel(end_binaries, signal);

    // Stop client threads.
    _client_manager.cancel();

    // Stop the client registrar.
    _client_registrar.cancel();
    std::map<std::string, std::string> details;
    details["PID"] = boost::lexical_cast<std::string>(getpid());

    putRAS(MASTER_SHUTDOWN_RAS, details);
    // End the DB updater.
    _updater.end();
}

void
MasterController::handleErrorMessage(
        const std::string& msg
        )
{
    LOG_ERROR_MSG( __FUNCTION__ << ": " << msg);
    std::ostringstream errmsg;
    std::vector<ClientProtocolPtr> deadClients;
    boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
    errmsg << now << ": " << msg;
    _err_buff.push_back(errmsg.str());
    // Send it to all of the monitors.
    BGMasterClientProtocolSpec::ErrorMessage error(errmsg.str());
    boost::mutex::scoped_lock scoped_lock(_monitor_prots_mutex);
    BOOST_FOREACH(const ClientProtocolPtr& prot, _monitor_prots) {
        try {
	  prot->sendOnly(error.getClassName(), error);
        } catch (const CxxSockets::Error& e) {
            // If we get an error we assume that the client has been killed and should be removed
	    // otherwise we have a potential memory leak.
            LOG_WARN_MSG(e.what());
	    deadClients.push_back(prot);
        }
    }
    // Loop through the deadClients and remove them from _monitor_prots container.
    for (unsigned i = 0; i < deadClients.size(); ++i) {
        LOG_WARN_MSG("Removing master_monitor client instance after socket error ...");
        MasterController::get_monitor_prots().erase(std::remove(MasterController::get_monitor_prots().begin(),
							        MasterController::get_monitor_prots().end(),
							        deadClients[i]), MasterController::get_monitor_prots().end());
    }
    return;
}

void
MasterController::getErrorMessages(
        std::vector<std::string>& messages
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    // Going to pop items out of the circular buffer.
    _err_buff.getContents(messages);
}

void
MasterController::addHistoryMessage(
        const std::string& message
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::ostringstream msg;
    std::vector<ClientProtocolPtr> deadClients;
    boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
    msg << now << ": " << message;
    _history_buff.push_back(msg.str());
    // Send it to all of the monitors.
    BGMasterClientProtocolSpec::EventMessage event(msg.str());
    boost::mutex::scoped_lock scoped_lock(_monitor_prots_mutex);
    BOOST_FOREACH(const ClientProtocolPtr& prot, _monitor_prots) {
        try {
	  prot->sendOnly(event.getClassName(), event);
        } catch (const CxxSockets::Error& e) {
            // If we get an error we assume that the client has been killed and should be removed
	    // otherwise we have a potential memory leak.
            LOG_WARN_MSG(e.what());
	    deadClients.push_back(prot);
        }
    }
    // Loop through the deadClients and remove them from _monitor_prots container.
    for (unsigned i = 0; i < deadClients.size(); ++i) {
        LOG_WARN_MSG("Removing master_monitor client instance after socket error ...");
        MasterController::get_monitor_prots().erase(std::remove(MasterController::get_monitor_prots().begin(),
							        MasterController::get_monitor_prots().end(),
							        deadClients[i]), MasterController::get_monitor_prots().end());
    }
    return;
}

void
MasterController::getHistoryMessages(
        std::vector<std::string>& messages
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    _history_buff.getContents(messages);
}

void
MasterController::buildHostList(
        const bgq::utility::Properties::Section& hosts,
        const std::vector<std::string>& exclude_list,
        std::ostringstream& failmsg
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    bool firstdup = true;

    BOOST_FOREACH(const bgq::utility::Properties::Pair& keyval, hosts) {
        bool found = false;

        BOOST_FOREACH(const AliasPtr& al, _aliases) {
            const std::string &all_hosts(keyval.second);
            if (keyval.first == al->get_name()) {
                std::vector<std::string>::const_iterator it = std::find(exclude_list.begin(), exclude_list.end(), al->get_name());
                if (it != exclude_list.end()) {
                    // This was defined previously.  Refresh is additive.  We don't want to mess with this on a running system.
                    if (firstdup) {
                        firstdup = false;
                        failmsg << "Host list previously set for " << al->get_name();
                    } else {
                        failmsg << ", " << al->get_name();
                    }
                    std::ostringstream msg;
                    msg << "Host list previously set for " << al->get_name();
                    handleErrorMessage(msg.str());
                } else {
                    // Found an existing alias so parse out the hosts and update the alias
                    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
                    boost::char_separator<char> sep(","); // comma separated
                    tokenizer tok(all_hosts, sep);
                    bool first = true;
                    BOOST_FOREACH(const std::string& curr_host, tok) {
                        try {
                            CxxSockets::Host h(curr_host);
                            if (first) {
                                LOG_DEBUG_MSG("Host " << h.uhn() << " is the preferred host.");
                                h.set_primary(true);
                            }
                            first = false;
                            LOG_DEBUG_MSG("Adding host " << h.uhn() << " to alias " << al->get_name());
                            al->add_host(h);
                        } catch ( const CxxSockets::Error& e ) {
                            throw exceptions::APIUserError(exceptions::INFO, e.what());
                        }
                    }
                }
                found = true;
                break;
            }
        }

        if (!found) {
            // Didn't find an alias.  Assume the [master.binmap] entry has been commented out.
	    // Log a warning message in case something else is going on.
	    LOG_WARN_MSG( "Alias " << keyval.first << " was not defined. Skipping [master.policy.host_list] entry ...");
        }
    }
}

void
MasterController::buildFailover(
        const bgq::utility::Properties::Section& failover,
        std::multimap<Policy::Trigger,Behavior>& behaviors
        )
{
    LOG_TRACE_MSG(__FUNCTION__);

    // Finally, go through the failover list
    BOOST_FOREACH(const bgq::utility::Properties::Pair& keyval, failover) {

        LOG_DEBUG_MSG("Building policy for " << keyval.first << "="  << keyval.second);

        // First create a policy object, then assign to an alias.
        // Tokenize the policy.
        const std::string failover_policy = boost::algorithm::erase_all_copy(keyval.second, " ");
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(","); // Comma separated
        tokenizer tok(failover_policy, sep);
        tokenizer::iterator curr_tok = tok.begin();

        if (curr_tok == tok.end()) {
            // No policy
            // Bad news, config file is wrong
            std::ostringstream msg;
            msg << "Invalid failover policy configuration. No policy specified for " << keyval.first;
            handleErrorMessage(msg.str());
            throw exceptions::ConfigError(exceptions::WARN, msg.str());
        }

        // Trigger is first
        Policy::Trigger my_trigger = Policy::string_to_trigger(*curr_tok);
        if (my_trigger == Policy::INVALID_TRIGGER) {
            // Bad news, config file is wrong
            std::ostringstream msg;
            msg << "Invalid failover trigger " << my_trigger << " specified.";
            handleErrorMessage(msg.str());
            throw exceptions::ConfigError(exceptions::WARN, msg.str());
        }

        ++curr_tok; // second is the action to take
        if (curr_tok == tok.end()) {
            // Bad. No action specified
            std::ostringstream msg;
            msg << "Invalid failure configuration for " << keyval.first <<  " . No action specified.";
            handleErrorMessage(msg.str());
            throw exceptions::ConfigError(exceptions::WARN, msg.str());
        }

        Behavior::Action act = Behavior::string_to_action(*curr_tok);
        if (act == Behavior::INVALID_ACTION) {
            std::ostringstream msg;
            msg << "Invalid failure action '" << *curr_tok << "' specified.";
            handleErrorMessage(msg.str());
            throw exceptions::ConfigError(exceptions::WARN, msg.str());
        }

        ++curr_tok; // third is the number of retries
        if (curr_tok == tok.end()) {
            // Bad.  No action specified
            std::ostringstream msg;
            msg << "Invalid failure configuration for " << keyval.first <<  " . No retries specified.";
            handleErrorMessage(msg.str());
            throw exceptions::ConfigError(exceptions::WARN, msg.str());
        }

        std::ostringstream msg;
        unsigned short retries = 0;
        try {
            retries = boost::lexical_cast<unsigned short>(*curr_tok);
        } catch (const boost::bad_lexical_cast& e) {
            msg << e.what();
        }

        if (retries == 0) {
            // Bad.  No action specified
            msg << "Invalid retry configuration for " << keyval.first <<  ". Number of retries, " << retries << ", must be greater than zero.";
            handleErrorMessage(msg.str());
            throw exceptions::ConfigError(exceptions::WARN, msg.str());
        }

        ++curr_tok; // fourth is the associated host pairs

        std::map<CxxSockets::Host, CxxSockets::Host> failpairs;

        if (curr_tok == tok.end()) {
            // We're at the end.  This is OK.  If our action is failover,
            // we'll do the default of picking a random target, but note it in the log.
            if (act == Behavior::FAILOVER) {
                std::ostringstream msg;
                msg << "Failure configuration for " << keyval.first <<  " 'failover' action specified, but no failover pair specified.";
                LOG_INFO_MSG(msg.str());
            }
        } else {
            if (act == Behavior::RESTART) {
                // If there's a restart action, failover pairs make no sense!
                std::ostringstream msg;
                msg << "Bad policy configuration specified, cannot specify failover pairs"  << " for a \"restart\" policy.";
                handleErrorMessage(msg.str());
                throw exceptions::ConfigError(exceptions::WARN, msg.str());
            }

            // We have failover pairs to check. Parse it to a list of failover pairs.
            std::string host_pairs = *curr_tok;

            // Host pair is in the format of host:host|host:host
            // So loop through all tokens separated by a '|' and build a pair for each.
            boost::char_separator<char> bar_sep("|");
            tokenizer bar_tok(host_pairs, bar_sep);

            BOOST_FOREACH(const std::string& current, bar_tok) {
                if (current.find_first_of(":") == std::string::npos) {
                    std::ostringstream msg;
                    msg << "Failover configuration syntax incorrect. Missing \":\"";
                    handleErrorMessage(msg.str());
                    throw exceptions::ConfigError(exceptions::WARN, msg.str());
                }

                if (current.find_first_of(":") != current.find_last_of(":")) {
                    // More than one ':' in this failover pair!!!
                    std::ostringstream msg;
                    msg << "Failover configuration syntax incorrect. More than one ':' in the pair \"" << current << " \"";
                    handleErrorMessage(msg.str());
                    throw exceptions::ConfigError(exceptions::WARN, msg.str());
                }

                boost::char_separator<char> col_sep(":");
                tokenizer col_tok(current, col_sep);
                tokenizer::iterator col_tok_it = col_tok.begin();
                try {
                    CxxSockets::Host from_host(*col_tok_it);
                    ++col_tok_it;
                    if (col_tok_it == col_tok.end()) {
                        std::ostringstream msg;
                        msg << "Failover pair for policy " << keyval.first << " missing a target.";
                        handleErrorMessage(msg.str());
                        throw exceptions::ConfigError(exceptions::WARN, msg.str());
                    }
                    CxxSockets::Host to_host(*col_tok_it);
                    LOG_DEBUG_MSG("Created failover pair for " << from_host.uhn() << " to "
                                  << to_host.uhn() << " for alias " << keyval.first);
                    failpairs[from_host] = to_host;
                } catch ( const CxxSockets::Error& e ) {
                    throw exceptions::APIUserError(exceptions::INFO, e.what());
                }
            }

        }
        // Now create the behavior to associate with the trigger
        Behavior my_behavior(keyval.first, act, failpairs, retries);
        behaviors.insert(std::pair<Policy::Trigger,Behavior>(my_trigger,my_behavior));
    }
}

void
MasterController::buildInstances(
        const bgq::utility::Properties::Section& instances,
        const std::vector<std::string>& exclude_list,
        std::ostringstream& failmsg
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    // Update the alias object for each instance found
    bool firstdup = true;
    BOOST_FOREACH(const bgq::utility::Properties::Pair& keyval, instances) {
        bool found = false;

        BOOST_FOREACH(const AliasPtr& al, _aliases) {
            const std::string &instance_policy(keyval.second);
            if (keyval.first == al->get_name()) {
                const std::vector<std::string>::const_iterator it = std::find(exclude_list.begin(), exclude_list.end(), al->get_name());
                if (it != exclude_list.end()) {
                    // This was defined previously. Refresh is additive. We don't want to mess with this on a running system.
                    std::ostringstream msg;
                    if (firstdup) {
                        msg << "Instances already defined for ";
                        firstdup = false;
                        msg << al->get_name();
                    } else
                        msg << ", " << al->get_name();
                    failmsg << msg.str();
                } else {
                    // Found an existing alias so update the policy
                    try {
                        const int ip = boost::lexical_cast<int>(instance_policy);
                        al->policy().changeInstances(static_cast<unsigned short>(ip));
                        if (ip <= 0 || ip > std::numeric_limits<unsigned short>::max()) {
                            std::ostringstream msg;
                            msg << "Bad instance value " << instance_policy << " defined for " << al->get_name();
                            LOG_WARN_MSG(msg.str());
                            throw exceptions::ConfigError(exceptions::WARN, msg.str());
                        }
                    } catch (const boost::bad_lexical_cast& e) {
                        std::ostringstream msg;
                        msg << "Bad instance value " << instance_policy << " defined. " << e.what();
                        LOG_WARN_MSG(msg.str());
                        throw exceptions::ConfigError(exceptions::WARN, msg.str());
                    }
                    LOG_DEBUG_MSG("Found instance of " << instance_policy << " for alias " << al->get_name());
                }
                found = true;
                break;
            }
        }

        if (!found) {
            // Didn't find an alias, Assume the [master.binmap] entry has been commented out.
	    // Log a warning message in case something else is going on.
	    LOG_WARN_MSG("Alias " << keyval.first << " not defined.  Skipping [master.policy.instances] entry ... ");
        }
    }
    if (failmsg.str().length() != 0) {
        failmsg << ". ";
    }
}

void
MasterController::buildArgs(
        const bgq::utility::Properties::Section& args
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    // Args are optional. Don't complain if we find nothing.
    BOOST_FOREACH(const bgq::utility::Properties::Pair& keyval, args) {
        bool found = false;
        BOOST_FOREACH(const AliasPtr& al, _aliases) {
            if (keyval.first == al->get_name()) {
                found = true;
                LOG_DEBUG_MSG("Adding args " << keyval.second << " for " << keyval.first);
                al->set_args(keyval.second);
            }
        }
        if (!found) {
            // Didn't find an alias, Assume the [master.binmap] entry has been commented out.
	    // Log a warning message in case something else is going on.
	    LOG_WARN_MSG( "Alias " << keyval.first << " not defined in [master.binmap] section. Skipping [master.binargs] entry ...");
        }
    }
}

void
MasterController::addBehaviors(
        const bgq::utility::Properties::Section& failmap,
        std::multimap<Policy::Trigger,Behavior>& behaviors
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    typedef std::multimap<Policy::Trigger, Behavior> Bevmap;

    bool firstdup = true;

    // For each entry in the failmap, find the related behavior and the related alias, and put it in the alias list.

    BOOST_FOREACH(const bgq::utility::Properties::Pair& keyval, failmap) {
        // There can be several policies related to the alias represented by keyval.first.
        // So, tokenize and find each one and put it in the right alias.
        const std::string &current_alias(keyval.first);
        const std::string &policy_set(keyval.second);
        bool alias_found = false;

        boost::char_separator<char> sep(",");
        tokenizer tok(policy_set, sep);
        LOG_TRACE_MSG("Checking for policies for alias " << current_alias << " against " << policy_set);
        BOOST_FOREACH(const std::string& current_policy, tok) {
            // Now find the policy in the behavior list
            LOG_TRACE_MSG("Evaluating policy " << current_policy << " against alias " << current_alias);
            bool policy_found = false;
            for (Bevmap::iterator it = behaviors.begin(); it != behaviors.end(); ++it) {
                if (current_policy == it->second.get_name()) {
                    // Got a match.  Now find the alias with the name that matches keyval.first and insert the behavior.
                    policy_found = true;
                    std::ostringstream logmsg;
                    BOOST_FOREACH(const AliasPtr& al, _aliases) {
                        LOG_TRACE_MSG("Comparing alias " << al->get_name() << " to policy alias " << keyval.first);
                        if (keyval.first == al->get_name()) {
                            // Found an existing alias so update the policy
                            alias_found = true;
                            if (al->policy().addBehavior(it->first, it->second) == false) {
                                if (firstdup) {
                                    firstdup = false;
                                    logmsg << "Duplicate policy triggers defined: ";
                                }
                                logmsg << "Ignoring duplicate policy triggers ";
                                logmsg << current_policy << " ";
                            } else {
                                // Now check to make sure that if there are failover pairs,
                                // all hosts are in the alias' host list.
                                typedef std::pair<CxxSockets::Host, CxxSockets::Host> HostPair;
                                BOOST_FOREACH(const HostPair& pair, it->second.get_host_pairs()) {
                                    std::string failname = "";
                                    if (al->find_host(pair.first) == false) {
                                        failname = pair.first.fqhn();
                                    }
                                    if (al->find_host(pair.second) == false) {
                                        failname = pair.second.fqhn();
                                    }
                                    if (failname.length() > 0) {
                                        LOG_ERROR_MSG("Host " << failname << " not in host list for alias " << al->get_name());
                                    }
                                }
                                LOG_DEBUG_MSG("Added trigger " << it->first << " and behavior "
                                             << it->second.get_name() << " to " << al->get_name());
                            }
                            break;
                        }
                    }
                    if (logmsg.str().length() != 0) {
                        handleErrorMessage(logmsg.str());
                    }
                }
            }
            if (!policy_found) {
                // Didn't find a policy, Assume the [master.binmap] entry has been commented out. 
	        // Log warning message in case something else is going on.
	        LOG_WARN_MSG("Policy " << current_policy << " found for undefined alias: " << keyval.first << " Skipping [master.policy.map] entry ...");

            }
        }
        if (!alias_found) {
            // Didn't find an alias.  Assume the [master.binmap] entry has been commented out.
	    // Log a warning message in case something else is going on.
	    LOG_WARN_MSG("Alias " << keyval.first << " not defined in section [master.binmap].  Skipping failover configuration for this alias ...");
        }
    }
}

void
MasterController::buildStartList(
        const bgq::utility::Properties::Section& startlist
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BOOST_FOREACH(const bgq::utility::Properties::Pair& keyval, startlist) {
        if (keyval.first == "start_servers") {
            if (keyval.second == "true") {
                _start_servers = true;
            } else {
                _start_servers = false;
            }
            LOG_DEBUG_MSG("Start servers is " << _start_servers);
        }
    }
}

void
MasterController::buildUidList(
        const bgq::utility::Properties::Section& uidlist
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BOOST_FOREACH(const bgq::utility::Properties::Pair& keyval, uidlist) {
        bool found = false;
        BOOST_FOREACH(const AliasPtr& al, _aliases) {
            if (keyval.first == al->get_name()) {
                found = true;
                LOG_DEBUG_MSG("Adding user id " << keyval.second << " for " << keyval.first);
                al->set_user(keyval.second);
            }
        }
        if (!found) {
            // Didn't find an alias.  Assume that the [master.binmap] entry has been commented out.
	    // Log a warning message in case something else is going on.
	    LOG_WARN_MSG("Alias " << keyval.first << " not defined in section [master.binmap].  Skipping [master.user] entry ...");
        }
    }
}

void
MasterController::buildLogDirs(
        const bgq::utility::Properties::Section& logdirs
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    // For each alias/directory pair, find the alias.
    BOOST_FOREACH(const bgq::utility::Properties::Pair& keyval, logdirs) {
        bool found = false;
        BOOST_FOREACH(const AliasPtr& al, _aliases) {
            if (keyval.first == al->get_name()) {
                found = true;
                LOG_DEBUG_MSG("Adding log directory " << keyval.second << " for " << keyval.first);
                al->set_logdir(keyval.second);
            }
        }
        if (!found) {
            // Didn't find an alias.  Assume that the [master.binmap] entry has been commented out.
	    // Log a warning message in case something else is going on.
	    LOG_WARN_MSG("Alias " << keyval.first << " not defined in section [master.binmap]. Skipping [master.logdirs] entry ...");
        }
    }
}

void
MasterController::buildPolicies(
        std::ostringstream& failmsg
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    typedef bgq::utility::Properties::Section Sect;

    // Will contain the number of seconds to wait for an alias' preferred agent to check in.
    int preferredHostWait;

    // Protect the policy building process
    boost::mutex::scoped_lock scoped_lock(_policy_build_mutex);

    Sect master, args, hosts, instances, failover, failmap, startlist, uidlist, logdirs;
    try {
        master = _props->getValues("master.binmap");
        args = _props->getValues("master.binargs");
        hosts = _props->getValues("master.policy.host_list");
        instances = _props->getValues("master.policy.instances");
        failover = _props->getValues("master.policy.failure");
        failmap = _props->getValues("master.policy.map");
        startlist = _props->getValues("master.startup");
        uidlist = _props->getValues("master.user");
        _start_once = false;
    } catch (const std::invalid_argument& e) {
        std::ostringstream msg;
        msg << "Properties file error. " << e.what();
        handleErrorMessage(msg.str());

        // Only exit if we are starting bgmaster_server , not on a refresh.
        if (_start_once) {
            throw exceptions::ConfigError(exceptions::WARN, msg.str());
        } else {
            return;
        }
    }

    bool dologdirs = false;
    try {
        logdirs = _props->getValues("master.logdirs");
        dologdirs = true;
    } catch (const std::invalid_argument& e) {
        std::ostringstream msg;
        msg << "Properties file error. " << e.what();
        handleErrorMessage(msg.str());
    }

    std::vector<std::string> exclude_list;

    // Get the $BG_DRIVER environment variable
    std::string driver = "";
    char* drv = getenv("BG_DRIVER");
    if (drv != NULL)
        driver = drv;
    if (driver.length() == 0)
        driver = "/bgsys/drivers/ppcfloor"; // reasonable default
    LOG_DEBUG_MSG("BG_DRIVER environment variable is " << driver);

    // Need to get preferred_host_wait time to send to the alias constructor.
    try {
        preferredHostWait = boost::lexical_cast<int>(_props->getValue("master.server", "preferred_host_wait"));
	if (  preferredHostWait <= 0 ) {
	    std::ostringstream msg;
	    msg << "Invalid preferred_host_wait setting in [master.server] section: Value must be an integer greater than 0.";
	    LOG_ERROR_MSG(msg.str());
	    throw exceptions::ConfigError(exceptions::WARN, msg.str());
	} else {
	    std::ostringstream msg;
	    msg << "Found user preferred_host_wait key with value: " << preferredHostWait << " in [master.server] section.";
	    LOG_DEBUG_MSG(msg.str());
	}
    } catch (const std::invalid_argument& e) {
        // It is ok if it is not set.  We default it to 15 here.
        std::ostringstream msg;
        msg << "Did not find optional preferred_host_wait key in [master.server] section. Setting default of 15 seconds.";
        LOG_DEBUG_MSG(msg.str());
	preferredHostWait=15;
    } catch (const boost::bad_lexical_cast& e) {
        std::ostringstream msg;
        msg << "Invalid preferred_host_wait setting in [master.server] section: " << e.what();
        LOG_ERROR_MSG(msg.str());
        throw exceptions::ConfigError(exceptions::WARN, msg.str());
    }


    // Create an alias object for every alias in the master map
    BOOST_FOREACH(const bgq::utility::Properties::Pair& keyval, master) {
        AliasPtr alp;
        if (_aliases.find_alias(keyval.first, alp) == false) {
            // Alias doesn't yet exist, make it so.
            Policy pol; // Empty policy with default single instance
            std::string path = keyval.second;
            const AliasPtr al(new Alias(keyval.first, path, pol, "", _master_logdir, preferredHostWait));
            LOG_DEBUG_MSG("Adding alias " << keyval.first << " for " << path);
            _aliases.push_back(al);
            alp = al;
        } else {
            exclude_list.push_back(keyval.first);  // Refreshing!
        }

        // The $BG_DRIVER environment variable can be part of the path.
        // We need to replace what's in the property file with the env var.
        std::string path = keyval.second;
        const size_t path_loc = path.find("$BG_DRIVER");
        if (path_loc != std::string::npos) { // If $BG_DRIVER is in the path...
            path.replace(path_loc, 10, driver); // ...replace it with the driver variable
        }
        LOG_DEBUG_MSG("Setting path to " << path);
        alp->set_path(path);
    }

    try {
        const std::string value( _props->getValue("master.server","max_agents_per_host") );
        const int max_agents( boost::lexical_cast<int>(value) );
        if ( max_agents <= 0 ) {
            std::ostringstream msg;
            msg << "Invalid max_agents_per_host setting in [master.server] section: value must be greater than zero";
            LOG_ERROR_MSG(msg.str());
            throw exceptions::ConfigError(exceptions::WARN, msg.str());
        }
        _agent_manager.setCount(static_cast<unsigned>(max_agents));
    } catch (const std::invalid_argument& e) {
        // this is OK, missing means default to 1
        LOG_DEBUG_MSG( "missing max_agents_per_host setting in [master.server] section, using default value of 1" );
    } catch (const boost::bad_lexical_cast& e) {
        std::ostringstream msg;
        msg << "Invalid max_agents_per_host setting in [master.server] section: " << e.what();
        LOG_ERROR_MSG(msg.str());
        throw exceptions::ConfigError(exceptions::WARN, msg.str());
    }

    buildArgs(args);

    buildHostList(hosts, exclude_list, failmsg);
    if ( !failmsg.str().empty() ) {
        failmsg << ". ";
    }

    buildInstances(instances, exclude_list, failmsg);
    std::multimap<Policy::Trigger,Behavior> behaviors;
    buildFailover(failover, behaviors);
    addBehaviors(failmap, behaviors);
    buildStartList(startlist);
    buildUidList(uidlist);
    if (dologdirs) {
        buildLogDirs(logdirs);
    }
}

void
MasterController::startServers(
        std::map<std::string, std::string>& failed_aliases,
        AgentRepPtr agentrep
        )
{
    LOG_TRACE_MSG(__FUNCTION__);

    LOG_INFO_MSG("Starting all listed binaries for agent " << (agentrep ? agentrep->get_agent_id().str() : "all agents") << ".");
    std::vector<AliasPtr> servers_to_start = _aliases.get_list_copy();
    while (!servers_to_start.empty() && !_master_terminating) {
        const AliasPtr al = servers_to_start.front();
        // Now take it out of the list, we'll put it in the back later if we can't start it.
        servers_to_start.erase(std::remove(servers_to_start.begin(), servers_to_start.end(), al), servers_to_start.end());
        if (al->get_name() == "bgmaster_server" || al->get_name() == "bgmaster") {
            continue; // Don't start ourselves
        }

        BinaryId id;
        if (al->running(id)) {
            continue; // Don't start one already running.
        }

        LOG_INFO_MSG("Attempting to start alias " << al->get_name() << ".");

        // Find an agent we can use. Supply an empty ID and let the alias pick one based on its policy.
        BGAgentId aid;
        AgentRepPtr agent;
        std::string failreason;
        try {
            agent = al->validateStartAgent(aid);
        } catch (const exceptions::InternalError& e) {
            LOG_ERROR_MSG("Unexpected error finding an agent: " << e.what());
            if (agentrep && !al->find_host(agentrep->get_host())) {
                // this agent was not configured to start this alias, keep going
                continue;
            } else {
                failreason = e.what();
                sleep( 1 );
            }
        }

        if (agent) {
            // If we got here, we have an agent ready.
            const BGMasterAgentProtocolSpec::StartRequest agentreq(
                    al->get_path(), 
                    al->get_args(),
                    al->get_logdir(), 
                    al->get_name(),
                    al->get_user()
                    );
            BGMasterAgentProtocolSpec::StartReply reply;
            reply._rc = exceptions::OK;

            const BinaryId bid = agent->startBin(agentreq, reply);
            if (bid.str() == "0") {
                std::ostringstream msg;
                msg << "Attempt to start binary for alias " << al->get_name() << " failed, check RAS.";
                failed_aliases[al->get_name()] = msg.str();
                handleErrorMessage(msg.str());
                std::map<std::string, std::string> details;
                details["ALIAS"] = al->get_name();
                putRAS(ALIAS_FAIL_RAS, details);
            }
        } else {
            // Didn't start it, so put it back in the list.
            servers_to_start.push_back(al);
            LOG_DEBUG_MSG("No agent found running for alias " << al->get_name());
            failed_aliases[al->get_name()] = failreason;
        }
    }
}

void
MasterController::startup(
        const int signal_fd
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::ostringstream version;
    version << "Blue Gene/Q";
    version << " " << bgq::utility::DriverName;
    version << "(revision " << bgq::utility::Revision << ")";
    version << " " << __DATE__ << " " << __TIME__;
    _version_string = version.str();
    LOG_INFO_MSG("bgmaster_server [" << getpid() << "] " << _version_string << " starting...");;
    LOG_INFO_MSG("Using " << _props->getFilename() << " for properties.");
    _master_db = false;
    std::string db_val = "true";
    _start_time = boost::posix_time::second_clock::local_time();

    try {
        db_val = _props->getValue("master.server", "db");
    } catch (const std::invalid_argument& e) {
        LOG_DEBUG_MSG("Invalid or missing db parameter in [master.server] section of properties file: " << e.what());
    }

    if (db_val.empty()) {
        db_val = "true";
    }

    if (db_val == "true") {
        // Initialize the database for the process
        // This must be done before starting any threads, due to the initialization of static variables
        LOG_DEBUG_MSG("Initializing database connection pool");
        BGQDB::DBConnectionPool::reset();
        // We aren't cutting RAS very often, just one DB pool thread.
        BGQDB::DBConnectionPool::init(_props, 1);
        _updater.start(); // Start the DB updater.
        _master_db = true;
    } else {
        if (db_val != "false") {
            LOG_WARN_MSG("Value " << "\"" << db_val << "\" " << "for \"db\" in bg.properties' \"master.server\" section "
                         << " is invalid. It must be either \"true\" or \"false\". Will not log RAS to the database.");
        }
        _master_db = false;
    }

    try {
        _master_logdir = _props->getValue("master.server", "logdir");
        if (access(_master_logdir.c_str(), R_OK|W_OK) < 0) {
            std::ostringstream errmsg;
            errmsg << "Log directory " << _master_logdir << " is not accessible to bgmaster_server.";
            handleErrorMessage(errmsg.str());
            throw exceptions::ConfigError(exceptions::FATAL, errmsg.str());
        }
    } catch (const std::invalid_argument& e) {
        LOG_WARN_MSG("No log directory found. Will use default.");
    }

    // Read policy information from the config file
    std::ostringstream failmsg;
    buildPolicies(failmsg);

    if (!failmsg.str().empty()) {
        std::ostringstream msg;
        msg << "Invalid configuration: " << failmsg.str();
        handleErrorMessage(msg.str());
        throw exceptions::ConfigError(exceptions::FATAL, failmsg.str());
    }

    _agent_registrar.run(true);
    _client_registrar.run(false);

    // Update database with ras message
    std::map<std::string, std::string> details;
    details["PID"] = boost::lexical_cast<std::string>(_pid);
    putRAS(MASTER_STARTUP_RAS, details);
    std::ostringstream startmsg;
    startmsg << "bgmaster_server startup completed";
    addHistoryMessage(startmsg.str());

    _start_barrier.wait();

    while ( !_master_terminating ) {
        struct pollfd pollfd;
        pollfd.fd = signal_fd;
        pollfd.events = POLLIN;
        pollfd.revents = 0;
        const int seconds = 5;
        const int rc = poll( &pollfd, 1, seconds );

        if ( rc == -1 ) {
            if ( errno != EINTR) {
                char errorText[256];
                LOG_ERROR_MSG("Could not poll: " << std::string(strerror_r(errno, errorText, 256)));
            }
        } else if ( rc ) {
            // Read siginfo from pipe
            siginfo_t siginfo;
            while ( 1 ) {
                const ssize_t rc = read( signal_fd, &siginfo, sizeof(siginfo) );
                if ( rc > 0 ) {
                    break;
                }
                if ( rc == -1 && errno == EINTR ) {
                    continue;
                } else {
                    char errorText[256];
                    LOG_FATAL_MSG("Could not read from pipe: " << std::string(strerror_r(errno, errorText, 256)));
                    exit(1);
                }
            }
            if ( siginfo.si_signo == SIGUSR1 || siginfo.si_signo == SIGPIPE || siginfo.si_signo == SIGHUP) {
                LOG_DEBUG_MSG( "Received signal " << siginfo.si_signo << " from " << siginfo.si_pid );
            } else {
                LOG_FATAL_MSG("bgmaster_server ending due to signal " << siginfo.si_signo << " from " << siginfo.si_pid << ".");
                // Send RAS
                std::map<std::string, std::string> details;
                details["PID"] = boost::lexical_cast<std::string>(getpid());
                details["SIGNAL"] = boost::lexical_cast<std::string>(siginfo.si_signo);
                putRAS(MASTER_FAIL_RAS, details);

                if (lock_file) {
                    delete lock_file;
                    lock_file = NULL;
                }

                // use _exit instead of exit since other threads are running and we don't want to
                // run global destructors 
                _exit( 128 + siginfo.si_signo );
            }
        } else if ( !rc ) {
            // Check if either registrar has failed. If so, restart it.
            bool reregister = false;
            if (_agent_registrar.get_failed()) {
                _client_registrar.cancel();
                _agent_registrar.run(true);
                _client_registrar.run(false);
                reregister = true;
            } else if (_client_registrar.get_failed()) {
                _agent_registrar.cancel();
                _client_registrar.run(false);
                _agent_registrar.run(true);
                reregister = true;
            }
            if (reregister) {  // Only barrier wait if we restarted registrars
                _start_barrier.wait();
            }
        }
    }
}
