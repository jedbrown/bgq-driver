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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include "ComputeNode.h"

#include "Direction.h"
#include "../BCNodeInfo.h"
#include "../BlockControllerBase.h"

#include <bgq_util/include/Location.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/tableapi/gensrc/DBTLinkblockmap.h>
#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace neighbor {

ComputeNode::ComputeNode(
        const BlockPtr& block,
        const int msgid,
        const bgq::util::Location& location
        ) :
    Info( block, msgid, location )
{
    LOG_TRACE_MSG( _location );
}

void
ComputeNode::impl(
        const char* rawdata
        )
{
    BCNodeInfo* const nodeInfo( _block->findNodeInfo(_location) );
    if ( !nodeInfo ) {
        LOG_WARN_MSG( "Could not find node info for " << _location );
        return;
    }

    const Direction direction( rawdata );
    if ( direction.dim() == Dimension::Invalid ) return;

    BCNodeInfo* const neighbor(
            this->getNeighbor(nodeInfo, direction)
            );
    if ( !neighbor ) {
        LOG_WARN_MSG( "Could not find " << direction << " neighbor for " << _location );
        _details << "NEIGHBOR=N/A";
        return;
    }

    // special case for ND Sender events, we need to swap the from and to locations
    // to correctly callout the link chip and cables used in this path
    const bool swap( _msgId == 0x00080038 );

    const std::string linkInfo = BGQTopology::neighborInfo(
            swap ? neighbor->_pos.jtagPort() : nodeInfo->_pos.jtagPort(),
            swap ? neighbor->_pos.nodeCard() : nodeInfo->_pos.nodeCard(),
            swap ? nodeInfo->_pos.nodeCard() : neighbor->_pos.nodeCard(),
            swap ? neighbor->midplanePos() : nodeInfo->midplanePos(),
            swap ? nodeInfo->midplanePos() : neighbor->midplanePos(),
            this->isWrapped( direction, nodeInfo, neighbor ),
            direction.buf()
            );

    LOG_DEBUG_MSG( _location << " " << direction << " neighbor is " << neighbor->location() );
    _details << "NEIGHBOR=" << neighbor->location();
    _details << " ";
    _details << linkInfo;

    if ( neighbor->midplanePos() != nodeInfo->midplanePos() ) {
        try {
            this->addPassThrough(
                    direction,
                    nodeInfo->midplanePos(),
                    neighbor->midplanePos()
                    );
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( "Could not add pass-through neighbor details: " << e.what() );
        }
    }
}

void
ComputeNode::addPassThrough(
        const Direction& direction,
        const std::string& me,
        const std::string& neighbor
        )
{
    // assume input is midplane locations that are not equal
    BOOST_ASSERT( bgq::util::Location(me).getType() == bgq::util::Location::Midplane );
    BOOST_ASSERT( bgq::util::Location(neighbor).getType() == bgq::util::Location::Midplane );
    BOOST_ASSERT( me != neighbor );
    
    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    if ( !connection ) {
        LOG_WARN_MSG( __FUNCTION__ << " Could not get database connection" );
        return;
    }

    // to detect pass-through, we'll query starting from our midplane until we hit our neighbor.
    // If there is only one hop, that means there is no pass through
    static std::string sql(
            std::string() +
            "SELECT substr(linkid,11,6) as neighbor FROM " + BGQDB::DBTLinkblockmap().getTableName() + " "
            "WHERE blockid=? AND substr(linkid,1,10)=?"
            );
    const cxxdb::QueryStatementPtr query(
            connection->prepareQuery(
                sql,
                boost::assign::list_of("blockid")("linkid")
                )
            );
    query->parameters()[ "blockid" ].set( _block->getBlockName() );

    // linkid column has the syntax of D_Rxx-MxD_Rxx-Mx where D is the
    // A, B, C, or D dimension
    query->parameters()[ "linkid" ].set( 
            boost::lexical_cast<std::string>(direction.dim()) + "_" + me +
            boost::lexical_cast<std::string>(direction.dim()) + "_"
            );
    LOG_DEBUG_MSG(
            "checking for pass-through midplanes from " << me << " to " << 
            neighbor << " in " << direction << " direction"
            );


    std::vector<std::string> passThroughMidplanes;
    while (1) {
        const cxxdb::ResultSetPtr results( query->execute() );
        if ( !results->fetch() ) {
            LOG_DEBUG_MSG( __FUNCTION__ << " no results" );
            break;
        }

        if ( neighbor == results->columns()[ "neighbor" ].getString() ) break;

        passThroughMidplanes.push_back(
                results->columns()[ "neighbor" ].getString()
                );
        LOG_DEBUG_MSG( results->columns()[ "neighbor" ].getString() );

        // look for next midplane
        query->parameters()[ "linkid" ].set( 
                boost::lexical_cast<std::string>(direction.dim()) + "_" +
                results->columns()["neighbor"].getString() +
                boost::lexical_cast<std::string>(direction.dim()) + "_"
                );
    }

    if ( passThroughMidplanes.empty() ) {
        // no pass through used, no need to for additional details
        LOG_DEBUG_MSG( 
                __FUNCTION__ << " " << me << " is cabled directly to " <<
                neighbor << " in the " << direction << " direction" 
                );
        return;
    }

    _details << " passing through midplane" << (passThroughMidplanes.size() == 1 ? " " : "s ");
    std::copy( 
            passThroughMidplanes.begin(), 
            passThroughMidplanes.end() - 1,
            std::ostream_iterator<std::string>( _details, ", " )
            );
    if ( passThroughMidplanes.size() > 1 ) {
        _details << "and ";
    }
    _details << *passThroughMidplanes.rbegin();
}

} } } // namespace mmcs::server::neighbor
