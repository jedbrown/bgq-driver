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
#ifndef RUNJOB_SERVER_JOB_KILL_TIMER_H_
#define RUNJOB_SERVER_JOB_KILL_TIMER_H_

#include "common/fwd.h"

#include "server/fwd.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/asio/deadline_timer.hpp>

#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief
 */
class KillTimer
{
public:
    /*!
     * \brief ctor.
     */
    KillTimer(
            const boost::shared_ptr<Server>& server,    //!< [in]
            const JobInfo& info                         //!< [in]
            );

    /*!
     * \brief Start the timer.
     *
     * \throws std::logic_error if the timer has already been started.
     */
    void start(
            size_t seconds,                         //!< [in]
            const boost::shared_ptr<Job>& job       //!< [in]
            );

    /*!
     * \brief Get the time when this timer expires.
     */
    boost::posix_time::time_duration expires() const { return _timer.expires_from_now(); }

    /*!
     * \brief Stop the timer.
     */
    void stop();

private:
    void handler(
            const boost::system::error_code& error,
            const boost::shared_ptr<Job>& job,
            size_t seconds
            );

    void insertRas(
            const boost::shared_ptr<Job>& job,
            size_t timeout
            );

    size_t getNodeCount(
            const boost::shared_ptr<Job>& job
            ) const;

    void breadcrumbs(
            const boost::shared_ptr<Job>& job
            ) const;

    void cleanup(
            const boost::shared_ptr<Job>& job
            ) const;

private:
    boost::asio::deadline_timer _timer;
    bool _force;
};

} // job
} // server
} // runjob

#endif
