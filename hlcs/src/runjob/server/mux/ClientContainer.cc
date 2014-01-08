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
#include "server/mux/ClientContainer.h"

#include "server/job/Signal.h"

#include "server/Job.h"

#include "common/logging.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace mux {

ClientContainer::Ptr
ClientContainer::create(
        boost::asio::io_service& io_service
        )
{
    const Ptr result(
            new ClientContainer( io_service )
            );
    
    return result;
}

ClientContainer::ClientContainer(
        boost::asio::io_service& io_service
        ) :
    _strand( io_service )
{

}

void
ClientContainer::add(
        uint64_t client,
        const Callback& callback
        )
{
    _strand.dispatch(
            boost::bind(
                &ClientContainer::addImpl,
                shared_from_this(),
                client,
                callback
                )
            );
}

void
ClientContainer::addImpl(
        uint64_t client,
        const Callback& callback
        )
{
    const bool result = _clients.insert(
            Container::value_type( client, boost::weak_ptr<Job>() )
            ).second;
    if ( result ) {
        LOG_TRACE_MSG( "added client " << client );
    }

    callback( result );
}

void
ClientContainer::update(
        uint64_t client,
        const Job::Ptr& job,
        const Callback& callback
        )
{
    _strand.dispatch(
            boost::bind(
                &ClientContainer::updateImpl,
                shared_from_this(),
                client,
                job,
                callback
                )
            );
}

void
ClientContainer::updateImpl(
        uint64_t client,
        const Job::Ptr& job,
        const Callback& callback
        )
{
    BOOST_ASSERT( job );
    LOGGING_DECLARE_BLOCK_MDC( job->info().getBlock() );
    LOGGING_DECLARE_JOB_MDC( job->client() );

    const Container::iterator result = _clients.find( client );
    if ( result != _clients.end() ) {
        result->second = job;
        LOG_TRACE_MSG( "assigned job ID " << job->id() );
    }

    callback( result != _clients.end() );
}

void
ClientContainer::kill(
        const boost::shared_ptr<message::KillJob>& message,
        const Callback& callback
        )
{
    _strand.dispatch(
            boost::bind(
                &ClientContainer::killImpl,
                shared_from_this(),
                message,
                callback
                )
            );
}

void
ClientContainer::killImpl(
        const boost::shared_ptr<message::KillJob>& message,
        const Callback& callback
        )
{
    const Container::iterator result = _clients.find( message->getClientId() );
    if ( result != _clients.end() ) {
        if ( Job::Ptr job = result->second.lock() ) {
            job->strand().post(
                    boost::bind(
                        &ClientContainer::signalImpl,
                        shared_from_this(),
                        job,
                        message
                        )
                    );
        } else {
            _clients.erase( result );
            LOG_TRACE_MSG( "removed client " << message->getClientId() );
        }
    }

    callback( result != _clients.end() );
}


void
ClientContainer::remove(
        uint64_t client
        )
{
    _strand.dispatch(
            boost::bind(
                &ClientContainer::removeImpl,
                shared_from_this(),
                client
                )
            );
}

void
ClientContainer::removeImpl(
        uint64_t client
        )
{
    const Container::iterator result = _clients.find( client );
    if ( result != _clients.end() ) {
        _clients.erase( result );
        LOG_TRACE_MSG( "removed client " << client );
    }
}

void
ClientContainer::eof()
{
    _strand.dispatch(
            boost::bind(
                &ClientContainer::eofImpl,
                this
                )
            );
}

void
ClientContainer::eofImpl()
{
    // deliver signals to jobs that have we know about
    BOOST_FOREACH( const Container::value_type& i, _clients ) {
        if ( const Job::Ptr job = i.second.lock() ) {
            job->queue().clientDisconnected();
            job::Signal::create( job, SIGKILL );
        }
    }

    // for clients that aren't associated with a Job object yet,
    // removing them from this container will cause them to abort
    // prematurely
    _clients.clear();
}

void
ClientContainer::signalImpl(
        const Job::Ptr& job,
        const boost::shared_ptr<message::KillJob>& message
        )
{
    LOGGING_DECLARE_BLOCK_MDC( job->info().getBlock() );
    LOGGING_DECLARE_JOB_MDC( job->id() );

    if ( message->_clientDisconnected ) {
        job->queue().clientDisconnected();
    }

    // deliver signal
    job::Signal::create( job, message->_signal ? message->_signal : SIGKILL );
}

} // mux
} // server
} // runjob
