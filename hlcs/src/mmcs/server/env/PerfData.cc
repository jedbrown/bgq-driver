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

#include "PerfData.h"

#include "McServerConnection.h"

#include <db/include/api/tableapi/gensrc/DBTComponentperf.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

PerfData::PerfData(
        boost::asio::io_service& io_service
        ) :
    Polling( io_service, 300 )
{

}

cxxdb::ConnectionPtr
PerfData::prepareInserts(
        cxxdb::UpdateStatementPtr& perfInsert
        )
{
    const cxxdb::ConnectionPtr result = BGQDB::DBConnectionPool::Instance().getConnection();
    if ( !result ) {
        return result;
    }

    // prepare insert statements
    BGQDB::ColumnsBitmap columns;
    columns.set();
    BGQDB::DBTComponentperf cp( columns );
    perfInsert = result->prepareUpdate(
            cp.getInsertStatement(),
            cp.calcColumnNames()
            );

    return result;
}

void
PerfData::impl(
        const McServerConnection::Ptr& mc_server
        )
{
    cxxdb::ConnectionPtr connection;
    cxxdb::UpdateStatementPtr insert;
    connection = this->prepareInserts( insert );

    if ( !connection ) {
        LOG_ERROR_MSG("Could not get database connection.");
        return;
    }

    MCServerMessageSpec::ReadPerfStatsRequest request;
    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &PerfData::readHandler,
                boost::static_pointer_cast<PerfData>( shared_from_this() ),
                _1,
                mc_server,
                connection,
                insert
                )
            );
}

void
PerfData::readHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& insert
        )
{
    MCServerMessageSpec::ReadPerfStatsReply perfReply;
    perfReply.read( response );

    for (unsigned ss = 0; ss < perfReply._statSet.size(); ++ss) {
        for (unsigned dp = 0; dp < perfReply._statSet[ss]._dataPoints.size(); ++dp) {
            bool truncated( false );
            insert->parameters()[ BGQDB::DBTComponentperf::ID_COL ].set( perfReply._statSet[ss]._dataPoints[dp]._id, &truncated );
            insert->parameters()[ BGQDB::DBTComponentperf::COMPONENT_COL ].set( perfReply._statSet[ss]._dataPoints[dp]._component, &truncated );
            insert->parameters()[ BGQDB::DBTComponentperf::FUNCTION_COL ].set( perfReply._statSet[ss]._dataPoints[dp]._function, &truncated );
            insert->parameters()[ BGQDB::DBTComponentperf::SUBFUNCTION_COL ].set( perfReply._statSet[ss]._dataPoints[dp]._subfunction, &truncated );

            // convert from microseconds to seconds
            insert->parameters()[ BGQDB::DBTComponentperf::DURATION_COL ].cast(
                    static_cast<double>(perfReply._statSet[ss]._dataPoints[dp]._duration) / 1000000
                    );

            insert->parameters()[ BGQDB::DBTComponentperf::ENTRYDATE_COL ].set(
                    boost::posix_time::from_time_t(
                        perfReply._statSet[ss]._dataPoints[dp]._timestamp
                        )
                    );
            insert->parameters()[ BGQDB::DBTComponentperf::DETAIL_COL ].set( perfReply._statSet[ss]._dataPoints[dp]._otherData, &truncated );
            insert->parameters()[ BGQDB::DBTComponentperf::QUALIFIER_COL ].set( perfReply._statSet[ss]._dataPoints[dp]._qualifier, &truncated );
            insert->parameters()[ BGQDB::DBTComponentperf::MODE_COL ].set( perfReply._statSet[ss]._dataPoints[dp]._mode, &truncated );

            try {
                insert->execute();
            } catch ( const std::exception& e ) {
                LOG_ERROR_MSG( e.what() );
            }
        }
    }

    this->wait();
}

} } } // namespace mmcs::server::env
