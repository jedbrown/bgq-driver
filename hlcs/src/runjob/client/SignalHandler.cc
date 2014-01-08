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
#include "client/SignalHandler.h"

#include "client/Job.h"

#include "common/logging.h"
#include "common/LogSignalInfo.h"

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

SignalHandler::Ptr
SignalHandler::create(
        bgsched::runjob::Client::Impl& runjob
        )
{
    const Ptr result(
            new SignalHandler( runjob )
            );

    result->start();

    return result;
}

SignalHandler::SignalHandler(
        bgsched::runjob::Client::Impl& runjob
        ) :
    _signals( runjob.ioService() ),
    _runjob( runjob )
{
    // ignore SIGPIPE
    struct sigaction sa;
    std::memset( &sa, 0, sizeof(sa) );
    sa.sa_handler = SIG_IGN;
    if ( sigaction( SIGPIPE, &sa, NULL ) != 0 ) {
        LOG_WARN_MSG( "could not ignore SIGPIPE" );
    }
}

SignalHandler::~SignalHandler()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
SignalHandler::start()
{
    LOG_DEBUG_MSG( "waiting for signals: TERM, INT, and XCPU" );
    _signals.async_wait(
            boost::bind(
                &SignalHandler::handle,
                shared_from_this(),
                _1,
                _2
                )
            );
}

void
SignalHandler::handle(
        const boost::system::error_code& error,
        const siginfo_t& siginfo
        )
{
    if ( error == boost::asio::error::operation_aborted ) return;

    if ( error ) {
        LOG_ERROR_MSG( "signal handler: " << boost::system::system_error(error).what() );
        return;
    }

    LogSignalInfo info( siginfo );

    const boost::shared_ptr<Job> job( _runjob.job().lock() );
    if ( !job ) return;
    job->kill();

    // wait for another signal
    _signals.async_wait(
            boost::bind(
                &SignalHandler::handle,
                shared_from_this(),
                _1,
                _2
                )
            );
}

} // client
} // runjob
