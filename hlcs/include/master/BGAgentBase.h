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

#ifndef _BGAGENT_BASE_H
#define _BGAGENT_BASE_H

#include <tr1/memory>
#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <boost/utility.hpp>
#include "Properties.h"
#include "Ids.h"
#include "BGMasterAgentProtocol.h"

// Forward declares
class BGMasterAgentProtocol;
class BinaryController;
class Host;
class Alias;

typedef boost::shared_ptr<boost::thread> ThreadPtr;

//! \brief Base class providing agent definition
//! Both master and agent use this.
class BGAgentBase : boost::noncopyable {
public:
    typedef boost::shared_ptr<BinaryController> BinaryControllerPtr;
    //! \brief Constructor.
    BGAgentBase() : _ending(false), _failover(true), _orderly(false) {}

    virtual ~BGAgentBase() { }

    Host get_host() { return _host; }
    BGAgentId get_agent_id() { return _agent_id; }
    //! \brief find a binary controller with a binary id
    bool find_binary(BinaryId& id, boost::shared_ptr<BinaryController>& p);
    boost::shared_ptr<BinaryController>& get_last_bin() { return _last_bin; }

    //! \brief Find out if the specified alias is running on this agent.
    //! \param al Alias pointer to check
    //! \returns true or false
    bool runningAlias(boost::shared_ptr<Alias>& al);

    //! \brief Return a COPY of the internal binary vector.
    std::vector<boost::shared_ptr<BinaryController> > get_binaries() { return _binaries; }
    bool ending() { return _ending; }

protected:
    void addController(boost::shared_ptr<BinaryController> controller) {
        _binaries.push_back(controller); }

    //! \brief Protocol management object
    BGMasterAgentProtocolPtr _prot;

    //! \brief agent is ending.
    bool _ending;
    //! \brief allows agent to fail over its processes to another
    bool _failover;
    //! \brief bgmaster is ending this agent in an orderly fashion.
    bool _orderly;

    //! Vector of binaries managed by this agent
    std::vector<BinaryControllerPtr> _binaries;

    //! \brief Archived last binary run
    boost::shared_ptr<BinaryController> _last_bin;

    //! \brief Thread to wait for requests
    boost::thread _agent_socket_poller;

    pthread_t _my_tid;

    //! \brief  Agent's unique identifier
    BGAgentId _agent_id;

    //! \brief Agent's host name
    Host _host;

    // Must be mutable even when the rest of the world is
    // const because locks don't get copied.
    mutable boost::mutex _agent_mutex;
};

typedef boost::shared_ptr<BGAgentBase> BGAgentBasePtr;

#endif
