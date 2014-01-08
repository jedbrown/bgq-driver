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
#include "ConnectionHandle.h"
#include "Environment.h"
#include "EnvironmentHandle.h"
#include "exceptions.h"
#include "StatementHandle.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <sqlext.h>

#include <iostream>
#include <stdexcept>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE cxxdb
#include <boost/test/unit_test.hpp>

using std::cout;
using std::logic_error;
using std::string;

struct Fixture
{
    static bgq::utility::Properties::Ptr properties_ptr;
    static string db_name;
    static string db_user;
    static string db_pwd;
    static string db_schema;

    Fixture()
    {
        properties_ptr = bgq::utility::Properties::create( string() );
        bgq::utility::initializeLogging( *properties_ptr );
        db_name = properties_ptr->getValue( "database", "name" );
        db_user = properties_ptr->getValue( "database", "user" );
        db_pwd = properties_ptr->getValue( "database", "password" );
        db_schema = properties_ptr->getValue( "database", "schema_name" );
    }
};


bgq::utility::Properties::Ptr Fixture::properties_ptr;
string Fixture::db_name;
string Fixture::db_user;
string Fixture::db_pwd;
string Fixture::db_schema;


BOOST_GLOBAL_FIXTURE( Fixture );


BOOST_AUTO_TEST_CASE( test_handle )
{
    // Tests some basic stuff with handles.

    cxxdb::EnvironmentHandle env_handle;
    env_handle.setOdbcVersion( SQL_OV_ODBC3 );
    cxxdb::ConnectionHandle conn_handle( env_handle );
    conn_handle.connect( Fixture::db_name, Fixture::db_user, Fixture::db_pwd );

    {
        cxxdb::StatementHandle stmt_handle( conn_handle );
        stmt_handle.execDirect( string() + "SET SCHEMA " + Fixture::db_schema );
    }

    {
        cxxdb::StatementHandle stmt_handle( conn_handle );
        stmt_handle.execDirect( "SELECT * FROM bgqUserPrefs WHERE userName='CXXDBTEST1'" );
    }

    conn_handle.disconnect();
}


BOOST_AUTO_TEST_CASE( test_application_environment )
{
    // Can initialize the application environment,
    // if initialized can create a new connection,
    // then can reset the application environment.

    cxxdb::Environment::initializeApplicationEnvironment();
    BOOST_CHECK( cxxdb::Environment::getApplicationEnvironment().newConnection( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ) );
    BOOST_CHECK( cxxdb::Connection::create( Fixture::db_name, Fixture::db_user, Fixture::db_pwd ) );
    cxxdb::Environment::resetApplicationEnvironment();
}


BOOST_AUTO_TEST_CASE( test_application_environment_not_initialized )
{
    // If try to get application environment when haven't initialized it then get exception.

    BOOST_CHECK_THROW( cxxdb::Environment::getApplicationEnvironment(), logic_error );
}
