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

#include "cxxdb/Transaction.h"

#include "BasicConnection.h"

#include <utility/include/Log.h>

#include <stdexcept>
#include <string>

using std::string;

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

Transaction::Transaction(
        Connection& conn
    ) :
        _conn(conn)
{
    _conn.startTransaction();
}

Transaction::~Transaction()
{
    try {
        _conn.endTransaction();
    } catch ( std::exception& e ) {
        LOG_ERROR_MSG( string() + "Failed to end transaction, " + e.what() );
    }
}


} // namespace cxxdb
