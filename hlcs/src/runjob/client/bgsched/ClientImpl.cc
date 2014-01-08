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

#include "client/bgsched/ClientImpl.h"
#include "client/options/Parser.h"
#include "client/Job.h"

#include "common/logging.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( runjob::client::log );

namespace bgsched {
namespace runjob {

Client::Impl::Impl(
        const int argc,
        char** const argv
        ) :
    _io_service(),
    _options( boost::make_shared< ::runjob::client::options::Parser>(argc, argv) ),
    _job( ),
    _started( false )
{
    LOG_TRACE_MSG( __FUNCTION__ ); 
}

Client::Impl::~Impl()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

bgq::utility::ExitStatus
Client::Impl::start(
            const int input,
            const int output,
            const int error
        )
{
    // second byte is what WEXITSTATUS looks at, so we want it to be 1
    bgq::utility::ExitStatus result( 0x100 );
    
    // ensure we setup exit status correctly
    BOOST_ASSERT( result.exited() );
    BOOST_ASSERT( result.getExitStatus() == 1 );

    {
        const boost::shared_ptr< ::runjob::client::Job> job(
                new ::runjob::client::Job(_io_service, _options, result)
                );
        _job = job;
        _started = true;
        job->start( input, output, error );
    }

    _io_service.run();

    return result;
}

void
Client::Impl::kill(
        const int signal
        )
{
    if ( !_started ) {
        BOOST_THROW_EXCEPTION(
                std::logic_error( "job has not been started" )
                );
    }

    const boost::shared_ptr< ::runjob::client::Job> job( _job.lock() );
    if ( !job ) {
        BOOST_THROW_EXCEPTION(
                std::logic_error( "job already terminated" )
                );
    }

    _io_service.post(
            boost::bind(
                &::runjob::client::Job::kill,
                job,
                signal
                )
            );
}

} // runjob
} // bgsched
