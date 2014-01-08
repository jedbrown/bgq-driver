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
#ifndef RUNJOB_SERVER_JOB_CREATE_H
#define RUNJOB_SERVER_JOB_CREATE_H

#include "server/block/fwd.h"

#include "server/job/fwd.h"
#include "server/job/IoNode.h"

#include "server/mux/fwd.h"

#include "server/performance/Counters.h"

#include "common/error.h"
#include "common/JobInfo.h"
#include "common/Shape.h"
#include "common/Uci.h"

#include "server/fwd.h"

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace job  {

/*!
 * \brief Named parameter idiom to create runjob::server::Job objects.
 */
class Create
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Create> Ptr;

    /*!
     * \brief arbitration callback.
     */
    typedef boost::function<void(const error_code::rc,const boost::shared_ptr<Job>&)> Callback;

public:
    /*!
     * \brief ctor.
     */
    Create(
            const JobInfo& info //!< [in]
          );

    /*!
     * \brief dtor.
     */
    ~Create();
  
    /*!
     * \brief set Server.
     */
    Create* server(
            const boost::shared_ptr<Server>& server //!< [in]
            )
    {
        _server = server;
        return this;
    }

    /*!
     * Set ID.
     */
    Create* client_id(
            uint64_t id //!< [in]
            )
    {
        _client_id = id;
        return this;
    }

    /*!
     * Set node count.
     */
    Create* node_count(
            unsigned count //!< [in]
            )
    {
        _node_count = count;
        return this;
    }

    /*!
     * \brief Set mux connection.
     */
    Create* mux_connection(
            const boost::shared_ptr<mux::Connection>& mux    //!< [in[
            )
    {
        _mux = mux;
        return this;
    }

    /*!
     * \brief Set compute block.
     */
    Create* compute_block(
            const boost::shared_ptr<block::Compute>& block  //!< [in]
            )
    {
        _block = block;
        return this;
    }

    /*!
     * \brief Set sub-node job pacing.
     */
    Create* pacing(
            const boost::shared_ptr<SubNodePacing>& pacing   //!< [in]
            )
    {
        _pacing = pacing;
        return this;
    }

    /*!
     * \brief
     */
    Create* error(
            const std::string& message,     //!< [in]
            runjob::error_code::rc error    //!< [in]
            )
    {
        _message = message;
        _error = error;
        return this;
    }

    /*!
     * \brief
     */
    Create* arbitration(
            const performance::Counters::JobContainer::Timer::Ptr& timer
            )
    {
        _arbitration_timer = timer;
        return this;
    }

    /*!
     * \brief
     */
    Create* security(
            const performance::Counters::JobContainer::Timer::Ptr& timer
            )
    {
        _security_timer = timer;
        return this;
    }

    /*!
     * \brief
     */
    Create* io(
            const boost::shared_ptr<block::IoNode>& io,     //!< [in]
            unsigned computes                               //!< [in]
            );

    /*!
     * \brief
     */
    Create* io(
            const Uci& node,                        //!< [in]
            unsigned computes                       //!< [in]
            );

    /*!
     * \brief
     */
    Create* reconnect(
            BGQDB::job::Id id
            )
    {
        _reconnect = id;
        return this;
    }

    /*!
     * \brief
     */
    Create* callback(
            const Callback& callback
            )
    {
        _callback = callback;
        return this;
    }

    /*!
     * \brief
     */
    boost::weak_ptr<Job> create();

private:
    friend class runjob::server::Job;
    friend class runjob::server::block::Compute;
    friend class runjob::server::block::Midplane;

    boost::shared_ptr<Job> _job;
    boost::shared_ptr<Server> _server;
    uint64_t _client_id;
    IoNode::Map _io;
    unsigned _node_count;
    JobInfo _job_info;
    boost::shared_ptr<mux::Connection> _mux;
    boost::shared_ptr<block::Compute> _block;
    boost::shared_ptr<SubNodePacing> _pacing;
    std::string _message;
    error_code::rc _error;
    performance::Counters::JobContainer::Timer::Ptr _arbitration_timer;
    performance::Counters::JobContainer::Timer::Ptr _security_timer;
    BGQDB::job::Id _reconnect;
    Callback _callback;
};

} // job
} // server
} // runjob

#endif
