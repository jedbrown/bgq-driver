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
#include "mux/performance/Counters.h"

#include "common/message/PerfCounters.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include "mux/server/Connection.h"

#include "mux/Options.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace performance {

Counters::Counters(
        boost::asio::io_service& io_service,
        const Options& options,
        const boost::shared_ptr<server::Connection>& server
        ) :
    _options( options ),
    _timer( io_service ),
    _container( new Container("runjob_mux") ),
    _server( server ),
    _interval( options )
{
    _container->setConnection( _server );
}

Counters::~Counters()
{
    LOG_TRACE_MSG( "terminating" );
}

void
Counters::start()
{
    // get interval
    unsigned interval( _interval.get() );
    LOG_TRACE_MSG( "using interval of " << interval << " seconds" );

    // start timer
    _timer.expires_from_now( boost::posix_time::seconds(interval) );
    _timer.async_wait(
            boost::bind(
                &Counters::timerHandler,
                shared_from_this(),
                boost::asio::placeholders::error
                )
            );
}

void
Counters::timerHandler(
        const boost::system::error_code& error
        )
{
    if ( error == boost::asio::error::operation_aborted ) return;
    if ( error ) {
        LOG_WARN_MSG( "timer failed: " << boost::system::system_error(error).what() );
        return;
    }

    // output statistics
    _container->output();

    // get interval
    unsigned interval( _interval.get() );
    LOG_TRACE_MSG( "using interval of " << interval << " seconds" );

    // start timer
    _timer.expires_from_now( boost::posix_time::seconds(interval) );
    _timer.async_wait(
            boost::bind(
                &Counters::timerHandler,
                shared_from_this(),
                boost::asio::placeholders::error
                )
            );
}

} // performance
} // mux
} // runjob
