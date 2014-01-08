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
#include "QueryStatement.h"
#include "ResultSet.h"
#include "Statement.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/assign.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE cxxdbColumns
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
ConnectionPtr Fixture::conn_ptr;


BOOST_GLOBAL_FIXTURE( Fixture );



BOOST_AUTO_TEST_CASE( test_index_out_of_range_ex )
{
    // If try to get the column by index and out of range then fails with logic_error.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "SELECT blockId FROM BGQBlock" ));
    BOOST_CHECK_THROW( rs_ptr->columns()[0], std::logic_error );
    BOOST_CHECK_THROW( rs_ptr->columns()[2], std::logic_error );
}


BOOST_AUTO_TEST_CASE( test_by_name )
{
    // Can look up the index for a column name. The case of the column name doesn't matter.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "SELECT blockId FROM BGQBlock" ));
    BOOST_CHECK_EQUAL( rs_ptr->columns().columnNameToIndex( "BLOCKID" ), 1U );
    BOOST_CHECK_EQUAL( rs_ptr->columns().columnNameToIndex( "blockid" ), 1U );
}


BOOST_AUTO_TEST_CASE( test_invalid_name_ex )
{
    // If try to get the index for a column and the name doesn't exist then throws a logic_error.
    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "SELECT blockId FROM BGQBlock" ));
    BOOST_CHECK_THROW( rs_ptr->columns().columnNameToIndex( "NOTACOLUMN" ), std::logic_error );
    BOOST_CHECK_THROW( rs_ptr->columns()["NOTACOLUMN"], std::logic_error );
}


BOOST_AUTO_TEST_CASE( test_char_n )
{
    // Can bind a CHAR(n) column.
    // When getString(), trailing spaces removed.

    // If try to get CHAR(n) column as other type then fails with WrongType.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( 'CXXDBTEST_BC' AS CHAR(20) ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );

    BOOST_CHECK( rs_ptr->columns()[1] ); // Can test in boolean context to find out if not null.

    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getString(), "CXXDBTEST_BC" );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt32(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt64(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDouble(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getBytes(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_char_1 )
{
    // Can do getChar() on CHAR(1) column.
    // Can do getString() on CHAR(1) column.
    // If try to get CHAR(1) column as wrong type then fails with WrongType exception.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( 'Q' AS CHAR(1) ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getChar(), 'Q' );

    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getString(), "Q" );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt32(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt64(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDouble(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getBytes(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_varchar_n )
{
    // Can bind a VARCHAR(n) column.
    // If try to get as other type then fails with WrongType exception.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( 'E1' AS VARCHAR(1024) ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getString(), "E1" );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt32(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt64(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDouble(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getBytes(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_smallint )
{
    // Can bind a SMALLINT column and get back as an int16.

    // Allow up-casting to int32, int64, double.

    // If try to get back as wrong type then fails.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( 50 AS SMALLINT ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getInt16(), 50 );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getInt32(), 50 );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getInt64(), 50 );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getDouble(), 50 );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getString(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getBytes(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_integer )
{
    // Can bind an INTEGER column and get back as an int32

    // Allow up-casting to int64, double.

    // If try to get as different type then fails with WrongType exception.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( 35000 AS INTEGER ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );

    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getInt32(), 35000 );

    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getInt64(), 35000 );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getDouble(), 35000 );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getString(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getBytes(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_bigint )
{
    // Can bind a BIGINT column and get back as an int64.

    // Allow up-casting to double.

    // If try to get as different type then fails with WrongType exception.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( 4000000000 AS BIGINT ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );

    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getInt64(), 4000000000 );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getDouble(), 4000000000 );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getString(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt32(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getBytes(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_double )
{
    // Can bind a FLOAT column and get back as a double.
    // If try to get as different type then fails with WrongType.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( 123 AS FLOAT ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getDouble(), 123.0 );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getString(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt32(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt64(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getBytes(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_timestamp )
{
    // Can bind a TIMESTAMP column and get as a string or ptime
    // If try to get as other type then fails with WrongType exception.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( TIMESTAMP( '2010-03-21 14:34:41.229121' ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );

    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getString(), "2010-03-21 14:34:41.229121" );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getTimestamp(), boost::posix_time::time_from_string( "2010-03-21 14:34:41.229121" ) );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt32(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt64(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDouble(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getBytes(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_date )
{
    // Can bind a DATE column and get as string or boost::gregorian::date. If try to get as othe rtype then fails with WrongType exception.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( DATE( '2010-07-26' ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );

    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getString(), "2010-07-26" );
    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].getDate(), boost::gregorian::date( 2010, 7, 26 ) );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt32(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt64(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDouble(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getBytes(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_char_for_bit_data )
{
    // Can bind a CHAR(n) FOR BIT DATA column and get as Bytes.
    // If try to get as other type then fails with WrongType exception.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( X'DEADBEEF' AS CHAR(32) FOR BIT DATA ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );

    Bytes bytes(rs_ptr->columns()[1].getBytes());
    Bytes expected_bytes( 32, ' ' );
    expected_bytes[0] = 0xDE;
    expected_bytes[1] = 0xAD;
    expected_bytes[2] = 0xBE;
    expected_bytes[3] = 0xEF;

    BOOST_REQUIRE_EQUAL( bytes.size(), expected_bytes.size() );
    BOOST_CHECK( std::equal( bytes.begin(), bytes.end(), expected_bytes.begin() ) );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getString(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt32(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt64(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDouble(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_varchar_for_bit_data )
{
    // Can bind a VARCHAR(n) FOR BIT DATA column and get as Bytes.
    // If try to get as other type then fails with WrongType exception.

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( X'DEADBEEF' AS VARCHAR(32) FOR BIT DATA ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );

    Bytes bytes(rs_ptr->columns()[1].getBytes());
    Bytes expected_bytes;
    expected_bytes.resize( 4 );
    expected_bytes[0] = 0xDE;
    expected_bytes[1] = 0xAD;
    expected_bytes[2] = 0xBE;
    expected_bytes[3] = 0xEF;

    BOOST_REQUIRE_EQUAL( bytes.size(), expected_bytes.size() );
    BOOST_CHECK( std::equal( bytes.begin(), bytes.end(), expected_bytes.begin() ) );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getString(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt32(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt64(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDouble(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), WrongType );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), WrongType );
}


BOOST_AUTO_TEST_CASE( test_col_is_null )
{
    // Can test a value is null.
    // If try to get back a NULL value then get exception (logic_error).

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( NULL AS CHAR(32) ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );

    BOOST_CHECK( rs_ptr->columns()[1].isNull() );

    BOOST_CHECK( ! rs_ptr->columns()[1] ); // Can also test in boolean context.

    BOOST_CHECK_THROW( rs_ptr->columns()[1].getString(), std::logic_error );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getChar(), std::logic_error );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getBytes(), std::logic_error );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt16(), std::logic_error );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt32(), std::logic_error );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getInt64(), std::logic_error );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDouble(), std::logic_error );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getTimestamp(), std::logic_error );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].getDate(), std::logic_error );
}


BOOST_AUTO_TEST_CASE( test_as )
{
    // Can get a numerical column as<T> which is more forgiving then the get* functions.
    // e.g., can get a BIGINT column as a uint32_t,
    // unless the value wouldn't fit in a uint32_t (ValueTooBig)

    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "VALUES ( CAST( -50000 AS INTEGER ) )" ));

    BOOST_REQUIRE( rs_ptr->fetch() );

    BOOST_CHECK_EQUAL( rs_ptr->columns()[1].as<int32_t>(), int32_t( -50000 ) );

    BOOST_CHECK_THROW( rs_ptr->columns()[1].as<uint32_t>(), ValueTooBig );
    BOOST_CHECK_THROW( rs_ptr->columns()[1].as<int16_t>(), ValueTooBig );
}
