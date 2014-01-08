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

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"

#include "server/handlers/locate_rank/ActiveJob.h"
#include "server/handlers/locate_rank/Exception.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/assign/list_of.hpp>

#include <boost/throw_exception.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

ActiveJob::ActiveJob(
        const cxxdb::ConnectionPtr& connection,
        BGQDB::job::Id job 
        ) :
    Job( job )
{
    const cxxdb::QueryStatementPtr statement = connection->prepareQuery(
            std::string("SELECT ") + 
            BGQDB::DBTJob::BLOCKID_COL + ", " +
            BGQDB::DBTJob::MAPPING_COL + ", " +
            BGQDB::DBTJob::PROCESSESPERNODE_COL + ", " +
            BGQDB::DBTJob::NP_COL + ", " +
            BGQDB::DBTJob::CORNER_COL + ", " +
            BGQDB::DBTJob::SHAPEA_COL + ", " +
            BGQDB::DBTJob::SHAPEB_COL + ", " +
            BGQDB::DBTJob::SHAPEC_COL + ", " +
            BGQDB::DBTJob::SHAPED_COL + ", " +
            BGQDB::DBTJob::SHAPEE_COL + ", " +
            BGQDB::DBTJob::STARTTIME_COL + " " +
            "FROM " + BGQDB::DBTJob().getTableName() + " " +
            "WHERE " + BGQDB::DBTJob::ID_COL + "=?",
            boost::assign::list_of
            ( BGQDB::DBTJob::ID_COL )
            );
    statement->parameters()[ BGQDB::DBTJob::ID_COL ].set( static_cast<int64_t>(job) );

    const cxxdb::ResultSetPtr results = statement->execute();
    if ( !results->fetch() ) {
        BOOST_THROW_EXCEPTION(
                Exception(
                    "could not find job " +
                    boost::lexical_cast<std::string>( job ) +
                    " in bgqjob table",
                    runjob::commands::error::job_not_found
                    )
                );
    }

    const cxxdb::Columns& columns = results->columns();

    _shape[0] = columns[ BGQDB::DBTJob::SHAPEA_COL ].getInt32();
    _shape[1] = columns[ BGQDB::DBTJob::SHAPEB_COL ].getInt32();
    _shape[2] = columns[ BGQDB::DBTJob::SHAPEC_COL ].getInt32();
    _shape[3] = columns[ BGQDB::DBTJob::SHAPED_COL ].getInt32();
    _shape[4] = columns[ BGQDB::DBTJob::SHAPEE_COL ].getInt32();
    _shape[5] = columns[ BGQDB::DBTJob::PROCESSESPERNODE_COL ].getInt32();
           
    _blockId = columns[ BGQDB::DBTJob::BLOCKID_COL ].getString();
    LOG_DEBUG_MSG( "uses block " << _blockId );
    if ( !columns[ BGQDB::DBTJob::CORNER_COL ].isNull() ) {
        _corner = columns[ BGQDB::DBTJob::CORNER_COL ].getString();
        LOG_DEBUG_MSG( "with corner " << _corner );
    }
    LOG_DEBUG_MSG(
            "with shape " <<
            columns[ BGQDB::DBTJob::SHAPEA_COL ].getInt32() << "x" <<
            columns[ BGQDB::DBTJob::SHAPEB_COL ].getInt32() << "x" <<
            columns[ BGQDB::DBTJob::SHAPEC_COL ].getInt32() << "x" <<
            columns[ BGQDB::DBTJob::SHAPED_COL ].getInt32() << "x" <<
            columns[ BGQDB::DBTJob::SHAPEE_COL ].getInt32()
            );

    _mapping = boost::lexical_cast<Mapping>( columns[ BGQDB::DBTJob::MAPPING_COL ].getString() );
    LOG_DEBUG_MSG( "with mapping " << _mapping );
    LOG_DEBUG_MSG( 
            "with " << columns[ BGQDB::DBTJob::PROCESSESPERNODE_COL ].getInt32() <<
            " rank" << ( columns[ BGQDB::DBTJob::PROCESSESPERNODE_COL ].getInt32() == 1 ? "" : "s" ) <<
            " per node"
            );
    
    _np = columns[ BGQDB::DBTJob::NP_COL ].getInt32();
    LOG_DEBUG_MSG( "with np " << _np );
}

} // locate_rank
} // handlers
} // server
} // runjob

