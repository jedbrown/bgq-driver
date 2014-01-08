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

#include "common/logging.h"

#include "server/Options.h"
#include "server/Server.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTBpblockmap.h>
#include <db/include/api/tableapi/gensrc/DBTMidplane.h>
#include <db/include/api/tableapi/gensrc/DBTNode.h>
#include <db/include/api/tableapi/gensrc/DBTSmallblock.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <boost/assign/list_of.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace block {

void
Reconnect::create(
        const Server::Ptr& server,
        const Callback& callback
        )
{
    try {
        const Ptr result(
                new Reconnect( server )
                );

        server->getBlocks()->loadMachine(
                boost::bind(
                    &Reconnect::loadMachineCallback,
                    result,
                    _1,
                    _2,
                    callback
                )
            );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return;
}

Reconnect::~Reconnect()
{
    const boost::posix_time::ptime now( boost::posix_time::microsec_clock::local_time() );

    LOG_INFO_MSG(
            "Reconnected to " <<
            _computeBlockCount << " compute block" << (_computeBlockCount == 1 ? "" : "s") <<
            " and " << _ioBlockCount << " I/O block" << (_ioBlockCount == 1 ? "" : "s") <<
            " in " << boost::posix_time::to_simple_string(now - _startTime) 
            );
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    job::Reconnect::create( server );
}

Reconnect::Reconnect(
        const Server::Ptr& server
        ) :
    _server( server ),
    _connection( BGQDB::DBConnectionPool::instance().getConnection() ),
    _results(),
    _strand( server->getIoService() ),
    _smallBlock(),
    _largeBlockNodes(),
    _largeBlockMidplanes(),
    _machine(),
    _ioBlockCount( 0 ),
    _computeBlockCount( 0 ),
    _startTime( boost::posix_time::microsec_clock::local_time() ),
    _sequence( 0 )
{
    if ( !_connection ) {
        LOG_WARN_MSG( "could not get database connection" );
        return;
    }

    using namespace BGQDB;

    // order I/O blocks first so we reconnect to them before compute blocks
    _results = _connection->query(
            "SELECT "
            + DBTBlock::BLOCKID_COL + "," + DBTBlock::STATUS_COL + ","
            + DBTBlock::NUMCNODES_COL + "," + DBTBlock::NUMIONODES_COL + ","
            + DBTBlock::SEQID_COL +
            " FROM " + DBTBlock().getTableName() +
            " ORDER BY " + DBTBlock::NUMIONODES_COL + " DESC, " + DBTBlock::SEQID_COL + " DESC"
            );
       
    // these queries are used to find any nodes in a Software (F)ailure status in both
    // small and large blocks
    _smallBlock = _connection->prepareQuery(
            std::string("SELECT ") +
            DBTNode().getTableName() + "." + DBTNode::MIDPLANEPOS_COL + "," + 
            DBTNode().getTableName() + "." + DBTNode::NODECARDPOS_COL + "," + 
            DBTNode().getTableName() + "." + DBTNode::POSITION_COL+ 
            " FROM " + DBTNode().getTableName() + 
            " INNER JOIN " + DBTSmallblock().getTableName() + " ON " +
            DBTNode().getTableName() + "." + DBTNode::MIDPLANEPOS_COL + "=" + 
            DBTSmallblock().getTableName() + "." + DBTSmallblock::POSINMACHINE_COL+ 
            " AND " + 
            DBTNode().getTableName() + "." + DBTNode::NODECARDPOS_COL + "=" + 
            DBTSmallblock().getTableName() + "." + DBTSmallblock::NODECARDPOS_COL+ 
            " AND " + 
            DBTSmallblock().getTableName() + "." + DBTSmallblock::BLOCKID_COL + "=?" +
            " AND " + DBTNode().getTableName() + "." + DBTNode::STATUS_COL + " = '" + SOFTWARE_FAILURE + "'",
            boost::assign::list_of( DBTSmallblock::BLOCKID_COL)
            );

    _largeBlockNodes = _connection->prepareQuery(
            std::string("SELECT ") +
            DBTNode().getTableName() + "." + DBTNode::MIDPLANEPOS_COL + "," + 
            DBTNode().getTableName() + "." + DBTNode::NODECARDPOS_COL + "," + 
            DBTNode().getTableName() + "." + DBTNode::POSITION_COL+ 
            " FROM " + DBTNode().getTableName() + 
            " INNER JOIN " + DBTBpblockmap().getTableName() + " ON " +
            DBTNode().getTableName() + "." + DBTNode::MIDPLANEPOS_COL + "=" + 
            DBTBpblockmap().getTableName() + "." + DBTBpblockmap::BPID_COL+ 
            " AND " + 
            DBTBpblockmap().getTableName() + "." + DBTBpblockmap::BLOCKID_COL + "=?" +
            " AND " + DBTNode().getTableName() + "." + DBTNode::STATUS_COL + " = '" + SOFTWARE_FAILURE + "'",
            boost::assign::list_of( DBTBpblockmap::BLOCKID_COL )
            );

    // this query handles midplanes in a Software (F)ailure status
    _largeBlockMidplanes = _connection->prepareQuery(
            std::string("SELECT ") +
            "BGQMidplane.LOCATION"
            " FROM " + DBTMidplane().getTableName() + 
            " INNER JOIN " + DBTBpblockmap().getTableName() + " ON " +
            "BGQMidplane.LOCATION=" +
            DBTBpblockmap().getTableName() + "." + DBTBpblockmap::BPID_COL +
            " AND " + 
            DBTBpblockmap().getTableName() + "." + DBTBpblockmap::BLOCKID_COL + "=?" +
            " AND " + DBTMidplane().getTableName() + "." + DBTMidplane::STATUS_COL + " = '" + SOFTWARE_FAILURE + "'",
            boost::assign::list_of( DBTBpblockmap::BLOCKID_COL )
            );
}

void
Reconnect::start()
{
    BOOST_ASSERT( _results );
    BOOST_ASSERT( _machine );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    while ( _results->fetch() ) {
        const cxxdb::Columns& columns = _results->columns();
        const std::string id = columns[ BGQDB::DBTBlock::BLOCKID_COL ].getString();
        LOGGING_DECLARE_BLOCK_MDC( id );
                
        const int cnodes = columns[ BGQDB::DBTBlock::NUMCNODES_COL ].as<int32_t>();
        const int ionodes = columns[ BGQDB::DBTBlock::NUMIONODES_COL ].as<int32_t>();
        const std::string status = columns[ BGQDB::DBTBlock::STATUS_COL ].getString();

        if ( columns[ BGQDB::DBTBlock::SEQID_COL ].getInt64() > _sequence ) {
            _sequence = columns[ BGQDB::DBTBlock::SEQID_COL ].getInt64();
        }

        if ( server->getOptions().reconnect().scope() == runjob::server::Reconnect::Scope::None ) continue;

        // we only care about booting and initialized blocks
        if ( status != BGQDB::BLOCK_INITIALIZED && status != BGQDB::BLOCK_BOOTING ) {
            LOG_TRACE_MSG( 
                    " (" << status << ")" <<
                    " (" << (cnodes ? cnodes : ionodes) << " nodes)" <<
                    " sequence " << columns[ BGQDB::DBTBlock::SEQID_COL ].getInt64()
                );
            continue;
        }
        
        LOG_DEBUG_MSG( 
                "(" << status << ")" <<
                " (" << (cnodes ? cnodes : ionodes) << " nodes)" <<
                " sequence " << columns[ BGQDB::DBTBlock::SEQID_COL ].getInt64()
                );

        if ( cnodes ) ++_computeBlockCount;
        if ( ionodes ) ++_ioBlockCount;

        server->getBlocks()->create(
                id,
                _machine,
                boost::bind(
                    &Reconnect::createCallback,
                    shared_from_this(),
                    _1,
                    _2,
                    id,
                    ionodes,
                    cnodes,
                    status
                    )
                );
    }
}

void
Reconnect::createCallback(
        const error_code::rc error,
        const std::string& message,
        const std::string& name,
        const size_t numionodes,
        const size_t numcnodes,
        const std::string& status
        )
{
    LOGGING_DECLARE_BLOCK_MDC( name );

    if ( error ) {
        LOG_ERROR_MSG( "could not create block" );
        LOG_ERROR_MSG( error_code::toString(error) << ": " << message );
        return;
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;
    
    LOG_DEBUG_MSG( __FUNCTION__ );

    if ( numionodes != 0 ) {
        LOG_DEBUG_MSG( "skipping Software Failure query" );

        // I/O blocks that are booting do not need to be initialized
        if ( status != BGQDB::BLOCK_INITIALIZED ) {
            return;
        }

        server->getBlocks()->initialized(
                name,
                boost::bind(
                    &Reconnect::initializedCallback,
                    shared_from_this(),
                    name,
                    _1,
                    _2
                    )
                );

        return;
    }

    // get compute block so we can find any nodes that have a status of Software (F)ailure
    server->getBlocks()->find(
            name,
            _strand.wrap(
                boost::bind(
                    &Reconnect::findCallback,
                    shared_from_this(),
                    _1,
                    name,
                    numcnodes
                    )
                )
            );
}

void
Reconnect::initializedCallback(
        const std::string& name,
        const error_code::rc error,
        const std::string& message
        )
{
    LOGGING_DECLARE_BLOCK_MDC( name );

    if ( error ) {
        LOG_ERROR_MSG( "could not initialize block" );
        LOG_ERROR_MSG( error_code::toString(error) << ": " << message );
        return;
    }

    LOG_INFO_MSG( __FUNCTION__ );
}

void
Reconnect::findCallback(
        const block::Compute::Ptr& block,
        const std::string& name,
        const size_t numcnodes
        )
{
    // assume this handler is protected by _strand
    BOOST_ASSERT( _smallBlock );
    BOOST_ASSERT( _largeBlockNodes );
    BOOST_ASSERT( _largeBlockMidplanes );
    LOGGING_DECLARE_BLOCK_MDC( name );

    if ( !block ) {
        LOG_WARN_MSG( "could not find block" );
        return;
    }

    cxxdb::QueryStatementPtr query;
    if ( numcnodes < BGQDB::Nodes_Per_Midplane ) {
        query = _smallBlock;
        _smallBlock->parameters()[ BGQDB::DBTSmallblock::BLOCKID_COL ].set( name );
    } else {
        query = _largeBlockNodes;
        _largeBlockNodes->parameters()[ BGQDB::DBTBpblockmap::BLOCKID_COL ].set( name );
    }
    
    try {
        const cxxdb::ResultSetPtr results = query->execute();
        this->nodeSoftwareFailures( block, results );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "node software failures: " << e.what() );
    }
    
    if ( numcnodes >= BGQDB::Nodes_Per_Midplane ) {
        this->midplaneSoftwareFailures( block );
    }
}

void
Reconnect::midplaneSoftwareFailures(
        const block::Compute::Ptr& block
        )
{
    BOOST_ASSERT( block );

    _largeBlockMidplanes->parameters()[ BGQDB::DBTBpblockmap::BLOCKID_COL ].set( block->name() );
    try {
        const cxxdb::ResultSetPtr result = _largeBlockMidplanes->execute();
        size_t count = 0;
        while ( result->fetch() && ++count ) {
            const std::string location( result->columns()[ "LOCATION" ].getString() );
            const block::Compute::Midplanes::iterator mp = block->_midplanes.find( location );
            if (  mp == block->_midplanes.end() ) {
                LOG_WARN_MSG( "could not find midplane " << location );
                continue;
            }

            const Midplane::NodeArray& nodes = mp->second._nodes;
            for ( auto i = nodes.origin(); i < (nodes.origin() + nodes.num_elements()); ++i ) {
                (*i)->unavailable();
            }
        }
        if ( count ) {
            LOG_INFO_MSG( 
                    "found " << count << " Software Failure midplane" <<
                    (count == 1 ? "" : "s")
                    );
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "midplane software failures: " << e.what() );
    }
}

void
Reconnect::nodeSoftwareFailures(
        const block::Compute::Ptr& block,
        const cxxdb::ResultSetPtr& results
        )
{
    // assume this handler is protected by _strand
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
    }

    if ( count ) {
        LOG_INFO_MSG( 
                "found " << count << " Software Failure node" <<
                (count == 1 ? "" : "s")
                );
    }
}

void
Reconnect::loadMachineCallback(
        const boost::shared_ptr<BGQMachineXML>& machine,
        const error_code::rc error,
        const Callback& callback
        )
{
    if ( error ) {
        LOG_WARN_MSG( "could not get machine description: " << error );
        callback( _sequence );
        return;
    }
    LOG_INFO_MSG( "machine XML description loaded" );
    _machine = machine;

    this->start();

    callback( _sequence );
}

} // block
} // server
} // runjob
