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

#include "Connection.h"
#include "Environment.h"
#include "exceptions.h"
#include "fwd.h"
#include "QueryStatement.h"
#include "ResultSet.h"
#include "Statement.h"
#include "UpdateStatement.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/assign.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE cxxdbStatement
#include <boost/test/unit_test.hpp>

using namespace boost::assign;

using namespace cxxdb;

using std::cout;
using std::string;

struct Fixture
{
    static bgq::utility::Properties::Ptr properties_ptr;
    static string db_name;
    static string db_user;
    static string db_pwd;
    static string db_schema;

    static cxxdb::ConnectionPtr conn_ptr;


    Fixture()
    {
        properties_ptr = bgq::utility::Properties::create( string() );
        bgq::utility::initializeLogging( *properties_ptr );
        db_name = properties_ptr->getValue( "database", "name" );
        db_user = properties_ptr->getValue( "database", "user" );
        db_pwd = properties_ptr->getValue( "database", "password" );
        db_schema = properties_ptr->getValue( "database", "schema_name" );

        cxxdb::Environment::initializeApplicationEnvironment();

        conn_ptr = cxxdb::Connection::create( db_name, db_user, db_pwd );
        conn_ptr->execute( string() + "SET SCHEMA " + db_schema );
    }

    ~Fixture()
    {
        conn_ptr.reset();
        cxxdb::Environment::resetApplicationEnvironment();
    }
};


bgq::utility::Properties::Ptr Fixture::properties_ptr;
string Fixture::db_name;
string Fixture::db_user;
string Fixture::db_pwd;
string Fixture::db_schema;
cxxdb::ConnectionPtr Fixture::conn_ptr;


BOOST_GLOBAL_FIXTURE( Fixture );


BOOST_AUTO_TEST_CASE( test_statement_multi_execute )
{
    // Can execute a prepared statement multiple times.

    {
        StatementPtr stmt_ptr(Fixture::conn_ptr->prepare( "INSERT INTO TBGQEVENTLOG ( category ) VALUES ( 'CXXDBTST_ME_S' )" ));
        stmt_ptr->execute();
        stmt_ptr->execute();
    }

    {
        UpdateStatementPtr del_stmt_ptr(Fixture::conn_ptr->prepareUpdate( "DELETE FROM TBGQEVENTLOG WHERE category='CXXDBTST_ME_S'" ));

        unsigned rows_affected;
        del_stmt_ptr->execute( &rows_affected );
        BOOST_CHECK_EQUAL( rows_affected, 2U );
    }
}


BOOST_AUTO_TEST_CASE( test_update_multi_execute )
{
    // Can execute a prepared update statement multiple times.

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate( "INSERT INTO TBGQEVENTLOG ( category ) VALUES ( 'CXXDBTST_ME_U' )" ));
    ins_stmt_ptr->execute();
    ins_stmt_ptr->execute();

    UpdateStatementPtr del_stmt_ptr(Fixture::conn_ptr->prepareUpdate( "DELETE FROM TBGQEVENTLOG WHERE category='CXXDBTST_ME_U'" ));

    unsigned rows_affected;
    del_stmt_ptr->execute( &rows_affected );
    BOOST_CHECK_EQUAL( rows_affected, 2U );
}


BOOST_AUTO_TEST_CASE( test_query_multi_execute )
{
    // Can execute a prepared query statement multiple times.

    QueryStatementPtr q_stmt_ptr(Fixture::conn_ptr->prepareQuery( "SELECT * FROM tbgqBlock WHERE blockId='NOTABLOCK'" ));

    ResultSetPtr rs_ptr(q_stmt_ptr->execute());
    BOOST_CHECK( ! rs_ptr->fetch() );

    rs_ptr = q_stmt_ptr->execute();
    BOOST_CHECK( ! rs_ptr->fetch() );
}


BOOST_AUTO_TEST_CASE( test_invalid_stmt )
{
    // If connection is destroyed, any statements on it are invalidated.
    // Calling methods on invalid statements throws cxxdb::InvalidObjectException.

    ConnectionPtr conn_ptr(Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ));

    // Create a few statements on the conn.

    StatementPtr stmt_ptr(conn_ptr->prepare( string() + "SET SCHEMA " + Fixture::db_schema ));
    stmt_ptr->execute();

    UpdateStatementPtr upd_stmt_ptr(conn_ptr->prepareUpdate( "INSERT INTO TBGQEVENTLOG ( category ) VALUES ( 'CXXDBTEST3' )" ));

    QueryStatementPtr q_stmt_ptr(conn_ptr->prepareQuery( "SELECT * FROM tbgqblock WHERE blockId = 'NOTABLOCK'" ) );


    BOOST_CHECK( stmt_ptr->isValid() );
    BOOST_CHECK( upd_stmt_ptr->isValid() );
    BOOST_CHECK( q_stmt_ptr->isValid() );

    conn_ptr.reset(); // invalidates all the statements!

    BOOST_CHECK( ! stmt_ptr->isValid() );
    BOOST_CHECK_THROW( stmt_ptr->execute(), InvalidObjectException );
    BOOST_CHECK_THROW( stmt_ptr->parameters(), InvalidObjectException );

    BOOST_CHECK( ! upd_stmt_ptr->isValid() );
    BOOST_CHECK_THROW( upd_stmt_ptr->execute(), InvalidObjectException );
    BOOST_CHECK_THROW( upd_stmt_ptr->parameters(), InvalidObjectException );

    BOOST_CHECK( ! q_stmt_ptr->isValid() );
    BOOST_CHECK_THROW( q_stmt_ptr->execute(), InvalidObjectException );
    BOOST_CHECK_THROW( q_stmt_ptr->parameters(), InvalidObjectException );
}


BOOST_AUTO_TEST_CASE( test_destroy_query_statement_invalidates_result_set )
{
    // If destroy the query statement, then invalidates the result set from it.
    // Calling methods on invalid result set gives InvalidObjectException.

    QueryStatementPtr q_stmt_ptr(Fixture::conn_ptr->prepareQuery( "SELECT * FROM tbgqBlock WHERE blockId='NOTABLOCK'" ));

    ResultSetPtr rs_ptr(q_stmt_ptr->execute());

    BOOST_CHECK( rs_ptr->isValid() );

    q_stmt_ptr.reset();

    BOOST_CHECK( ! rs_ptr->isValid() );

    BOOST_CHECK_THROW( rs_ptr->fetch(), InvalidObjectException );
    BOOST_CHECK_THROW( rs_ptr->columns(), InvalidObjectException );
}


BOOST_AUTO_TEST_CASE( test_invalidate_query_statement_invalidates_result_set )
{
    // If query statement is invalidated (connection goes away) then the result set is invalidated, too.

    ConnectionPtr conn_ptr(Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ));

    conn_ptr->execute( string() + "SET SCHEMA " + Fixture::db_schema );

    QueryStatementPtr q_stmt_ptr(conn_ptr->prepareQuery( "SELECT * FROM tbgqBlock WHERE blockId='NOTABLOCK'" ));

    ResultSetPtr rs_ptr(q_stmt_ptr->execute());

    BOOST_CHECK( rs_ptr->isValid() );

    conn_ptr.reset();

    BOOST_CHECK( ! rs_ptr->isValid() );
}


BOOST_AUTO_TEST_CASE( test_execute_again_invalidates_result_set )
{
    // If execute on query statement again then invalidates original result set.

    QueryStatementPtr q_stmt_ptr(Fixture::conn_ptr->prepareQuery( "SELECT * FROM tbgqBlock WHERE blockId='NOTABLOCK'" ));

    ResultSetPtr rs_ptr(q_stmt_ptr->execute());

    BOOST_CHECK( rs_ptr->isValid() );

    ResultSetPtr rs_ptr2(q_stmt_ptr->execute());

    BOOST_CHECK( ! rs_ptr->isValid() );
    BOOST_CHECK( rs_ptr2->isValid() );
}


BOOST_AUTO_TEST_CASE( test_result_set_internalize )
{
    // if internalize the statement pointer, then doesn't get invalidated.

    ResultSetPtr rs_ptr;

    {
        QueryStatementPtr q_stmt_ptr(Fixture::conn_ptr->prepareQuery( "SELECT * FROM tbgqBlock WHERE blockId='NOTABLOCK'" ));
        rs_ptr = q_stmt_ptr->execute();
        rs_ptr->internalize( q_stmt_ptr );
    }

    BOOST_CHECK( rs_ptr->isValid() );
}


BOOST_AUTO_TEST_CASE( test_prepare_later )
{
    // Can create a statement and later prepare it.

    QueryStatementPtr q_stmt_ptr(Fixture::conn_ptr->createQuery());

    q_stmt_ptr->prepare( "SELECT * FROM tbgqBlock WHERE blockId='NOTABLOCK'" );

    ResultSetPtr rs_ptr(q_stmt_ptr->execute());
    BOOST_CHECK( ! rs_ptr->fetch() );
}
