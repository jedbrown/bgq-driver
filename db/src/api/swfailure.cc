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
/* (C) Copyright IBM Corp.  2010, 2013                              */
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

#include "swfailure.h"

#include "cxxdb/cxxdb.h"
#include "tableapi/DBConnectionPool.h"
#include "tableapi/gensrc/DBTBpblockmap.h"
#include "tableapi/gensrc/DBVMidplane.h"
#include "tableapi/gensrc/DBTNode.h"
#include "tableapi/gensrc/DBTSmallblock.h"
#include "tableapi/gensrc/DBVIonode.h"

#include <bgq_util/include/Location.h>
#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

#include <string>
#include <vector>

LOG_DECLARE_FILE( "database" );

typedef std::vector<std::string> Locations;

namespace {
    
void
ioBlock(
        const BGQDB::DBTBlock& block,
        const cxxdb::ConnectionPtr& connection
       )
{
    cxxdb::UpdateStatementPtr update;
    if ( static_cast<unsigned>(block._numionodes) < bgq::util::Location::ComputeCardsOnIoBoard ) {
        update = connection->prepareUpdate(
                std::string() +
                "UPDATE BGQIoNode set STATUS='" + BGQDB::HARDWARE_AVAILABLE + "' WHERE STATUS='" + BGQDB::SOFTWARE_FAILURE + "' AND " +
                BGQDB::DBVIonode::LOCATION_COL + "=?",
                boost::assign::list_of(BGQDB::DBVIonode::LOCATION_COL)
                );
    } else {
        update = connection->prepareUpdate(
                std::string() +
                "UPDATE BGQIoNode set STATUS='" + BGQDB::HARDWARE_AVAILABLE + "' WHERE STATUS='" + BGQDB::SOFTWARE_FAILURE + "' AND " +
                "substr(" + BGQDB::DBVIonode::LOCATION_COL + ",1,6)=?",
                boost::assign::list_of(BGQDB::DBVIonode::LOCATION_COL)
                );
    }

    // get all locations in this I/O block
    const cxxdb::ResultSetPtr results(
            connection->query( std::string() + "SELECT LOCATION from BGQIoBlockMap where BLOCKID='" + block._blockid + "'" )
            );

    Locations locations;
    while ( results->fetch() ) {
        locations.push_back( results->columns()[ "LOCATION" ].getString() );
    }
    LOG_DEBUG_MSG( 
            block._blockid << " found " << locations.size() << " " <<
            (static_cast<unsigned>(block._numionodes) < bgq::util::Location::ComputeCardsOnIoBoard ? "nodes" : "drawers")
            );

    BOOST_FOREACH( const std::string& i, locations ) {
        update->parameters()[ BGQDB::DBVIonode::LOCATION_COL ].set( i );
        try {
            unsigned rows;
            update->execute( &rows );
            if ( !rows ) continue;
            LOG_DEBUG_MSG(
                    "updated " << rows << " node" <<
                    (rows == 1 ? "" : "s") << " to " << BGQDB::HARDWARE_AVAILABLE
                    );
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( "Could not update " << i << " to status " << BGQDB::HARDWARE_AVAILABLE << ": " << e.what() );
        }
    }
}

void
largeBlock(
        const BGQDB::DBTBlock& block,
        const cxxdb::ConnectionPtr& connection
        )
{
    const cxxdb::ResultSetPtr results(
            connection->query( std::string() + "SELECT BPID from BGQBpBlockMap where BLOCKID='" + block._blockid + "'" )
            );
    Locations midplanes;
    while ( results->fetch() ) {
        midplanes.push_back( results->columns()[ "BPID" ].getString() );
    }
    LOG_DEBUG_MSG( block._blockid << " found " << midplanes.size() << " midplanes" );

    const cxxdb::UpdateStatementPtr nodeUpdate(
            connection->prepareUpdate(
                std::string() +
                "UPDATE BGQNode SET STATUS='" + BGQDB::HARDWARE_AVAILABLE + "' WHERE " +
                "STATUS='" + BGQDB::SOFTWARE_FAILURE + "' AND " +
                BGQDB::DBTNode::MIDPLANEPOS_COL + "=? AND " +
                BGQDB::DBTNode::NODECARDPOS_COL + "=?",
                boost::assign::list_of( BGQDB::DBTNode::MIDPLANEPOS_COL )( BGQDB::DBTNode::NODECARDPOS_COL )
                )
            );
    BOOST_FOREACH( const std::string& i, midplanes ) {
        nodeUpdate->parameters()[ BGQDB::DBTNode::MIDPLANEPOS_COL ].set( i );
        for ( unsigned j = 0; j < bgq::util::Location::NodeBoardsOnMidplane; ++j ) {
            std::ostringstream location;
            location << "N" << std::setw(2) << std::setfill('0') << j;
            nodeUpdate->parameters()[ BGQDB::DBTNode::NODECARDPOS_COL ].set( location.str() );
            try {
                unsigned rows;
                nodeUpdate->execute( &rows );
                if ( !rows ) continue;
                LOG_DEBUG_MSG( 
                        "updated " << rows << " node" <<
                        (rows == 1 ? "" : "s") << " in " << i << "-" << location.str() << " to " << BGQDB::HARDWARE_AVAILABLE 
                        );
            } catch ( const std::exception& e ) {
                LOG_WARN_MSG( "Could not update nodes in " << i << "-" << location.str() << " to " << BGQDB::HARDWARE_AVAILABLE );
            }
        }
    }

    const cxxdb::UpdateStatementPtr midplaneUpdate(
            connection->prepareUpdate(
                std::string() +
                "UPDATE " + BGQDB::DBVMidplane().getTableName() + " " +
                "SET STATUS='" + BGQDB::HARDWARE_AVAILABLE + "' WHERE " +
                "STATUS='" + BGQDB::SOFTWARE_FAILURE + "' AND " +
                BGQDB::DBVMidplane::LOCATION_COL + "=?",
                boost::assign::list_of( BGQDB::DBVMidplane::LOCATION_COL )
                )
            );
    
    BOOST_FOREACH( const std::string& i, midplanes ) {
        midplaneUpdate->parameters()[ BGQDB::DBVMidplane::LOCATION_COL ].set( i );
        try {
            unsigned rows;
            midplaneUpdate->execute( &rows );
            if ( !rows ) continue;
            LOG_DEBUG_MSG( "updated " << i << " to " << BGQDB::HARDWARE_AVAILABLE );
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( "Could not update " << i << " to " << BGQDB::HARDWARE_AVAILABLE );
        }
    }
}

void
smallBlock(
        const BGQDB::DBTBlock& block,
        const cxxdb::ConnectionPtr& connection
        )
{
    const cxxdb::ResultSetPtr results(
            connection->query( std::string() + "SELECT POSINMACHINE, NODECARDPOS from BGQSmallBlock where BLOCKID='" + block._blockid + "'" )
            );
    typedef std::pair<std::string, std::string> NodeBoard; // midplane location, nodeboard position
    typedef std::vector<NodeBoard> SmallBlock;
    SmallBlock nodeboards;
    while ( results->fetch() ) {
        nodeboards.push_back( 
                std::make_pair(
                    results->columns()[ "POSINMACHINE" ].getString(),
                    results->columns()[ "NODECARDPOS" ].getString()
                    )
                );
    }
    LOG_DEBUG_MSG( block._blockid << " found " << nodeboards.size() << " nodeboards" );

    const cxxdb::UpdateStatementPtr nodeUpdate(
            connection->prepareUpdate(
                std::string() +
                "UPDATE BGQNode SET STATUS='" + BGQDB::HARDWARE_AVAILABLE + "' WHERE " +
                "STATUS='" + BGQDB::SOFTWARE_FAILURE + "' AND " +
                BGQDB::DBTNode::MIDPLANEPOS_COL + "=? AND " +
                BGQDB::DBTNode::NODECARDPOS_COL + "=?",
                boost::assign::list_of( BGQDB::DBTNode::MIDPLANEPOS_COL )( BGQDB::DBTNode::NODECARDPOS_COL )
                )
            );
    BOOST_FOREACH( const NodeBoard& i, nodeboards ) {
        nodeUpdate->parameters()[ BGQDB::DBTNode::MIDPLANEPOS_COL ].set( i.first );
        nodeUpdate->parameters()[ BGQDB::DBTNode::NODECARDPOS_COL ].set( i.second );
        try {
            unsigned rows;
            nodeUpdate->execute( &rows );
            if ( !rows ) continue;
            LOG_DEBUG_MSG(
                    "updated " << rows << " node" <<
                    (rows == 1 ? "" : "s") << " in " << i.first << "-" << i.second << " to " << BGQDB::HARDWARE_AVAILABLE 
                    );
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( "Could not update nodes in " << i.first << "-" << i.second << " to " << BGQDB::HARDWARE_AVAILABLE );
        }
    }
}

const size_t ComputesPerMidplane = bgq::util::Location::NodeBoardsOnMidplane * bgq::util::Location::ComputeCardsOnNodeBoard;

} // anonymous namespace

namespace BGQDB {

void
clearSoftwareFailure(
        const DBTBlock& block
        )
{
    const cxxdb::ConnectionPtr connection( DBConnectionPool::instance().getConnection() );
    if ( !connection ) {
        LOG_WARN_MSG( "could not get a connection" );
        return;
    }

    try {
        if ( block._numionodes ) {
            ioBlock( block, connection );
        } else if ( static_cast<size_t>(block._numcnodes) >= ComputesPerMidplane ) {
            largeBlock( block, connection );
        } else {
            smallBlock( block, connection );
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

} // BGQDB
