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

#include "server/handlers/locate_rank/Exception.h"
#include "server/handlers/locate_rank/HistoryJob.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTJob_history.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/assign/list_of.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

HistoryJob::HistoryJob(
        const cxxdb::ConnectionPtr& connection,
        const BGQDB::job::Id job
        ) :
    Job( job )
{
    const cxxdb::QueryStatementPtr statement = connection->prepareQuery(
            std::string("SELECT ") + 
            BGQDB::DBTJob_history::BLOCKID_COL + ", " +
            BGQDB::DBTJob_history::USERNAME_COL + ", " +
            BGQDB::DBTJob_history::STARTTIME_COL + ", " +
            BGQDB::DBTJob_history::ENTRYDATE_COL + ", " +
            BGQDB::DBTJob_history::MAPPING_COL + ", " +
            BGQDB::DBTJob_history::PROCESSESPERNODE_COL + ", " +
            BGQDB::DBTJob_history::NP_COL + ", " +
            BGQDB::DBTJob_history::CORNER_COL + ", " +
            BGQDB::DBTJob_history::SHAPEA_COL + ", " +
            BGQDB::DBTJob_history::SHAPEB_COL + ", " +
            BGQDB::DBTJob_history::SHAPEC_COL + ", " +
            BGQDB::DBTJob_history::SHAPED_COL + ", " +
            BGQDB::DBTJob_history::SHAPEE_COL + " " +
            "FROM " + BGQDB::DBTJob_history().getTableName() + " " +
            "WHERE " + BGQDB::DBTJob_history::ID_COL + "=?",
            boost::assign::list_of
            ( BGQDB::DBTJob_history::ID_COL )
            );
    statement->parameters()[ BGQDB::DBTJob_history::ID_COL ].set( static_cast<int64_t>(job) );

    const cxxdb::ResultSetPtr results = statement->execute();
    if ( !results->fetch() ) {
        BOOST_THROW_EXCEPTION(
                Exception(
                    "could not find job " +
                    boost::lexical_cast<std::string>( job ) +
                    " in bgqjob_history table",
                    runjob::commands::error::job_not_found
                    )
                );
    }
            
    const cxxdb::Columns& columns = results->columns();

    _shape[0] = columns[ BGQDB::DBTJob_history::SHAPEA_COL ].getInt32();
    _shape[1] = columns[ BGQDB::DBTJob_history::SHAPEB_COL ].getInt32();
    _shape[2] = columns[ BGQDB::DBTJob_history::SHAPEC_COL ].getInt32();
    _shape[3] = columns[ BGQDB::DBTJob_history::SHAPED_COL ].getInt32();
    _shape[4] = columns[ BGQDB::DBTJob_history::SHAPEE_COL ].getInt32();
    _shape[5] = columns[ BGQDB::DBTJob_history::PROCESSESPERNODE_COL ].getInt32();

    _blockId = columns[ BGQDB::DBTJob_history::BLOCKID_COL ].getString();
    LOG_DEBUG_MSG( "used block " << _blockId );
    _owner = columns[ BGQDB::DBTJob_history::USERNAME_COL ].getString();
    LOG_DEBUG_MSG( "owned by " << _owner );
    _startTime = columns[ BGQDB::DBTJob_history::STARTTIME_COL ].getTimestamp();
    LOG_DEBUG_MSG( "started at " << _startTime );
    LOG_DEBUG_MSG( "ended at " << columns[ BGQDB::DBTJob_history::ENTRYDATE_COL ].getTimestamp() );

    if ( !columns[ BGQDB::DBTJob_history::CORNER_COL ].isNull() ) {
        _corner = columns[ BGQDB::DBTJob_history::CORNER_COL ].getString();
        LOG_DEBUG_MSG( "with corner " << _corner );
    }
    LOG_DEBUG_MSG(
            "with shape " <<
            columns[ BGQDB::DBTJob_history::SHAPEA_COL ].getInt32() << "x" <<
            columns[ BGQDB::DBTJob_history::SHAPEB_COL ].getInt32() << "x" <<
            columns[ BGQDB::DBTJob_history::SHAPEC_COL ].getInt32() << "x" <<
            columns[ BGQDB::DBTJob_history::SHAPED_COL ].getInt32() << "x" <<
            columns[ BGQDB::DBTJob_history::SHAPEE_COL ].getInt32()
            );

    _mapping = Mapping(
            Mapping::getType( columns[BGQDB::DBTJob_history::MAPPING_COL].getString() ),
            columns[ BGQDB::DBTJob_history::MAPPING_COL ].getString(),
            false // skip mapping file validation
            );
    LOG_DEBUG_MSG( "with mapping " << _mapping );

    LOG_DEBUG_MSG( 
            "with " << columns[ BGQDB::DBTJob_history::PROCESSESPERNODE_COL ].getInt32() <<
            " rank" << ( columns[ BGQDB::DBTJob_history::PROCESSESPERNODE_COL ].getInt32() == 1 ? "" : "s" ) <<
            " per node"
            );
    _np = columns[ BGQDB::DBTJob_history::NP_COL ].getInt32();
    LOG_DEBUG_MSG( "with np " << _np );
}

} // locate_rank
} // handlers
} // server
} // runjob

