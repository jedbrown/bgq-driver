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

#include "server/handlers/locate_rank/ActiveBlock.h"
#include "server/handlers/locate_rank/Exception.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTBpblockmap.h>
#include <db/include/api/tableapi/gensrc/DBTSmallblock.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/assign/list_of.hpp>
#include <boost/throw_exception.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

ActiveBlock::ActiveBlock(
        const cxxdb::ConnectionPtr& db,
        const boost::shared_ptr<Job>& job
        ) : 
    Block( job )
{
    // get number of compute nodes for this block to see if it is
    // a small or large block, this will dictate how we prepare
    // additional queries
    const cxxdb::QueryStatementPtr statement = db->prepareQuery(
            std::string("SELECT ") +
            BGQDB::DBTBlock::NUMCNODES_COL + "," +
            BGQDB::DBTBlock::CREATIONID_COL +
            " FROM " +
            BGQDB::DBTBlock().getTableName() +
            " WHERE " +
            BGQDB::DBTBlock::BLOCKID_COL + "=?",
            boost::assign::list_of(BGQDB::DBTBlock::BLOCKID_COL)
            );
    statement->parameters()[ BGQDB::DBTBlock::BLOCKID_COL ].set( _id );

    const cxxdb::ResultSetPtr results = statement->execute();
    if ( !results->fetch() ) {
        BOOST_THROW_EXCEPTION(
                Exception(
                    "could not find block '" + _id + "' in table " +
                    BGQDB::DBTBlock().getTableName(),
                    runjob::commands::error::block_not_found
                    )
                );
    }

    LOG_TRACE_MSG(
            results->columns()[ BGQDB::DBTBlock::NUMCNODES_COL ].getInt32() << " compute nodes"
            );

    _small = static_cast<uint32_t>(results->columns()[ BGQDB::DBTBlock::NUMCNODES_COL ].getInt32()) < BGQDB::Nodes_Per_Midplane;
    _creationId = results->columns()[ BGQDB::DBTBlock::CREATIONID_COL ].getInt32();
    LOG_TRACE_MSG( "creation ID " << _creationId );
}

std::string
ActiveBlock::getTableName() const
{
    return _small ? BGQDB::DBTSmallblock().getTableName() : BGQDB::DBTBpblockmap().getTableName();
}

} // locate_rank
} // handlers
} // server
} // runjob

