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
#include "server/job/EndOfFile.h"

#include "server/job/Signal.h"

#include "server/mux/Connection.h"

#include "server/Job.h"

#include "common/error.h"
#include "common/logging.h"

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

void
EndOfFile::create(
        const Job::Ptr& job,
        const Uci& location
        )
{
    const Ptr eof(
            new EndOfFile( 
                job
                )
            );

    // dispatch used instead of post since the handler can be invoked immediately
    // should we already be inside of the strand
    job->strand().dispatch(
            boost::bind(
                &EndOfFile::impl,
                eof,
                location
                )
            );
}

EndOfFile::EndOfFile(
        const Job::Ptr& job
        ) :
    _job( job )
{

}

void
EndOfFile::impl(
        const Uci& location
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->id() );
    LOGGING_DECLARE_LOCATION_MDC( location );

    const IoNode::Map::iterator node = _job->io().find( location );
    if ( node == _job->io().end() ) {
        LOG_TRACE_MSG( "does not use I/O node: " << location );
        return;
    }

    _job->setError(
            "Aborted due to end of file from I/O node " + boost::lexical_cast<std::string>(location),
            error_code::io_node_failure
            );

    // big hammer here, the job cannot continue since this I/O node is presumably dead 
    _job->io().erase( node );
    LOG_DEBUG_MSG( "removed I/O node" );

    if ( _job->io().empty() ) {
        _job->remove();
    } else {
        Signal::create( _job, SIGKILL );
    }
}

} // job
} // server
} // runjob
