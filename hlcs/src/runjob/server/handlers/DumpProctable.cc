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
#include "server/handlers/DumpProctable.h"

#include "common/JobInfo.h"
#include "common/logging.h"
#include "common/SubBlock.h"

#include "server/cios/Connection.h"

#include "server/job/Container.h"
#include "server/job/IoNode.h"
#include "server/job/RankMapping.h"

#include "server/block/Compute.h"
#include "server/block/IoNode.h"

#include "server/CommandConnection.h"
#include "server/Job.h"
#include "server/Security.h"
#include "server/Server.h"

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {

DumpProctable::DumpProctable(
        const Server::Ptr& server
        ) :
    CommandHandler( server ),
    _request(),
    _response( boost::make_shared<runjob::commands::response::DumpProctable>() ),
    _connection(),
    _error( runjob::commands::error::unknown_failure ),
    _message()
{

}

DumpProctable::~DumpProctable()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( !_connection ) return;

    if ( _error ) {
        _response->setError( _error );
    }

    if ( !_message.str().empty() ) {
        _response->setMessage( _message.str() );
        LOG_WARN_MSG( _response->getMessage() );
    }

    _connection->write( _response );
}

void
DumpProctable::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    _request = boost::static_pointer_cast<runjob::commands::request::DumpProctable>( request );
    _connection = connection;

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // get job object
    server->getJobs()->find(
            _request->_job,
            boost::bind(
                &DumpProctable::findHandler,
                shared_from_this(),
                _1
                )
            );
}

void
DumpProctable::findHandler(
        const Job::Ptr& job
        )
{
    LOGGING_DECLARE_JOB_MDC( _request->_job );

    if ( !job ) {
        LOG_WARN_MSG( "could not find job" );
        _error = runjob::commands::error::job_not_found;
        return;
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( _connection->getUserType() == bgq::utility::portConfig::UserType::Administrator ) {
        // user presented administrative certificate, let them do anything
        //
        // fall through
        LOG_DEBUG_MSG( "skipping security check for administrator: " << _connection->getUser()->getUser() );
    } else if ( _connection->getUserType() == bgq::utility::portConfig::UserType::Normal ) {
        // dump proctable requires read authority
        const bool validate = server->getSecurity()->validate(
                _connection->getUser(),
                hlcs::security::Action::Read,
                _request->_job
                );

        if ( !validate ) {
            _error = runjob::commands::error::permission_denied;
            return;
        }
    } else {
        BOOST_ASSERT( !"unhandled user type" );
    }
   
    this->validateRequest( job );
}

void
DumpProctable::validateRequest(
        const Job::Ptr& job
        )
{
    BOOST_ASSERT( job );

    if ( _request->_ranks.empty() ) {
        this->addBoilerPlate(  job );
        return;
    }

    // validate ranks
    const JobInfo& info = job->info();
    const SubBlock& sub_block = info.getSubBlock();
    const block::Compute::Ptr& block = job->block();

    // use --np if provided
    unsigned max_rank = info.getNp();

    if ( !max_rank && sub_block.isValid() ) {
        max_rank = sub_block.shape().size() * info.getRanksPerNode();
    } else if ( !max_rank ) {
        max_rank = block->size() * info.getRanksPerNode();
    }
    LOG_TRACE_MSG( "maximum rank: " << max_rank );

    BOOST_FOREACH( unsigned rank, _request->_ranks ) {
        if ( rank >= max_rank ) {
            _error = runjob::commands::error::rank_invalid;
            _message <<
                    "rank " <<
                    boost::lexical_cast<std::string>( rank ) <<
                    " is greater than job --np value " <<
                    boost::lexical_cast<std::string>( max_rank )
                    ;
            return;
        } else {
            LOG_TRACE_MSG( "rank " << rank );
        }
    }

    this->addBoilerPlate( job );
}

void
DumpProctable::addBoilerPlate(
        const Job::Ptr& job
        )
{
    BOOST_ASSERT( job );

    const JobInfo& info = job->info();

    _response->_block = job->block()->name();
    _response->_ranksPerNode = info.getRanksPerNode();
    _response->_mapping = info.getMapping();

    // use --np if provided, otherwise it will be job size
    if ( info.getNp() ) {
        _response->_np = info.getNp();
    }

    const SubBlock& sub_block = info.getSubBlock();
    if ( sub_block.isValid() ) {
        // set corner and shape
        const Shape& shape = sub_block.shape();
        _response->_corner = boost::lexical_cast<std::string>( sub_block.corner() );
        _response->_shape = shape;

        // set --np if not provided
        if ( !_response->_np ) {
            _response->_np = shape.a() * shape.b() * shape.c() * shape.d() * shape.e() * _response->_ranksPerNode;
        }
    } else if ( !_response->_np ) {
        // full block without --np
        _response->_np = job->block()->size() * _response->_ranksPerNode;
    }

    this->addIo( job );
}

void
DumpProctable::addIo(
        const Job::Ptr& job
        )
{
    BOOST_FOREACH( const job::IoNode::Map::value_type& i, job->io() ) {
        const Uci& location = i.first;
        const job::IoNode& node = i.second;

        // get I/O node IP address
        const block::IoNode::Ptr& io = node.get();
        const cios::Connection::Ptr control = io->getControl();
        if ( !control ) {
            LOG_WARN_MSG( "lost connection to I/O node" );
            continue;
        }

        LOG_TRACE_MSG( location << " " << control->getEndpoint().address() << " " << std::setfill('0') << std::hex << location );

        _response->_io.insert(
                std::make_pair( location, boost::lexical_cast<std::string>(control->getEndpoint().address()) )
                );
    }

    this->addProctable( job );
}

void
DumpProctable::addProctable(
        const Job::Ptr& job
        )
{
    try {
        const job::RankMapping mapping( job, _response->_proctable );
        _error = runjob::commands::error::success;
    } catch ( const std::exception & e ) {
        LOG_WARN_MSG( e.what() );
        _message << 
            "Could not generate mapping from " <<
            job->info().getMapping()
            ;
    }
}

} // handlers
} // server
} // runjob
