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

#ifndef ALIAS_H
#define ALIAS_H


#include "common/Ids.h"
#include "Policy.h"
#include "types.h"

#include <utility/include/cxxsockets/Host.h>

#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <list>
#include <string>


//! \brief Alias class.  Represents the association between a name,
//! an executable, its policies, and its active instances.
class Alias : private boost::noncopyable
{
public:
    Alias(
            const std::string& name, 
            const std::string& path, 
            const Policy& p, 
            const std::string& user = std::string(),
            const std::string& logdir = std::string(),
            const int preferredHostWait = int()
         );

    void set_path(const std::string& path) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _path = path;
    }
    void set_args(const std::string& args) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _args = args;
    }
    void set_user(const std::string& uid) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _user = uid;
    }
    void set_logdir(const std::string& logdir) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _logdir = logdir;
    }
    void add_host(const CxxSockets::Host& host) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _hosts.push_back(host);
    }
    void add_binary(const BinaryId& id) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _binaries.push_back(id);
    }

    void remove_binary(const BinaryId& id);

    void resetRetries() { _retry_count = 0; }

    //! \brief validate this agent id against policy
    //! \return AgentRepPtr if policy allows this agent
    AgentRepPtr validateStartAgent(const BGAgentId& agent_id);

    //! \brief Evaluate what we need to do based on the associated policy
    //! \return AgentRepPtr of the agent which must run this Alias next (if any)
    AgentRepPtr evaluatePolicy(Policy::Trigger trig, BGAgentId& agent, const BinaryId& failed_bid, BinaryControllerPtr bptr);

    bool check_instances() const { if (_my_policy.limit() <= _binaries.size()) return false; else return true; }

    //! \brief See if this alias has a specific binary id associated
    //! \param id Binary id to look for
    //! \return true if found, false if not
    bool find_binary(const BinaryId& id) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        BOOST_FOREACH(BinaryId idit, _binaries) {
            if (id == idit) return true;
        }
        return false;
    }

    //! \brief See if this alias has any running binaries
    //! \param id ID of first binary running under this alias will be returned
    //! \return true if we have an associated binary id
    bool running(BinaryId& id) const {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        if (!_binaries.empty()) {
            id = _binaries.front();
            return true;
        }
        return false;
    }

    //! \brief See if there are any running binaries, don't return any either.
    //! \return true if there are any associated binaries.
    bool running() const {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return !_binaries.empty();
    }

    //! \brief If the passed host is in our list return true
    bool find_host(const CxxSockets::Host& host) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return find_host_internal(host);
    }

    Policy& policy() {
        return _my_policy;
    }

    const std::string& get_name() const { return _name; }
    const std::string& get_path() const { return _path; }
    const std::string& get_args() const { return _args; }
    const std::string& get_user() const { return _user; }
    const std::string& get_logdir() const { return _logdir; }

private:
    AgentRepPtr runPolicy(const BGAgentId& agent_id, bool restart);
    
    //! \brief If the passed host is in our list return true
    bool find_host_internal(const CxxSockets::Host& host) {
        BOOST_FOREACH(CxxSockets::Host h, _hosts) {
            if (h == host) return true;
        }
        return false;
    }

    //! This is the identifier name associated with a specific bin
    const std::string _name;

    std::string _path;
    std::string _args;

    //! \brief user id under which this process should run
    std::string _user;

    //! \brief directory in which to log binaries associated with this alias.
    std::string _logdir;

    //! \brief time in seconds to wait for the preferred host's bgagent to become available.
    const int _preferredHostWait;

    //! \brief hosts on which alias can run
    std::list<CxxSockets::Host> _hosts;

    //! \brief List of binary instances associated with this alias
    std::list<BinaryId> _binaries;

    mutable boost::mutex _mutex;

    //! \brief policy associated with this binary
    Policy _my_policy;
    //! \brief Number of start retries attempted for this alias.
    unsigned _retry_count;
    //! \brief This flag is set if we are waiting for an agent to start.
    bool _waiting_for_agent;
    //! \brief This flag is set if we need to stop waiting for an agent to start.
    bool _halt_waiting_for_agent;
    
    //! \brief The time we failed to start this alias on its preferred host
    boost::posix_time::ptime _preferred_start_time;
};

#endif
