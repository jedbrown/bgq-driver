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

#ifndef MASTER_AGENT_MANAGER_H_
#define MASTER_AGENT_MANAGER_H_


#include "common/Ids.h"
#include "types.h"

#include <utility/include/cxxsockets/Host.h>

#include <boost/thread/mutex.hpp>

#include <string>
#include <vector>


//! One instance.  Manages all bgagents
class AgentManager
{
public:
    AgentManager();

    void startup() {}

    //! \brief set the number of agents we are allowed to have for each host
    //! \param count number of agents
    void setCount(unsigned count);

    //! \brief End processing of all agent threads
    //! \param end_binaries Also end all managed binaries
    //! \param signal to use to end binaries
    void cancel(bool end_binaries, int signal);

    //! \brief Insert a new agent in the list
    bool addNew(AgentRepPtr agent);

    //! \brief Agent is gone.  Pull it out
    void removeAgent(AgentRepPtr agent);

    //! \brief Figure out which agent to run the bin
    AgentRepPtr pickAgent();

    //! \brief Find an agent id associated with a host
    BGAgentId findAgentId(const CxxSockets::Host& host);

    //! \brief Find a particular agent
    AgentRepPtr findAgentRep(const BGAgentId& aid);

    //! \brief Find an agent by the host name
    AgentRepPtr findAgentRep(const CxxSockets::Host& host);

    //! \brief Find a specific binary by id
    bool findBinary(const BinaryId& id, BinaryLocation& loc);

    //! \brief Find all binaries by alias
    bool findBinary(const std::string& alias, std::vector<BinaryLocation>& loc);

    //! \brief Agents are ending
    bool get_ending_agents() { return _ending_agents; }

    //! \brief get a COPY of the agent vector.
    std::vector<AgentRepPtr> get_agent_list() const { return _agents; }

private:

    //! \brief vector of BGAgents
    std::vector<AgentRepPtr> _agents;
    boost::mutex _agent_manager_mutex;
    bool _ending_agents;
    unsigned int _agents_per_host;
};


#endif
