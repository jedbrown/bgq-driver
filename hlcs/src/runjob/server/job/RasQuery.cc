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
#include "server/job/RasQuery.h"

#include "common/logging.h"

#include "server/Job.h"

#include <db/include/api/tableapi/gensrc/DBTEventlog.h>
#include <db/include/api/tableapi/gensrc/DBTJob_history.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

RasQuery::RasQuery(
        const BGQDB::job::Id job,
        const ExitStatus& status
        ) :
    _count(),
    _message(),
    _severity()
{
    try {
        const cxxdb::ConnectionPtr connection(
                BGQDB::DBConnectionPool::instance().getConnection()
                );
        if ( !connection ) {
            LOG_WARN_MSG( "could not get connection" );
            return;
        }

        this->count( connection, job );
        this->message( connection, job, status);
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

void
RasQuery::count(
        const cxxdb::ConnectionPtr& connection,
        const BGQDB::job::Id job
        )
{
    BOOST_ASSERT( connection );

    const cxxdb::ResultSetPtr results = connection->query(
            std::string("SELECT count(*) as count, " + BGQDB::DBTEventlog::SEVERITY_COL + " FROM ") +
            BGQDB::DBTEventlog().getTableName() + 
            " WHERE " +
            BGQDB::DBTEventlog::JOBID_COL + "=" + boost::lexical_cast<std::string>( job ) +
            " AND " +
            BGQDB::DBTEventlog::EVENT_TIME_COL + " >" + 
            " (SELECT " + BGQDB::DBTJob_history::STARTTIME_COL + 
            " FROM " + BGQDB::DBTJob_history().getTableName() +
            " WHERE " + BGQDB::DBTJob_history::ID_COL + "=" + boost::lexical_cast<std::string>( job ) + ")" +
            " GROUP BY severity"
            );
    while ( results->fetch() ) {
        _count.insert(
                SeverityCount::value_type(
                    results->columns()[ BGQDB::DBTEventlog::SEVERITY_COL ].getString(),
                    results->columns()[ "count" ].getInt32()
                    )
                );
        LOG_DEBUG_MSG(
                    results->columns()[ "count" ].getInt32() << " " <<
                    results->columns()[ BGQDB::DBTEventlog::SEVERITY_COL ].getString() << " " <<
                    "ras events"
                );
    }
}

void
RasQuery::message(
        const cxxdb::ConnectionPtr& connection,
        const BGQDB::job::Id job,
        const ExitStatus& status
        )
{
    if ( _count.empty() ) return;

    cxxdb::ResultSetPtr results;
    if ( int recid = status.getRecId() ) {
        results = connection->query(
                std::string("SELECT ") +
                BGQDB::DBTEventlog::MESSAGE_COL + "," +
                BGQDB::DBTEventlog::SEVERITY_COL + 
                " FROM " +
                BGQDB::DBTEventlog().getTableName() + 
                " WHERE " +
                BGQDB::DBTEventlog::RECID_COL + "=" + boost::lexical_cast<std::string>( recid )
                );
    } else {
        results = connection->query(
                std::string("SELECT ") +
                BGQDB::DBTEventlog::MESSAGE_COL + "," +
                BGQDB::DBTEventlog::SEVERITY_COL + 
                " FROM " +
                BGQDB::DBTEventlog().getTableName() + 
                " WHERE " +
                BGQDB::DBTEventlog::JOBID_COL + "=" + boost::lexical_cast<std::string>( job ) +
                " AND " +
                BGQDB::DBTEventlog::EVENT_TIME_COL + " >" +
                " (SELECT " + BGQDB::DBTJob_history::STARTTIME_COL + 
                " FROM " + BGQDB::DBTJob_history().getTableName() +
                " WHERE " + BGQDB::DBTJob_history::ID_COL + "=" + boost::lexical_cast<std::string>( job ) + ") " +
                " ORDER BY " + BGQDB::DBTEventlog::EVENT_TIME_COL + " desc FETCH first row only"
                );
    }
    results->fetch();
    _message = results->columns()[ BGQDB::DBTEventlog::MESSAGE_COL ].getString();
    _severity = results->columns()[ BGQDB::DBTEventlog::SEVERITY_COL].getString();
    LOG_TRACE_MSG( _message );
}

void
RasQuery::add(
        const message::ExitJob::Ptr& message
        ) const
{
    message->_rasCount = _count;
    message->_rasMessage = _message;
    message->_rasSeverity = _severity;
}


} // job
} // server
} // runjob

