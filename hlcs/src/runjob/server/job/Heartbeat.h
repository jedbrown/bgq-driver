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
#ifndef RUNJOB_SERVER_JOB_HEARTBEAT_H
#define RUNJOB_SERVER_JOB_HEARTBEAT_H

#include "common/Uci.h"

#include "server/fwd.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/asio/deadline_timer.hpp>

#include <boost/shared_ptr.hpp>

#include <map>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief
 */
class Heartbeat
{
public:
    /*!
     * \brief
     */
    typedef std::map<Uci, unsigned> ToolCount;

public:
    /*!
     * \brief ctor.
     */
    Heartbeat(
            const boost::shared_ptr<Server>& server    //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Heartbeat();

    /*!
     * \brief
     */
    void start(
            const boost::shared_ptr<Job>& job   //!< [in]
            );

    /*!
     * \brief
     */
    void stop();

    /*!
     * \brief
     */
    void pulse(
            const boost::shared_ptr<Job>& job,
            const Uci& node,
            unsigned count
            );

private:
    void wait(
            const boost::shared_ptr<Job>& job
            );

    unsigned sum() const;

    void handler(
            const boost::system::error_code& error,
            const boost::shared_ptr<Job>& job
            );

private:
    boost::asio::deadline_timer _timer;
    unsigned _waitTime;
    boost::posix_time::ptime _start;
    ToolCount _status;
    bool _stopped;
    unsigned _cycles;
};

} // job
} // server
} // runjob

#endif
