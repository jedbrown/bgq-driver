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

#include <map>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/scope_exit.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <iostream>
#include <tr1/memory>
#include <utility/include/Log.h>
#include <RasEventImpl.h>
#include <RasEventHandlerChain.h>
#include <utility/include/version.h>

#ifdef WITH_DB
#include "api/tableapi/DBConnectionPool.h"
#include "api/tableapi/TxObject.h"
#include "DBUpdater.h"
#endif

#include "BGMasterAgentProtocol.h"
#include "BGAgent.h"
#include "BGAgentBase.h"
#include "BGMaster.h"
#include "BGMasterExceptions.h"
#include "Host.h"
#include "Alias.h"
#include "MasterRasMetadata.h"
#include "LockingRingBuffer.h"

LOG_DECLARE_FILE( "master" );

const int CIRC_BUFFER_SIZE = 50;

extern volatile int signal_number;

//! \brief static member instantiations
boost::mutex BGMasterController::_policy_build_mutex;
bgq::utility::Properties::Ptr BGMasterController::_master_props;
bool BGMasterController::_master_terminating = false;
bool BGMasterController::_end_requested = false;
bool BGMasterController::_start_servers = false;
AliasList BGMasterController::_aliases;
boost::barrier BGMasterController::_start_barrier(3);
pid_t BGMasterController::_master_pid;
std::string BGMasterController::_master_logdir;
std::vector<AliasPtr> BGMasterController::_start_list;
std::vector<Host> BGMasterController::_configured_hosts;
bool BGMasterController::_master_db;
bool BGMasterController::_stop_once = false;
bool BGMasterController::_start_once = true;
boost::posix_time::ptime BGMasterController::_start_time;
std::string BGMasterController::_version_string = "";
LockingStringRingBuffer BGMasterController::_err_buff(CIRC_BUFFER_SIZE);
LockingStringRingBuffer BGMasterController::_history_buff(CIRC_BUFFER_SIZE);
std::vector<BGMasterClientProtocolPtr> BGMasterController::_monitor_prots;

#ifdef WITH_DB
DBUpdater BGMasterController::_updater;
#endif

BGMasterController::BGMasterController() {
    
}

#ifdef WITH_DB
void BGMasterController::putRAS(unsigned int id, std::map<std::string, std::string>& details) {
    if(_master_db) { // Only bother with ras if we've got a db

        RasEventImpl event = RasEventImpl(id);
        for(std::map<std::string, std::string>::iterator it = details.begin();
            it != details.end(); ++it) {
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

#else
void BGMasterController::putRAS(unsigned int id, std::map<std::string, std::string>& details) { }
#endif

void BGMasterController::stopThreads(const bool end_agents, const bool end_binaries, const unsigned signal) {
    LOG_INFO_MSG(__FUNCTION__);
    if(_stop_once) return;
    else _stop_once = true;
    LOG_INFO_MSG("Stopping all threads");

    // Stop the agent registrar.
    _agent_registrar.cancel();

    // Stop agent threads.
    _agent_manager.cancel(end_agents, end_binaries, signal);

    // Stop client threads.
    _client_manager.cancel();

    // Stop the client registrar.
    _client_registrar.cancel();
    std::map<std::string, std::string> details;
    details["PID"] = boost::lexical_cast<std::string>(getpid());

    BGMasterController::putRAS(MASTER_SHUTDOWN_RAS, details);
#ifdef WITH_DB
    // End the DB updater.
    BGMasterController::_updater.end();
#endif

}

void BGMasterController::handleErrorMessage(std::string msg) {
    LOG_ERROR_MSG(__FUNCTION__ << " " << msg);
    std::ostringstream errmsg;
    boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
    errmsg << now << ": " << msg;
    _err_buff.push_back(errmsg.str());
    // Send it to all of the monitors.
    BGMasterClientProtocolSpec::ErrorMessage error(errmsg.str());
    BOOST_FOREACH(BGMasterClientProtocolPtr& prot, _monitor_prots) {
        try {
            prot->sendOnly(error.getClassName(), error);
        } catch(CxxSockets::CxxError& e) {
            // Just log it.  It's not a big deal
            // if don't send back to a monitor.
            LOG_WARN_MSG(e.what());
        }
    }
    return;
}

void BGMasterController::getErrorMessages(std::vector<std::string>& messages) {
    LOG_INFO_MSG(__FUNCTION__);
    // Going to pop items out of the circular buffer.
    _err_buff.getContents(messages);
}

void BGMasterController::addHistoryMessage(std::string message) {
    LOG_INFO_MSG(__FUNCTION__);
    std::ostringstream msg;
    boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
    msg << now << ": " << message;
    _history_buff.push_back(msg.str());
    // Send it to all of the monitors.
    BGMasterClientProtocolSpec::EventMessage event(msg.str());
    BOOST_FOREACH(BGMasterClientProtocolPtr& prot, _monitor_prots) {
        try {
            prot->sendOnly(event.getClassName(), event);
        } catch(CxxSockets::CxxError& e) {
            // Just log it.  It's not a big deal
            // if don't send back to a monitor.
            LOG_WARN_MSG(e.what());
        }
    }
}

void BGMasterController::getHistoryMessages(std::vector<std::string>& messages) {
    LOG_INFO_MSG(__FUNCTION__);
    _history_buff.getContents(messages);
}

void BGMasterController::buildHostList(bgq::utility::Properties::Section& hosts,
                                       std::vector<std::string>& exclude_list,
                                       std::ostringstream& failmsg) {
    LOG_INFO_MSG(__FUNCTION__);

    bool firstdup = true;

    BOOST_FOREACH( bgq::utility::Properties::Pair keyval, hosts ) {
        bool found = false;

        BOOST_FOREACH(AliasPtr& al, _aliases) {
            std::string all_hosts = keyval.second;
            if(keyval.first == al->get_name()) {
                std::vector<std::string>::iterator it =
                    std::find(exclude_list.begin(), exclude_list.end(), al->get_name());
                if(it != exclude_list.end()) {
                    // This was defined previously.  Refresh is additive.  We don't
                    // want to mess with this on a running system.

                    if(firstdup == true) {
                        firstdup = false;
                        failmsg << "Host list previously set for " << al->get_name();
                    } else {
                        failmsg << ", " << al->get_name();
                    }
                    std::ostringstream msg;
                    msg << "Host list previously set for " << al->get_name();
                    BGMasterController::handleErrorMessage(msg.str());
                } else {

                    // Found an existing alias so parse out the hosts
                    // and update the alias
                    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
                    boost::char_separator<char> sep(","); // comma separated
                    tokenizer tok(all_hosts, sep);
                    bool first = true;
                    BOOST_FOREACH(std::string curr_host, tok) {
                        Host h(curr_host);
                        if(first) {
                            LOG_DEBUG_MSG("Host " << h.uhn() << " is the preferred host.");
                            h.set_primary(true);
                        }
                        first = false;
                        LOG_DEBUG_MSG("Adding host " << h.uhn() << " to alias "
                                      << al->get_name());
                        al->add_host(h);
                        // If we haven't added it already, put it in the
                        // list of configured hosts
                        if(std::find(_configured_hosts.begin(), _configured_hosts.end(), h)
                           == _configured_hosts.end())
                            _configured_hosts.push_back(h);
                    }
                }
                found = true;
                break;
            }
        }

        if(!found) {
            std::ostringstream msg;
            // Didn't find an alias, bad config file.  Bail out.
            msg << "Invalid host list config.  Alias " << keyval.first
                << " not defined.";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }
    }
    if(failmsg.str().length() != 0)
        failmsg << ". ";
}

void BGMasterController::buildFailover(bgq::utility::Properties::Section& failover,
                                       std::multimap<Policy::Trigger,Policy::Behavior>& behaviors) {
    LOG_INFO_MSG(__FUNCTION__);

    // Finally, go through the failover list
    BOOST_FOREACH( bgq::utility::Properties::Pair keyval, failover ) {

        LOG_DEBUG_MSG("Building policy for " << keyval.first << "="
                      << keyval.second);

        // First create a policy object, then assign to an alias
        // Tokenize the policy
        const std::string failover_policy = boost::algorithm::erase_all_copy(keyval.second, " ");
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(","); // comma separated
        tokenizer tok(failover_policy, sep);
        tokenizer::iterator curr_tok = tok.begin();

        if(curr_tok == tok.end()) {
            // No policy
            // Bad news, config file is wrong
            std::ostringstream msg;
            msg << "Invalid failover policy.  No policy specified for "
                << keyval.first;
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }

        // trigger is first
        Policy::Trigger my_trigger = Policy::string_to_trigger(*curr_tok);
        if(my_trigger == Policy::INVALID_TRIGGER) {
            // Bad news, config file is wrong
            std::ostringstream msg;
            msg << "Invalid failover trigger " << my_trigger
                << " specified";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }

        ++curr_tok; // second is the action to take
        if(curr_tok == tok.end()) {
            // Bad.  No action specified
            std::ostringstream msg;
            msg << "Invalid failure config for " << keyval.first
                <<  " No action specified";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }

        Policy::Action act = Policy::string_to_action(*curr_tok);
        if(act == Policy::INVALID_ACTION) {
            std::ostringstream msg;
            msg << "Invalid failure action '" << *curr_tok
                << "' specified";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }

        ++curr_tok; // third is the number of retries
        if(curr_tok == tok.end()) {
            // Bad.  No action specified
            std::ostringstream msg;
            msg << "Invalid failure config for " << keyval.first
                <<  " No retries specified";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }

        std::ostringstream msg;
        short retries = 0;
        try {
            retries = boost::lexical_cast<short>(*curr_tok);
        } catch(boost::bad_lexical_cast& e) {
            retries = -1;
            msg << e.what();
        }

        if(retries <= 0) {
            // Bad.  No action specified
            msg << "Invalid retry config for " << keyval.first
                <<  ".  Number of retries, " << retries << ", must be greater than zero. ";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }

        ++curr_tok; // fourth is the associated host pairs

        std::map<Host, Host> failpairs;

        if(curr_tok == tok.end()) {
            // We're at the end.  This is OK.  If our action is failover,
            // we'll do the default of picking a random target, but note
            // it in the log.
            if(act == Policy::FAILOVER) {
                std::ostringstream msg;
                msg << "Failure config for " << keyval.first
                    <<  " 'failover' action specified, but no failover pair specified.";
                LOG_INFO_MSG(msg.str());
            }
        } else {
            if(act == Policy::RESTART) {
                // If there's a restart action, failover pairs make no sense!
                std::ostringstream msg;
                msg << "Bad policy configuration specified.  Cannot specify failover pairs"
                    << " for a \"restart\" policy.";
                LOG_INFO_MSG(msg.str());
                BGMasterController::handleErrorMessage(msg.str());
                throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
            }

            // We have failover pairs to check.
            // Parse it to a list of failover pairs
            std::string host_pairs = *curr_tok;

            // host pair is in the format of host:host|host:host
            // So loop through all tokens separated by a '|' and
            // build a pair for each.
            boost::char_separator<char> bar_sep("|");
            tokenizer bar_tok(host_pairs, bar_sep);

            BOOST_FOREACH(std::string current, bar_tok) {

                if(current.find_first_of(":") == std::string::npos) {
                    std::ostringstream msg;
                    msg << "Failover config syntax incorrect. Missing \":\"";
                    BGMasterController::handleErrorMessage(msg.str());
                    throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
                }

                if(current.find_first_of(":") != current.find_last_of(":")) {
                    // More than one ':' in this failover pair!!!
                    std::ostringstream msg;
                    msg << "Failover config syntax incorrect. More than one ':' in the pair \""
                        << current << " \"";
                    BGMasterController::handleErrorMessage(msg.str());
                    throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
                }

                boost::char_separator<char> col_sep(":");
                tokenizer col_tok(current, col_sep);
                tokenizer::iterator col_tok_it = col_tok.begin();
                Host from_host(*col_tok_it);
                ++col_tok_it;
                if(col_tok_it == col_tok.end()) {
                    std::ostringstream msg;
                    msg << "Failover pair for policy " << keyval.first << " lacks a target.";
                    BGMasterController::handleErrorMessage(msg.str());
                    throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());                    
                }
                Host to_host(*col_tok_it);
                LOG_DEBUG_MSG("Created failover pair for " << from_host.uhn() << " to "
                              << to_host.uhn() << " for alias " << keyval.first);
                failpairs[from_host] = to_host;
            }

        }
        // Now create the behavior to associate with the trigger
        Policy::Behavior my_behavior(keyval.first, act, failpairs, retries);
        behaviors.insert(std::pair<Policy::Trigger,Policy::Behavior>(my_trigger,my_behavior));
        //        behaviors[my_trigger] = my_behavior;
    }
}

void BGMasterController::buildInstances(bgq::utility::Properties::Section& instances,
                                        std::vector<std::string>& exclude_list,
                                        std::ostringstream& failmsg) {
    LOG_INFO_MSG(__FUNCTION__);
    // Update the alias object for each instance found
    bool firstdup = true;
    BOOST_FOREACH( bgq::utility::Properties::Pair keyval, instances ) {
        bool found = false;

        BOOST_FOREACH(AliasPtr& al, _aliases) {
            std::string instance_policy = keyval.second;
            if(keyval.first == al->get_name()) {
                std::vector<std::string>::iterator it =
                    std::find(exclude_list.begin(), exclude_list.end(), al->get_name());
                if(it != exclude_list.end()) {
                    // This was defined previously.  Refresh is additive.  We don't
                    // want to mess with this on a running system.
                    std::ostringstream msg;
                    if(firstdup == true) {
                        msg << "Instances already defined for ";
                        firstdup = false;
                        msg << al->get_name();
                    } else
                        msg << ", " << al->get_name();
                    failmsg << msg.str();
                } else {
                    // Found an existing alias so update the policy
                    try {
                        int ip = boost::lexical_cast<int>(instance_policy);
                        al->policy().changeInstances(ip);
                        if(ip < 0) {
                            std::ostringstream msg;
                            msg << "Bad instance value " << instance_policy 
                                << " defined for " << al->get_name();
                            LOG_WARN_MSG(msg.str());
                            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
                        }
                    } catch(boost::bad_lexical_cast& e) {
                        std::ostringstream msg;
                        msg << "Bad instance value " << instance_policy << " defined. " << e.what();
                        LOG_WARN_MSG(msg.str());
                        throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
                    }
                    LOG_DEBUG_MSG("Found instance of " << instance_policy << " for Alias " << al->get_name());
                }
                found = true;
                break;
            }
        }

        if(!found) {
            // Didn't find an alias, bad config file.  Bail out
            std::ostringstream msg;
            msg <<"Invalid instance config.  Alias " << keyval.first
                << " not defined.";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }
    }
    if(failmsg.str().length() != 0)
        failmsg << ". ";
}

void BGMasterController::buildArgs(bgq::utility::Properties::Section& args) {
    LOG_INFO_MSG(__FUNCTION__);
    // Args aren't necessary.  Don't complain if we find nothing.
    BOOST_FOREACH( bgq::utility::Properties::Pair keyval, args) {
        bool found = false;
        BOOST_FOREACH(AliasPtr& al, _aliases) {
            if(keyval.first == al->get_name()) {
                found = true;
                LOG_DEBUG_MSG("Adding args " << keyval.second << " for "
                              << keyval.first);
                al->set_args(keyval.second);
            }
        }
        if(!found) {
            // Didn't find an alias, bad config file.  Bail out
            std::ostringstream msg;
            msg <<"Invalid binargs config.  Alias " << keyval.first
                << " not defined.";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }
    }
}

void BGMasterController::addBehaviors(bgq::utility::Properties::Section& failmap,
                                      std::multimap<Policy::Trigger,Policy::Behavior>& behaviors,
                                      std::ostringstream& failmsg) {
    LOG_INFO_MSG(__FUNCTION__);
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    typedef std::multimap<Policy::Trigger, Policy::Behavior> Bevmap;

    bool firstdup = true;

    // For each entry in the failmap, find the related behavior
    // and the related alias, and put it in the alias list

    BOOST_FOREACH(bgq::utility::Properties::Pair keyval, failmap ) {
        // There can be several policies related to the alias
        // represented by keyval.first.  So, tokenize and find
        // each one and put it in the right alias
        std::string current_alias = keyval.first;
        std::string policy_set = keyval.second;
        bool alias_found = false;

        boost::char_separator<char> sep(",");
        tokenizer tok(policy_set, sep);
        LOG_TRACE_MSG("Checking for policies for " << current_alias << " against " << policy_set);
        BOOST_FOREACH(std::string current_policy, tok) {
            // Now find the policy in the behavior list
            LOG_TRACE_MSG("Evaluating " << current_policy << " against " << current_alias);
            bool policy_found = false;
            for(Bevmap::iterator it = behaviors.begin();
                it != behaviors.end(); ++it) {
                if(current_policy == it->second.get_name()) {
                    // Got a match.  Now find the alias with
                    // the name that matches keyval.first and
                    // insert the behavior.
                    policy_found = true;
                    std::ostringstream logmsg;
                    BOOST_FOREACH(AliasPtr& al, _aliases) {
                        std::string failover_policy = keyval.second;
                        LOG_TRACE_MSG("Comparing alias " << al->get_name() << " to policy alias " << keyval.first);
                        if(keyval.first == al->get_name()) {
                            // Found an existing alias so update the policy
                            alias_found = true;
                            if(al->policy().addBehavior(it->first, it->second) == false) {
                                if(firstdup == true) {
                                    firstdup = false;
                                    logmsg << "Duplicate policy triggers defined: ";
                                }
                                logmsg << "Ignoring duplicate policy triggers ";
                                logmsg << current_policy << " ";
                            } else {
                                // Now check to make sure that if there are failover pairs,
                                // all hosts are in the alias' host list.
                                typedef std::pair<Host, Host> HostPair;
                                BOOST_FOREACH(HostPair pair, it->second.get_host_pairs()) {
                                    std::string failname = "";
                                    if(al->host_in(pair.first) == false) {
                                        failname = pair.first.fqhn();
                                    }
                                    if(al->host_in(pair.second) == false) {
                                        failname = pair.second.fqhn();
                                    }
                                    if(failname.length() > 0) {
                                        LOG_ERROR_MSG("Host " << failname 
                                                      << " not in host list for "
                                                      << al->get_name());
                                    }
                                }
                                LOG_INFO_MSG("Added trigger " << it->first << " and behavior "
                                             << it->second.get_name() << " to " << al->get_name());
                            }
                            break;
                        }
                    }
                    if(logmsg.str().length() != 0)
                        BGMasterController::handleErrorMessage(logmsg.str());
                }
            }
            if(!policy_found) {
                // Didn't find a policy, bad config file.  Bail out
                std::ostringstream msg;
                msg << "Invalid failover config.  Policy " << current_policy 
                    << " for alias " << keyval.first
                    << " not defined.";
                BGMasterController::handleErrorMessage(msg.str());
                throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
            
            }
        }
        if(!alias_found) {
            // Didn't find an alias, bad config file.  Bail out
            std::ostringstream msg;
            msg << "Invalid failover config.  Alias " << keyval.first
                << " not defined.";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }
    }
}

void BGMasterController::buildStartList(bgq::utility::Properties::Section& startlist) {
    // exactly two keys in this section.  A start servers flag and a comma separated
    // list of servers to start.  First, check the flag, then build the start list
    // with pointers to the matching aliases.
    LOG_INFO_MSG(__FUNCTION__);
    BOOST_FOREACH(bgq::utility::Properties::Pair keyval, startlist ) {
        if(keyval.first == "start_servers") {
            if(keyval.second == "true")
                _start_servers = true;
            else
                _start_servers = false;
            LOG_DEBUG_MSG("Start servers is " << _start_servers);
        } else if(keyval.first == "start_order") {
            typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
            boost::char_separator<char> sep(",");
            tokenizer tok(keyval.second, sep);
            BOOST_FOREACH(std::string current_alias, tok) {
                // Find the alias matching the string and put it in the
                // start list
                AliasPtr al;
                bool found = _aliases.find_alias(current_alias, al);
                if(found) {
                    LOG_DEBUG_MSG("Adding alias " << al->get_name() << " to start list.");
                    _start_list.push_back(al);
                } else {
                    LOG_WARN_MSG("Ignoring invalid alias " << current_alias << " in start_order.");
                }
            }
        }
    }
}

void BGMasterController::buildUidList(bgq::utility::Properties::Section& uidlist) {
    LOG_INFO_MSG(__FUNCTION__);
    BOOST_FOREACH( bgq::utility::Properties::Pair keyval, uidlist) {
        bool found = false;
        BOOST_FOREACH(AliasPtr& al, _aliases) {
            if(keyval.first == al->get_name()) {
                found = true;
                LOG_DEBUG_MSG("Adding user id " << keyval.second << " for "
                              << keyval.first);
                al->set_user(keyval.second);
            }
        }
        if(!found) {
            std::ostringstream msg;
            // Didn't find an alias, bad config file.  Bail out.
            msg << "Invalid [master.user] config.  Alias " << keyval.first
                << " not defined.";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }
    }
}

void BGMasterController::buildLogDirs(bgq::utility::Properties::Section& logdirs) {
    LOG_INFO_MSG(__FUNCTION__);
    // For each alias/directory pair, find the alias.
    BOOST_FOREACH( bgq::utility::Properties::Pair keyval, logdirs) {
        bool found = false;
        BOOST_FOREACH(AliasPtr& al, _aliases) { 
            if(keyval.first == al->get_name()) {
                found = true;
                LOG_DEBUG_MSG("Adding log directory " << keyval.second << " for "
                              << keyval.first);
                al->set_logdir(keyval.second);
            }
        }
        if(!found) {
            std::ostringstream msg;
            // Didn't find an alias, bad config file.  Bail out.
            msg << "Invalid [master.logdirs] config.  Alias " << keyval.first
                << " not defined.";
            BGMasterController::handleErrorMessage(msg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        }
    }
}

void BGMasterController::buildPolicies(std::ostringstream& failmsg) {
    LOG_INFO_MSG(__FUNCTION__);
    typedef bgq::utility::Properties::Section Sect;

    // Protect the policy building process
    boost::mutex::scoped_lock scoped_lock(_policy_build_mutex);

    Sect master, args, hosts, instances, failover, failmap, startlist, uidlist, logdirs;
    try {
        master = _master_props->getValues("master.binmap");
        args = _master_props->getValues("master.binargs");
        hosts = _master_props->getValues("master.policy.host_list");
        instances = _master_props->getValues("master.policy.instances");
        failover = _master_props->getValues("master.policy.failure");
        failmap = _master_props->getValues("master.policy.map");
        startlist = _master_props->getValues("master.startup");
        uidlist = _master_props->getValues("master.user");
        _start_once = false;
    } catch(std::invalid_argument& e) {
        std::ostringstream msg;
        msg << "Properties file error. " << e.what();
        BGMasterController::handleErrorMessage(msg.str());

        // Only exit if we are starting bgmaster, not on a refresh.
        if(_start_once == true)
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
        else return;
    }

    bool dologdirs = false;
    try {
        logdirs = _master_props->getValues("master.logdirs");
        dologdirs = true;
    } catch(std::invalid_argument& e) {
        std::ostringstream msg;
        msg << "Properties file error. " << e.what();
        BGMasterController::handleErrorMessage(msg.str());
    }

    std::vector<std::string> exclude_list;

    //Get the $BG_DRIVER environment variable
    std::string driver = "";
    char* drv = getenv("BG_DRIVER");
    if(drv != NULL)
        driver = drv;
    if(driver.length() == 0)
        driver = "/bgsys/drivers/ppcfloor"; // reasonable default
    LOG_DEBUG_MSG("BG_DRIVER environment variable is " << driver);

    // Create an alias object for every alias in the master map
    BOOST_FOREACH( bgq::utility::Properties::Pair keyval, master) {
        AliasPtr alp;
        if(_aliases.find_alias(keyval.first, alp) == false) {
            // Alias doesn't yet exist, make it so.
            Policy pol; // Empty policy with default single instance
            std::string path = keyval.second;
            AliasPtr al(new Alias(keyval.first, path, pol, "", _master_logdir));
            LOG_DEBUG_MSG("Adding alias " << keyval.first << " for " << path);
            _aliases.push_back(al);
            alp = al;
        } else {
            exclude_list.push_back(keyval.first);  // Refreshing!
        }

        // The $BG_DRIVER environment variable can be part
        // of the path.  We need to replace what's in the
        // property file with the env var.
        std::string path = keyval.second;
        size_t path_loc = path.find("$BG_DRIVER");
        if(path_loc != std::string::npos) { // If $BG_DRIVER is in the path...
            path.replace(path_loc, 10, driver); // ...replace it with the driver variable
        }
        LOG_DEBUG_MSG("setting path to " << path);
        alp->set_path(path);
    }

    try {
        _agent_manager.setCount(boost::lexical_cast<unsigned>(_master_props->getValue("master.server","max_agents_per_host")));
    } catch(std::invalid_argument& e) {
        std::ostringstream msg;
        msg << "Invalid max agents per host: " << e.what();
        LOG_ERROR_MSG(msg.str());
        throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
    } catch(boost::bad_lexical_cast& e) {
        std::ostringstream msg;
        msg << "Invalid max agents per host: " << e.what();
        LOG_ERROR_MSG(msg.str());
        throw BGMasterExceptions::ConfigError(BGMasterExceptions::WARN, msg.str());
    }

    buildArgs(args);
    buildHostList(hosts, exclude_list, failmsg);
    buildInstances(instances, exclude_list, failmsg);
    std::multimap<Policy::Trigger,Policy::Behavior> behaviors;
    buildFailover(failover, behaviors);
    addBehaviors(failmap, behaviors, failmsg);
    buildStartList(startlist);
    buildUidList(uidlist);
    if(dologdirs)
        buildLogDirs(logdirs);
}

void BGMasterController::waitHosts() {
    LOG_INFO_MSG(__FUNCTION__);
    unsigned total_hosts = _configured_hosts.size();
    if(total_hosts == 0) total_hosts = 1;  // Always expect at least one.
    unsigned host_count = 0;
    unsigned retries = 2; // Six second wait for all hosts to check in.
    LOG_DEBUG_MSG("Waiting up to " << retries << " seconds for "
                  << total_hosts << " agents to start.");
    std::vector<Host> found_hosts;
    while(retries > 0) {
        BOOST_FOREACH(Host& curr_host, _configured_hosts) {
            BGAgentId aid = get_agent_manager().findAgentId(curr_host);
            if(aid.str() != "" &&
               std::find(found_hosts.begin(), found_hosts.end(), curr_host) ==
               found_hosts.end()
               )
                {
                    LOG_DEBUG_MSG("Agent " << aid.str() << " reported in.");
                    found_hosts.push_back(curr_host);
                    ++host_count;
                }
        }
        if(host_count >= total_hosts) {
            LOG_DEBUG_MSG("All agents checked in");
            break;
        }
        sleep(1);
        --retries;
    }
}

void BGMasterController::startServers(std::map<std::string, std::string>& failed_aliases, BGAgentRepPtr agentrep) {
    LOG_INFO_MSG(__FUNCTION__);

    std::string agents_to_start = "all agents";
    if(agentrep)
        agents_to_start = agentrep->get_agent_id().str();

    LOG_INFO_MSG("Starting all listed servers for agent: " << agents_to_start << ".");
    std::vector<AliasPtr> servers_to_start = _start_list;
    int giveup = 15;
    while(servers_to_start.size() > 0 && giveup && !_master_terminating) {
        AliasPtr al = servers_to_start.front();
        // Now take it out of the list, we'll put it in the back later
        // if we can't start it.
        servers_to_start.erase(std::remove(servers_to_start.begin(), 
                                           servers_to_start.end(),
                                           al), 
                               servers_to_start.end());
        std::string failreason = "";
        if(al->get_name() == "bgmaster_server" || al->get_name() == "bgmaster") continue; // Don't start ourselves

        BinaryId id;
        if(al->running(id) == true) {
            continue; // Don't start one already running.
        }

        LOG_INFO_MSG("Attempting to start alias " << al->get_name());

        // Find an agent we can use.  Supply an empty ID and
        // let the alias pick one based on its policy.
        BGAgentId aid;
        BGAgentRepPtr agent;
        try {
            agent = al->validateStartAgent(aid);
            // Now take it out.
#if 0
            if(agentrep != 0) {
                if(agentrep->get_agent_id().str() != agent->get_agent_id().str()) {
                    // If they gave us an agent, that's the only one we use.
                    // Break out and go to the next alias
                    std::ostringstream msg;
                    msg << al->get_name() << " not assigned to agent " << agentrep->get_agent_id().get_host().fqhn();
                    LOG_INFO_MSG(msg.str());
                    failreason = msg.str();
                }
            } 
#endif
     
        } catch (BGMasterExceptions::InternalError& e) {
            LOG_INFO_MSG("Exception finding an agent: " << e.what());
            // Wait a second before trying again if we failed.
            failreason = e.what();
        }

        if(agent != 0) {
            // If we got here, we have an agent ready.
            BGMasterAgentProtocolSpec::StartRequest agentreq(al->get_path(), al->get_args(),
                                                             al->get_logdir(), al->get_name(),
                                                             al->get_user());
            BGMasterAgentProtocolSpec::StartReply agentrep;
            agentrep._rc = BGMasterExceptions::OK;

            BinaryId bid = agent->startBin(agentreq, agentrep);
            if(bid.str() == "0") {
                std::ostringstream msg;
                msg << "Attempting to start binary for alias " << al->get_name()
                    << " failed.  Check RAS.";
                failed_aliases[al->get_name()] = msg.str();
                BGMasterController::handleErrorMessage(msg.str());
                std::map<std::string, std::string> details;
                details["ALIAS"] = al->get_name();
                putRAS(ALIAS_FAIL_RAS, details);
            }
#if 0
            if(bid.str() != "0") {
                al->add_binary(bid);
                LOG_INFO_MSG("Adding started binary " << bid.str() << " for alias " << al->get_name());
            } else {
                std::ostringstream msg;
                msg << "Attempting to start binary for alias " << al->get_name()
                    << " failed.  Check RAS.";
                failed_aliases[al->get_name()] = msg.str();
                BGMasterController::handleErrorMessage(msg.str());
                std::map<std::string, std::string> details;
                details["ALIAS"] = al->get_name();
                putRAS(ALIAS_FAIL_RAS, details);
            }
#endif
        } else {
            --giveup;
            sleep(1);
            // Didn't start it, so put it back in the back.
            servers_to_start.push_back(al);
            LOG_DEBUG_MSG("No agent found running for " << al->get_name());
            failed_aliases[al->get_name()] = failreason;
        }
        usleep(750);  // wait a bit before trying the next one
    }
}

void BGMasterController::startup(bgq::utility::Properties::Ptr& props) {
    LOG_INFO_MSG(__FUNCTION__);
    std::ostringstream version;
    version << "BG/Q";
    version << " " << bgq::utility::DriverName;
    version << "(revision " << bgq::utility::Revision << ")";
    version << " " << __DATE__ << " " << __TIME__;
    std::cout << "bgmaster_server [" << getpid() << "] " << version.str() << " starting..." << std::endl;
    std::cout << "Using " << props->getFilename() << " for properties." << std::endl;
    _master_pid = getpid();
    _master_props = props;
    _master_db = false;
    std::string db_val = "true";
    _start_time = boost::posix_time::second_clock::local_time();

    try {
        db_val = _master_props->getValue("master.server", "db");
    } catch(std::invalid_argument& e) {
        LOG_DEBUG_MSG("Invalid or missing db= parameter specified in properties file. " << e.what());
    }

    if(db_val.length() == 0)
        db_val = "true";

    if(db_val == "true") {
#ifdef WITH_DB
        // Initialize the database for the process
        // This must be done before starting any threads, due to the initialization of static variables
        LOG_DEBUG_MSG("Initializing DB connection pool");
        bgq::utility::Properties::ConstPtr p = props;
        BGQDB::DBConnectionPool::reset();
        // We aren't cutting RAS very often, just one DB pool thread.
        BGQDB::DBConnectionPool::init(p, 1);
        _updater.start(); // Start the DB updater.
        _master_db = true;
#endif
    } else {
        if(db_val != "false") {
            LOG_WARN_MSG("Value " << "\"" << db_val << "\" "
                         << "for \"db\" in bg.properties' \"master.server\" section "
                         << " invalid.  It must be either \"true\" or \"false\".  "
                         << "Will not log RAS to the database.");
        }
        _master_db = false;
    }

    try {
        _master_logdir = _master_props->getValue("master.server", "logdir");
        if(access(_master_logdir.c_str(), R_OK|W_OK) < 0) {
            std::ostringstream errmsg;
            errmsg << "Directory " << _master_logdir << " inaccessible to bgmaster_server";
            BGMasterController::handleErrorMessage(errmsg.str());
            throw BGMasterExceptions::ConfigError(BGMasterExceptions::FATAL, errmsg.str());
        }
    } catch(std::invalid_argument& e) {
            LOG_WARN_MSG("No log directory found.  Will use default.");
    }

    // Read policy information from the config file
    std::ostringstream failmsg;
    buildPolicies(failmsg);

    if(!failmsg.str().empty()) {
        std::ostringstream msg;
        msg << "Invalid config:" << failmsg.str();
        BGMasterController::handleErrorMessage(msg.str());
        throw BGMasterExceptions::ConfigError(BGMasterExceptions::FATAL, failmsg.str());
    }

    _agent_registrar.run(true);
    sleep(3);
    _client_registrar.run(false);

    // Update database with ras message
    std::map<std::string, std::string> details;
    details["PID"] = boost::lexical_cast<std::string>(_master_pid);
    putRAS(MASTER_STARTUP_RAS, details);
    std::ostringstream startmsg;
    boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
    startmsg << "bgmaster_server startup finished at " << now;
    BGMasterController::addHistoryMessage(startmsg.str());

    _start_barrier.wait();

    while (signal_number == 0 && _master_terminating == false) {
        // Check if either registrar has failed.  If so, restart it.
        bool reregister = false;
        if(_agent_registrar.get_failed() == true) {
            _client_registrar.cancel();
            _agent_registrar.run(true);
            _client_registrar.run(false);
            reregister = true;
        }
        else if(_client_registrar.get_failed() == true) {
            _agent_registrar.cancel();
            _client_registrar.run(false);
            _agent_registrar.run(true);
            reregister = true;
        }
        if(reregister)  // Only barrier wait if we restarted registrars
            _start_barrier.wait();
        // Waiting for sig
        sleep(5);
    }
}
