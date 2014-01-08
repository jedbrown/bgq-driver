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

#include "cxxdb/EnvironmentHandle.h"

#include "cxxdb/diagnostics.h"

#include <utility/include/Log.h>

#include <sqlext.h>

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

EnvironmentHandle::EnvironmentHandle()
{
    SQLRETURN rc;

    LOG_DEBUG_MSG( "Creating database environment handle." );

    rc = SQLAllocHandle(
            SQL_HANDLE_ENV,
            SQL_NULL_HANDLE,
            &_henv
        );

    if ( rc == SQL_SUCCESS ) {
        LOG_DEBUG_MSG( "Created database environment handle. henv=" << _henv );
        return;
    }

    checkResult( "SQLAllocHandle(ENV)", rc, SQL_HANDLE_ENV, _henv );
}


void EnvironmentHandle::setAttribute(
        SQLINTEGER attr,
        SQLPOINTER value_ptr,
        SQLINTEGER string_length
    )
{
    SQLRETURN rc = SQLSetEnvAttr(
            _henv,
            attr,
            value_ptr,
            string_length
        );

    checkResult( "SQLSetEnvAttr", rc, SQL_HANDLE_ENV, _henv );
}


void EnvironmentHandle::setOdbcVersion( unsigned long odbc_version )
{
    setAttribute(
            SQL_ATTR_ODBC_VERSION,
            reinterpret_cast<SQLPOINTER>( odbc_version ),
            0
        );
}


EnvironmentHandle::~EnvironmentHandle()
{
    LOG_DEBUG_MSG( "Destroying database environment handle. henv=" << _henv );

    SQLRETURN rc;

    rc = SQLFreeHandle( SQL_HANDLE_ENV, _henv );

    if ( rc != SQL_SUCCESS ) {
        LOG_ERROR_MSG( "Failed to free database environment handle. rc=" << rc );
    }
}


} // namespace cxxdb

