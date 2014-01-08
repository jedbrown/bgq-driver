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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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


#include "ioUsage.h"

#include "Exception.h"

#include "cxxdb/cxxdb.h"

#include "tableapi/DBConnectionPool.h"

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/throw_exception.hpp>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "database" );


namespace BGQDB {
namespace ioUsage {


namespace statics {

static const unsigned DEFAULT_USAGE_LIMIT(256);

static const string MAX_CONNECTED_NODES_SECTION_NAME("cios");
static const string MAX_CONNECTED_NODES_PROPERTY_NAME("max_connected_nodes");

static unsigned limit = 0;


static boost::shared_ptr<boost::mutex> mutex_ptr;

}


void init()
{
    if ( statics::mutex_ptr )  return; // already initialized.

    try {

        const string &node_usage_limit_str(DBConnectionPool::Instance().getProperties()->getValue( statics::MAX_CONNECTED_NODES_SECTION_NAME, statics::MAX_CONNECTED_NODES_PROPERTY_NAME ));
        statics::limit = lexical_cast<unsigned>( node_usage_limit_str );

    } catch ( std::exception& e ) {

        LOG_WARN_MSG(
                "Failed to get I/O node usage limit from [" << statics::MAX_CONNECTED_NODES_SECTION_NAME << "]." << statics::MAX_CONNECTED_NODES_PROPERTY_NAME << ","
                " using default " << statics::DEFAULT_USAGE_LIMIT << "."
                " Error is " << e.what()
            );

        statics::limit = statics::DEFAULT_USAGE_LIMIT;
    }


    statics::mutex_ptr.reset( new boost::mutex() );
}


unsigned getUsageLimit()
{
    return statics::limit;
}


void update(
        const BGQMidplaneNodeConfig::ComputeCount& cn_counts,
        const std::string& block_id
    )
{
    // Lock around here so only one update can occur at a time, also we don't have multiple processes so mutex is OK.

    // In a transaction,
    //   insert all the cn counts
    //   check if any I/O nodes are now being used beyond the limit.
    //   If any are, ROLLBACK and report an error
    //   Otherwise, COMMIT.

    boost::lock_guard<boost::mutex> lg( *statics::mutex_ptr );

    cxxdb::ConnectionPtr conn_ptr(DBConnectionPool::Instance().getConnection());

    if ( ! conn_ptr ) {
        BOOST_THROW_EXCEPTION( Exception(
                CONNECTION_ERROR,
                "no DB connection"
            ) );
    }

    cxxdb::Transaction xact( *conn_ptr );

    { // Insert all the CN counts.
        cxxdb::UpdateStatementPtr stmt_ptr(conn_ptr->prepareUpdate(
                "INSERT INTO TBGQIoUsage VALUES ( ?, ?, ? )",
                boost::assign::list_of( "blockId" )( "computeNode" )( "count" )
            ));

        stmt_ptr->parameters()["blockId"].set( block_id );

        for ( BGQMidplaneNodeConfig::ComputeCount::const_iterator i(cn_counts.begin()) ; i != cn_counts.end() ; ++i ) {
            stmt_ptr->parameters()["computeNode"].set( i->first );
            stmt_ptr->parameters()["count"].cast( i->second );

            stmt_ptr->execute();
        }
    }

    { // Query to see if any I/O nodes now above the limit.
        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery(
                "SELECT ioNode, count FROM BGQIoUsageSum WHERE count > ? FETCH FIRST ROW ONLY",
                boost::assign::list_of( "limit" )
            ));

        stmt_ptr->parameters()["limit"].cast( statics::limit );

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( rs_ptr->fetch() ) {

            string io_node(rs_ptr->columns()["ioNode"].getString());
            unsigned count(rs_ptr->columns()["count"].as<unsigned>());

            // There's an I/O node being used too much. Fail.
            conn_ptr->rollback();

            LOG_DEBUG_MSG( "failed to update I/O node usage for " << block_id << ", " << io_node << " using " << count << " over limit of " << statics::limit );

            BOOST_THROW_EXCEPTION( UsageExceeded( block_id, io_node, count ) );

            return;
        }
    }

    // If got here then the update was successful and should commit the xact.
    conn_ptr->commit();
}


//---------------------------------------------------------------------
// Class UsageExceeded

namespace statics {


std::string calcUsageExceededMessage(
        const std::string& block_id,
        const std::string& io_node,
        unsigned count
    )
{
    return (string() + "failed to update I/O node usage for " + block_id + ", " + io_node + " using " + lexical_cast<string>( count ) + " over limit of " + lexical_cast<string>( statics::limit ));
}


} // namespace statics


UsageExceeded::UsageExceeded(
        const std::string& block_id,
        const std::string& io_node,
        unsigned count
    ) :
        Exception(
                FAILED,
                statics::calcUsageExceededMessage( block_id, io_node, count )
            ),
        _io_node(io_node),
        _count(count)

{
    // Nothing to do.
}


} } // namespace BGQDB::ioUsage
