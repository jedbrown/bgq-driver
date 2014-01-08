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
#include "server/block/Reconnect.h"

#include "server/block/Compute.h"
#include "server/block/ComputeNode.h"
#include "server/block/Container.h"
#include "server/block/Midplane.h"

#include "server/job/Reconnect.h"

#include "server/realtime/Connection.h"

#include "common/logging.h"

#include "server/Options.h"
#include "server/Server.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTBpblockmap.h>
#include <db/include/api/tableapi/gensrc/DBTNode.h>
#include <db/include/api/tableapi/gensrc/DBTSmallblock.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <boost/assign/list_of.hpp>

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace block {

void
Reconnect::create(
        const Server::Ptr& server
        )
{
    try {
        const Reconnect::Ptr result(
                new Reconnect( server )
                );

        if ( server->getOptions().reconnect().scope() == runjob::server::Reconnect::Scope::None ) {
            LOG_INFO_MSG( "skipping" );
            return;
        }

        result->nextBlock();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

Reconnect::~Reconnect()
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    job::Reconnect::create( server );

    try {
        if ( !_sequence ) {
            // special case when no blocks are reconnected, we don't want to poll for
            // every block that has changed since sequence ID zero so we find the 
            // maximum
            const cxxdb::ResultSetPtr results(
                    _connection->query(
                        "SELECT max(" +
                        BGQDB::DBTBlock::SEQID_COL + ") AS " +
                        BGQDB::DBTBlock::SEQID_COL + " FROM " +
                        BGQDB::DBTBlock().getTableName()
                        )
                    );
            if ( results->fetch() ) {
                _sequence = results->columns()[ BGQDB::DBTBlock::SEQID_COL ].getInt64();
            }
        }
    } catch ( const std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
    }

    server->getRealtimeConnection()->start( _sequence );
}

Reconnect::Reconnect(
        const Server::Ptr& server
        ) :
    _server( server ),
    _connection( BGQDB::DBConnectionPool::instance().getConnection() ),
    _results(),
    _sequence( 0 )
{
    if ( !_connection ) {
        LOG_WARN_MSG( "could not get database connection" );
        return;
    }

    // order I/O blocks first so we reconnect to them before compute blocks
    _results = _connection->query(
            "SELECT "
            + BGQDB::DBTBlock::BLOCKID_COL + "," + BGQDB::DBTBlock::STATUS_COL + ","
            + BGQDB::DBTBlock::NUMCNODES_COL + "," + BGQDB::DBTBlock::NUMIONODES_COL + ","
            + BGQDB::DBTBlock::SEQID_COL + "," + BGQDB::DBTBlock::QUALIFIER_COL +
            " FROM " + BGQDB::DBTBlock().getTableName() +
            " WHERE " + BGQDB::DBTBlock::STATUS_COL + " in ('" + BGQDB::BLOCK_INITIALIZED + "','" + BGQDB::BLOCK_BOOTING + "')"
            " ORDER BY " + BGQDB::DBTBlock::NUMIONODES_COL + " DESC"
            );
}

void
Reconnect::nextBlock()
{
    if ( !_results->fetch() ) {
        LOG_INFO_MSG( "done" );
        return;
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    const cxxdb::Columns& columns = _results->columns();
    const std::string id =  columns[ BGQDB::DBTBlock::BLOCKID_COL ].getString();
    LOGGING_DECLARE_BLOCK_MDC( id );

    LOG_DEBUG_MSG( 
            " (" << columns[ BGQDB::DBTBlock::STATUS_COL ].getChar() << ")" <<
            " (" << columns[ BGQDB::DBTBlock::NUMCNODES_COL ].as<int32_t>() << "c)" <<
            " (" << columns[ BGQDB::DBTBlock::NUMIONODES_COL ].as<int32_t>() << "i)"
            " sequence " << columns[ BGQDB::DBTBlock::SEQID_COL ].getInt64() <<
            " qualifier " << columns[ BGQDB::DBTBlock::QUALIFIER_COL ].getString()
            );
    if ( columns[ BGQDB::DBTBlock::SEQID_COL ].getInt64() > _sequence ) {
        _sequence = columns[ BGQDB::DBTBlock::SEQID_COL ].getInt64();
        LOG_DEBUG_MSG( "updated sequence ID to " << _sequence );
    }

    server->getBlocks()->create(
            columns[ BGQDB::DBTBlock::BLOCKID_COL ].getString(),
            boost::bind(
                &Reconnect::createCallback,
                shared_from_this(),
                _1,
                _2
                )
            );
}

void
Reconnect::createCallback(
        const error_code::rc error,
        const std::string& message
        )
{
    const cxxdb::Columns& columns = _results->columns();
    const std::string& id = columns[ BGQDB::DBTBlock::BLOCKID_COL ].getString();
    LOGGING_DECLARE_BLOCK_MDC( id );

    if ( error ) {
        LOG_ERROR_MSG( "could not create block" );
        LOG_ERROR_MSG( error_code::toString(error) << ": " << message );
        this->nextBlock();
        return;
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;
    
    LOG_DEBUG_MSG( __FUNCTION__ );

    const bool io = columns[ BGQDB::DBTBlock::NUMIONODES_COL ].as<int32_t>() != 0;
    if ( io ) {
        LOG_DEBUG_MSG( "skipping Software Failure query" );

        // I/O blocks that are booting do not need to be initialized
        if ( columns[ BGQDB::DBTBlock::STATUS_COL ].getString() != BGQDB::BLOCK_INITIALIZED ) {
            this->nextBlock();
            return;
        }

        server->getBlocks()->initialized(
                id,
                boost::bind(
                    &Reconnect::initializedCallback,
                    shared_from_this(),
                    _1,
                    _2
                    )
                );

        return;
    }

    // get compute block so we can find any nodes that have a status of Software (F)ailure
    server->getBlocks()->find(
            id,
            boost::bind(
                &Reconnect::findCallback,
                shared_from_this(),
                _1
                )
            );
}

void
Reconnect::initializedCallback(
        const error_code::rc error,
        const std::string& message
        )
{
    const cxxdb::Columns& columns = _results->columns();
    const std::string& id = columns[ BGQDB::DBTBlock::BLOCKID_COL ].getString();
    LOGGING_DECLARE_BLOCK_MDC( id );

    if ( error ) {
        LOG_ERROR_MSG( "could not initialize block" );
        LOG_ERROR_MSG( error_code::toString(error) << ": " << message );
        this->nextBlock();
        return;
    }

    LOG_INFO_MSG( __FUNCTION__ );
    this->nextBlock();
}

void
Reconnect::findCallback(
        const block::Compute::Ptr& block
        )
{
    const cxxdb::Columns& columns = _results->columns();
    const std::string& id = columns[ BGQDB::DBTBlock::BLOCKID_COL ].getString();
    LOGGING_DECLARE_BLOCK_MDC( id );

    if ( !block ) {
        LOG_WARN_MSG( "could not find block" );
        this->nextBlock();
        return;
    }

    std::ostringstream sql;
    sql <<
        "SELECT " <<
        BGQDB::DBTNode().getTableName() << "." << BGQDB::DBTNode::MIDPLANEPOS_COL << "," <<
        BGQDB::DBTNode().getTableName() << "." << BGQDB::DBTNode::NODECARDPOS_COL << "," <<
        BGQDB::DBTNode().getTableName() << "." << BGQDB::DBTNode::POSITION_COL <<
        " FROM " << BGQDB::DBTNode().getTableName() <<
        " INNER JOIN "
        ;
    if ( static_cast<uint32_t>(columns[ BGQDB::DBTBlock::NUMCNODES_COL ].getInt32()) < BGQDB::Nodes_Per_Midplane ) {
        // small block, join on the smallblock table
        sql <<
                BGQDB::DBTSmallblock().getTableName() <<
                " ON " <<
                BGQDB::DBTNode().getTableName() << "." << BGQDB::DBTNode::MIDPLANEPOS_COL << "=" <<
                BGQDB::DBTSmallblock().getTableName() << "." << BGQDB::DBTSmallblock::POSINMACHINE_COL <<
                " AND " <<
                BGQDB::DBTNode().getTableName() << "." << BGQDB::DBTNode::NODECARDPOS_COL << "=" <<
                BGQDB::DBTSmallblock().getTableName() << "." << BGQDB::DBTSmallblock::NODECARDPOS_COL <<
                " AND " <<
                BGQDB::DBTSmallblock().getTableName() << "." << BGQDB::DBTSmallblock::BLOCKID_COL << "=?"
                ;
    } else {
        // large block, join on the bpblockmap table
        sql <<
            BGQDB::DBTBpblockmap().getTableName() <<
            " ON " <<
            BGQDB::DBTNode().getTableName() << "." << BGQDB::DBTNode::MIDPLANEPOS_COL << "=" <<
            BGQDB::DBTBpblockmap().getTableName() << "." << BGQDB::DBTBpblockmap::BPID_COL <<
            " AND " <<
            BGQDB::DBTBpblockmap().getTableName() << "." << BGQDB::DBTBpblockmap::BLOCKID_COL << "=?"
            ;
    }
    
    // and we only care about nodes that have a status of SOFTWARE (F)AILURE
    sql <<
        " AND " <<
        BGQDB::DBTNode().getTableName() << "." << BGQDB::DBTNode::STATUS_COL << " = '" << BGQDB::SOFTWARE_FAILURE << "'"
        ;

    try {
        const cxxdb::QueryStatementPtr query = _connection->prepareQuery(
                sql.str(),
                boost::assign::list_of( "BLOCK" )
                );
        query->parameters()[ "BLOCK" ].set( id );

        const cxxdb::ResultSetPtr results = query->execute();

        this->softwareFailures( block, results );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        LOG_TRACE_MSG( sql.str() );
    }
        
    this->nextBlock();
}

void
Reconnect::softwareFailures(
        const block::Compute::Ptr& block,
        const cxxdb::ResultSetPtr& results
        )
{
    size_t count = 0;
    while ( results->fetch() && ++count) {
        const cxxdb::Columns& columns = results->columns();

        // convert columns into a location string
        const std::string midplane = columns[ BGQDB::DBTNode::MIDPLANEPOS_COL ].getString();
        const std::string board = columns[ BGQDB::DBTNode::NODECARDPOS_COL ].getString();
        const std::string compute = columns[ BGQDB::DBTNode::POSITION_COL ].getString();
        const Uci location( midplane + "-" + board + "-" + compute );

        const block::Compute::Midplanes::iterator mp = block->_midplanes.find( midplane );
        if (  mp == block->_midplanes.end() ) {
            LOG_WARN_MSG( "could not find midplane " << midplane );
            continue;
        }

        // use Midplane's corner mapping to get coordinates of the location
        const block::Midplane::CornerMap::iterator corner = mp->second._corners.find( location );
        if ( corner == mp->second._corners.end() ) {
            LOG_WARN_MSG( "could not find coordinates for " << location );
            continue;
        }

        const ComputeNode::Ptr node = mp->second.node( corner->second );
        if ( !node ) {
            LOG_WARN_MSG( "could not find compute node: " << location );
            continue;
        }

        // remember this node is now unavailable
        node->unavailable();
        LOG_DEBUG_MSG( location );
    }

    if ( count ) {
        LOG_INFO_MSG( 
                "found " << count << " Software Failure node" <<
                (count == 1 ? "" : "s")
                );
    }
}

} // block
} // server
} // runjob
