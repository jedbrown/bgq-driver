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
#include "server/performance/Counters.h"

#include "common/message/PerfCounters.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include "server/Options.h"
#include "server/Server.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace performance {

Counters::Ptr
Counters::create(
        const Server::Ptr& server
        )
{
    const Counters::Ptr result(
            new Counters( server )
            );
    
    // get interval and start waiting
    const unsigned interval( result->_interval.get() );

    LOG_TRACE_MSG( "using interval of " << interval << " seconds" );
    result->_timer.expires_from_now( boost::posix_time::seconds(interval) );
    result->_timer.async_wait(
            boost::bind(
                &Counters::timerHandler,
                result,
                boost::asio::placeholders::error
                )
            );

    return result;
}

Counters::Counters(
        const Server::Ptr& server
        ) :
    _options( server->getOptions() ),
    _timer( server->getIoService() ),
    _jobs( new JobContainer("runjob") ),
    _misc( new MiscContainer("runjob") ),
    _interval( server->getOptions() )
{

}

Counters::~Counters()
{
    LOG_TRACE_MSG( "terminating" );
}

void
Counters::timerHandler(
        const boost::system::error_code& error
        )
{
    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, we were asked to abort
        return;
    }

    if ( error ) {
        LOG_WARN_MSG( "timer failed: " << boost::system::system_error(error).what() );
        return;
    }

    if ( _jobs->getCount() > 0 ) {
        LOG_TRACE_MSG( _jobs->getCount() << " job counters" );
        try {
            MiscContainer::Timer::Ptr timer = _misc->create()
                ->function( "performance counters" )
                ->subFunction( "output" )
                ->id( "jobs" )
                ->otherData( _jobs->getCount() )
                ;
            _jobs->output();
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( e.what() );
        }
    }

    if ( _misc->getCount() > 0 ) {
        LOG_TRACE_MSG( _misc->getCount() << " miscellaneous counters" );
        try {
            _misc->output();
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( e.what() );
        }
    }

    // reset timer and start waiting
    const unsigned interval( _interval.get() );
    LOG_TRACE_MSG( "using interval of " << interval << " seconds" );
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
} // server
} // runjob
