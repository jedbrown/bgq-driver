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
#ifndef RUNJOB_SERVER_JOB_SIGNAL_H
#define RUNJOB_SERVER_JOB_SIGNAL_H

#include "common/defaults.h"

#include "server/fwd.h"

#include <hlcs/include/runjob/commands/error.h>

#include <sstream>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief Deliver a signal to a job.
 */
class Signal
{
public:
    /*!
     * \brief Callback type.
     */
    typedef boost::function<
        void(
                runjob::commands::error::rc,
                const std::string& message
            )
        > Callback;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Job>& job,                  //!< [in]
            int number,                                         //!< [in]
            size_t timeout = defaults::ServerKillJobTimeout,    //!< [in]
            const Callback& callback = Callback()               //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Signal();

private:
    Signal(
            const boost::shared_ptr<Job>& job,
            const Callback& callback
          );

    void impl(
            int number,
            size_t timeout
            );

    bool startTimer( 
            int number,
            size_t timeout
            );

private:
    boost::shared_ptr<Job> _job;
    const Callback _callback;
    runjob::commands::error::rc _error;
    std::ostringstream _message;
};

} // job
} // server
} // runjob

#endif
