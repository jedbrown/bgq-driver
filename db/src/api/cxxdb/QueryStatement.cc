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

#include "cxxdb/QueryStatement.h"

#include "cxxdb/ConnectionHandle.h"
#include "cxxdb/exceptions.h"
#include "cxxdb/ResultSet.h"
#include "cxxdb/StatementHandle.h"


#define CHECK_VALID { if ( ! isValid() ) { CXXDB_THROW_EXCEPTION( cxxdb::InvalidObjectException( "QueryStatement", __FUNCTION__ ) ); } }


namespace cxxdb {


QueryStatement::QueryStatement(
        BasicConnectionPtr connection_ptr,
        ConnectionHandle& connection_handle,
        const std::string& sql,
        const ParameterNames& parameter_names
    ) :
        AbstractStatement(
                connection_ptr,
                connection_handle,
                sql,
                parameter_names
            )
{
    _columns_ptr.reset( new Columns( *_handle_ptr ) );
}


QueryStatement::QueryStatement(
        BasicConnectionPtr connection_ptr,
        ConnectionHandle& connection_handle
    ) :
        AbstractStatement(
                connection_ptr,
                connection_handle
            )
{
    // Nothing to do.
}


void QueryStatement::prepare(
        const std::string& sql,
        const ParameterNames& parameter_names
    )
{
    AbstractStatement::prepare( sql, parameter_names );
    _columns_ptr.reset( new Columns( *_handle_ptr ) );
}


ResultSetPtr QueryStatement::execute()
{
    CHECK_VALID;

    {
        ResultSetPtr result_set_ptr(_result_set_ptr.lock());

        if ( result_set_ptr ) {
            result_set_ptr->_invalidate();
            _handle_ptr->closeCursor();
        }
    }

    _result_set_ptr.reset();

    _handle_ptr->execute();

    ResultSetPtr ret( new ResultSet(
            shared_from_this(),
            false
        ) );

    _result_set_ptr = ret; // Store a weak pointer so can invalidate if execute again.

    return ret;
}


ResultSetPtr QueryStatement::_executeRetain()
{
    _handle_ptr->execute();

    ResultSetPtr ret( new ResultSet(
            shared_from_this(),
            true
        ) );

    _result_set_ptr = ret; // Store a weak pointer so can invalidate if execute again.

    return ret;
}


void QueryStatement::_invalidate()
{
    ResultSetPtr result_set_ptr(_result_set_ptr.lock());

    if ( result_set_ptr ) {
        result_set_ptr->_invalidate();
    }

    AbstractStatement::_invalidate();
}


bool QueryStatement::_fetch()
{
    return _handle_ptr->fetch();
}


void QueryStatement::_notifyResultSetDestroyed()
{
    _handle_ptr->closeCursor();
    _result_set_ptr.reset();
}


} // namespace cxxdb
