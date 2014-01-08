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

#include "common/logging.h"

#include "server/realtime/Polling.h"
#include "server/block/Container.h"
#include "server/Block.h"
#include "server/Server.h"

#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTBlock_history.h>
#include <db/include/api/tableapi/DBConnectionPool.h>

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace realtime {

const unsigned int Polling::DefaultInterval = 5;

Polling::Ptr
Polling::create(
        const Server::Ptr& server,
        const bgsched::SequenceId sequence
        )
{
    const Ptr result(
            new Polling( server, sequence )
            );

    result->wait();
    return result;
}

Polling::~Polling()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    if ( _callback ) {
        _callback( _sequence );
    }
}

Polling::Polling(
        const Server::Ptr& server,
        const bgsched::SequenceId sequence
        ) :
    _server( server ),
    _timer( server->getIoService() ),
    _stopped( false ),
    _sequence( sequence ),
    _callback( )
{
    LOGGING_DECLARE_LOCATION_MDC( _sequence );
    LOG_INFO_MSG( "starting " << std::hex << this );
}

cxxdb::QueryStatementPtr
Polling::prepareSequenceQuery(
        const cxxdb::ConnectionPtr& database
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    return database->prepareQuery(
            std::string("SELECT ") +
            BGQDB::DBTBlock::BLOCKID_COL + "," +
            BGQDB::DBTBlock::STATUS_COL + "," +
            BGQDB::DBTBlock::SEQID_COL + ", " +
            BGQDB::DBTBlock::STATUSLASTMODIFIED_COL + " " +
            "FROM " +
            BGQDB::DBTBlock().getTableName() + " " +
            "WHERE " +
            BGQDB::DBTBlock::SEQID_COL + " > ?"
            "ORDER BY " + BGQDB::DBTBlock::SEQID_COL,
            { BGQDB::DBTBlock::SEQID_COL }
            );
}

cxxdb::QueryStatementPtr
Polling::prepareDeletionQuery(
        const cxxdb::ConnectionPtr& database
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    return database->prepareQuery(
            std::string("SELECT ") +
            BGQDB::DBTBlock_history::BLOCKID_COL + "," +
            BGQDB::DBTBlock_history::STATUSLASTMODIFIED_COL + " " +
            "FROM " +
            BGQDB::DBTBlock_history().getTableName() + " " +
            "WHERE " +
            BGQDB::DBTBlock::CREATIONID_COL + " = ?",
            { BGQDB::DBTBlock_history::CREATIONID_COL }
            );
}

void
Polling::stop(
        const Callback& callback
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( _stopped ) return;

    _callback = callback;
    _stopped = true;
    boost::system::error_code error;
    _timer.cancel( error );
    if ( error ) {
        LOG_WARN_MSG( "could not cancel: " << boost::system::system_error(error).what() );
    }
}

void
Polling::wait()
{
    _timer.expires_from_now( boost::posix_time::seconds(DefaultInterval) );
    _timer.async_wait(
            boost::bind(
                &Polling::impl,
                shared_from_this(),
                boost::asio::placeholders::error
                )
            );
}

void
Polling::impl(
        const boost::system::error_code& error
        )
{
    LOG_TRACE_MSG( __FUNCTION__ << " " << _sequence );
    if ( error ) {
        if ( error == boost::asio::error::operation_aborted || _stopped ) {
            // asked to terminate, fall through
        } else {
            LOG_WARN_MSG( "could not wait: " << boost::system::system_error(error).what() );
            return;
        }
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    try {
        const cxxdb::ConnectionPtr database(
                BGQDB::DBConnectionPool::instance().getConnection()
                );
        if ( !database ) {
            BOOST_THROW_EXCEPTION(
                    std::runtime_error( "could not get database connection" )
                    );
        }

        const cxxdb::QueryStatementPtr statement( this->prepareSequenceQuery(database) );

        statement->parameters()[ BGQDB::DBTBlock::SEQID_COL ].set( static_cast<int64_t>(_sequence) );
        const cxxdb::ResultSetPtr results( statement->execute() );

        while ( results->fetch() ) {
            const cxxdb::Columns& columns( results->columns() );
            const std::string id = columns[ BGQDB::DBTBlock::BLOCKID_COL ].getString();

            LOGGING_DECLARE_BLOCK_MDC( id );
            LOGGING_DECLARE_LOCATION_MDC( columns[ BGQDB::DBTBlock::SEQID_COL ].getInt64() );
            const std::string status = columns[ BGQDB::DBTBlock::STATUS_COL ].getString();
            LOG_DEBUG_MSG(
                    "status=" << status << " at " <<
                    columns[ BGQDB::DBTBlock::STATUSLASTMODIFIED_COL ].getTimestamp()
                    );

            if ( status == BGQDB::BLOCK_BOOTING || status == BGQDB::BLOCK_INITIALIZED ) {
                server->getBlocks()->create( 
                        id,
                        boost::shared_ptr<BGQMachineXML>(),
                        boost::bind(
                            &Polling::createCallback,
                            shared_from_this(),
                            id,
                            status,
                            _1,
                            _2
                            )
                        );
            } else if ( status == BGQDB::BLOCK_TERMINATING || status == BGQDB::BLOCK_FREE ) {
                server->getBlocks()->remove( 
                        id,
                        boost::bind(
                            &Polling::removeCallback,
                            shared_from_this(),
                            id,
                            _1,
                            _2
                            )
                        );
            }

            // remember sequence ID
            _sequence = columns[ BGQDB::DBTBlock::SEQID_COL ].getInt64();
        }

        if ( _stopped ) {
            // fall through
        } else {
            server->getBlocks()->get(
                    boost::bind(
                        &Polling::getBlocksCallback,
                        shared_from_this(),
                        _1,
                        database
                        )
                    );
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "could not poll: " << e.what() );
        if ( _stopped ) {
            // fall through
        } else {
            this->wait();
        }
    }
}

void
Polling::getBlocksCallback(
        const block::Container::Blocks& blocks,
        const cxxdb::ConnectionPtr& database
        )
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    const cxxdb::QueryStatementPtr statement( this->prepareDeletionQuery(database) );

    try {
        LOG_DEBUG_MSG( "querying for " << blocks.size() << " deleted blocks" );
        BOOST_FOREACH( const Block::Ptr& block, blocks ) {
            statement->parameters()[ BGQDB::DBTBlock_history::CREATIONID_COL ].cast(
                    block->config()->blockId()
                    );
            const cxxdb::ResultSetPtr results( statement->execute() );
            if ( results->fetch() ) {
                const std::string id = results->columns()[ BGQDB::DBTBlock_history::BLOCKID_COL ].getString();
                LOG_DEBUG_MSG( "removing deleted block " << id );
                server->getBlocks()->remove( 
                        id,
                        boost::bind(
                            &Polling::removeCallback,
                            shared_from_this(),
                            id,
                            _1,
                            _2
                            )
                        );
            }
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    this->wait();
}

void
Polling::createCallback(
        const std::string& block,
        const std::string& status,
        const error_code::rc error,
        const std::string& message
        )
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    LOGGING_DECLARE_BLOCK_MDC( block );

    if ( error ) {
        LOG_DEBUG_MSG( message );
    }

    if ( status != BGQDB::BLOCK_INITIALIZED ) return;

    server->getBlocks()->initialized(
            block,
            boost::bind(
                &Polling::initializedCallback,
                shared_from_this(),
                block,
                _1,
                _2
                )
            );
}

void
Polling::removeCallback(
        const std::string& block,
        const error_code::rc error,
        const std::string& message
        ) const
{
    LOGGING_DECLARE_BLOCK_MDC( block );

    if ( error ) {
        LOG_INFO_MSG( "Remove: " << message );
        return;
    }
}

void
Polling::initializedCallback(
        const std::string& block,
        const error_code::rc error,
        const std::string& message
        ) const
{
    LOGGING_DECLARE_BLOCK_MDC( block );

    if ( error ) {
        LOG_INFO_MSG( "Initialized: " << message );
        return;
    }
}

} // realtime
} // server
} // runjob
