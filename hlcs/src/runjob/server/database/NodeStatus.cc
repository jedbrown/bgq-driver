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

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTBpblockmap.h>
#include <db/include/api/tableapi/gensrc/DBTSmallblock.h>
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

    _sql <<
        "UPDATE " << BGQDB::DBVNodeall().getTableName() <<
        " SET " << BGQDB::DBVNodeall::STATUS_COL << "='" << BGQDB::SOFTWARE_FAILURE << "'" <<
        " WHERE" <<
        " " << BGQDB::DBVNodeall::STATUS_COL << "='" << BGQDB::HARDWARE_AVAILABLE << "' AND "
        ;

    try {
        if ( isSubBlock ) {
            this->subBlock();
        } else {
            this->fullBlock();
        }
    } catch ( const std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
    }
}

void
NodeStatus::subBlock()
{
    LOG_TRACE_MSG( "sub block" );
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

    const Shape& shape = subBlock.shape();
    const Shape::Nodes& nodes = shape.nodes();
    for ( Shape::Nodes::const_iterator i = nodes.begin(); i != nodes.end(); ++i ) {
        typedef Shape::Nodes::mapped_type Nodes;
        const Nodes& nodes = i->second;

        // update up to 32 rows in a single transaction
        const cxxdb::Transaction tx( *_connection );

        for ( Nodes::const_iterator j = nodes.begin(); j != nodes.end(); ++j ) {
            LOGGING_DECLARE_LOCATION_MDC( 
                    std::string() +
                    BGQTopology::nodeCardNameFromPos(i->first) +
                    "-" +
                    BGQTopology::processorCardNameFromJtagPort(*j) 
                    );

            // set node board and node locations
            _statement->parameters()[ BGQDB::DBVNodeall::NODECARDPOS_COL ].set( BGQTopology::nodeCardNameFromPos(i->first) );
            _statement->parameters()[ BGQDB::DBVNodeall::POSITION_COL ].set( BGQTopology::processorCardNameFromJtagPort(*j) );

            _statement->execute();
        }

        // if we get here, commit transaction
        _connection->commit();
    }
}

void
NodeStatus::fullBlock()
{
    LOG_TRACE_MSG( "full block" );

    // add midplane and node board clauses
    if ( _block->size() < BGQDB::Nodes_Per_Midplane ) {
        _sql << BGQDB::DBVNodeall::MIDPLANEPOS_COL << "=? AND ";
        _sql << BGQDB::DBVNodeall::NODECARDPOS_COL << "=?";
        _statement = _connection->prepareUpdate(
                _sql.str(),
                boost::assign::list_of
                (BGQDB::DBVNodeall::MIDPLANEPOS_COL)
                (BGQDB::DBVNodeall::NODECARDPOS_COL)
                );
    } else {
        _sql << BGQDB::DBVNodeall::MIDPLANEPOS_COL << "=?";
        _statement = _connection->prepareUpdate(
            _sql.str(),
            boost::assign::list_of
            (BGQDB::DBVNodeall::MIDPLANEPOS_COL)
            );
    }

    // iterate through midplanes in the block
    for ( 
            block::Compute::Midplanes::const_iterator i = _block->midplanes().begin();
            i != _block->midplanes().end();
            ++i
        )
    {
        // update up to 512 (1 midplane) rows in a single transaction
        const cxxdb::Transaction tx( *_connection );
        LOGGING_DECLARE_LOCATION_MDC(i->first);

        const block::Midplane& midplane = i->second;
        if ( _block->size() < BGQDB::Nodes_Per_Midplane ) {
            for (
                    block::Midplane::Nodeboards::const_iterator j = midplane.nodeboards().begin();
                    j != midplane.nodeboards().end();
                    ++j
                )
            {
                _statement->parameters()[ BGQDB::DBVNodeall::NODECARDPOS_COL ].set( *j );
                _statement->parameters()[ BGQDB::DBVNodeall::MIDPLANEPOS_COL ].set( i->first );

                unsigned rows;
                _statement->execute( &rows );
                LOG_DEBUG_MSG( "updated " << rows << " nodes to '" << BGQDB::SOFTWARE_FAILURE << "'" );
            }
        } else {
            _statement->parameters()[ BGQDB::DBVNodeall::MIDPLANEPOS_COL ].set( i->first );
            unsigned rows;
            _statement->execute( &rows );
            LOG_DEBUG_MSG( "updated " << rows << " nodes to '" << BGQDB::SOFTWARE_FAILURE << "'" );
        }

        // if we get here, commit transaction
        _connection->commit();
    }
}

} // database
} // server
} // runjob
