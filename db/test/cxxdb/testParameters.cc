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
#include "UpdateStatement.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/assign.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE cxxdbParameters
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

        // Insert a block to use in other tests.

        try {
            conn_ptr->executeUpdate( "INSERT INTO tbgqNodeConfig ( nodeConfig ) VALUES ( '' )" );
        } catch ( std::exception& e ) {
            std::cerr << "Error inserting to nodeconfig. " << e.what();
            // Ignore fail to insert.
        }

        conn_ptr->executeUpdate( "INSERT INTO tbgqBlock ( blockId, numCnodes, owner, nodeConfig ) VALUES ( 'CXXDBTESTPARAMS', 1, '', '' )" );
    }

    ~Fixture()
    {
        // Remove the block I inserted!
        conn_ptr->executeUpdate( "DELETE FROM tbgqBlock WHERE blockId = 'CXXDBTESTPARAMS'" );

        try {
            conn_ptr->executeUpdate( "DELETE FROM tbgqNodeConfig where nodeConfig=''" );
        } catch ( std::exception& e ) {
            // Ignore fail to insert.
        }

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



BOOST_AUTO_TEST_CASE( test_index )
{
    // Parameters are bound by index.

    string ins_sql( "INSERT INTO TBGQJob ( username, schedulerData, executable ) VALUES ( ?, ?, ? )" );

    UpdateStatementPtr ins_stmt_ptr( Fixture::conn_ptr->prepareUpdate( ins_sql ) );

    const Parameter &username_param_info(ins_stmt_ptr->parameters()[1]);

    BOOST_REQUIRE_EQUAL( username_param_info.getIndexes().size(), 1U );
    BOOST_CHECK_EQUAL( username_param_info.getIndexes()[0], 1U );

    const Parameter &schedulerData_param_info(ins_stmt_ptr->parameters()[2]);

    BOOST_REQUIRE_EQUAL( schedulerData_param_info.getIndexes().size(), 1U );
    BOOST_CHECK_EQUAL( schedulerData_param_info.getIndexes()[0], 2U );

    const Parameter &executable_param_info(ins_stmt_ptr->parameters()[3]);

    BOOST_REQUIRE_EQUAL( executable_param_info.getIndexes().size(), 1U );
    BOOST_CHECK_EQUAL( executable_param_info.getIndexes()[0], 3U );
}


BOOST_AUTO_TEST_CASE( test_out_of_range_ex )
{
    // If try to use a parameter out of range then fails with logic_error.

    UpdateStatementPtr stmt_ptr(Fixture::conn_ptr->prepareUpdate( "SELECT status FROM TBGQJob WHERE schedulerData=?" ));

    BOOST_CHECK_THROW( stmt_ptr->parameters()[0].set( "CXXDBTEST_BPX" ), std::logic_error );
    BOOST_CHECK_THROW( stmt_ptr->parameters()[2].set( "CXXDBTEST_BPX" ), std::logic_error );
}


BOOST_AUTO_TEST_CASE( test_name )
{
    // Can assign names to the parameter markers.
    // If invalid name, throws logic_error.
    // Names are case-insensitive.

    StatementPtr ins_stmt_ptr(
            Fixture::conn_ptr->prepare(
                    "INSERT INTO TBGQJob ( username, schedulerData, executable ) VALUES ( ?, ?, ? )",
                    list_of( "username" )( "schedulerData" )( "executable" )
                )
        );

    const Parameter &hw_loc_param_info(ins_stmt_ptr->parameters()["username"]);

    BOOST_REQUIRE_EQUAL( hw_loc_param_info.getIndexes().size(), 1U );
    BOOST_CHECK_EQUAL( hw_loc_param_info.getIndexes()[0], 1U );

    const Parameter &status_param_info(ins_stmt_ptr->parameters()["schedulerData"]);

    BOOST_REQUIRE_EQUAL( status_param_info.getIndexes().size(), 1U );
    BOOST_CHECK_EQUAL( status_param_info.getIndexes()[0], 2U );

    const Parameter &desc_param_info(ins_stmt_ptr->parameters()["Executable"]);

    BOOST_REQUIRE_EQUAL( desc_param_info.getIndexes().size(), 1U );
    BOOST_CHECK_EQUAL( desc_param_info.getIndexes()[0], 3U );

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["NOTAPARAMETERNAME"], std::logic_error );

}


BOOST_AUTO_TEST_CASE( test_name_two_params )
{
    // Can bind the same name to two parameters
    // Exception if try to use index when multiple indexes for name.

    StatementPtr ins_stmt_ptr(
            Fixture::conn_ptr->prepare(
                    "INSERT INTO tbgqBlock ( blockId, numCnodes, sizeA, sizeB, sizeC, sizeD, sizeE, owner, nodeConfig ) VALUES ( ?, 1, ?, ?, ?, ?, ?, '', '' )",
                    list_of( "blockId" )( "size" )( "size" )( "size" )( "size" )( "size" )
                )
        );

    const Parameter &size_param_info(ins_stmt_ptr->parameters()["size"]);

    BOOST_REQUIRE_EQUAL( size_param_info.getIndexes().size(), 5U );
    BOOST_CHECK_EQUAL( size_param_info.getIndexes()[0], 2U );
    BOOST_CHECK_EQUAL( size_param_info.getIndexes()[1], 3U );
    BOOST_CHECK_EQUAL( size_param_info.getIndexes()[2], 4U );
    BOOST_CHECK_EQUAL( size_param_info.getIndexes()[3], 5U );
    BOOST_CHECK_EQUAL( size_param_info.getIndexes()[4], 6U );

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[2], std::logic_error );

    ins_stmt_ptr->parameters()["blockId"].set( "TESTCXXDB_BPMN" );
    ins_stmt_ptr->parameters()["size"].set( int32_t(4) );

    ins_stmt_ptr->execute();

    Fixture::conn_ptr->execute( "DELETE FROM tbgqBlock WHERE blockId='TESTCXXDB_BPMN'");
}


BOOST_AUTO_TEST_CASE( test_name_diff_type )
{
    // If use same name for params where the type is different then fails.

    string sql( "INSERT INTO tbgqBlock ( blockId, numCnodes, sizeA, owner, nodeConfig ) VALUES ( ?, 1, ?, '', '' )" );
    ParameterNames param_names = list_of( "blockId" )( "blockId" );

    BOOST_CHECK_THROW( Fixture::conn_ptr->prepare( sql, param_names ), std::logic_error );
}


BOOST_AUTO_TEST_CASE( test_name_not_all_named )
{
    // Don't have to name all the parameters: empty string means no name and can have shorter list than number of parameter markers.

    UpdateStatementPtr ins_stmt_ptr(
            Fixture::conn_ptr->prepareUpdate(
                    "INSERT INTO tbgqBlock ( blockId, numCnodes, sizeA, sizeB, sizeC, sizeD, sizeE, owner, nodeConfig ) VALUES ( ?, 1, ?, ?, ?, ?, ?, '', '' )",
                    list_of( "blockId" )( "size" )( "size" )( "" )( "size" )
                )
        );

    ins_stmt_ptr->parameters()["blockId"].set( "TESTCXXDB_BPMN" );
    ins_stmt_ptr->parameters()["size"].set( int32_t(3) );
    ins_stmt_ptr->parameters()[4].set( int32_t(5) );
    ins_stmt_ptr->parameters()[6].set( int32_t(6) );

    ins_stmt_ptr->execute();

    QueryStatementPtr q_stmt_ptr(
            Fixture::conn_ptr->prepareQuery(
                    "SELECT * FROM tbgqBlock WHERE blockId=?",
                    list_of( "blockId" )
                )
        );

    q_stmt_ptr->parameters()["blockId"].set( "TESTCXXDB_BPMN" );
    ResultSetPtr rs_ptr(q_stmt_ptr->execute());
    rs_ptr->fetch();
    BOOST_CHECK_EQUAL( rs_ptr->columns()["sizeA"].getInt32(), 3 );
    BOOST_CHECK_EQUAL( rs_ptr->columns()["sizeB"].getInt32(), 3 );
    BOOST_CHECK_EQUAL( rs_ptr->columns()["sizeC"].getInt32(), 5 );
    BOOST_CHECK_EQUAL( rs_ptr->columns()["sizeD"].getInt32(), 3 );
    BOOST_CHECK_EQUAL( rs_ptr->columns()["sizeE"].getInt32(), 6 );

    UpdateStatementPtr del_stmt_ptr(
            Fixture::conn_ptr->prepareUpdate(
                    "DELETE FROM tbgqBlock WHERE blockId=?",
                    list_of( "blockId" )
                )
        );

    del_stmt_ptr->parameters()["blockId"].set( "TESTCXXDB_BPMN" );
    del_stmt_ptr->execute();
}


BOOST_AUTO_TEST_CASE( test_col_char_string )
{
    // Can bind a CHAR(n) parameter.
    // If try to set other type then fails.

    Fixture::conn_ptr->executeUpdate(
            "INSERT INTO TBGQJob ( username, blockId, executable, workingDir, nodesUsed, shapea, shapeb, shapec, shaped, shapee, processesPerNode, hostname, pid, schedulerData )"
            " VALUES ( 'username', 'CXXDBTESTPARAMS', 'e', 'w', 1, 1, 1, 1, 1, 1, 1, 'h', 123, 'CXXDB_PARAMS_CHAR' )"
        );

    QueryStatementPtr stmt_ptr(Fixture::conn_ptr->prepareQuery(
            "SELECT username FROM TBGQJob WHERE username=? AND schedulerData=?",
            list_of( "username" )( "schedulerData" )
        ) );

    stmt_ptr->parameters()["username"].set( "username" );
    stmt_ptr->parameters()["schedulerData"].set( "CXXDB_PARAMS_CHAR" );

    BOOST_CHECK_THROW( stmt_ptr->parameters()["username"].set( 'E' ), WrongType );
    BOOST_CHECK_THROW( stmt_ptr->parameters()["username"].set( int16_t(5) ), WrongType );
    BOOST_CHECK_THROW( stmt_ptr->parameters()["username"].set( int32_t(512) ), WrongType );
    BOOST_CHECK_THROW( stmt_ptr->parameters()["username"].set( int64_t(12345) ), WrongType );
    BOOST_CHECK_THROW( stmt_ptr->parameters()["username"].set( double(123.0) ), WrongType );
    BOOST_CHECK_THROW( stmt_ptr->parameters()["username"].set( Bytes() ), WrongType );
    BOOST_CHECK_THROW( stmt_ptr->parameters()["username"].set( boost::posix_time::ptime() ), WrongType );

    ResultSetPtr rs_ptr(stmt_ptr->execute());

    BOOST_CHECK( rs_ptr->fetch() );

    BOOST_CHECK_EQUAL( rs_ptr->columns()["username"].getString(), "username" );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQJob WHERE schedulerData='CXXDB_PARAMS_CHAR'" );
}


BOOST_AUTO_TEST_CASE( test_col_char_1 )
{
    // Can bind a CHAR(1) parameter and use setChar to set to a char.
    // If try to set other type then fails.

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate(
            "INSERT INTO TBGQJob ( username, blockId, executable, workingDir, nodesUsed, shapea, shapeb, shapec, shaped, shapee, processesPerNode, hostname, pid, schedulerData, status )"
            " VALUES ( 'u', 'CXXDBTESTPARAMS', 'e', 'w', 1, 1, 1, 1, 1, 1, 1, 'h', 123, 'CXXDB_PARAMS_CHAR1', ? )",
            list_of( "status" )
        ));

    ins_stmt_ptr->parameters()["status"].set( 'R' );

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["status"].set( int16_t(5) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["status"].set( int32_t(512) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["status"].set( int64_t(12345) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["status"].set( double(123.0) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["status"].set( Bytes() ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["status"].set( boost::posix_time::ptime() ), WrongType );

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["status"].set( "ab" ), ValueTooBig );

    ins_stmt_ptr->execute();

    ResultSetPtr q_stmt_ptr(Fixture::conn_ptr->query( "SELECT status FROM TBGQJob WHERE schedulerData='CXXDB_PARAMS_CHAR1'" ));

    BOOST_CHECK( q_stmt_ptr->fetch() );

    BOOST_CHECK_EQUAL( q_stmt_ptr->columns()["status"].getChar(), 'R' );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQJob WHERE schedulerData='CXXDB_PARAMS_CHAR1'" );
}


BOOST_AUTO_TEST_CASE( test_col_varchar )
{
    // Can bind a VARCHAR(n) parameter.
    // If try to set other type then fails.

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate(
            "INSERT INTO TBGQJob ( username, blockId, executable, workingDir, nodesUsed, shapea, shapeb, shapec, shaped, shapee, processesPerNode, hostname, pid, schedulerData )"
            " VALUES ( 'u', 'CXXDBTESTPARAMS', ?, 'w', 1, 1, 1, 1, 1, 1, 1, 'h', 123, 'CXXDB_PARAMS_VC' )",
            list_of( "executable" )
        ));

    ins_stmt_ptr->parameters()["executable"].set( "This is a test." );

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["executable"].set( 'E' ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["executable"].set( int16_t(5) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["executable"].set( int32_t(512) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["executable"].set( int64_t(12345) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["executable"].set( double(123.0) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["executable"].set( Bytes() ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["executable"].set( boost::posix_time::ptime() ), WrongType );

    ins_stmt_ptr->execute();

    ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT executable FROM TBGQJob WHERE schedulerData='CXXDB_PARAMS_VC'" ));

    BOOST_CHECK( stmt_ptr->fetch() );

    BOOST_CHECK_EQUAL( stmt_ptr->columns()["executable"].getString(), "This is a test." );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQJob WHERE schedulerData='CXXDB_PARAMS_VC'" );
}


BOOST_AUTO_TEST_CASE( test_col_integer )
{
    // Can bind an INTEGER parameter, and set as int32_t or int16_t
    // If try to set other type then fails.

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate(
            "INSERT INTO TBGQBlock ( blockId, numCnodes, owner, nodeConfig ) VALUES ( 'CXXDBTEST_BPINT', ?, '', '' )",
            list_of( "numCnodes" )
        ));

    {
        ins_stmt_ptr->parameters()["numCnodes"].set( int32_t(512) );
        ins_stmt_ptr->execute();

        ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT numCnodes FROM bgqBlock WHERE blockId='CXXDBTEST_BPINT'" ));
        BOOST_CHECK( stmt_ptr->fetch() );
        BOOST_CHECK_EQUAL( stmt_ptr->columns()["numCnodes"].getInt32(), 512 );

        Fixture::conn_ptr->executeUpdate( "DELETE FROM bgqBlock WHERE blockId='CXXDBTEST_BPINT'" );
    }

    {
        ins_stmt_ptr->parameters()["numCnodes"].set( int16_t(5) );
        ins_stmt_ptr->execute();

        ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT numCnodes FROM bgqBlock WHERE blockId='CXXDBTEST_BPINT'" ));
        BOOST_CHECK( stmt_ptr->fetch() );
        BOOST_CHECK_EQUAL( stmt_ptr->columns()["numCnodes"].getInt32(), 5 );

        Fixture::conn_ptr->executeUpdate( "DELETE FROM bgqBlock WHERE blockId='CXXDBTEST_BPINT'" );
    }

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["numCnodes"].set( "TEST" ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["numCnodes"].set( 'E' ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["numCnodes"].set( int64_t(12345) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["numCnodes"].set( double(123.0) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["numCnodes"].set( Bytes() ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["numCnodes"].set( boost::posix_time::ptime() ), WrongType );
}


BOOST_AUTO_TEST_CASE( test_col_bigint )
{
    // Can bind a BIGINT parameter, and set as int64_t, int32_t, int16_t

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate( "INSERT INTO TBGQDIAGSEVENTLOG ( location, runId ) VALUES ( 'CXXDBTEST_BPBIGINT', ? )" ));

    {
        ins_stmt_ptr->parameters()[1].set( int64_t(12345) );
        ins_stmt_ptr->execute();

        ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT runId FROM TBGQDIAGSEVENTLOG WHERE location='CXXDBTEST_BPBIGINT'" ));
        BOOST_CHECK( stmt_ptr->fetch() );
        BOOST_CHECK_EQUAL( stmt_ptr->columns()["runId"].getInt64(), 12345 );

        Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQDIAGSEVENTLOG WHERE location='CXXDBTEST_BPBIGINT'" );
    }

    {
        ins_stmt_ptr->parameters()[1].set( int32_t(512) );
        ins_stmt_ptr->execute();

        ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT runId FROM TBGQDIAGSEVENTLOG WHERE location='CXXDBTEST_BPBIGINT'" ));
        BOOST_CHECK( stmt_ptr->fetch() );
        BOOST_CHECK_EQUAL( stmt_ptr->columns()["runId"].getInt64(), 512 );

        Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQDIAGSEVENTLOG WHERE location='CXXDBTEST_BPBIGINT'" );
    }

    {
        ins_stmt_ptr->parameters()[1].set( int16_t(5) );
        ins_stmt_ptr->execute();

        ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT runId FROM TBGQDIAGSEVENTLOG WHERE location='CXXDBTEST_BPBIGINT'" ));
        BOOST_CHECK( stmt_ptr->fetch() );
        BOOST_CHECK_EQUAL( stmt_ptr->columns()["runId"].getInt64(), 5 );

        Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQDIAGSEVENTLOG WHERE location='CXXDBTEST_BPBIGINT'" );
    }

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[1].set( "TEST" ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[1].set( 'E' ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[1].set( double(123.0) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[1].set( Bytes() ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[1].set( boost::posix_time::ptime() ), WrongType );
}


BOOST_AUTO_TEST_CASE( test_col_float )
{
    // Can bind an FLOAT parameter, and set as double, int16, int32, or int64

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate( "INSERT INTO TBGQServiceCardEnvironment ( location, voltageV12P ) VALUES ( 'R00-M0-S', ? )" ));

    {
        ins_stmt_ptr->parameters()[1].set( double(123.0) );
        ins_stmt_ptr->execute();

        ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT voltageV12P FROM BGQServiceCardEnvironment WHERE location='R00-M0-S'" ));
        BOOST_CHECK( stmt_ptr->fetch() );
        BOOST_CHECK_EQUAL( stmt_ptr->columns()["voltageV12P"].getDouble(), 123.0 );

        Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQServiceCardEnvironment WHERE location='R00-M0-S'" );
    }

    {
        ins_stmt_ptr->parameters()[1].set( int16_t(5) );
        ins_stmt_ptr->execute();

        ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT voltageV12P FROM BGQServiceCardEnvironment WHERE location='R00-M0-S'" ));
        BOOST_CHECK( stmt_ptr->fetch() );
        BOOST_CHECK_EQUAL( stmt_ptr->columns()["voltageV12P"].getDouble(), 5.0 );

        Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQServiceCardEnvironment WHERE location='R00-M0-S'" );
    }

    {
        ins_stmt_ptr->parameters()[1].set( int32_t(512) );
        ins_stmt_ptr->execute();

        ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT voltageV12P FROM BGQServiceCardEnvironment WHERE location='R00-M0-S'" ));
        BOOST_CHECK( stmt_ptr->fetch() );
        BOOST_CHECK_EQUAL( stmt_ptr->columns()["voltageV12P"].getDouble(), 512.0 );

        Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQServiceCardEnvironment WHERE location='R00-M0-S'" );
    }

    {
        ins_stmt_ptr->parameters()[1].set( int64_t(12345) );
        ins_stmt_ptr->execute();

        ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT voltageV12P FROM BGQServiceCardEnvironment WHERE location='R00-M0-S'" ));
        BOOST_CHECK( stmt_ptr->fetch() );
        BOOST_CHECK_EQUAL( stmt_ptr->columns()["voltageV12P"].getDouble(), 12345.0 );

        Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQServiceCardEnvironment WHERE location='R00-M0-S'" );
    }

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[1].set( "TEST" ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[1].set( 'E' ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[1].set( Bytes() ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[1].set( boost::posix_time::ptime() ), WrongType );

}


BOOST_AUTO_TEST_CASE( test_col_char_for_bit_data )
{
    // Can bind a CHAR(n) FOR BIT DATA column and set as Bytes.
    // If try to set as other type then fails with WrongType exception.

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate(
            "INSERT INTO TBGQBlock ( blockId, numCnodes, owner, nodeConfig, securityKey ) VALUES ( 'CXXDBTEST_CFBD', 1, '', '', ? )",
            list_of( "securityKey" )
        ));

    Bytes security_key_bytes = list_of( 0xDE )( 0xAD )( 0xBE )( 0xEF );
    ins_stmt_ptr->parameters()["securityKey"].set( security_key_bytes );

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["securityKey"].set( "TEST" ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["securityKey"].set( 'E' ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["securityKey"].set( int16_t(5) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["securityKey"].set( int32_t(512) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["securityKey"].set( int64_t(12345) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["securityKey"].set( double(123.0) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["securityKey"].set( boost::posix_time::ptime() ), WrongType );

    ins_stmt_ptr->execute();

    ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT securityKey FROM TBGQBlock WHERE blockId='CXXDBTEST_CFBD'" ));

    BOOST_REQUIRE( stmt_ptr->fetch() );

    Bytes bytes(stmt_ptr->columns()["securityKey"].getBytes());
    Bytes expected_bytes( 32, ' ' );
    expected_bytes[0] = 0xDE;
    expected_bytes[1] = 0xAD;
    expected_bytes[2] = 0xBE;
    expected_bytes[3] = 0xEF;

    BOOST_REQUIRE_EQUAL( bytes.size(), expected_bytes.size() );

#if 0 // debug stuff.
    {
        cout << "  bytes: ";
        for ( Bytes::const_iterator i(bytes.begin()) ; i != bytes.end() ; ++i ) {
            cout << std::hex << int(*i) << ":";
        }
        cout << "\n";
    }
#endif

    BOOST_CHECK( std::equal( bytes.begin(), bytes.end(), expected_bytes.begin() ) );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQBlock WHERE blockId='CXXDBTEST_CFBD'" );
}


BOOST_AUTO_TEST_CASE( test_varchar_for_bit_data )
{
    // Can bind a VARCHAR(n) FOR BIT DATA column and set as Bytes.
    // If try to set as other type then fails with WrongType exception.

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate(
            "INSERT INTO TBGQMidplane_history ( productId, vpd )"
            " VALUES ( 'cxxdbtest', ? )",
            list_of( "vpd" )
        ));

    Bytes nodes_bytes = list_of( 0xDE )( 0xAD )( 0xBE )( 0xEF );
    ins_stmt_ptr->parameters()["vpd"].set( nodes_bytes );

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["vpd"].set( "TEST" ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["vpd"].set( 'E' ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["vpd"].set( int16_t(5) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["vpd"].set( int32_t(512) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["vpd"].set( int64_t(12345) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["vpd"].set( double(123.0) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["vpd"].set( boost::posix_time::ptime() ), WrongType );

    ins_stmt_ptr->execute();

    ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT vpd FROM TBGQMidplane_history WHERE productId='cxxdbtest'" ));

    BOOST_REQUIRE( stmt_ptr->fetch() );

    Bytes bytes(stmt_ptr->columns()["vpd"].getBytes());
    Bytes expected_bytes;
    expected_bytes.resize( 4 );
    expected_bytes[0] = 0xDE;
    expected_bytes[1] = 0xAD;
    expected_bytes[2] = 0xBE;
    expected_bytes[3] = 0xEF;

    BOOST_REQUIRE_EQUAL( bytes.size(), expected_bytes.size() );
    BOOST_CHECK( std::equal( bytes.begin(), bytes.end(), expected_bytes.begin() ) );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQMidplane_history WHERE productId='cxxdbtest'" );
}


BOOST_AUTO_TEST_CASE( test_timestamp )
{
    // Can bind a TIMESTAMP column and set as ptime.
    // If try to set as other type then fails with WrongType exception.

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate(
            "INSERT INTO TBGQJob ( username, blockId, executable, workingDir, nodesUsed, shapea, shapeb, shapec, shaped, shapee, processesPerNode, hostname, pid, schedulerData, startTime )"
            " VALUES ( 'username', 'CXXDBTESTPARAMS', 'e', 'w', 1, 1, 1, 1, 1, 1, 1, 'h', 123, 'CXXDB_PARAMS_TSTS', ? )",
            list_of( "startTime" )
        ));

    boost::posix_time::ptime t( boost::gregorian::date( 2010, 3, 21 ), boost::posix_time::time_duration( 14, 34, 41 ) );

    ins_stmt_ptr->parameters()["startTime"].set( t );

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["startTime"].set( 'E' ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["startTime"].set( int16_t(5) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["startTime"].set( int32_t(512) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["startTime"].set( int64_t(12345) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["startTime"].set( double(123.0) ), WrongType );
    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["startTime"].set( Bytes() ), WrongType );

    ins_stmt_ptr->execute();

    ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT startTime FROM TBGQJob WHERE schedulerData='CXXDB_PARAMS_TSTS'" ));

    BOOST_REQUIRE( stmt_ptr->fetch() );
    BOOST_CHECK_EQUAL( stmt_ptr->columns()["startTime"].getTimestamp(), t );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQJob WHERE schedulerData='CXXDB_PARAMS_TSTS'" );
}


BOOST_AUTO_TEST_CASE( test_not_a_timestamp_is_null )
{
    // If set TIMESTAMP column to posix_time::not_a_date_time then is set to NULL

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate(
            "INSERT INTO TBGQDiagRuns ( runId, logDir, endTime )"
            " VALUES ( 1, '', ? )",
            list_of( "endTime" )
        ));

    ins_stmt_ptr->parameters()["endTime"].set( boost::posix_time::ptime() );

    ins_stmt_ptr->execute();

    ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT endTime FROM TBGQDiagRuns WHERE runId=1" ));

    BOOST_REQUIRE( stmt_ptr->fetch() );
    BOOST_CHECK( stmt_ptr->columns()["endTime"].isNull() );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQDiagRuns WHERE runid=1" );
}


BOOST_AUTO_TEST_CASE( test_timestamp_set_string )
{
    // Can bind a TIMESTAMP column and set as string.
    // If try to set as other type then fails with WrongType exception.

    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate(
            "INSERT INTO TBGQJob ( username, blockId, executable, workingDir, nodesUsed, shapea, shapeb, shapec, shaped, shapee, processesPerNode, hostname, pid, schedulerData, startTime )"
            " VALUES ( 'username', 'CXXDBTESTPARAMS', 'e', 'w', 1, 1, 1, 1, 1, 1, 1, 'h', 123, 'CXXDB_PARAMS_TSS', ? )",
            list_of( "startTime" )
        ));

    ins_stmt_ptr->parameters()["startTime"].set( "2010-03-21 14:34:41.229121" );

    ins_stmt_ptr->execute();

    ResultSetPtr stmt_ptr(Fixture::conn_ptr->query( "SELECT startTime FROM TBGQJob WHERE schedulerData='CXXDB_PARAMS_TSS'" ));

    BOOST_REQUIRE( stmt_ptr->fetch() );
    BOOST_CHECK_EQUAL(
            stmt_ptr->columns()["startTime"].getTimestamp(),
            boost::posix_time::time_from_string( "2010-03-21 14:34:41.229121" )
        );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQJob WHERE schedulerData='CXXDB_PARAMS_TSS'" );
}


BOOST_AUTO_TEST_CASE( test_timestamp_invalid_string )
{
    UpdateStatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepareUpdate(
            "INSERT INTO TBGQJob ( username, schedulerData, executable, workingDir, mapping, startTime, hostname, pid )"
            " VALUES ( 'cxxdbtest', 'CXXDBTEST_BPTS', 'noexec', '', '', ?, 'hostname', 123 )",
            list_of( "startTime" )
        ));

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()["startTime"].set( std::string("NOTATIMESTAMP") ), boost::bad_lexical_cast );
}


BOOST_AUTO_TEST_CASE( test_dont_bind_ex )
{
    StatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepare( "INSERT INTO TBGQJob ( schedulerData, executable ) VALUES ( 'CXXDBTEST_NOINS', ? )" ));
    BOOST_CHECK_THROW( ins_stmt_ptr->execute(), DatabaseException );
}


BOOST_AUTO_TEST_CASE( test_bind_null )
{
    // Can bind a column to a NULL value.

    StatementPtr ins_stmt_ptr(Fixture::conn_ptr->prepare(
            "INSERT INTO TBGQBlock ( blockId, numCnodes, nodeConfig, owner, userName ) VALUES ( 'CXXDBTEST_BN', 1, '', '', ? )",
            list_of( "userName" )
        ));
    ins_stmt_ptr->parameters()["userName"].setNull();
    ins_stmt_ptr->execute();


    ResultSetPtr rs_ptr(Fixture::conn_ptr->query( "SELECT userName FROM TBGQBlock WHERE blockId='CXXDBTEST_BN'" ));

    BOOST_CHECK( rs_ptr->fetch() );
    BOOST_CHECK( rs_ptr->columns()["userName"].isNull() );

    Fixture::conn_ptr->execute( "DELETE FROM TBGQBlock WHERE blockId='CXXDBTEST_BN'" );
}


BOOST_AUTO_TEST_CASE( test_string_too_long_ex )
{
    // If set a string parameter and the string is longer than the parameter allows then throws ValueTooBig.

    BOOST_CHECK_THROW( Fixture::conn_ptr->prepare(
            "INSERT INTO TBGQJob ( blockId ) VALUES ( ? )",
            list_of( "blockId" )
        )->parameters()["blockId"].set( "TEST56781012345678201234567830123" ), ValueTooBig );
}


BOOST_AUTO_TEST_CASE( test_for_bit_data_too_long_ex )
{
    // If set a Bytes parameter and the Bytes is longer than the parameter allows then throws ValueTooBig.

    Bytes bytes( 33, ' ' );

    BOOST_CHECK_THROW( Fixture::conn_ptr->prepare(
            "INSERT INTO TBGQBlock ( blockId, numCnodes, owner, nodeConfig, securityKey ) VALUES ( 'CXXDBTEST_BDTLE', 1, '', '', ? )",
            list_of( "securityKey" )
        )->parameters()["securityKey"].set( bytes ), ValueTooBig );
}


BOOST_AUTO_TEST_CASE( test_truncate_string )
{
    // If set a string parameter and the string is longer than the parameter allows and the truncate flag is passed in,
    // then truncates and sets truncate flag to true.

    UpdateStatementPtr ins_stmt_ptr(
            Fixture::conn_ptr->prepareUpdate(
                "INSERT INTO TBGQBlock ( blockId, numCnodes, owner, nodeConfig ) VALUES ( ?, 1, 'CXXDBTSTT_S1', '' )",
                list_of( "blockId" ) )
        );

    bool truncated;

    ins_stmt_ptr->parameters()["blockId"].set( "TEST567810123456782012345678301", &truncated );

    BOOST_CHECK( ! truncated );

    ins_stmt_ptr->parameters()["blockId"].set( "TEST56781012345678201234567830123", &truncated );

    BOOST_CHECK( truncated );

    ins_stmt_ptr->execute();

    ResultSetPtr q_stmt_ptr(Fixture::conn_ptr->query( "SELECT blockId FROM TBGQBlock WHERE owner='CXXDBTSTT_S1'" ));

    BOOST_REQUIRE( q_stmt_ptr->fetch() );
    BOOST_CHECK_EQUAL( q_stmt_ptr->columns()["blockId"].getString(), "TEST5678101234567820123456783012" );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQBlock WHERE owner='CXXDBTSTT_S1'" );
}


BOOST_AUTO_TEST_CASE( test_truncate_bytes )
{
    // If set a Bytes parameter and the Bytes is longer than the parameter allows and the truncate flag is passed in,
    // then truncates and sets truncate flag to true.

    UpdateStatementPtr ins_stmt_ptr(
            Fixture::conn_ptr->prepareUpdate(
                "INSERT INTO TBGQBlock ( blockId, numCnodes, owner, nodeConfig, securityKey ) VALUES ( 'CXXDBTST_TB', 1, '', '', ? )",
                list_of( "securityKey" ) )
        );

    bool truncated;

    {
        Bytes key_bytes_short( 32, 'a' );
        ins_stmt_ptr->parameters()["securityKey"].set( key_bytes_short, &truncated );
    }

    BOOST_CHECK( ! truncated );

    Bytes key_bytes_long( 33, 'b' );

    ins_stmt_ptr->parameters()["securityKey"].set( key_bytes_long, &truncated );

    BOOST_CHECK( truncated );

    ins_stmt_ptr->execute();

    ResultSetPtr q_stmt_ptr(Fixture::conn_ptr->query( "SELECT securityKey FROM TBGQBlock WHERE blockId='CXXDBTST_TB'" ));

    Bytes key_bytes_exp( 32, 'b' );

    BOOST_REQUIRE( q_stmt_ptr->fetch() );

    Bytes key_bytes(q_stmt_ptr->columns()["securityKey"].getBytes());

    BOOST_REQUIRE_EQUAL( key_bytes.size(), key_bytes_exp.size() );
    BOOST_CHECK( std::equal( key_bytes.begin(), key_bytes.end(), key_bytes_exp.begin() ) );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQBlock WHERE blockId='CXXDBTST_TB'" );
}


BOOST_AUTO_TEST_CASE( test_cast )
{
    // Cast is more lenient as far as type checking goes, but throws runtime exceptions.
    // Can cast a small uint64_t to a BIGINT col.
    // If cast a large uint64_t to a BIGINT then throws ValueTooBig

    UpdateStatementPtr ins_stmt_ptr( Fixture::conn_ptr->prepareUpdate( "INSERT INTO TBGQDIAGSEVENTLOG ( runId ) VALUES ( ? )" ) );
    ins_stmt_ptr->parameters()[1].cast( uint64_t( 23456 ) );

    BOOST_CHECK_THROW( ins_stmt_ptr->parameters()[1].cast( uint64_t( -2 ) ), ValueTooBig );

    ins_stmt_ptr->execute();

    ResultSetPtr q_stmt_ptr( Fixture::conn_ptr->query( "SELECT runId FROM TBGQDIAGSEVENTLOG WHERE runId = 23456" ) );

    BOOST_REQUIRE( q_stmt_ptr->fetch() );

    BOOST_CHECK_EQUAL( q_stmt_ptr->columns()["runId"].getInt64(), 23456 );

    Fixture::conn_ptr->executeUpdate( "DELETE FROM TBGQDIAGSEVENTLOG WHERE runId = 23456" );

    // SMALLINT
    Fixture::conn_ptr->prepareUpdate( "INSERT INTO TBGQICON ( iConChecksum ) VALUES ( ? )" )->parameters()[1].cast( 32767 );
    BOOST_CHECK_THROW(
            Fixture::conn_ptr->prepareUpdate( "INSERT INTO TBGQICON ( iConChecksum ) VALUES ( ? )" )->parameters()[1].cast( 32768 ),
            ValueTooBig
        );
}
