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

#include "PooledConnection.h"

#include "cxxdb/ConnectionPool.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

PooledConnection::PooledConnection(
        ConnectionPtr conn_ptr,
        ConnectionPool& pool
    ) :
        _conn_ptr(conn_ptr),
        _pool(pool)
{
    // Nothing to do.
}

PooledConnection::~PooledConnection()
{
    _pool._returnConnection( _conn_ptr );
}

} // namespace cxxdb
