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
#include "Environment.h"
#include "exceptions.h"
#include "QueryStatement.h"
#include "ResultSet.h"
#include "Statement.h"
#include "Transaction.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <iostream>
#include <stdexcept>
#include <string>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE cxxdbConnection
#include <boost/test/unit_test.hpp>

using std::cout;
using std::logic_error;
using std::string;

BOOST_GLOBAL_FIXTURE( Fixture );

BOOST_AUTO_TEST_CASE( test_connect_user )
{
    // Can connect to a database provided user name and pwd.
    // Connection's create method uses the application environment

    BOOST_CHECK( cxxdb::Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ) );
}

BOOST_AUTO_TEST_CASE( test_fails_invalid_creds )
{
    // If try to connect with invalid user name and password then throws exception.
    BOOST_CHECK_THROW( cxxdb::Connection::create( Fixture::db_name, "NOTAUSER", "NOTMYPASSWORD" ), cxxdb::DatabaseException );
}

BOOST_AUTO_TEST_CASE( test_invalid_db_name_exception )
{
    // If try to connect to a database that doesn't exist (by name) then get an exception.
    BOOST_CHECK_THROW( cxxdb::Connection::create( "NOTADATABASE" ), cxxdb::DatabaseException );
}

BOOST_AUTO_TEST_CASE( test_execute )
{
    // Can execute a simple sql on a connection.

    cxxdb::ConnectionPtr conn_ptr(cxxdb::Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ));
    conn_ptr->execute( string() + "SET SCHEMA " + Fixture::db_schema );
}

BOOST_AUTO_TEST_CASE( test_executeUpdate_rows_affected )
{
    // Can pass pointer in to execute to get the number of rows affected.

    uint64_t affected_row_count;

    cxxdb::ConnectionPtr conn_ptr(cxxdb::Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ));
    conn_ptr->execute( string() + "SET SCHEMA " + Fixture::db_schema );
    conn_ptr->executeUpdate( "INSERT INTO TBGQNodeConfig ( NodeConfig ) VALUES ( 'CXXDBTEST2-1' )" );
    conn_ptr->executeUpdate( "UPDATE TBGQNodeConfig SET RASPolicy='M1' WHERE NodeConfig='CXXDBTEST2-1'", &affected_row_count );
    BOOST_CHECK_EQUAL( affected_row_count, 1U );

    conn_ptr->executeUpdate( "INSERT INTO TBGQNodeConfig ( NodeConfig, RASPolicy ) VALUES ( 'CXXDBTEST2-2', 'M1' )" );

    conn_ptr->executeUpdate( "UPDATE TBGQNodeConfig SET RASPolicy='M2' WHERE RASPolicy='M1'", &affected_row_count );
    BOOST_CHECK_EQUAL( affected_row_count, 2U );

    conn_ptr->executeUpdate( "DELETE FROM TBGQNodeConfig WHERE RASPolicy='M2'", &affected_row_count );
    BOOST_CHECK_EQUAL( affected_row_count, 2U );
}

BOOST_AUTO_TEST_CASE( test_query )
{
    // Can do a simple query by just calling query()

    cxxdb::ConnectionPtr conn_ptr(cxxdb::Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ));
    conn_ptr->execute( string() + "SET SCHEMA " + Fixture::db_schema );
    cxxdb::ResultSetPtr stmt_ptr(conn_ptr->query( "SELECT COUNT(*) FROM bgqBlock WHERE blockId='NOTABLOCKID'" ));
    BOOST_CHECK( stmt_ptr->fetch() );
    BOOST_CHECK_EQUAL( stmt_ptr->columns()[1].getInt64(), 0 );
}

BOOST_AUTO_TEST_CASE( test_invalid_conn )
{
    // If the environment goes away on a connection, the connection is invalidated.
    // The connection invalidates any statements.
    // Calling methods on an invalid connection throws an exception.

    cxxdb::EnvironmentPtr env_ptr(cxxdb::Environment::create());

    cxxdb::ConnectionPtr conn_ptr(env_ptr->newConnection( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ));

    cxxdb::StatementPtr stmt1_ptr(conn_ptr->prepare( string() + "SET SCHEMA " + Fixture::db_schema ) );
    cxxdb::StatementPtr stmt2_ptr(conn_ptr->prepare( string() + "SET SCHEMA " + Fixture::db_schema ) );

    BOOST_CHECK( conn_ptr->isValid() );
    BOOST_CHECK( stmt1_ptr->isValid() );
    BOOST_CHECK( stmt2_ptr->isValid() );

    env_ptr.reset(); // *conn_ptr invalidated

    BOOST_CHECK( ! conn_ptr->isValid() );
    BOOST_CHECK( ! stmt1_ptr->isValid() );
    BOOST_CHECK( ! stmt2_ptr->isValid() );

    BOOST_CHECK_THROW( conn_ptr->execute( string() + "SET SCHEMA " + Fixture::db_schema ), cxxdb::InvalidObjectException );
    BOOST_CHECK_THROW( conn_ptr->prepare( string() + "SET SCHEMA " + Fixture::db_schema ), cxxdb::InvalidObjectException );
    BOOST_CHECK_THROW( conn_ptr->startTransaction(), cxxdb::InvalidObjectException );
    BOOST_CHECK_THROW( conn_ptr->commit(), cxxdb::InvalidObjectException );
    BOOST_CHECK_THROW( conn_ptr->rollback(), cxxdb::InvalidObjectException );
    BOOST_CHECK_THROW( conn_ptr->endTransaction(), cxxdb::InvalidObjectException );
}

BOOST_AUTO_TEST_CASE( test_start_transaction )
{
    // Can start a transaction, commit the transaction.

    cxxdb::ConnectionPtr conn_ptr(cxxdb::Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ));
    conn_ptr->execute( string() + "SET SCHEMA " + Fixture::db_schema );

    {
        cxxdb::Transaction tx( *conn_ptr );
        conn_ptr->execute( "INSERT INTO TBGQNodeConfig ( NodeConfig ) VALUES ( 'CXXDBTEST_TX' )" );
        conn_ptr->commit();
    }

    uint64_t rows_affected;
    conn_ptr->executeUpdate( "DELETE FROM TBGQNodeConfig WHERE NodeConfig='CXXDBTEST_TX'", &rows_affected );
    BOOST_CHECK_EQUAL( rows_affected, 1U );
}

BOOST_AUTO_TEST_CASE( test_transaction_rollback )
{
    // If don't commit the transaction then it's rolled back.

    cxxdb::ConnectionPtr conn_ptr(cxxdb::Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ));
    conn_ptr->execute( string() + "SET SCHEMA " + Fixture::db_schema );

    {
        cxxdb::Transaction tx( *conn_ptr );
        conn_ptr->execute( "INSERT INTO TBGQNodeConfig ( NodeConfig ) VALUES ( 'CXXDBTEST_TX_R' )" );
        // no commit so the record should have been not inserted.
    }

    uint64_t rows_affected;
    conn_ptr->executeUpdate( "DELETE FROM TBGQNodeConfig WHERE NodeConfig='CXXDBTEST_R'", &rows_affected );
    BOOST_CHECK_EQUAL( rows_affected, 0U );
}

BOOST_AUTO_TEST_CASE( test_multi_transaction_fails )
{
    // If try to create transaction when already a transaction then fails with logic_error.

    cxxdb::ConnectionPtr conn_ptr(cxxdb::Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ));
    cxxdb::Transaction tx1( *conn_ptr );
    BOOST_CHECK_THROW( cxxdb::Transaction tx2( *conn_ptr ), logic_error );
}
