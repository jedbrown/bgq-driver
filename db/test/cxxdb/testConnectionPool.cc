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

#include "InitializedFixture.hpp"

#include "Connection.h"
#include "ConnectionPool.h"

#include <string>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE cxxdbConnectionPool
#include <boost/test/unit_test.hpp>

using cxxdb::ConnectionPool;
using cxxdb::ConnectionPtr;

using std::string;

BOOST_GLOBAL_FIXTURE( Fixture );

cxxdb::ConnectionPtr createConnection()
{
    cxxdb::ConnectionPtr conn_ptr(cxxdb::Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ));
    conn_ptr->execute( string() + "SET SCHEMA " + Fixture::db_schema );
    return conn_ptr;
}

BOOST_AUTO_TEST_CASE( test_get_connection )
{
    // Can get a connection from a connection pool.

    ConnectionPool pool(
            &createConnection,
            0
        );

    {
        ConnectionPool::Status status(pool.getStatus());
        BOOST_CHECK_EQUAL( status.connsAvailable, unsigned( 0 ) );
        BOOST_CHECK_EQUAL( status.connsInUse, unsigned( 0 ) );
    }

    ConnectionPtr conn_ptr(pool.getConnection());

    {
        ConnectionPool::Status status(pool.getStatus());
        BOOST_CHECK_EQUAL( status.connsAvailable, unsigned( 0 ) );
        BOOST_CHECK_EQUAL( status.connsInUse, unsigned( 1 ) );
    }
}

BOOST_AUTO_TEST_CASE( test_size_1 )
{
    // If set size to 1, then if get two and return 2 then 1 conn avail.

    ConnectionPool pool(
            &createConnection,
            1
        );

    {
        ConnectionPool::Status status(pool.getStatus());
        BOOST_CHECK_EQUAL( status.connsAvailable, unsigned( 0 ) );
        BOOST_CHECK_EQUAL( status.connsInUse, unsigned( 0 ) );
    }

    ConnectionPtr conn1_ptr(pool.getConnection());
    {
        ConnectionPool::Status status(pool.getStatus());
        BOOST_CHECK_EQUAL( status.connsAvailable, unsigned( 0 ) );
        BOOST_CHECK_EQUAL( status.connsInUse, unsigned( 1 ) );
    }

    ConnectionPtr conn2_ptr(pool.getConnection());
    {
        ConnectionPool::Status status(pool.getStatus());
        BOOST_CHECK_EQUAL( status.connsAvailable, unsigned( 0 ) );
        BOOST_CHECK_EQUAL( status.connsInUse, unsigned( 2 ) );
    }

    conn1_ptr.reset();
    {
        ConnectionPool::Status status(pool.getStatus());
        BOOST_CHECK_EQUAL( status.connsAvailable, unsigned( 0 ) );
        BOOST_CHECK_EQUAL( status.connsInUse, unsigned( 1 ) );
    }

    conn2_ptr.reset();
    {
        ConnectionPool::Status status(pool.getStatus());
        BOOST_CHECK_EQUAL( status.connsAvailable, unsigned( 1 ) );
        BOOST_CHECK_EQUAL( status.connsInUse, unsigned( 0 ) );
    }
}
