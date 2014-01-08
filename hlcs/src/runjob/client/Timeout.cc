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
#include "client/Timeout.h"

#include "client/Job.h"

#include "common/logging.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

Timeout::Timeout(
        boost::asio::io_service& io_service,
        const boost::shared_ptr<Job>& job
        ) :
    _timer( io_service ),
    _job( job )
{

}

Timeout::~Timeout()
{
    LOG_DEBUG_MSG( __FUNCTION__ );
}

void
Timeout::start(
        const uint32_t value
        )
{
    if ( value == 0 ) {
        LOG_DEBUG_MSG( "no timeout provided" );
        return;
    }

    LOG_DEBUG_MSG( "after " << value << " seconds" );
    _timer.expires_from_now(
            boost::posix_time::seconds( value )
            );
    _timer.async_wait( 
            boost::bind(
                &Timeout::handler,
                shared_from_this(),
                value,
                _1
                )
            );
}

void
Timeout::stop()
{
    boost::system::error_code error;
    _timer.cancel( error );
    if ( error ) {
        LOG_WARN_MSG( "could not cancel: " << boost::system::system_error(error).what() );
    }
}

void
Timeout::handler(
        const uint32_t value,
        const boost::system::error_code& error
        )
{
    if ( error == boost::asio::error::operation_aborted ) {
        // asked to terminate
        return;
    }

    if ( error ) {
        LOG_ERROR_MSG( "could not wait: " << boost::system::system_error(error).what() );
        return;
    }

    const boost::shared_ptr<Job> job( _job.lock() );
    if ( !job ) return;

    LOG_FATAL_MSG( "timed out after " << value << " seconds" );
    job->kill();
}

} // client
} // runjob
