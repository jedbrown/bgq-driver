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

#ifndef _ALIAS_H
#define _ALIAS_H

#include "Policy.h"

#define LOGGING_DECLARE_ALIAS_MDC(name) \
    log4cxx::MDC _location_mdc( "ALIAS", std::string("{") + name + "} " );

class BGAgentRep;
class Policy;
class BinaryController;

//! \brief Alias class.  Represents the association between a name,
//! an executable, its policies, and its active instances.
class Alias {
    typedef boost::shared_ptr<BGAgentRep> BGAgentRepPtr;
public:

    // Default, do nothing
    Alias() : _waiting_for_agent(false), _halt_waiting_for_agent(false), _preferred_fails(0) {}

    // Constructor
    Alias(std::string& name, std::string& path, Policy& p, 
          std::string user = "", std::string logdir = "") :
        _name(name),
        _path(path),
        _user(user),
        _logdir(logdir),
        _my_policy(p),
        _retry_count(0),
        _waiting_for_agent(false),
        _halt_waiting_for_agent(false),
        _preferred_fails(0) {}

    // Copy constructor
    Alias(const Alias& al) : _name(al._name),
                             _path(al._path),
                             _args(al._args),
                             _user(al._user),
                             _logdir(al._logdir),
                             _my_policy(al._my_policy),
                             _retry_count(al._retry_count),
                             _waiting_for_agent(false),
                             _halt_waiting_for_agent(false),
                             _preferred_fails(0) {  }

    const Alias& operator=(Alias al) {
        if(this == & al)
            return *this;
        al.lockme();
        _mutex.lock();

        _name = al._name;
        _my_policy = al._my_policy;
        _retry_count = al._retry_count;
        _user = al._user;
        _preferred_fails = 0;
        _mutex.unlock();
        al.unlockme();
        return *this;
    }

    void set_path(std::string& path) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _path = path; }
    void set_args(std::string& args) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _args = args; }
    void set_user(std::string& uid) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _user = uid; }
    void set_logdir(std::string& logdir) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _logdir = logdir;}
    void add_host(Host host) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _hosts.push_back(host); }
    bool host_in(Host& host) {
        BOOST_FOREACH(Host& h, _hosts) {
            if(h == host)
                return true;
        }
        return false;
    }
    void add_binary(BinaryId& id) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _binaries.push_back(id);
    }

    void remove_binary(BinaryId& id);

    void resetRetries() { _retry_count = 0; }

    //! \brief validate this agent id against policy
    //! \return BGAgentRepPtr if policy allows this agent
    boost::shared_ptr<BGAgentRep> validateStartAgent(BGAgentId& agent_id);

    //! \brief Evaluate what we need to do based on the associated policy
    //! \return BGAgentRepPtr of the agent which must run this Alias next (if any)
    boost::shared_ptr<BGAgentRep> evaluatePolicy(Policy::Trigger trig, BGAgentId& agent, BinaryId& failed_bid, boost::shared_ptr<BinaryController> bptr);

    bool check_instances() { if(policy().limit() <= _binaries.size()) return false; else return true; }

    //! \brief See if this alias has a specific binary id associated
    //! \param id Binary id to look for
    //! \return true if found, false if not
    bool find_binary(const BinaryId& id) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        BOOST_FOREACH(BinaryId idit, _binaries) {
            if(id == idit) return true;
        }
        return false;
    }

    //! \brief See if this alias has any running binaries
    //! \param id ID of first binary running under this alias will be returned
    //! \return true if we have an associated binary id
    bool running(BinaryId& id) {
        if(_binaries.size() > 0) {
            id = _binaries.front();
            return true;
        } return false;
    }

    //! \brief See if there are any running binaries, don't return any either.
    //! \return true if there are any associated binaries.
    bool running() {
        return (_binaries.size()?true:false);
    }

    //! \brief If the passed host is in our list return true
    bool find_host(const Host host) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return find_host_internal(host);
    }

    Policy& policy() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _my_policy; }
    std::string get_name() { return _name; }
    std::string get_path() { return _path; }
    std::string get_args() { return _args; }
    std::string get_user() { return _user; }
    std::string get_logdir() { return _logdir; }
private:

    //BinaryId start(BGAgentId& agent_id);
    BGAgentRepPtr runPolicy(BGAgentId& agent_id, bool restart);
    
    void lockme() {
        _mutex.lock();
    }

    void unlockme() {
        _mutex.unlock();
    }

    //! \brief If the passed host is in our list return true
    bool find_host_internal(const Host host) {
        BOOST_FOREACH(Host h, _hosts) {
            if(h == host) return true;
        }
        return false;
    }

    //! This is the identifier name associated with a specific bin
    std::string _name;

    std::string _path;
    std::string _args;

    //! \brief user id under which this process should run
    std::string _user;

    //! \brief directory in which to log binaries associated with this alias.
    std::string _logdir;

    //! \brief hosts on which alias can run
    std::list<Host> _hosts;

    //! \brief List of binary instances associated with this alias
    std::list<BinaryId> _binaries;

    // Must be mutable even when the rest of the world is
    // const because locks don't get copied.
    mutable boost::mutex _mutex;

    //! \brief policy associated with this binary
    Policy _my_policy;
    //! \brief Number of start retries attempted for this alias.
    unsigned _retry_count;
    //! \brief This flag is set if we are waiting for an agent to start.
    bool _waiting_for_agent;
    //! \brief This flag is set if we need to stop waiting for an agent to start.
    bool _halt_waiting_for_agent;
    
    //! \brief The number of times we've failed to find an agent on the preferred host.
    unsigned _preferred_fails;
};

typedef boost::shared_ptr<Alias> AliasPtr;

//! \brief Locking wrapper for alias vector
class AliasList {
    std::vector<AliasPtr> _alias_list;
    mutable boost::mutex _mutex;
public:
    //! \brief Find the alias associated with the passed string
    bool find_alias(std::string& al, AliasPtr& alias);

    // Get a copy of the list for local usage.  Don't try to
    // update it, it's a copy!
    std::vector<AliasPtr> get_list_copy() { return _alias_list; }

    typedef std::vector<AliasPtr>::iterator iterator;
    typedef std::vector<AliasPtr>::const_iterator const_iterator;
    size_t size() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.size(); }
    void push_back(AliasPtr sp) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _alias_list.push_back(sp); }
    void pop_back() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _alias_list.pop_back(); }
    iterator insert (iterator position, AliasPtr sp) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.insert(position, sp); }
    iterator begin() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.begin(); }
    iterator end() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.end(); }
    iterator erase ( iterator first, iterator last ) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.erase(first, last); }
    const_iterator begin() const {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.begin(); }
    const_iterator end() const {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.end(); }
    void clear() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _alias_list.clear(); }
};

#endif
