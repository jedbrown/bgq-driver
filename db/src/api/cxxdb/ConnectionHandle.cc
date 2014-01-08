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

#include "cxxdb/ConnectionHandle.h"

#include "cxxdb/diagnostics.h"
#include "cxxdb/EnvironmentHandle.h"

#include <utility/include/Log.h>

#include <boost/regex.hpp>

#include <sqlext.h>

#include <sstream>

LOG_DECLARE_FILE( "database" );

static std::string protectConnectString( const std::string& connect_string )
{
    static const boost::regex pwd_re( "[Pp][Ww][Dd]\\s*=.*?;" );
    return boost::regex_replace( connect_string, pwd_re, "PWD=xxx;" );
}

namespace cxxdb {

ConnectionHandle::ConnectionHandle(
        EnvironmentHandle& env_handle
    )
{
    LOG_DEBUG_MSG( "Creating database connection handle..." );

    SQLRETURN rc;

    rc = SQLAllocHandle(
            SQL_HANDLE_DBC,
            env_handle.getSqlHandle(),
            &_hdbc
        );

    if ( rc == SQL_SUCCESS ) {
        LOG_DEBUG_MSG( "Created database connection handle " << _hdbc );
        return;
    }

    checkResult( "SQLAllocHandle(DBC)", rc, SQL_HANDLE_DBC, _hdbc );
}


void ConnectionHandle::setAttribute(
        SQLINTEGER attr,
        SQLPOINTER value_ptr,
        SQLINTEGER string_len
    )
{
    LOG_DEBUG_MSG( "Setting attribute " << attr << " on " << _hdbc );

    SQLRETURN rc = SQLSetConnectAttr(
            _hdbc,
            attr,
            value_ptr,
            string_len
        );

    checkResult( "SQLSetConnectAttr", rc, SQL_HANDLE_DBC, _hdbc );
}


void ConnectionHandle::connect(
        const std::string& db_name,
        const std::string* user_name_p,
        const std::string* pwd_p
    )
{
    LOG_DEBUG_MSG( "Connecting to database '" << db_name << "'..." );

    SQLRETURN rc = SQLConnect(
            _hdbc,
            (SQLCHAR*)(db_name.c_str()), SQL_NTS,
            (SQLCHAR*)(user_name_p ? user_name_p->c_str() : NULL), SQL_NTS,
            (SQLCHAR*)(pwd_p ? pwd_p->c_str() : NULL), SQL_NTS
        );

    if ( rc == SQL_SUCCESS ) {
        LOG_DEBUG_MSG( "Connected to '" << db_name << "'." );
        return;
    }

    checkResult( "SQLConnect", rc, SQL_HANDLE_DBC, _hdbc );
}


std::string ConnectionHandle::driverConnect(
        const std::string& connection_string
    )
{
    std::vector<char> out_connection_string_buf(1024);
    SQLSMALLINT out_connection_string_len;

    LOG_DEBUG_MSG( "Connecting using '" << protectConnectString( connection_string ) << "'" );

    SQLRETURN rc = SQLDriverConnect(
            _hdbc,
            NULL,
            (SQLCHAR*)(connection_string.c_str()), SQL_NTS,
            (SQLCHAR*)(out_connection_string_buf.data()), out_connection_string_buf.size(),
            &out_connection_string_len,
            SQL_DRIVER_NOPROMPT
        );

    if ( rc == SQL_SUCCESS ) {
        std::string out_connection_string( out_connection_string_buf.begin(), out_connection_string_buf.begin() + out_connection_string_len );

        LOG_DEBUG_MSG( "Connected using '" << protectConnectString( connection_string ) << "',"
                       " out connection string='" << protectConnectString( out_connection_string ) << "'" );

        return out_connection_string;

    }

    checkResult( "SQLDriverConnect", rc, SQL_HANDLE_DBC, _hdbc );

    // Shouldn't get here.
    return std::string();
}


void ConnectionHandle::setCommitAutomatically()
{
    LOG_DEBUG_MSG( "Setting autocommit ON on " << _hdbc );

    setAttribute(
            SQL_ATTR_AUTOCOMMIT,
            reinterpret_cast<SQLPOINTER>(SQL_AUTOCOMMIT_ON),
            SQL_NTS
        );
}


void ConnectionHandle::setCommitRequired()
{
    LOG_DEBUG_MSG( "Setting autocommit OFF on " << _hdbc );

    setAttribute(
            SQL_ATTR_AUTOCOMMIT,
            reinterpret_cast<SQLPOINTER>(SQL_AUTOCOMMIT_OFF),
            SQL_NTS
        );
}


void ConnectionHandle::endTransaction(
        SQLSMALLINT completion_type
    )
{
    LOG_DEBUG_MSG( "Ending transaction on " << _hdbc << " with " << completion_type );

    SQLRETURN rc = SQLEndTran( SQL_HANDLE_DBC, _hdbc, completion_type );

    checkResult( "SQLEndTran", rc, SQL_HANDLE_DBC, _hdbc );
}


void ConnectionHandle::disconnect()
{
    LOG_DEBUG_MSG( "Disconnecting " << _hdbc );

    SQLRETURN rc = SQLDisconnect( _hdbc );

    checkResult( "SQLDisconnect", rc, SQL_HANDLE_DBC, _hdbc );
}


ConnectionHandle::~ConnectionHandle()
{
    LOG_DEBUG_MSG( "Destroying database connection handle " << _hdbc );

    SQLRETURN rc = SQLFreeHandle( SQL_HANDLE_DBC, _hdbc );
    _hdbc = SQL_NULL_HDBC;

    if ( rc == SQL_SUCCESS ) {
        return;
    }

    LOG_ERROR_MSG( "Failed to free database connection handle. rc=" << rc );
}


} // namespace cxxdb
