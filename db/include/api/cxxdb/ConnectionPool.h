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

#ifndef CXXDB_CONNECTION_POOL_H_
#define CXXDB_CONNECTION_POOL_H_

#include "fwd.h"

#include <boost/function.hpp>
#include <boost/utility.hpp>

#include <boost/thread/mutex.hpp>

#include <set>

namespace cxxdb {

/*! \brief A connection pool. */
class ConnectionPool : boost::noncopyable
{
public:

    struct Status {
        unsigned connsAvailable; //!< Connections waiting in the pool.
        unsigned connsInUse; //!< Number of connections in use.
        unsigned connsMax; //!< Maximum number of connections in use.
    };


    //! \brief Function that creates a connection for the pool.
    typedef boost::function<ConnectionPtr()> CreateConnectionFn;


    /*! \brief Constructor. */
    ConnectionPool(
            CreateConnectionFn create_connection_fn, //!< function called to create a Connection.
            unsigned size //!< Number of connections to hold in the pool.
        );


    /*! \brief Get a connection from the connection pool. */
    ConnectionPtr getConnection();

    /*! \brief Get a snapshot of the current status of the ConnectionPool. */
    Status getStatus() const;


private:

    typedef boost::mutex _Mutex;
    typedef boost::lock_guard<boost::mutex> _LockGuard;

    unsigned _size;

    CreateConnectionFn _create_connection_fn;

    mutable _Mutex _mtx;

    std::set<ConnectionPtr> _pooled_conns;
    unsigned _conns_in_use;
    unsigned _high_water_mark;;


    /*! \brief This is called by PooledConnection to return the connection to the pool. */
    void _returnConnection( ConnectionPtr conn_ptr );


    friend class PooledConnection;
};


} // namespace cxxdb

#endif
