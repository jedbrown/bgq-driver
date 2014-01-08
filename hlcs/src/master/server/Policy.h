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

#ifndef MASTER_POLICY_H_
#define MASTER_POLICY_H_


#include "Behavior.h"

#include <stack>
#include <string>
#include <map>


//! \brief What to do and where when we get a failure.
//! A "policy" is a complete set of triggers and behaviors that
//! is related back to a binary alias.  A Trigger is a cause, a
//! Behavior is what to do about it.  Policies also govern the
//! number of instances an alias is allowed and its dependencies.
class Policy 
{
public:
    //! \brief Acceptable triggers.
    enum Trigger { INVALID_TRIGGER, KILL_REQUESTED, BINARY_ABEND, AGENT_ABEND };


    Policy(unsigned short max_count = 1) : _occurrences(max_count) {}
    Policy(Trigger& t, Behavior& b, unsigned short max_count = 1) :
        _occurrences(max_count)
    {
        _action_policies[t] = b;
    }

    //! \brief Change the number of allowable instances.
    void changeInstances(unsigned short instances) {
        _occurrences = instances;
    }

    bool addBehavior(Trigger t, const Behavior& b) {
        // If the trigger isn't already here, add it
        if(_action_policies.find(t) == _action_policies.end()) {
            _action_policies[t] = b;
            return true;
        } else return false;
    }

    //! \brief Return the action for the trigger
    bool get_behavior(Trigger t, Behavior& bvr) const {
        if(_action_policies.empty()) {
            return false;
        } else {
            std::map<Trigger,Behavior>::const_iterator it = _action_policies.find(t);
            if(it != _action_policies.end()) {
                bvr = it->second;
                return true;
            }
        }
        return false;
    }

    unsigned short limit() const { return _occurrences; }

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

private:
    //! \brief map of Behaviors to triggers
    std::map<Trigger, Behavior> _action_policies;

    //! \brief Number of instances of the associated binary allowed.
    unsigned short _occurrences;
};


#endif
