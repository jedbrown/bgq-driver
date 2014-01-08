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
#include "common/Uci.h"

#include "server/handlers/locate_rank/ActiveBlock.h"
#include "server/handlers/locate_rank/ActiveJob.h"
#include "server/handlers/locate_rank/Block.h"
#include "server/handlers/locate_rank/HistoryBlock.h"
#include "server/handlers/locate_rank/HistoryJob.h"
#include "server/handlers/locate_rank/Job.h"

#include <bgq_util/include/Location.h>

#include <control/include/bgqconfig/BGQTopology.h>

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTBpblockmap.h>
#include <db/include/api/tableapi/gensrc/DBTSmallblock.h>
#include <db/include/api/BGQDBlib.h>

#include <boost/assign/list_of.hpp>

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/throw_exception.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

Block::Ptr
Block::create(
        const cxxdb::ConnectionPtr& db,
        const Job::Ptr& job
        )
{
    Block::Ptr result;
    if ( boost::dynamic_pointer_cast<ActiveJob>(job) ) {
        result = boost::make_shared<ActiveBlock>( db, job );
    } else if ( boost::shared_ptr<HistoryJob> history_job = boost::dynamic_pointer_cast<HistoryJob>(job) ) {
        // for history jobs we need to find if their block still exists, or if
        // it is in the bgqblock_history table
        const cxxdb::QueryStatementPtr statement = db->prepareQuery(
                std::string("SELECT ") +
                BGQDB::DBTBlock::BLOCKID_COL + " " +
                "FROM " + BGQDB::DBTBlock().getTableName() + " " +
                "WHERE " +
                BGQDB::DBTBlock::BLOCKID_COL + " =? AND " +
                BGQDB::DBTBlock::CREATEDATE_COL + " <?",
                boost::assign::list_of
                ( BGQDB::DBTBlock::BLOCKID_COL )
                ( BGQDB::DBTBlock::CREATEDATE_COL )
                );
        statement->parameters()[ BGQDB::DBTBlock::BLOCKID_COL ].set( history_job->block() );
        statement->parameters()[ BGQDB::DBTBlock::CREATEDATE_COL ].set( history_job->startTime() );
        const cxxdb::ResultSetPtr results = statement->execute();
        if ( results->fetch() ) {
            result = boost::make_shared<ActiveBlock>( db, job );
        } else {
            result = boost::make_shared<HistoryBlock>( db, history_job );
        }
    } else {
        BOOST_ASSERT( !"not an active or history job" );
    }
   
    if ( result->_small ) {
        result->querySmallBlock( db );
    } else {
        result->queryLargeBlock( db );
    }

    result->findCorner( job->corner() );

    return result;
}

Block::Block(
        const Job::Ptr& job
        ) :
    _statement(),
    _id( job->block() ),
    _small( false ),
    _offset(),
    _corner(),
    _midplane(),
    _creationId( 0 )
{

}

Block::~Block()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
Block::findCorner(
        const std::string& corner
        )
{
    if ( corner.empty() ) {
        _corner[0] = 0;
        _corner[1] = 0;
        _corner[2] = 0;
        _corner[3] = 0;
        _corner[4] = 0;

        return;
    }

    // we can assume the corner exists within the same midplane as the rank in question since that
    // is a limitation of sub-block job geometries

    LOG_TRACE_MSG( "finding coordinates for corner " << corner );
    const bgq::util::Location location( corner );

    const BGQTopology::Coord coordinates = BGQTopology::ABCDEglobalMap[ location.getNodeBoard() ][ location.getComputeCard() ];
    LOG_TRACE_MSG( 
            "corner coordinates (" <<
            coordinates.a << "," <<
            coordinates.b << "," <<
            coordinates.b << "," <<
            coordinates.d << "," <<
            coordinates.e << ")"
            );
   
    // offset node's coordinates with small block offset 
    _corner[0] = coordinates.a - _offset[0];
    _corner[1] = coordinates.b - _offset[1];
    _corner[2] = coordinates.c - _offset[2];
    _corner[3] = coordinates.d - _offset[3];
    _corner[4] = coordinates.e - _offset[4];

    LOG_TRACE_MSG( 
            "offset corner coordinates (" <<
            _corner[0] << "," <<
            _corner[1] << "," <<
            _corner[2] << "," <<
            _corner[3] << "," <<
            _corner[4] << ")"
            );
}

Uci
Block::findNodeLocation(
        const BG_CoordinateMapping_t& coordinates
        ) 
{
    if ( !_small ) {
        this->executeLargeBlock( coordinates );
    }

    int nodeboard;
    int computecard;

    const unsigned a = coordinates.a + _offset[0] + _corner[0];
    const unsigned b = coordinates.b + _offset[1] + _corner[1];
    const unsigned c = coordinates.c + _offset[2] + _corner[2];
    const unsigned d = coordinates.d + _offset[3] + _corner[3];
    const unsigned e = coordinates.e + _offset[4] + _corner[4];
    LOG_TRACE_MSG(
            "(" << a << "," << b << "," << c << "," << d << "," << e << ") " <<
            "within block"
            );

    const unsigned midplane_a = a % BGQTopology::MAX_A_NODE;
    const unsigned midplane_b = b % BGQTopology::MAX_B_NODE;
    const unsigned midplane_c = c % BGQTopology::MAX_C_NODE;
    const unsigned midplane_d = d % BGQTopology::MAX_D_NODE;
    LOG_TRACE_MSG(
            "(" << midplane_a << "," << midplane_b << "," << midplane_c << "," << midplane_d << "," << e << ") " <<
            "within midplane"
            );

    BGQTopology::nodeCardnodeJtagFromABCDE(
            &nodeboard,
            &computecard,
            midplane_a,
            midplane_b,
            midplane_c,
            midplane_d,
            e
            );
    LOG_TRACE_MSG(
            "node board: " << BGQTopology::nodeCardNameFromPos( nodeboard ) << " " <<
            "compute card: " << BGQTopology::processorCardNameFromJtagPort( computecard )
            );

    return Uci(
            _midplane + "-" +
            std::string( BGQTopology::nodeCardNameFromPos(nodeboard) ) + "-" +
            std::string( BGQTopology::processorCardNameFromJtagPort(computecard) )
            );
}

void
Block::querySmallBlock(
        const cxxdb::ConnectionPtr& db
        )
{
    // get the list of nodeboards used by the small block
    _statement = db->prepareQuery(
            std::string("SELECT ") +
            BGQDB::DBTSmallblock::POSINMACHINE_COL + "," +
            BGQDB::DBTSmallblock::NODECARDPOS_COL +
            " FROM " +
            this->getTableName() +
            " WHERE " +
            BGQDB::DBTSmallblock::BLOCKID_COL + "=? " +
            this->getQualifier() +
            " ORDER BY " +
            BGQDB::DBTSmallblock::NODECARDPOS_COL,
            boost::assign::list_of(BGQDB::DBTSmallblock::BLOCKID_COL)
            );
    _statement->parameters()[ BGQDB::DBTSmallblock::BLOCKID_COL ].set( _id );
    
    const cxxdb::ResultSetPtr results = _statement->execute();

    // offset is maximum until we can iterate through all the node boards in this
    // small block to calculate which one has the smallest coordinates
    _offset[0] = BGQTopology::MAX_A_NODE;
    _offset[1] = BGQTopology::MAX_B_NODE;
    _offset[2] = BGQTopology::MAX_C_NODE;
    _offset[3] = BGQTopology::MAX_D_NODE;
    _offset[4] = BGQTopology::MAX_E_NODE;

    while ( results->fetch() ) {
        if ( _midplane.empty() ) {
            _midplane = results->columns()[ BGQDB::DBTSmallblock::POSINMACHINE_COL ].getString();
            LOG_TRACE_MSG( "midplane " << _midplane );
        }

        const std::string id(
                results->columns()[ BGQDB::DBTSmallblock::NODECARDPOS_COL ].getString() 
                );
        const int board = BGQTopology::nodeCardFromName( id.c_str() );
        for ( unsigned i = 0; i < JTAG_COMPUTE_PORTS_PER_NODE_CARD; ++i ) {
            const BGQTopology::Coord coordinates = BGQTopology::ABCDEglobalMap[board][i];
            if ( coordinates.a < _offset[0] ) _offset[0] = coordinates.a;
            if ( coordinates.b < _offset[1] ) _offset[1] = coordinates.b;
            if ( coordinates.c < _offset[2] ) _offset[2] = coordinates.c;
            if ( coordinates.d < _offset[3] ) _offset[3] = coordinates.d;
            if ( coordinates.e < _offset[4] ) _offset[4] = coordinates.e;
        }
    }

    LOG_TRACE_MSG(
            "small block offset (" <<
            _offset[0] << "," <<
            _offset[1] << "," <<
            _offset[2] << "," <<
            _offset[3] << "," <<
            _offset[4] << ")"
            );
}

void
Block::queryLargeBlock(
        const cxxdb::ConnectionPtr& db
        )
{
    // large blocks have no offset by definition
    _offset[0] = 0;
    _offset[1] = 0;
    _offset[2] = 0;
    _offset[3] = 0;
    _offset[4] = 0;

    _statement = db->prepareQuery(
            std::string("SELECT ") +
            BGQDB::DBTBpblockmap::BPID_COL + "," +
            BGQDB::DBTBpblockmap::ACOORD_COL + "," +
            BGQDB::DBTBpblockmap::BCOORD_COL + "," +
            BGQDB::DBTBpblockmap::CCOORD_COL + "," +
            BGQDB::DBTBpblockmap::DCOORD_COL + 
            " FROM " +
            this->getTableName() +
            " WHERE " +
            BGQDB::DBTBpblockmap::BLOCKID_COL + "=? AND " +
            BGQDB::DBTBpblockmap::ACOORD_COL + "=? AND " +
            BGQDB::DBTBpblockmap::BCOORD_COL + "=? AND " +
            BGQDB::DBTBpblockmap::CCOORD_COL + "=? AND " +
            BGQDB::DBTBpblockmap::DCOORD_COL + "=? " +
            this->getQualifier(),
            boost::assign::list_of
            (BGQDB::DBTBpblockmap::BLOCKID_COL)
            (BGQDB::DBTBpblockmap::ACOORD_COL)
            (BGQDB::DBTBpblockmap::BCOORD_COL)
            (BGQDB::DBTBpblockmap::CCOORD_COL)
            (BGQDB::DBTBpblockmap::DCOORD_COL)
            );
    _statement->parameters()[ BGQDB::DBTBpblockmap::BLOCKID_COL ].set( _id );

    // we will bind the midplane coordinates as additional parameters once we know
    // the rank's coordinates in Block::findNodeLocation
}

void
Block::executeLargeBlock(
        const BG_CoordinateMapping_t& coordinates
        )
{
    // convert the coordinates of the rank into midplane coordinates to find
    // the midplane location string where this rank ran
    const unsigned midplane_a = coordinates.a / BGQTopology::MAX_A_NODE;
    const unsigned midplane_b = coordinates.b / BGQTopology::MAX_B_NODE;
    const unsigned midplane_c = coordinates.c / BGQTopology::MAX_C_NODE;
    const unsigned midplane_d = coordinates.d / BGQTopology::MAX_D_NODE;
    _statement->parameters()[ BGQDB::DBTBpblockmap::ACOORD_COL ].cast( midplane_a );
    _statement->parameters()[ BGQDB::DBTBpblockmap::BCOORD_COL ].cast( midplane_b );
    _statement->parameters()[ BGQDB::DBTBpblockmap::CCOORD_COL ].cast( midplane_c );
    _statement->parameters()[ BGQDB::DBTBpblockmap::DCOORD_COL ].cast( midplane_d );

    const cxxdb::ResultSetPtr results = _statement->execute();
    if ( !results->fetch() ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error(
                    "could not find midplane (" +
                    boost::lexical_cast<std::string>(midplane_a) + "," +
                    boost::lexical_cast<std::string>(midplane_b) + "," +
                    boost::lexical_cast<std::string>(midplane_c) + "," +
                    boost::lexical_cast<std::string>(midplane_d) + ") in block " +
                    _id
                    )
                );
    }

    const unsigned a = results->columns()[ BGQDB::DBTBpblockmap::ACOORD_COL ].getInt32();
    const unsigned b = results->columns()[ BGQDB::DBTBpblockmap::BCOORD_COL ].getInt32();
    const unsigned c = results->columns()[ BGQDB::DBTBpblockmap::CCOORD_COL ].getInt32();
    const unsigned d = results->columns()[ BGQDB::DBTBpblockmap::DCOORD_COL ].getInt32();
    LOG_TRACE_MSG(
            results->columns()[ BGQDB::DBTBpblockmap::BPID_COL ].getString() << " (" <<
            a << "," << b << "," << c << "," << d << ")"
            );
    _midplane = results->columns()[ BGQDB::DBTBpblockmap::BPID_COL ].getString();
}

} // locate_rank
} // handlers
} // server
} // runjob

