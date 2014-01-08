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
#ifndef RUNJOB_CLIENT_SIGNAL_HANDLER_H
#define RUNJOB_CLIENT_SIGNAL_HANDLER_H

#include "client/bgsched/ClientImpl.h"

#include <utility/include/SignalHandler.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace client {

class Job;

/*!
 * \brief Handle signals delivered to runjob by killing the job.
 */
class SignalHandler : public boost::enable_shared_from_this<SignalHandler>
{
public:
    /*!
     * \brief Type of signals to watch.
     */
    typedef bgq::utility::SignalHandler<SIGINT,SIGTERM,SIGXCPU> Signals;

    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<SignalHandler> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            bgsched::runjob::Client::Impl& runjob //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~SignalHandler();

private:
    SignalHandler(
            bgsched::runjob::Client::Impl& runjob
            );

    void handle(
            const boost::system::error_code& error,
            const siginfo_t& siginfo
            );

    void start();

private:
    Signals _signals;
    bgsched::runjob::Client::Impl& _runjob;
};

} // client
} // runjob

#endif
