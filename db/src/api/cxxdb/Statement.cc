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

#include "cxxdb/Statement.h"

#include "BasicConnection.h"

#include "cxxdb/ConnectionHandle.h"
#include "cxxdb/exceptions.h"
#include "cxxdb/StatementHandle.h"

#include <utility/include/Log.h>

#include <boost/throw_exception.hpp>

#include <sqlext.h>

#include <stdexcept>
#include <string>

using std::string;

LOG_DECLARE_FILE( "database" );

#define CHECK_VALID { if ( ! isValid() ) { CXXDB_THROW_EXCEPTION( cxxdb::InvalidObjectException( "Statement", __FUNCTION__ ) ); } }

namespace cxxdb {

Statement::Statement(
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
    // Nothing to do.
}


void Statement::execute()
{
    CHECK_VALID;

    _handle_ptr->execute();
}


} // namespace cxxdb
