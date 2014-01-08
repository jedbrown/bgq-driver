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

#ifndef _POLICY_H
#define _POLICY_H

#include <list>
#include <stack>
#include <tr1/tuple>
#include <boost/thread/mutex.hpp>
#include <boost/foreach.hpp>

class Host;

//! \brief What to do and where when we get a failure.
//! A "policy" is a complete set of triggers and behaviors that
//! is related back to a binary alias.  A Trigger is a cause, a
//! Behavior is what to do about it.  Policies also govern the
//! number of instances an alias is allowed and its dependencies.
class Policy {
public:
    //! These are presented as enums here, but they are strings
    //! in the config file.  Maintaining them as enums forces
    //! validation wherever used.
    //! \brief Actions to apply when a trigger is detected.
    enum Action { INVALID_ACTION, FAILOVER, RESTART, CLEANUP };

    //! \brief Acceptable triggers.
    enum Trigger { INVALID_TRIGGER, KILL_REQUESTED, BINARY_ABEND, AGENT_ABEND };

    //! \brief Number of times to retry starting.
    typedef unsigned short Retries;

    //! \brief What to do when a trigger is met.
    //! A "behavior" is a complex of an action to perform,
    //! on what to perform it, and the number of times to
    //! attempt it.
    class Behavior {
        Action _action;
        std::map<Host, Host> _pairs;
        Retries _retries;
        std::string _name;
    public:
        typedef boost::shared_ptr<Behavior> BehaviorPtr;
        Behavior() : _action(INVALID_ACTION), _retries(0), _name("") { }
        ~Behavior() {}
        Behavior(const Behavior& b)
            : _action(b._action), _pairs(b._pairs),
              _retries(b._retries), _name(b._name) {}
        Behavior(std::string& name, Action a, std::map<Host, Host> fp, Retries r)
            : _action(a), _pairs(fp), _retries(r), _name(name){}
        Action get_action() { return _action; }
        Retries get_retries() { return _retries; }
        std::string get_name() { return _name; }
        std::map<Host, Host>& get_host_pairs() { return _pairs; }
        //! \brief Figure out where to fail over.
        //! \param failed_host Host that failed.
        //! \return The target host for the failover.
        Host findFailoverTarget(const Host& failed_host);
    };

    Policy(short max_count = 1) : _occurences(max_count) {}
    Policy(Trigger& t, Behavior& b, short max_count = 1) :
        _occurences(max_count)
    {
        _action_policies[t] = b;
    }

    Policy(std::stack<std::string>& dependency) :
        _dependencies(dependency), _occurences(1) { }

    //! \brief Change the number of allowable instances.
    void changeInstances(const int instances) {
        _occurences = instances;
    }

    bool addBehavior(const Trigger& t, const Behavior& b) {
        // If the trigger isn't already here, add it
        if(_action_policies.find(t) == _action_policies.end()) {
            _action_policies[t] = b;
            return true;
        } else return false;
    }

    void addDependency(const std::string& dependency) {
        _dependencies.push(dependency);
    }

    //! \brief Return the action for the trigger
    bool get_behavior(Trigger t, Behavior& bvr) {
        if(_action_policies.size() == 0) {
            return false;
        } else {
            std::map<Trigger,Behavior>::iterator it = _action_policies.find(t);
            if(it != _action_policies.end()) {
                bvr = it->second;
                return true;
            }
        }
        return false;
    }

    unsigned short limit() { return _occurences; }

    static Trigger string_to_trigger(const std::string& st) {
        Trigger t;
        if(st == "killed")
            t = KILL_REQUESTED;
        else if(st == "binary")
            t = BINARY_ABEND;
        else if(st == "agent")
            t = AGENT_ABEND;
        else
            t = INVALID_TRIGGER;
        return t;
    }

    static Action string_to_action(const std::string& st) {
        Action a;
        if(st == "failover")
            a = FAILOVER;
        else if(st == "cleanup")
            a = CLEANUP;
        else if(st == "restart")
            a = RESTART;
        else
            a = INVALID_ACTION;
        return a;
    }

private:

    //! \brief List of aliases that must be executing before the binary
    //  associated with this policy.
    std::stack<std::string> _dependencies;

    //! \brief map of Behaviors to triggers
    std::map<Trigger, Behavior> _action_policies;

    //! \brief Number of instances of the associated binary allowed.
    unsigned short _occurences;
};


#endif
