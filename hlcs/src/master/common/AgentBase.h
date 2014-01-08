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

#ifndef MASTER_AGENT_BASE_H_
#define MASTER_AGENT_BASE_H_

#include "Ids.h"
#include "types.h"

#include <utility/include/cxxsockets/Host.h>

#include <boost/utility.hpp>

#include <vector>


//! \brief Base class providing agent definition
//! Both master and agent use this.
class AgentBase : boost::noncopyable
{
public:
    typedef std::vector<BinaryControllerPtr> Binaries;

public:
    //! \brief Constructor.
    AgentBase() : _ending(false) {}

    virtual ~AgentBase() { }

    const CxxSockets::Host& get_host() const { return _host; }
    const BGAgentId& get_agent_id() const { return _agent_id; }

    //! \brief find a binary controller with a binary id
    bool find_binary(const BinaryId& id, BinaryControllerPtr& p) const;

    //! \brief Find out if the specified alias is running on this agent.
    //! \param al Alias pointer to check
    //! \returns true or false
    bool runningAlias(const std::string &alias_name) const;

    //! \brief Return a COPY of the internal binary container.
    const Binaries& get_binaries() const { return _binaries; }
    bool ending() const { return _ending; }

protected:
    void addController(const BinaryControllerPtr& controller);
    void removeController(const BinaryControllerPtr& controller);

    //! \brief Protocol management object
    AgentProtocolPtr _prot;

    //! \brief agent is ending.
    bool _ending;

    //! \brief  Agent's unique identifier
    BGAgentId _agent_id;

    //! \brief Agent's host name
    CxxSockets::Host _host;

private:
    //! Container of binaries managed by this agent
    Binaries _binaries;
};

#endif
