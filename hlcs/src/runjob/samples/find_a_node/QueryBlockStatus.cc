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
#include "QueryBlockStatus.h"

#include "logging.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/assign.hpp>
#include <boost/throw_exception.hpp>

#include <sstream>
#include <stdexcept>

LOG_DECLARE_FILE( find_a_node::log );

namespace find_a_node {

QueryBlockStatus::QueryBlockStatus(
        const std::string& id
        ) :
    _connection(),
    _statement(),
    _id( id ),
    _sql(),
    _status( BGQDB::INVALID_STATE ),
    _modified()
{
    // initialize BGQDB api
    const bgq::utility::Properties::Ptr properties(
            bgq::utility::Properties::create()
            );
    BGQDB::init( properties, 1 );

    _connection = BGQDB::DBConnectionPool::instance().getConnection();
    if ( !_connection ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error( "could not get database connection" )
                );
    }

    _sql  <<
        "SELECT " << BGQDB::DBTBlock::STATUS_COL << "," <<
        BGQDB::DBTBlock::STATUSLASTMODIFIED_COL <<
        " FROM " << BGQDB::DBTBlock().getTableName() << 
        " WHERE " << 
        BGQDB::DBTBlock::BLOCKID_COL << "= ? "
        ;
    _statement = _connection->prepareQuery(
            _sql.str(),
            boost::assign::list_of( BGQDB::DBTBlock::BLOCKID_COL )
            );
    _statement->parameters()[BGQDB::DBTBlock::BLOCKID_COL].set( _id );
}

void
QueryBlockStatus::execute()
{
    const cxxdb::ResultSetPtr results = _statement->execute();
    if ( !results->fetch() ) {
        LOG_ERROR_MSG( "could not get block status for block '" << _id << "'" );
        return;
    }
                   
    const cxxdb::Columns& columns = results->columns();
    const std::string status = columns[ BGQDB::DBTBlock::STATUS_COL ].getString();
    if ( status == BGQDB::BLOCK_FREE ) {
        _status = BGQDB::FREE;
    } else if ( status == BGQDB::BLOCK_ALLOCATED ) {
        _status = BGQDB::ALLOCATED;
    } else if ( status ==  BGQDB::BLOCK_BOOTING ) {
        _status = BGQDB::BOOTING;
    } else if ( status == BGQDB::BLOCK_INITIALIZED ) {
        _status = BGQDB::INITIALIZED;
    } else if ( status == BGQDB::BLOCK_TERMINATING ) {
        _status = BGQDB::TERMINATING;
    } else {
        _status = BGQDB::INVALID_STATE;
    }

    _modified = columns[ BGQDB::DBTBlock::STATUSLASTMODIFIED_COL ].getTimestamp();

    LOG_DEBUG_MSG( 
            _id << ": " << 
            columns[ BGQDB::DBTBlock::STATUS_COL ].getString() <<
            " " <<
            _modified
            );
}

} // find_a_node

