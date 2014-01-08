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


#include "dbConnectionPool.hpp"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <utility/include/Log.h>

#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace dbConnectionPool {


void initialize(
        bgq::utility::Properties::Ptr bg_properties_ptr
    )
{
    static const string database_connection_pool_settings_properties_section_name( "bgws" );

    LOG_DEBUG_MSG( "Initializing BGQDB..." );

    BGQDB::init(
            bg_properties_ptr,
            database_connection_pool_settings_properties_section_name
        );

    LOG_INFO_MSG( "Finished initializing BGQDB. Connection pool size=" << BGQDB::DBConnectionPool::instance().size() );
}


cxxdb::ConnectionPtr getConnection()
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    if ( ! conn_ptr ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "Failed to get database connection from connection pool." ) );
    }

    return conn_ptr;
}


} } // namespace bgws::dbConnectionPool
