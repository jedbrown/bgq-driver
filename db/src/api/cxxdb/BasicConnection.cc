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

#include "BasicConnection.h"

#include "cxxdb/AbstractStatement.h"
#include "cxxdb/ConnectionHandle.h"
#include "cxxdb/Environment.h"
#include "cxxdb/exceptions.h"
#include "cxxdb/QueryStatement.h"
#include "cxxdb/Statement.h"
#include "cxxdb/StatementHandle.h"
#include "cxxdb/Transaction.h"
#include "cxxdb/UpdateStatement.h"

#include <utility/include/Log.h>

#include <boost/numeric/conversion/cast.hpp>

#include <stdexcept>
#include <string>

using std::logic_error;
using std::string;

LOG_DECLARE_FILE( "database" );

#define CHECK_VALID if ( ! isValid() ) { CXXDB_THROW_EXCEPTION( cxxdb::InvalidObjectException( "BasicConnection", __FUNCTION__ ) ); }

namespace cxxdb {

BasicConnection::BasicConnection(
        EnvironmentPtr env_ptr,
        EnvironmentHandle& env_handle,
        const std::string& db_name,
        const std::string* user_name_p,
        const std::string* user_pwd_p,
        const std::string& extra_connection_parameters
    ) :
        _env_ptr(env_ptr),
        _handle_ptr(new ConnectionHandle( env_handle )),
        _transaction(false)
{
    LOG_DEBUG_MSG(
            "Connecting to database '" << db_name << "'" << (user_name_p ? string() + " as '" + *user_name_p + "'" : "") <<
            (extra_connection_parameters.empty() ? "" : (string() + " with connection parameters '" + extra_connection_parameters + "'"))
        );

    string connection_str;

    connection_str += string() + "DSN=" + db_name;

    if ( user_name_p ) {
        connection_str += string() + ";UID=" + *user_name_p;
    }
    if ( user_pwd_p ) {
        connection_str += string() + ";PWD=" + *user_pwd_p;
    }
    if ( ! extra_connection_parameters.empty() ) {
        connection_str += string() + ";" + extra_connection_parameters;
    }

    _handle_ptr->driverConnect(
            connection_str
        );

    LOG_DEBUG_MSG( "Connected." );
}


void BasicConnection::execute(
        const std::string& sql
    )
{
    LOG_DEBUG_MSG( "Executing '" << sql << "'..." );

    CHECK_VALID;

    StatementHandle hstmt( *_handle_ptr );

    hstmt.execDirect( sql );

    LOG_DEBUG_MSG( "Finished executing '" << sql << "'." );
}


void BasicConnection::executeUpdate(
        const std::string& sql,
        uint64_t* affected_row_count_out
    )
{
    LOG_DEBUG_MSG( "Executing update '" << sql << "'..." );

    CHECK_VALID;

    StatementHandle hstmt( *_handle_ptr );

    hstmt.execDirect( sql );

    LOG_DEBUG_MSG( "Finished executing '" << sql << "'." );

    if ( affected_row_count_out ) {
        *affected_row_count_out = boost::numeric_cast<uint64_t>( hstmt.rowCount() );
    }
}


ResultSetPtr BasicConnection::query(
        const std::string& sql
    )
{
    CHECK_VALID;

    QueryStatementPtr query_stmt_ptr( new QueryStatement( shared_from_this(), *_handle_ptr, sql, NoParameterNames ) );
    _stmt_ptrs[query_stmt_ptr.get()] = query_stmt_ptr; // Store a weak ptr to the new statement so can invalidate if conn destroyed.

    ResultSetPtr ret(query_stmt_ptr->_executeRetain());

    return ret;
}


StatementPtr BasicConnection::prepare(
        const std::string& sql,
        const ParameterNames& parameter_names
    )
{
    CHECK_VALID;

    StatementPtr ret( new Statement( shared_from_this(), *_handle_ptr, sql, parameter_names ) );
    _stmt_ptrs[ret.get()] = ret; // Store a weak ptr to the new statement so can invalidate if conn destroyed.
    return ret;
}


UpdateStatementPtr BasicConnection::prepareUpdate(
        const std::string& sql,
        const ParameterNames& parameter_names
    )
{
    CHECK_VALID;

    UpdateStatementPtr ret( new UpdateStatement( shared_from_this(), *_handle_ptr, sql, parameter_names ) );
    _stmt_ptrs[ret.get()] = ret; // Store a weak ptr to the new statement so can invalidate if conn destroyed.
    return ret;
}


QueryStatementPtr BasicConnection::prepareQuery(
        const std::string& sql,
        const ParameterNames& parameter_names
    )
{
    CHECK_VALID;

    QueryStatementPtr ret( new QueryStatement( shared_from_this(), *_handle_ptr, sql, parameter_names ) );
    _stmt_ptrs[ret.get()] = ret; // Store a weak ptr to the new statement so can invalidate if conn destroyed.
    return ret;
}


QueryStatementPtr BasicConnection::createQuery()
{
    CHECK_VALID;

    QueryStatementPtr ret( new QueryStatement( shared_from_this(), *_handle_ptr ) );
    _stmt_ptrs[ret.get()] = ret; // Store a weak ptr to the new statement so can invalidate if conn destroyed.
    return ret;
}


void BasicConnection::startTransaction()
{
    CHECK_VALID;

    if ( _transaction ) {
        CXXDB_THROW_EXCEPTION( logic_error( "Can't start a transaction when there's already a transaction on a connection." ) );
    }

    _transaction = true;
    _handle_ptr->setCommitRequired();
}


void BasicConnection::commit()
{
    CHECK_VALID;

    _handle_ptr->endTransaction( SQL_COMMIT );
}

void BasicConnection::rollback()
{
    CHECK_VALID;

    _handle_ptr->endTransaction( SQL_ROLLBACK );
}


void BasicConnection::endTransaction()
{
    CHECK_VALID;

    rollback();
    _transaction = false;
    _handle_ptr->setCommitAutomatically();
}


void BasicConnection::invalidate()
{
    if ( ! _handle_ptr ) {
        LOG_WARN_MSG( "Invalidating connection (already invalidated)." );
        return;
    }

    LOG_DEBUG_MSG( "Invalidating connection." );

    _invalidate();
}


void BasicConnection::notifyStatementDestroyed( AbstractStatement* stmt_p )
{
    LOG_DEBUG_MSG( "Notified statement destroyed with stmt_p=" << stmt_p );

    if ( ! _stmt_ptrs.erase( stmt_p ) ) {
        LOG_WARN_MSG( "Notified statement destroyed but didn't find. stmt_p=" << stmt_p );
    }
}


BasicConnection::~BasicConnection()
{
    if ( ! _handle_ptr ) {
        return;
    }

    _invalidate();

    // Notify the environment.
    EnvironmentPtr env_ptr(_env_ptr.lock());
    if ( env_ptr ) {
        env_ptr->notifyConnectionDestroyed( this );
    }
}


void BasicConnection::_invalidate()
{
    if ( ! _stmt_ptrs.empty() ) {
        LOG_WARN_MSG( "Invalidating " << _stmt_ptrs.size() << " statements." );
    }

    // Invalidate all my statements.
    for ( _StmtPtrs::iterator i(_stmt_ptrs.begin()) ; i != _stmt_ptrs.end() ; ++i ) {
        AbstractStatementPtr stmt_ptr(i->second.lock());
        if ( stmt_ptr ) {
            stmt_ptr->_invalidate();
        }
    }

    _handle_ptr->disconnect();

    _handle_ptr.reset();
}


} // namespace cxxdb
