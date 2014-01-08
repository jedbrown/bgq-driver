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
#include "server/handlers/locate_rank/HistoryBlock.h"
#include "server/handlers/locate_rank/HistoryJob.h"

#include <db/include/api/tableapi/gensrc/DBTBlock_history.h>
#include <db/include/api/tableapi/gensrc/DBTBpblockmap_history.h>
#include <db/include/api/tableapi/gensrc/DBTSmallblock_history.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/throw_exception.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

HistoryBlock::HistoryBlock(
        const cxxdb::ConnectionPtr& db,
        const boost::shared_ptr<HistoryJob>& job
        ) : 
    Block( job )
{
    // there can be multiple blocks with the same name in the history table, so we want to
    // get the creation ID of the first block that entered the bgqblock_history table with
    // a creation date column before the start time of the job
    const cxxdb::QueryStatementPtr statement = db->prepareQuery(
            std::string("SELECT ") +
            BGQDB::DBTBlock_history::CREATIONID_COL + ", " +
            BGQDB::DBTBlock_history::NUMCNODES_COL + 
            " FROM " + BGQDB::DBTBlock_history().getTableName() +
            " WHERE " +
            BGQDB::DBTBlock_history::BLOCKID_COL + "=? AND " +
            BGQDB::DBTBlock_history::CREATEDATE_COL + "<?" +
            " ORDER BY " +
            BGQDB::DBTBlock_history::ENTRYDATE_COL + " DESC",   // descending so we get the most recently 
            boost::assign::list_of                              // created block ordered first
            ( BGQDB::DBTBlock_history::BLOCKID_COL )
            ( BGQDB::DBTBlock_history::CREATEDATE_COL )
            );
    statement->parameters()[ BGQDB::DBTBlock_history::BLOCKID_COL ].set( job->block() );
    statement->parameters()[ BGQDB::DBTBlock_history::CREATEDATE_COL ].set( job->startTime() );
    const cxxdb::ResultSetPtr results = statement->execute();

    if ( !results->fetch() ) {
        BOOST_THROW_EXCEPTION(
                Exception(
                    "Could not find block '" +
                    job->block() + " in " + BGQDB::DBTBlock_history().getTableName() + " table created before " +
                    boost::lexical_cast<std::string>( job->startTime() ),
                    runjob::commands::error::block_not_found
                    )
                );
    }

    _creationId = results->columns()[ BGQDB::DBTBlock_history::CREATIONID_COL ].getInt32();
    LOG_TRACE_MSG( "creation ID " << _creationId );
    LOG_TRACE_MSG( results->columns()[ BGQDB::DBTBlock_history::NUMCNODES_COL ].getInt32() << " compute nodes" );

    _small = static_cast<uint32_t>(results->columns()[ BGQDB::DBTBlock_history::NUMCNODES_COL ].getInt32()) < BGQDB::Nodes_Per_Midplane;
}

std::string
HistoryBlock::getTableName() const
{
    return _small ? BGQDB::DBTSmallblock_history().getTableName() : BGQDB::DBTBpblockmap_history().getTableName();
}

std::string
HistoryBlock::getQualifier() const
{
    std::ostringstream os;
    os << "AND ";
    if ( _small ) {
        os << BGQDB::DBTSmallblock_history::CREATIONID_COL << "=" << _creationId;
    } else {
        os << BGQDB::DBTBpblockmap_history::CREATIONID_COL << "=" << _creationId;
    }

    return os.str();
}

} // locate_rank
} // handlers
} // server
} // runjob

