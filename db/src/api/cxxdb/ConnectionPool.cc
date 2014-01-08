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

#include "cxxdb/ConnectionPool.h"

#include "PooledConnection.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

ConnectionPool::ConnectionPool(
        CreateConnectionFn create_connection_fn,
        unsigned size
    ) :
        _size(size),
        _conns_in_use(0),
        _high_water_mark(0)
{
    LOG_DEBUG_MSG(
            "Initializing database connection pool"
            " with size=" << _size
        );

    _create_connection_fn = create_connection_fn;
}


ConnectionPtr ConnectionPool::getConnection()
{
    _LockGuard lg( _mtx );

    ConnectionPtr conn_ptr;

    if ( _pooled_conns.empty() ) {
        // Need a new connection.

        LOG_DEBUG_MSG( "Getting new connection because none available in the pool." );

        conn_ptr = _create_connection_fn();
    } else {
        // Use one from the pool.

        LOG_DEBUG_MSG( "Reusing connection." );

        conn_ptr = *_pooled_conns.begin();
        _pooled_conns.erase( _pooled_conns.begin() );
    }

    PooledConnection::Ptr pooled_connection_ptr( new PooledConnection( conn_ptr, *this ) );

    ++_conns_in_use;
    if ( _conns_in_use > _high_water_mark ) {
        _high_water_mark = _conns_in_use;
    }

    LOG_DEBUG_MSG( "Got a connection from the connection pool. in_use=" << _conns_in_use << " available=" << _pooled_conns.size() );

    return pooled_connection_ptr;
}


ConnectionPool::Status ConnectionPool::getStatus() const
{
    _LockGuard lg( _mtx );

    Status ret;

    ret.connsAvailable = _pooled_conns.size();
    ret.connsInUse = _conns_in_use;
    ret.connsMax = _high_water_mark;;

    return ret;
}


void ConnectionPool::_returnConnection( ConnectionPtr conn_ptr )
{
    _LockGuard lg( _mtx );

    --_conns_in_use;

    if ( _conns_in_use < _size ) {
        LOG_DEBUG_MSG( "Returning connection to the pool." );
        _pooled_conns.insert( conn_ptr );
    } else {
        LOG_DEBUG_MSG( "Discarding extra connection." );
    }

    LOG_DEBUG_MSG( "Connection returned to pool. in_use=" << _conns_in_use << " available=" << _pooled_conns.size() );
}

} // namespace cxxdb
