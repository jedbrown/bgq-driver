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

#include "tableapi/DBConnection.h"

#include "cxxdb/Connection.h"
#include "cxxdb/ConnectionHandle.h"
#include "cxxdb/Environment.h"
#include "cxxdb/exceptions.h"
#include "cxxdb/Transaction.h"

#include <utility/include/Log.h>

#include <boost/shared_ptr.hpp>

#include <stdexcept>
#include <string>

using boost::shared_ptr;

using std::exception;
using std::string;

LOG_DECLARE_FILE( "database" );

#define CATCH_DB_EXCEPTION( doing_what ) \
    catch ( cxxdb::DatabaseException& dbe ) { \
        LOG_ERROR_MSG( "Error " << doing_what << " " << dbe.what() ); \
        return dbe.getSqlrc(); \
    } catch ( std::exception& e ) { \
        LOG_ERROR_MSG( "Error " << doing_what << " " << e.what() ); \
        return SQL_ERROR; \
    } catch ( ... ) { \
        LOG_ERROR_MSG( "Unexpected exception " << doing_what << "." ); \
        return SQL_ERROR; \
    }

namespace BGQDB {

cxxdb::ConnectionPtr DBConnection::createConnection(
        const bgq::utility::Properties& properties,
        const std::string& extra_connection_parameters
    )
{
    string db(properties.getValue("database", "name"));

    shared_ptr<string> user;
    try {
        user.reset(new string(properties.getValue("database", "user")));
    } catch (exception& e) {
        LOG_DEBUG_MSG("No database user in properties file.");
    }

    shared_ptr<string> pwd;
    try {
        pwd.reset(new string(properties.getValue("database", "password")));
    } catch (exception& e) {
        LOG_DEBUG_MSG("No database password in properties file.");
    }

    shared_ptr<string> schema;
    try {
        schema.reset(new string(properties.getValue("database", "schema_name")));
    } catch (exception& e) {
        LOG_DEBUG_MSG("No database schema in properties file.");
    }

    // connect to the database
    LOG_DEBUG_MSG("Connecting to the database " << db);

    cxxdb::ConnectionPtr connection_ptr;
    uint32_t iter = 0;

    while ( ! connection_ptr ) {
        try {
            connection_ptr = cxxdb::Connection::create(
                    db,
                    user.get(),
                    pwd.get(),
                    extra_connection_parameters
                );
        } catch ( cxxdb::DatabaseException& dbe ) {
        	// Check for a specific error code that indicates that a db2profile wasn't sourced.
            if ( ! dbe.getDiagnosticRecords().empty() ) {
                if ( (dbe.getDiagnosticRecords()[0].sqlstate == "IM004") || (dbe.getDiagnosticRecords()[0].native_error == -1390) ) {
                    throw std::runtime_error( "make sure that you have sourced a DB2 profile." );
                }
            }

            LOG_ERROR_MSG("Error connecting to the database. " << dbe.what() );

            if ( ! dbe.getDiagnosticRecords().empty() ) {
                static const string INVALID_PASSWORD_SQL_STATE("08001");
                static const SQLINTEGER INVALID_PASSWORD_NATIVE_ERROR(-30082);

                const cxxdb::DiagnosticRecord &dr(dbe.getDiagnosticRecords().front());

                if ( dr.sqlstate == INVALID_PASSWORD_SQL_STATE && dr.native_error == INVALID_PASSWORD_NATIVE_ERROR ) {
                    LOG_DEBUG_MSG( "Detected invalid username or password, will not retry connection." );
                    throw;
                }

                static const string NO_CONNECT_PRIVILEGE_SQL_STATE("08004");
                static const SQLINTEGER NO_CONNECT_PRIVILEGE_NATIVE_ERROR(-1060);

                if ( dr.sqlstate == NO_CONNECT_PRIVILEGE_SQL_STATE && dr.native_error == NO_CONNECT_PRIVILEGE_NATIVE_ERROR ) {
                    LOG_DEBUG_MSG( "Detected user doesn't have CONNECT authority, will not retry connection." );
                    throw;
                }

            }

            if (++iter == 20) {
                // don't try forever
                throw;
            }

            sleep(1);
            LOG_TRACE_MSG("Retrying database connection");
        }
    }

    LOG_TRACE_MSG("Connected to the database: " << db);

    // if schema is set and schema is not user, set the schema.
    if ( schema ) {
        if ( (! user) || (*schema != *user) ) {
            LOG_DEBUG_MSG( "Setting schema to '" << *schema << "'" );
            connection_ptr->execute(string("set schema ") + *schema);
            try {
                connection_ptr->execute(string("set path = current path, ") + *schema);
            } catch ( cxxdb::DatabaseException& dbe ) {
                static const string DUPLICATE_SCHEMA_NAME("42732");
                const cxxdb::DiagnosticRecord &dr(dbe.getDiagnosticRecords().front());
                if ( dr.sqlstate == DUPLICATE_SCHEMA_NAME ) {
                    LOG_DEBUG_MSG( "current path already has schema name of " << *schema );
                    // fall through
                } else {
                    throw;
                }
            }
        }
    }
    return connection_ptr;
}


DBConnection::DBConnection(
        bgq::utility::Properties::ConstPtr properties,
        const std::string& extra_connection_parameters
    ) :
        _properties(properties),
        _extra_connection_parameters(extra_connection_parameters)
{
    if ( ! _properties ) {
        LOG_ERROR_MSG( "DBConnection invalid because no properties set." );
    }
}


DBConnection::DBConnection(
        bgq::utility::Properties::ConstPtr properties,
        const std::string& extra_connection_parameters,
        cxxdb::ConnectionPtr conn_ptr
    ) :
        _properties(properties),
        _extra_connection_parameters(extra_connection_parameters),
        _connection_ptr(conn_ptr)
{
    if ( ! _properties ) {
        LOG_ERROR_MSG( "DBConnection invalid because no properties set." );
    }
}


DBConnection::~DBConnection()
{
    _transaction_ptr.reset();
    _connection_ptr.reset();
}


SQLRETURN
DBConnection::getStatus()
{
    if ( _connection_ptr ) {
        return SQL_SUCCESS;
    }
    return SQL_ERROR;
}


SQLRETURN
DBConnection::commit()
{
    LOG_TRACE_MSG("Committing the transaction");

    try {
        _connection_ptr->commit();
    } CATCH_DB_EXCEPTION( "committing" )

    LOG_TRACE_MSG("Transaction committed");
    return SQL_SUCCESS;
}


SQLRETURN
DBConnection::connect()
{
    if ( _connection_ptr ) {
        // already connected
        return SQL_SUCCESS;
    }

    if ( ! _properties ) {
        LOG_ERROR_MSG( "Cannot connect because no properties set." );
        return SQL_ERROR;
    }

    try {
        _connection_ptr = createConnection(
                *_properties,
                _extra_connection_parameters
            );
    } CATCH_DB_EXCEPTION( "connecting" )

    return SQL_SUCCESS;
}


SQLRETURN
DBConnection::disconnect()
{
    if ( ! _connection_ptr ) {
        LOG_DEBUG_MSG( "Called disconnect when already disconnected." );
        return SQL_SUCCESS;
    }

    try {
        _transaction_ptr.reset();
        _connection_ptr.reset();
    } CATCH_DB_EXCEPTION( "disconnecting" )

    LOG_TRACE_MSG("Disconnected from the database");

    return SQL_SUCCESS;
}

bool
DBConnection::isAutoCommit()
{
    if ( _transaction_ptr )  return false;
    return true;
}

SQLRETURN
DBConnection::setAutoCommit(
        bool enableAutoCommit
)
{
    try {
        if (isAutoCommit() && enableAutoCommit) {
            return SQL_SUCCESS;   // it's the same
        }

        // set AUTOCOMMIT on/off
        if (enableAutoCommit == false) {
            LOG_TRACE_MSG("Setting autocommit on");
            _transaction_ptr.reset( new cxxdb::Transaction( *_connection_ptr ) );
        } else {
            LOG_TRACE_MSG("Setting autocommit off");
            _transaction_ptr.reset();
        }
    } CATCH_DB_EXCEPTION( "setting autocommit" );

    return SQL_SUCCESS;
}

SQLRETURN
DBConnection::rollback()
{
    try {
        _connection_ptr->rollback();
    } CATCH_DB_EXCEPTION( "rollback" );

    return SQL_SUCCESS;
}


SQLHANDLE
DBConnection::getConnHandle()
{
    if ( ! _connection_ptr ) {
        return SQL_NULL_HANDLE;
    }
    return _connection_ptr->handle().getSqlHandle();
}


SQLHANDLE
DBConnection::getEnvHandle()
{
    if ( ! _connection_ptr ) {
        return SQL_NULL_HANDLE;
    }
    return _connection_ptr->environment().handle().getSqlHandle();
}


SQLRETURN
DBConnection::setSchema(
        const char* schema
)
{
    try {
        _connection_ptr->execute(string("set schema ") + schema);
        _connection_ptr->execute(string("set path = current path, ") + schema);
    } CATCH_DB_EXCEPTION( "setSchema" );

    return SQL_SUCCESS;
}

} // BGQDB
