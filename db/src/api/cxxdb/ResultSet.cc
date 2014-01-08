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

#include "cxxdb/ResultSet.h"

#include "cxxdb/exceptions.h"
#include "cxxdb/QueryStatement.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

ResultSet::ResultSet(
        QueryStatementPtr qstmt_ptr,
        bool retain
    ) :
        _qstmt_ptr( qstmt_ptr )
{
    if ( retain ) {
        _qstmt_retained_ptr = qstmt_ptr;
    }
}

bool ResultSet::fetch()
{
    QueryStatementPtr qstmt_ptr(_qstmt_ptr.lock());

    if ( ! qstmt_ptr ) {
        CXXDB_THROW_EXCEPTION( cxxdb::InvalidObjectException( "ResultSet", __FUNCTION__ ) );
    }

    return qstmt_ptr->_fetch();
}

const Columns& ResultSet::columns() const
{
    QueryStatementPtr qstmt_ptr(_qstmt_ptr.lock());

    if ( ! qstmt_ptr ) {
        CXXDB_THROW_EXCEPTION( cxxdb::InvalidObjectException( "ResultSet", __FUNCTION__ ) );
    }

    return qstmt_ptr->_columns();
}

ResultSet::~ResultSet()
{
    QueryStatementPtr qstmt_ptr(_qstmt_ptr.lock());

    if ( qstmt_ptr ) {
        qstmt_ptr->_notifyResultSetDestroyed();
    }
}

void ResultSet::_invalidate()
{
    LOG_DEBUG_MSG( "Invalidating ResultSet." );
    _qstmt_ptr.reset();
}


} // namespace cxxdb
