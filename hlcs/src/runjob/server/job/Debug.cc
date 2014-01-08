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
#include "server/job/Debug.h"

#include "server/job/tool/Start.h"

#include "server/job/RankMapping.h"
#include "server/job/Transition.h"

#include "server/mux/Connection.h"

#include "server/cios/Connection.h"

#include "server/block/Compute.h"
#include "server/block/IoNode.h"

#include "server/Job.h"

#include "common/message/Proctable.h"
#include "common/message/Result.h"

#include "common/error.h"
#include "common/logging.h"
#include "common/Shape.h"

#include "server/Job.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

void
Debug::create(
        const Job::Ptr& job,
        const runjob::message::StartTool::Ptr& request,
        const mux::Connection::Ptr& mux
        )
{
    LOG_INFO_MSG( __FUNCTION__ );
    const Ptr result(
            new Debug( job, request, mux )
            );

    job->strand().post(
            boost::bind(
                &Debug::impl,
                result
                )
            );
}

Debug::Debug(
        const Job::Ptr& job,
        const runjob::message::StartTool::Ptr& request,
        const mux::Connection::Ptr& mux
        ) :
    _job( job ),
    _request( request ),
    _response( new runjob::message::Proctable ),
    _mux( mux )
{
    const job::RankMapping mapping( job, _response->_proctable );
    LOG_TRACE_MSG( "proctable has " << _response->_proctable.size() << " entries" );
}

void
Debug::impl()
{
    const Job::Ptr job( _job.lock() );
    if ( !job ) return;

    LOGGING_DECLARE_JOB_MDC( job->id() );

    // only set status to Debug if we are not running
    switch ( job->status().get() ) {
        case Status::ClientStarting:
            job->status().set(
                    Status::Debug,
                    job
                    );
            break;
        default:
            break;
    }

    job::tool::Start::create( 
            job,
            _request->_description,
            _response->_proctable,
            job->strand().wrap(
                boost::bind(
                    &Debug::callback,
                    shared_from_this(),
                    _1,
                    _2,
                    _3
                    )
                )
            );
}

void
Debug::callback(
        const unsigned id,
        const runjob::commands::error::rc error,
        const std::string& message
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _request->getClientId() );
    LOG_TRACE_MSG( __FUNCTION__ );

    const Job::Ptr job( _job.lock() );
    if ( !job ) return;

    const mux::Connection::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    if ( error ) {
        if ( 
                _request->getClientId() == job->client() &&
                job->status().get() == Status::Debug 
           )
        {
            // if the tool does not start when bootstrapping the job, we do not
            // attempt to start the job
            job->setError(
                    message,
                    runjob::error_code::job_failed_to_start
                    );

            Transition( job ).end();
        } else {
            const message::Result::Ptr result( new message::Result );
            result->setClientId( _request->getClientId() );
            result->setJobId( job->id() );
            result->setError( error_code::tool_invalid );
            result->setMessage( message );
            LOG_WARN_MSG( message );
            mux->write( result );
        }
            
        return;
    }

    _response->setClientId( _request->getClientId() );
    _response->setJobId( job->id() );
    _response->_id = id;

    try {
        this->addIo( job );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    mux->write( _response );

    LOG_INFO_MSG( "tool " << id << " started" );
}

void
Debug::addIo(
        const Job::Ptr& job
        )
{
    // container to hold I/O node locations that were used to start the tool
    std::set<Uci> io;

    // iterate through the proctable looking for unique I/O nodes in use
    for ( unsigned i = 0; i < _response->_proctable.size(); ++i ) {
        const unsigned rank = _response->_proctable[i].rank();
        const bool participating = _request->_description.getSubset().match( rank );
        if ( !participating ) continue; 

        io.insert( _response->_proctable[i].io() );
    }

    // add job's I/O nodes
    BOOST_FOREACH( const job::IoNode::Map::value_type& i, job->io() ) {
        const Uci& location = i.first;
        const bool in_use = io.find( location ) != io.end();
        if ( !in_use ) continue;

        const job::IoNode& node = i.second;

        // get I/O node IP address
        const block::IoNode::Ptr& io = node.get();
        const cios::Connection::Ptr control = io->getControl();
        if ( !control ) {
            LOG_WARN_MSG( "lost connection to I/O node" );
            continue;
        }

        LOG_TRACE_MSG( location << " " << control->getEndpoint().address() );

        _response->_io.insert(
                std::make_pair( location, boost::lexical_cast<std::string>(control->getEndpoint().address()) )
                );
    }
}

} // job
} // server
} // runjob

