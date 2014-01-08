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
#include "server/database/NodeStatus.h"

#include "server/block/Compute.h"
#include "server/block/Midplane.h"

#include "server/Job.h"
#include "server/Server.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"
#include "common/JobInfo.h"
#include "common/SubBlock.h"

#include <bgq_util/include/Location.h>

#include <db/include/api/tableapi/gensrc/DBVMidplane.h>
#include <db/include/api/tableapi/gensrc/DBVNodeall.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/BGQDBlib.h>

#include <sstream>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace database {

NodeStatus::NodeStatus(
        const block::Compute::Ptr& block,
        const JobInfo& info
        ) :
    _sql(),
    _connection( BGQDB::DBConnectionPool::instance().getConnection() ),
    _statement(),
    _block( block ),
    _info( info )
{
    if ( !_connection ) {
        LOG_RUNJOB_EXCEPTION( error_code::database_error, "could not get database connection" );
    }
}

NodeStatus::~NodeStatus()
{

}

void
NodeStatus::execute()
{
    const bool isSubBlock( _info.getSubBlock().isValid() );
    const bool isSmallBlock( _block->size() < BGQDB::Nodes_Per_Midplane );

    try {
        if ( isSubBlock ) {
            this->subBlock();
        } else if ( isSmallBlock ) {
            this->smallBlock();
        } else {
            this->largeBlock();
        }
    } catch ( const std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
    }
}

void
NodeStatus::subBlock()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    _sql <<
        "UPDATE " << BGQDB::DBVNodeall().getTableName() <<
        " SET " << BGQDB::DBVNodeall::STATUS_COL << "='" << BGQDB::SOFTWARE_FAILURE << "'" <<
        " WHERE" <<
        " " << BGQDB::DBVNodeall::STATUS_COL << "='" << BGQDB::HARDWARE_AVAILABLE << "' AND "
        ;

    const SubBlock& subBlock = _info.getSubBlock();
    const Corner& corner = subBlock.corner();
    const block::Compute::Midplanes& midplanes( _block->midplanes() );

    // find midplane in block used by sub-block
    const block::Compute::Midplanes::const_iterator i = midplanes.find( corner.getMidplane() );
    if ( i == midplanes.end() ) {
        LOG_WARN_MSG( "could not find midplane '" << corner.getMidplane() << "'" );
        return;
    }

    // add midplane, node board, and position clauses
    _sql << BGQDB::DBVNodeall::MIDPLANEPOS_COL << "=? AND ";
    _sql << BGQDB::DBVNodeall::NODECARDPOS_COL << "=? AND ";
    _sql << BGQDB::DBVNodeall::POSITION_COL << "=?";

    _statement = _connection->prepareUpdate(
            _sql.str(),
            {
            BGQDB::DBVNodeall::MIDPLANEPOS_COL,
            BGQDB::DBVNodeall::NODECARDPOS_COL,
            BGQDB::DBVNodeall::POSITION_COL
            }
            );

    // set midplane location
    _statement->parameters()[ BGQDB::DBVNodeall::MIDPLANEPOS_COL ].set( i->first );

    unsigned count( 0 );
    const Shape& shape = subBlock.shape();
    const Shape::Nodes& nodes = shape.nodes();
    for ( Shape::Nodes::const_iterator i = nodes.begin(); i != nodes.end(); ++i ) {
        typedef Shape::Nodes::mapped_type Nodes;
        const Nodes& nodes = i->second;

        // update up to 32 rows in a single transaction
        const cxxdb::Transaction tx( *_connection );

        for ( Nodes::const_iterator j = nodes.begin(); j != nodes.end(); ++j ) {
            // set node board and node locations
            _statement->parameters()[ BGQDB::DBVNodeall::NODECARDPOS_COL ].set( BGQTopology::nodeCardNameFromPos(i->first) );
            _statement->parameters()[ BGQDB::DBVNodeall::POSITION_COL ].set( BGQTopology::processorCardNameFromJtagPort(*j) );

            unsigned rows;
            _statement->execute( &rows );
            if ( !rows ) continue;
            count += rows;
        }

        // if we get here, commit transaction
        _connection->commit();
    }

    LOG_INFO_MSG(
            count << " node" <<
            (count == 1 ? "" : "s") << " updated to '" << BGQDB::SOFTWARE_FAILURE << "' status" 
            );
}

void
NodeStatus::smallBlock()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    
    _sql <<
        "UPDATE " << BGQDB::DBVNodeall().getTableName() <<
        " SET " << BGQDB::DBVNodeall::STATUS_COL << "='" << BGQDB::SOFTWARE_FAILURE << "'" <<
        " WHERE" <<
        " " << BGQDB::DBVNodeall::STATUS_COL << "='" << BGQDB::HARDWARE_AVAILABLE << "' AND "
        ;

    // add midplane and node board clauses
    _sql << BGQDB::DBVNodeall::MIDPLANEPOS_COL << "=? AND ";
    _sql << BGQDB::DBVNodeall::NODECARDPOS_COL << "=?";
    _statement = _connection->prepareUpdate(
            _sql.str(),
            boost::assign::list_of
            (BGQDB::DBVNodeall::MIDPLANEPOS_COL)
            (BGQDB::DBVNodeall::NODECARDPOS_COL)
            );

    LOGGING_DECLARE_LOCATION_MDC(_block->midplanes().begin()->first);

    size_t count( 0 );
    const block::Midplane& midplane = _block->midplanes().begin()->second;
    for (
            block::Midplane::Nodeboards::const_iterator j = midplane.nodeboards().begin();
            j != midplane.nodeboards().end();
            ++j
        )
    {
        _statement->parameters()[ BGQDB::DBVNodeall::NODECARDPOS_COL ].set( *j );
        _statement->parameters()[ BGQDB::DBVNodeall::MIDPLANEPOS_COL ].set(
                boost::lexical_cast<std::string>( midplane.location() )
                );

        unsigned rows;
        _statement->execute( &rows );
        if ( !rows ) continue;
        count += rows;
        LOG_DEBUG_MSG(
                "updated " << rows << " node" <<
                (rows == 1 ? "" : "s") << " to '" << BGQDB::SOFTWARE_FAILURE << "'"
                );
    }

    LOG_INFO_MSG(
            count << " node" <<
            (count == 1 ? "" : "s") << " updated to '" << BGQDB::SOFTWARE_FAILURE << "' status" 
            );
}

void
NodeStatus::largeBlock()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    _sql <<
        "UPDATE " << BGQDB::DBVMidplane().getTableName() <<
        " SET " << BGQDB::DBVMidplane::STATUS_COL << "='" << BGQDB::SOFTWARE_FAILURE << "'" <<
        " WHERE" <<
        " " << BGQDB::DBVMidplane::STATUS_COL << "='" << BGQDB::HARDWARE_AVAILABLE << "' AND "
        ;

    _sql << BGQDB::DBVMidplane::LOCATION_COL << "=?";
    _statement = _connection->prepareUpdate(
            _sql.str(),
            boost::assign::list_of
            (BGQDB::DBVMidplane::LOCATION_COL)
            );

    // iterate through midplanes in the block
    size_t count( 0 );
    for ( 
            block::Compute::Midplanes::const_iterator i = _block->midplanes().begin();
            i != _block->midplanes().end();
            ++i
        )
    {
        LOGGING_DECLARE_LOCATION_MDC(i->first);

        _statement->parameters()[ BGQDB::DBVMidplane::LOCATION_COL ].set( i->first );

        unsigned rows;
        _statement->execute( &rows );
        if ( !rows ) continue;
        count += rows;
    }
    LOG_INFO_MSG(
            count << " midplane" <<
            (count == 1 ? "" : "s") << " updated to '" << BGQDB::SOFTWARE_FAILURE << "' status"
            );
}

} // database
} // server
} // runjob
