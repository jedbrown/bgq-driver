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
#include "server/Shutdown.h"

#include "server/block/Io.h"
#include "server/block/IoNode.h"

#include "server/Job.h"

#include "common/logging.h"

#include "server/Server.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {

void
Shutdown::create(
        const Server::Ptr& server
        )
{
    server->_stopped = true;

    const boost::shared_ptr<Shutdown> result(
            new Shutdown( server )
            );

    // get blocks
    server->getBlocks()->get(
            boost::bind(
                &Shutdown::getBlocksHandler,
                result,
                _1
                )
            );
}

Shutdown::Shutdown(
        const Server::Ptr& server
        ) :
    _server( server ),
    _links(),
    _currentLink( ),
    _jobs(),
    _currentJob( _jobs.begin() )
{

}

void
Shutdown::getBlocksHandler(
        const block::Container::Blocks& blocks
        )
{
    LOG_TRACE_MSG( 
            "got " << blocks.size() << 
            " block" << ( blocks.size() != 1 ? "s" : "" )
            );

    // iterate through blocks looking for I/O links
    BOOST_FOREACH( const block::Container::Blocks::value_type& i, blocks ) {
        if ( const block::Io::Ptr io = boost::dynamic_pointer_cast<block::Io>(i) ) {
            const block::Io::Nodes& nodes = io->getNodes();
            BOOST_FOREACH( const block::Io::Nodes::value_type& j, nodes ) {
                _links.push_back( j->getControl() );
                _links.push_back( j->getData() );
            }
        }
    }

    LOG_TRACE_MSG( 
            _links.size() << " I/O " <<
            "link" << ( _links.size() != 1 ? "s" : "" )
            );

    if ( _links.empty() ) {
        const Server::Ptr server( _server.lock() );
        if ( !server ) return;

        server->getJobs()->get(
                boost::bind(
                    &Shutdown::getJobsHandler,
                    shared_from_this(),
                    _1
                    )
                );

        return;
    }

    _currentLink = _links.begin();
    while ( !*_currentLink ) {
        if ( _currentLink == _links.end() ) break;
        ++_currentLink;
    }

    if ( _currentLink == _links.end() ) {
        const Server::Ptr server( _server.lock() );
        if ( !server ) return;

        server->getJobs()->get(
                boost::bind(
                    &Shutdown::getJobsHandler,
                    shared_from_this(),
                    _1
                    )
                );
        return;
    }

    (*_currentLink)->stop(
            boost::bind(
                &Shutdown::stopLinkHandler,
                shared_from_this()
                )
            );
}

void
Shutdown::stopLinkHandler()
{
    const size_t i = std::distance( _links.begin(), _currentLink );
    LOG_TRACE_MSG( "stopped I/O link " << i + 1 << " of " << _links.size() );

    while ( ++_currentLink != _links.end() ) {
        if ( *_currentLink ) break;
    }

    if ( _currentLink == _links.end() ) {
        const Server::Ptr server( _server.lock() );
        if ( !server ) return;

        server->getJobs()->get(
                boost::bind(
                    &Shutdown::getJobsHandler,
                    shared_from_this(),
                    _1
                    )
                );

        return;
    }

    const cios::Connection::Ptr& ioLink = *_currentLink;
    ioLink->stop(
            boost::bind(
                &Shutdown::stopLinkHandler,
                shared_from_this()
                )
            );
}

void
Shutdown::getJobsHandler(
        const job::Container::Jobs& jobs
        )
{
    LOG_TRACE_MSG( 
            "got " << jobs.size() <<
            " job" << ( jobs.size() != 1 ? "s" : "" )
            );

    if ( jobs.empty() ) {
        this->stopIoService();
        return;
    }

    _jobs = jobs;
    _currentJob = _jobs.begin();

    const Job::Ptr& job = _currentJob->second;
    job->strand().dispatch(
            boost::bind(
                &Shutdown::removeJobHandler,
                shared_from_this()
                )
            );
}

void
Shutdown::removeJobHandler()
{
    const size_t i = std::distance( _jobs.begin(), _currentJob );
    LOG_TRACE_MSG( "stopped job " << i + 1 << " of " << _jobs.size() );

    Job::Ptr& job = _currentJob->second;
    
    LOGGING_DECLARE_JOB_MDC( job->id() );
    LOGGING_DECLARE_BLOCK_MDC( job->info().getBlock() );

    job->setError(
            "aborted due to runjob_server shutdown",
            runjob::error_code::runjob_server_shutdown
            );

    if ( ++_currentJob == _jobs.end() ) {
        this->stopIoService();
        return;
    }

    job = _currentJob->second;
    job->strand().dispatch(
            boost::bind(
                &Shutdown::removeJobHandler,
                shared_from_this()
                )
            );
}

void
Shutdown::stopIoService()
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // cancel I/O service
    LOG_WARN_MSG( "stopping I/O service" );
    server->getIoService().stop();
}

} // server
} // runjob


