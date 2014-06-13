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
#ifndef RUNJOB_SERVER_JOB_JOB_H_
#define RUNJOB_SERVER_JOB_JOB_H_

#include "server/block/fwd.h"

#include "server/database/fwd.h"

#include "server/job/class_route/fwd.h"

#include "server/job/tool/Container.h"

#include "server/job/ExitStatus.h"
#include "server/job/fwd.h"
#include "server/job/Heartbeat.h"
#include "server/job/IoNode.h"
#include "server/job/KillTimer.h"
#include "server/job/StdioQueue.h"
#include "server/job/Status.h"

#include "server/mux/fwd.h"

#include "server/fwd.h"

#include "common/error.h"
#include "common/JobInfo.h"

#include <db/include/api/job/types.h>

#include <boost/asio/strand.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {

/*!
 * \brief starts, monitors, and stops a %job
 */
class Job : public boost::enable_shared_from_this<Job>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Job> Ptr;

public:
    /*!
     * \brief ctor.
     */
    explicit Job(
            const job::Create& create  //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Job();

    /*!
     * \brief Set error code and message.
     */
    void setError(
            const std::string& message,     //!< [in]
            runjob::error_code::rc error    //!< [in]
            );

    /*!
     * \brief Get JobInfo.
     */
    const JobInfo& info() const { return _info; }

    /*!
     * \brief Get ID.
     */
    BGQDB::job::Id id() const { return _id; }

    /*!
     * \brief Get client ID.
     */
    uint64_t client() const { return _client; }

    /*!
     * \brief Get mux.
     */
    const boost::weak_ptr<mux::Connection>& mux() const { return _mux; }

    /*!
     * \brief Get block.
     */
    const boost::shared_ptr<block::Compute>& block() const { return _block; }

    /*!
     * \brief Get strand.
     */
    boost::asio::strand& strand() { return _strand; }

    /*!
     * \brief Get I/O map.
     */
    job::IoNode::Map& io() { return _io; }

    /*!
     * \brief Get output queue.
     */
    job::StdioQueue& queue() { return _queue; }

    /*!
     * \brief Get status.
     */
    job::Status& status() { return _status; }

    /*!
     * \brief Get exit status.
     */
    job::ExitStatus& exitStatus() { return _exitStatus; }

    /*!
     * \brief Get kill timeout.
     */
    job::KillTimer& killTimer() { return _killTimer; }

    /*!
     * \brief Get scr heartbeat.
     */
    job::Heartbeat& heartbeat() { return _heartbeat; }

    /*!
     * \brief Get tool container.
     */
    job::tool::Container& tools() { return _tools; }

    /*!
     * \brief
     */
    const boost::shared_ptr<job::SubNodePacing>& pacing() { return _pacing; }

    /*!
     * \brief Remove the job.
     */
    void remove();

private:
    friend class database::Insert;
    friend class job::Setup;
    friend class job::Signal;
    friend class job::Handle;
    friend class job::RankMapping;
    friend class mux::Reconnect;

    job::Status _status;                                            //!<
    const JobInfo _info;                                            //!<
    boost::asio::strand _strand;                                    //!<
    BGQDB::job::Id _id;                                             //!<
    job::IoNode::Map _io;                                           //!<
    boost::weak_ptr<mux::Connection> _mux;                          //!<
    job::StdioQueue  _queue;                                        //!<
    job::ExitStatus _exitStatus;                                    //!<
    job::KillTimer _killTimer;                                      //!<
    job::Heartbeat _heartbeat;                                      //!<
    const boost::shared_ptr<block::Compute> _block;                 //!<
    const uint64_t _client;                                         //!<
    const boost::weak_ptr<Server> _server;                          //!<
    job::tool::Container _tools;                                    //!<
    const boost::shared_ptr<job::SubNodePacing> _pacing;            //!<
    int _outstandingSignal;                                         //!<
};

} // server
} // runjob

#endif
