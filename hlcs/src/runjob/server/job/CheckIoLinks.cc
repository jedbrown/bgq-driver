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
#include "server/job/CheckIoLinks.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/JobInfo.h"

#include "server/block/IoNode.h"

#include "server/job/Setup.h"

#include "server/mux/ClientContainer.h"
#include "server/mux/Connection.h"

#include "server/Job.h"

#include <utility/include/ScopeGuard.h>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

void
CheckIoLinks::create(
        const Job::Ptr& job
        )
{
    const Ptr result(
            new CheckIoLinks( job )
            );
    
    BOOST_FOREACH( const IoNode::Map::value_type& i, job->io() ) {
        const auto location = i.first;
        const block::IoNode::Ptr& node = i.second.get();
        const cios::Connection::Ptr& control = node->getControl();
        result->_links.insert(
                IoLinks::value_type( location, control )
                );
        const cios::Connection::Ptr& data = node->getData();
        result->_links.insert(
                IoLinks::value_type( location, data )
                );
    }
  
    // remove job if any checks below fail
    bgq::utility::ScopeGuard guard(
            boost::bind(
                &Job::remove,
                job
                )
            );

    if ( result->_links.empty() ) {
        job->setError(
                "no connected I/O nodes",
                error_code::job_failed_to_start
                );
        return;
    }

    result->_link = result->_links.begin();
    const auto location = result->_link->first;
    const cios::Connection::Ptr& ioLink = result->_link->second;
    if ( !ioLink ) {
        job->setError(
                std::string("I/O node ") + boost::lexical_cast<std::string>(location) + " is not connected",
                error_code::job_failed_to_start
                );
        return;
    }
   
    guard.dismiss();

    ioLink->status(
            boost::bind(
                &CheckIoLinks::execute,
                result,
                _1
                )
            );
}

CheckIoLinks::CheckIoLinks(
        const Job::Ptr& job
        ) :
    _job( job ),
    _links(),
    _link( _links.begin() )
{
    LOG_TRACE_MSG( "starting" );
}

void
CheckIoLinks::execute( 
        const cios::Connection::SocketPtr& socket
        )
{
    // use post instead of dispatch to start a new callstack
    _job->strand().post(
            boost::bind(
                &CheckIoLinks::executeImpl,
                shared_from_this(),
                socket
                )
            );
}

void
CheckIoLinks::executeImpl(
        const cios::Connection::SocketPtr& socket
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->id() );
    LOGGING_DECLARE_BLOCK_MDC( _job->info().getBlock() );

    if ( !socket ) {
        _job->setError(
                std::string("I/O node ") + boost::lexical_cast<std::string>(_link->first) + " is not connected",
                error_code::job_failed_to_start
                );

        _job->remove();

        return;
    }
        
    LOG_TRACE_MSG( "link " << _link->first << " is connected" );

    const mux::Connection::Ptr mux( _job->mux().lock() );
    if ( !mux ) return;

    if ( ++_link == _links.end() ) {
        mux->clients()->update(
                _job->client(),
                _job,
                boost::bind(
                    &CheckIoLinks::updateClient,
                    shared_from_this(),
                    _1
                    )
                );

        return;
    }

    const cios::Connection::Ptr& ioLink = _link->second;

    if ( !ioLink ) {
        _job->setError(
                std::string("I/O node ") + boost::lexical_cast<std::string>(_link->first) + " is not connected",
                error_code::job_failed_to_start
                );

        _job->remove();

        return;
    }

    ioLink->status(
            boost::bind(
                &CheckIoLinks::execute,
                shared_from_this(),
                _1
                )
            );
}

void
CheckIoLinks::updateClient(
        const bool result
        )
{
    _job->strand().post(
            boost::bind(
                &CheckIoLinks::updateClientImpl,
                shared_from_this(),
                result
                )
            );
}

void
CheckIoLinks::updateClientImpl(
        const bool result
        )
{
    if ( !result ) {
        const JobInfo& info = _job->info();
        _job->setError(
                "client " + boost::lexical_cast<std::string>( _job->client() ) +
                " with pid " + boost::lexical_cast<std::string>( info.getPid() ) + 
                " on " + info.getHostname() + " disconnected prematurely",
                error_code::job_failed_to_start
                );
        _job->remove();

        return;
    }

    Setup::create( _job );
}

} // job
} // server
} // runjob

