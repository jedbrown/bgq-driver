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


#ifndef MASTER_BEHAVIOR_H_
#define MASTER_BEHAVIOR_H_


#include <utility/include/cxxsockets/Host.h>

#include <map>
#include <string>


//! \brief What to do when a trigger is met.
//! A "behavior" is a complex of an action to perform,
//! on what to perform it, and the number of times to
//! attempt it.
class Behavior
{
public:
    typedef unsigned short Retries;

    //! These are presented as enums here, but they are strings
    //! in the config file.  Maintaining them as enums forces
    //! validation wherever used.
    //! \brief Actions to apply when a trigger is detected.
    enum Action { INVALID_ACTION, FAILOVER, RESTART, CLEANUP };

private:
    Action _action;
    std::map<CxxSockets::Host, CxxSockets::Host> _pairs;
    Retries _retries;
    std::string _name;

public:
    //! \brief Number of times to retry starting.

    Behavior() : _action(INVALID_ACTION), _retries(0), _name("") { }
    ~Behavior() {}
    Behavior(const std::string& name, Action a, const std::map<CxxSockets::Host, CxxSockets::Host>& fp, Retries r)
        : _action(a), _pairs(fp), _retries(r), _name(name){}
    Action get_action() const { return _action; }
    Retries get_retries() const { return _retries; }
    const std::string& get_name() const { return _name; }
    const std::map<CxxSockets::Host, CxxSockets::Host>& get_host_pairs() const { return _pairs; }
    //! \brief Figure out where to fail over.
    //! \param failed_host Host that failed.
    //! \return The target host for the failover.
    CxxSockets::Host findFailoverTarget(const CxxSockets::Host& failed_host);


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
};

#endif
