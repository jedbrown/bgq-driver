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

#include <control/include/bgqconfig/BGQTopology.h>

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/job/SubBlockRas.h>
#include <db/include/api/tableapi/gensrc/DBTJob.h>
#include <db/include/api/tableapi/DBConnectionPool.h>

#include <utility/include/Log.h>

#include <boost/foreach.hpp>

#include <set>

LOG_DECLARE_FILE( "database" );

namespace BGQDB {
namespace job {

const cxxdb::ResultSetPtr
executeSql(
        const cxxdb::ConnectionPtr& connection,
        const std::string& block,
        const bgq::util::Location& location
        )
{
    return connection->query(
                std::string("SELECT ") +
                DBTJob::ID_COL + ", " +
                DBTJob::CORNER_COL + ", " +
                DBTJob::SHAPEA_COL + ", " +
                DBTJob::SHAPEB_COL + ", " +
                DBTJob::SHAPEC_COL + ", " +
                DBTJob::SHAPED_COL + ", " +
                DBTJob::SHAPEE_COL + " " +
                "FROM " + BGQDB::DBTJob().getTableName() + " " +
                "WHERE " + BGQDB::DBTJob::BLOCKID_COL + "='" + block + "' AND " +
                BGQDB::DBTJob::CORNER_COL +
                " like '" + location.getMidplaneLocation() + "%'"
            );
}

std::set<Id>
impl(
        const cxxdb::ConnectionPtr& connection,
        const std::string& block,
        const bgq::util::Location& location
    )
{
    std::set<Id> result;

    const cxxdb::ResultSetPtr results(
            executeSql( connection, block, location )
            );

    // two-dimensional array for every node within the midplane, initialize each job to zero
    Id midplane[ bgq::util::Location::NodeBoardsOnMidplane ][ bgq::util::Location::ComputeCardsOnNodeBoard ];
    for ( unsigned i = 0; i < bgq::util::Location::NodeBoardsOnMidplane; ++i ) {
        for ( unsigned j = 0; j < bgq::util::Location::ComputeCardsOnNodeBoard; ++j ) {
            midplane[i][j] = 0;
        }
    }

    while ( results->fetch() ) {
        const cxxdb::Columns& columns = results->columns();
        LOG_DEBUG_MSG(
            columns[ DBTJob::ID_COL ].getInt64() << " " <<
            columns[ DBTJob::CORNER_COL ].getString() << " " <<
            columns[ DBTJob::SHAPEA_COL ].getInt32() << "x" <<
            columns[ DBTJob::SHAPEB_COL ].getInt32() << "x" <<
            columns[ DBTJob::SHAPEC_COL ].getInt32() << "x" <<
            columns[ DBTJob::SHAPED_COL ].getInt32() << "x" <<
            columns[ DBTJob::SHAPEE_COL ].getInt32()
            );

        // extract the node board and compute card number from corner location string so we can
        // convert them to torus coordinates within the midplane
        const bgq::util::Location corner( columns[ DBTJob::CORNER_COL ].getString() );
        const BGQTopology::Coord corner_coordinates(
                BGQTopology::ABCDEglobalMap[ corner.getNodeBoard() ][ corner.getComputeCard() ]
                );
        LOG_TRACE_MSG( 
                "corner (" << 
                corner_coordinates.a << "," <<
                corner_coordinates.b << "," <<
                corner_coordinates.c << "," <<
                corner_coordinates.d << "," <<
                corner_coordinates.e << ")"
                );

        // iterate through the entire job shape
        for ( int a = 0; a < columns[ DBTJob::SHAPEA_COL ].getInt32(); ++a ) {
            for ( int b = 0; b < columns[ DBTJob::SHAPEB_COL ].getInt32(); ++b ) {
                for ( int c = 0; c < columns[ DBTJob::SHAPEC_COL ].getInt32(); ++c ) {
                    for ( int d = 0; d < columns[ DBTJob::SHAPED_COL ].getInt32(); ++d ) {
                        for ( int e = 0; e < columns[ DBTJob::SHAPEE_COL ].getInt32(); ++e ) {
                            // get the node board and compute card location for each node participating
                            // in the job
                            int board = 0;
                            int compute = 0;
                            BGQTopology::nodeCardnodeJtagFromABCDE(
                                    &board,
                                    &compute,
                                    corner_coordinates.a + a,
                                    corner_coordinates.b + b,
                                    corner_coordinates.c + c,
                                    corner_coordinates.d + d,
                                    corner_coordinates.e + e
                                    );

                            // remember this node is running a job
                            midplane[ board ][ compute ] = columns[ DBTJob::ID_COL ].getInt64();
                            LOG_TRACE_MSG(
                                    location.getMidplaneLocation() << "-N" <<
                                    std::setw(2) << std::setfill('0') << board <<
                                    std::setw(1) << std::setfill(' ') << "-J" << 
                                    std::setw(2) << std::setfill('0') << compute <<
                                    std::setw(1) << std::setfill(' ') << " " <<
                                    midplane[ board ][ compute ]
                                    );
                        }
                    }
                }
            }
        }
    }

    if ( location.getType() == bgq::util::Location::ComputeCardOnNodeBoard ) {
        const Id job = midplane[location.getNodeBoard()][location.getComputeCard()];
        if ( job ) result.insert( job );
    } else if ( location.getType() == bgq::util::Location::NodeBoard ) {
        // include any job using any node in this board
        for ( unsigned i = 0; i < BGQDB::Nodes_Per_Node_Board; ++i ) {
            const Id job = midplane[location.getNodeBoard()][i];
            if ( job ) result.insert( job );
        }
    }

    return result;
}

std::vector<Id>
subBlockRas(
        const std::string& block,
        const bgq::util::Location& location
    )
{
    std::vector<Id> result;

    switch ( location.getType() ) {
        case bgq::util::Location::ComputeCardOnNodeBoard:
        case bgq::util::Location::NodeBoard:
            break;
        default:
            LOG_DEBUG_MSG( location << " is not a compute node or node board" );
            return result;
    }

    const cxxdb::ConnectionPtr connection(
            DBConnectionPool::instance().getConnection()
            );
    if ( !connection ) {
        LOG_WARN_MSG( "could not get database connection" );
        return result;
    }

    try {
        const std::set<Id> jobs = impl( connection, block, location );
        BOOST_FOREACH( const Id i, jobs ) {
            result.push_back( i );
            LOG_TRACE_MSG( "found job " << i );
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return result;
}

} // job
} // BGQDB
