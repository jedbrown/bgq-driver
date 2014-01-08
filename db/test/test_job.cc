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

// Tests for BGQDB::job functions, inserting, updating, and deleting jobs.
// These APIs are called by runjob.

#include "api/job/Operations.h"
#include "api/job/exception.h"

#include "api/BGQDBlib.h"
#include "api/genblock.h"

#include "api/cxxdb/cxxdb.h"

#include "api/tableapi/DBConnectionPool.h"

#include "api/tableapi/gensrc/bgqtableapi.h"

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

using boost::lexical_cast;

using std::string;

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE job
#include <boost/test/unit_test.hpp>

class MyFixture
{
public:

    static cxxdb::ConnectionPtr s_conn_ptr;
    static boost::shared_ptr<BGQDB::job::Operations> s_job_ops_ptr;


    MyFixture() {
        bgq::utility::Properties::Ptr properties_ptr(bgq::utility::Properties::create());

        bgq::utility::initializeLogging( *properties_ptr );

        BGQDB::init( properties_ptr, 1 );

        s_job_ops_ptr.reset( new BGQDB::job::Operations() );

        s_conn_ptr = BGQDB::DBConnectionPool::instance().getConnection();

        BGQDB::genBlock( "TestJob0", "R00-M0", "nobody" ); // ignore error.
        if ( BGQDB::setBlockStatus( "TestJob0", BGQDB::ALLOCATED ) != BGQDB::OK ) { std::cout << "failed to set block state A\n"; }
        if ( BGQDB::setBlockStatus( "TestJob0", BGQDB::BOOTING ) != BGQDB::OK ) { std::cout << "failed to set block state B\n"; }
        if ( BGQDB::setBlockStatus( "TestJob0", BGQDB::INITIALIZED ) != BGQDB::OK ) { std::cout << "failed to set block state I\n"; }

        BGQDB::genBlock( "TestJobNotInitialized", "R00-M0", "nobody" ); // ignore error.

    }


    ~MyFixture() {

        BGQDB::setBlockStatus( "TestJob0", BGQDB::FREE );
        BGQDB::deleteBlock( "TestJob0" );

        BGQDB::deleteBlock( "TestJobNotInitialized" );

        s_conn_ptr.reset();
        s_job_ops_ptr.reset();

    }
};


cxxdb::ConnectionPtr MyFixture::s_conn_ptr;
boost::shared_ptr<BGQDB::job::Operations> MyFixture::s_job_ops_ptr;


BOOST_GLOBAL_FIXTURE( MyFixture );


BOOST_AUTO_TEST_CASE( test_format )
{
    BOOST_CHECK_EQUAL( BGQDB::job::InsertInfo::format( BGQDB::job::InsertInfo::Args() ), "" ); // no args.
    BOOST_CHECK_EQUAL( BGQDB::job::InsertInfo::format( boost::assign::list_of( "20" ) ), "20 " ); // simple
    BOOST_CHECK_EQUAL( BGQDB::job::InsertInfo::format( boost::assign::list_of( "" ) ), " " ); // an empty arg
    BOOST_CHECK_EQUAL( BGQDB::job::InsertInfo::format( boost::assign::list_of( " " ) ), "\\  " ); // spaces are escaped.
    BOOST_CHECK_EQUAL( BGQDB::job::InsertInfo::format( boost::assign::list_of( " \\n" ) ), "\\ \\\\n " ); // \ is escaped.
    BOOST_CHECK_EQUAL( BGQDB::job::InsertInfo::format( boost::assign::list_of( "-n" )( "my file name" ) ), "-n my\\ file\\ name " ); // multiple separated by space.
    BOOST_CHECK_EQUAL( BGQDB::job::InsertInfo::format( boost::assign::list_of( "HOME=/home/bknudson" )( "DISPLAY=:0" ) ), "HOME=/home/bknudson DISPLAY=:0 " ); // environmentals.
}


BOOST_AUTO_TEST_CASE( test_insert_full_block_and_delete )
{
    // Can insert a job, and the database is updated.
    // Can delete the job and the database is updated.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );
    insert_info.setExe( "test" );
    insert_info.setArgs( BGQDB::job::InsertInfo::Args() );
    insert_info.setEnvs( BGQDB::job::InsertInfo::Envs() );
    insert_info.setCwd( "testcwd" );
    insert_info.setUserName( "testuser" );
    insert_info.setProcessesPerNode( 1 );
    insert_info.setShape( BGQDB::job::Shape::Midplane );
    insert_info.setSchedulerData( "testscheddata" );
    insert_info.setHostname( "testhost" );
    insert_info.setPid( 1 );


    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );


    // Check was inserted properly.

    {
        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob WHERE id=" + lexical_cast<string>( job_id ) ));

        BOOST_REQUIRE( rs_ptr->fetch() );

        // check job fields...
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>(), job_id );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::USERNAME_COL].getString(), "testuser" );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::BLOCKID_COL].getString(), "TestJob0" );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::EXECUTABLE_COL].getString(), "test" );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::WORKINGDIR_COL].getString(), "testcwd" );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::STATUS_COL].getString(), BGQDB::job::status_code::Setup );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::ARGS_COL].getString(), "" );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::ENVS_COL].getString(), "" );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::MAPPING_COL].getString(), "" );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::NODESUSED_COL].as<uint64_t>(), 512UL );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEA_COL].as<uint16_t>(), 4U );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEB_COL].as<uint16_t>(), 4U );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEC_COL].as<uint16_t>(), 4U );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPED_COL].as<uint16_t>(), 4U );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEE_COL].as<uint16_t>(), 2U );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::PROCESSESPERNODE_COL].as<uint16_t>(), 1U );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SCHEDULERDATA_COL].getString(), "testscheddata" );
        BOOST_CHECK( rs_ptr->columns()[BGQDB::DBTJob::CORNER_COL].isNull() );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::HOSTNAME_COL].getString(), "testhost" );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::PID_COL].as<pid_t>(), 1 );

        BOOST_CHECK( ! rs_ptr->fetch() );

        // should not have inserted anything into tbgqjobmap for the job!

        rs_ptr = MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJobMap WHERE id=" + lexical_cast<string>( job_id ) );

        BOOST_CHECK( ! rs_ptr->fetch() );
    }


    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo() );

    {
        // Make sure the job was removed from job table.

        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob WHERE id=" + lexical_cast<string>( job_id ) ));
        BOOST_CHECK( ! rs_ptr->fetch() );

        // Make sure the job was removed from jobMap table.

        rs_ptr = MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJobMap WHERE id=" + lexical_cast<string>( job_id ) );
        BOOST_CHECK( ! rs_ptr->fetch() );

        // Make sure the job was added to the job_history table.

        rs_ptr = MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob_history WHERE id=" + lexical_cast<string>( job_id ) );

        if ( rs_ptr->fetch() ) {
            // check job_history fields...

            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::USERNAME_COL].getString(), "testuser" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::BLOCKID_COL].getString(), "TestJob0" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::EXECUTABLE_COL].getString(), "test" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::WORKINGDIR_COL].getString(), "testcwd" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::STATUS_COL].getString(), BGQDB::job::status_code::Terminated );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::ARGS_COL].getString(), "" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::ENVS_COL].getString(), "" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::MAPPING_COL].getString(), "" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::NODESUSED_COL].as<uint64_t>(), 512UL );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::SHAPEA_COL].as<uint16_t>(), 4U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::SHAPEB_COL].as<uint16_t>(), 4U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::SHAPEC_COL].as<uint16_t>(), 4U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::SHAPED_COL].as<uint16_t>(), 4U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::SHAPEE_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::PROCESSESPERNODE_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::SCHEDULERDATA_COL].getString(), "testscheddata" );
            BOOST_CHECK( rs_ptr->columns()[BGQDB::DBTJob_history::CORNER_COL].isNull() );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::HOSTNAME_COL].getString(), "testhost" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::PID_COL].as<pid_t>(), 1 );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::EXITSTATUS_COL].as<int>(), 0 );
            BOOST_CHECK( rs_ptr->columns()[BGQDB::DBTJob_history::ERRTEXT_COL].isNull() );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {

            BOOST_CHECK( false );

        }

    }
}


BOOST_AUTO_TEST_CASE( test_insert_subblock_full_midplane )
{
    // Can insert a job that's a subblock job over a midplane and the database is updated correctly.
    // CORNER should be set and a row in JobMap.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );
    insert_info.setCorner( "R00-M0-N00-J00" );
    insert_info.setShape( BGQDB::job::Shape::Midplane );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    // Check was inserted properly.

    {
        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob WHERE id=" + lexical_cast<string>( job_id ) ));

        if ( rs_ptr->fetch() ) {

            // check job fields...
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::BLOCKID_COL].getString(), "TestJob0" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::NODESUSED_COL].as<uint64_t>(), 512UL );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEA_COL].as<uint16_t>(), 4U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEB_COL].as<uint16_t>(), 4U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEC_COL].as<uint16_t>(), 4U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPED_COL].as<uint16_t>(), 4U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEE_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::CORNER_COL].getString(), "R00-M0-N00-J00" );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {

            BOOST_CHECK( false );

        }


        // check JobMap.

        rs_ptr = MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJobMap WHERE id=" + lexical_cast<string>( job_id ) );

        if ( rs_ptr->fetch() ) {

            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK( rs_ptr->columns()[BGQDB::DBTJobmap::NODEBOARD_COL].isNull() );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::MIDPLANE_COL].getString(), "R00-M0" );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {
            BOOST_CHECK( false );
        }
    }


    // cleanup!
    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo() );
}


BOOST_AUTO_TEST_CASE( test_insert_subblock_nodeboard )
{
    // Can insert a job that's a subblock job on a node board and the database is updated correctly.
    // CORNER should be set and a row in JobMap.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );
    insert_info.setCorner( "R00-M0-N00-J00" );
    insert_info.setShape( BGQDB::job::Shape::NodeBoard );
    insert_info.setNodeBoardPositions( boost::assign::list_of( "N00" ) );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    // Check was inserted properly.

    {
        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob WHERE id=" + lexical_cast<string>( job_id ) ));

        if ( rs_ptr->fetch() ) {

            // check job fields...
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::BLOCKID_COL].getString(), "TestJob0" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::NODESUSED_COL].as<uint64_t>(), 32UL );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEA_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEB_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEC_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPED_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEE_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::CORNER_COL].getString(), "R00-M0-N00-J00" );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {

            BOOST_CHECK( false );

        }


        // check JobMap.

        rs_ptr = MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJobMap WHERE id=" + lexical_cast<string>( job_id ) );

        if ( rs_ptr->fetch() ) {

            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::NODEBOARD_COL].getString(), "N00" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::MIDPLANE_COL].getString(), "R00-M0" );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {

            BOOST_CHECK( false );

        }
    }


    // cleanup!
    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo() );
}


BOOST_AUTO_TEST_CASE( test_insert_subblock_nodeboards )
{
    // Can insert a job that's a subblock job over two node boards and the database is updated correctly.
    // CORNER should be set and rows in JobMap.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );
    insert_info.setCorner( "R00-M0-N00-J00" );
    insert_info.setShape( BGQDB::job::Shape( 2, 2, 4, 2, 2 ) );
    insert_info.setNodeBoardPositions( boost::assign::list_of( "N00" )( "N01" ) );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    // Check was inserted properly.

    {
        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob WHERE id=" + lexical_cast<string>( job_id ) ));

        if ( rs_ptr->fetch() ) {

            // check job fields...
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::BLOCKID_COL].getString(), "TestJob0" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::NODESUSED_COL].as<uint64_t>(), 64UL );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEA_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEB_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEC_COL].as<uint16_t>(), 4U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPED_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEE_COL].as<uint16_t>(), 2U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::CORNER_COL].getString(), "R00-M0-N00-J00" );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {

            BOOST_CHECK( false );

        }


        // check JobMap.

        rs_ptr = MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJobMap WHERE id=" + lexical_cast<string>( job_id ) + " ORDER BY " + BGQDB::DBTJobmap::NODEBOARD_COL );

        if ( rs_ptr->fetch() ) {

            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::NODEBOARD_COL].getString(), "N00" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::MIDPLANE_COL].getString(), "R00-M0" );

        } else {

            BOOST_CHECK( false );

        }

        if ( rs_ptr->fetch() ) {

            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::NODEBOARD_COL].getString(), "N01" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::MIDPLANE_COL].getString(), "R00-M0" );

        } else {

            BOOST_CHECK( false );
        }

        BOOST_CHECK( ! rs_ptr->fetch() );

    }


    // cleanup!
    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo() );
}


BOOST_AUTO_TEST_CASE( test_insert_subblock_single_node )
{
    // Can insert a job that's a subblock job on a single node and the database is updated correctly.
    // CORNER should be set and a row in JobMap.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );
    insert_info.setCorner( "R00-M0-N00-J00" );
    insert_info.setShape( BGQDB::job::Shape::SingleNode );
    insert_info.setNodeBoardPositions( boost::assign::list_of( "N00" ) );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    // Check was inserted properly.

    {
        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob WHERE id=" + lexical_cast<string>( job_id ) ));

        if ( rs_ptr->fetch() ) {

            // check job fields...
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::BLOCKID_COL].getString(), "TestJob0" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::NODESUSED_COL].as<uint64_t>(), 1UL );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEA_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEB_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEC_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPED_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEE_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::CORNER_COL].getString(), "R00-M0-N00-J00" );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {

            BOOST_CHECK( false );

        }


        // check JobMap.

        rs_ptr = MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJobMap WHERE id=" + lexical_cast<string>( job_id ) );

        if ( rs_ptr->fetch() ) {

            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::NODEBOARD_COL].getString(), "N00" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::MIDPLANE_COL].getString(), "R00-M0" );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {

            BOOST_CHECK( false );

        }
    }


    // cleanup!
    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo() );
}

BOOST_AUTO_TEST_CASE( test_insert_subblock_single_core )
{
    // Can insert a job that's a subblock job on a single core and the database is updated correctly.
    // CORNER should be set and a row in JobMap.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );
    insert_info.setCorner( "R00-M0-N00-J00-C00" );
    insert_info.setShape( BGQDB::job::Shape::SingleCore );
    insert_info.setNodeBoardPositions( boost::assign::list_of( "N00" ) );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    // Check was inserted properly.

    {
        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob WHERE id=" + lexical_cast<string>( job_id ) ));

        if ( rs_ptr->fetch() ) {

            // check job fields...
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::BLOCKID_COL].getString(), "TestJob0" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::NODESUSED_COL].as<uint64_t>(), 0UL );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEA_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEB_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEC_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPED_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::SHAPEE_COL].as<uint16_t>(), 1U );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::CORNER_COL].getString(), "R00-M0-N00-J00-C00" );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {

            BOOST_CHECK( false );

        }


        // check JobMap.

        rs_ptr = MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJobMap WHERE id=" + lexical_cast<string>( job_id ) );

        if ( rs_ptr->fetch() ) {

            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::ID_COL].as<BGQDB::job::Id>(), job_id );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::NODEBOARD_COL].getString(), "N00" );
            BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJobmap::MIDPLANE_COL].getString(), "R00-M0" );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {

            BOOST_CHECK( false );

        }
    }


    // cleanup!
    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo() );
}

BOOST_AUTO_TEST_CASE( test_insert_subnode_user_conflict )
{
    // Multiple sub-node jobs on a single node by different users should fail

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );
    insert_info.setCorner( "R00-M0-N00-J00-C00" );
    insert_info.setShape( BGQDB::job::Shape::SingleNode );
    insert_info.setUserName( "foo" );
    insert_info.setNodeBoardPositions( boost::assign::list_of( "N00" ) );

    BGQDB::job::Id one;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &one
        );

    // insert second job with different user
    insert_info.setCorner( "R00-M0-N00-J00-C01" );
    insert_info.setUserName( "bar" );

    BGQDB::job::Id two;

    // it should fail
    BOOST_CHECK_THROW(
            MyFixture::s_job_ops_ptr->insert(
                insert_info,
                &two
                ),
            BGQDB::job::exception::SubNodeJobUserConflict
            );

    // but another job by the same user should work
    insert_info.setUserName( "foo" );

    BGQDB::job::Id three;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &three
        );

    // cleanup!
    MyFixture::s_job_ops_ptr->remove( one, BGQDB::job::RemoveInfo() );
    MyFixture::s_job_ops_ptr->remove( three, BGQDB::job::RemoveInfo() );
}

BOOST_AUTO_TEST_CASE( test_insert_subblock_user_no_conflict )
{
    // Multiple sub-block jobs on a single block by different users should not fail

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );
    insert_info.setCorner( "R00-M0-N00-J00" );
    insert_info.setShape( BGQDB::job::Shape::SingleNode );
    insert_info.setUserName( "foo" );
    insert_info.setNodeBoardPositions( boost::assign::list_of( "N00" ) );

    BGQDB::job::Id one;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &one
        );

    // insert second job with different user
    insert_info.setCorner( "R00-M0-N00-J01" );
    insert_info.setUserName( "bar" );

    BGQDB::job::Id two;
    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &two
        );

    // cleanup!
    MyFixture::s_job_ops_ptr->remove( one, BGQDB::job::RemoveInfo() );
    MyFixture::s_job_ops_ptr->remove( two, BGQDB::job::RemoveInfo() );
}


BOOST_AUTO_TEST_CASE( test_no_scheduler_data )
{
    // If insert job with no scheduler data, then scheduler data is set to NULL.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );


    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );


    // Check was inserted properly.

    {
        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob WHERE id=" + lexical_cast<string>( job_id ) ));

        BOOST_REQUIRE( rs_ptr->fetch() );

        // check job fields...
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>(), job_id );
        BOOST_CHECK( rs_ptr->columns()[BGQDB::DBTJob::SCHEDULERDATA_COL].isNull() );

        BOOST_CHECK( ! rs_ptr->fetch() );

        // should not have inserted anything into tbgqjobmap for the job!

        rs_ptr = MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJobMap WHERE id=" + lexical_cast<string>( job_id ) );

        BOOST_CHECK( ! rs_ptr->fetch() );
    }


    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo() );

    {
        // Make sure the job was added to the job_history table.

        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob_history WHERE id=" + lexical_cast<string>( job_id ) ));

        if ( rs_ptr->fetch() ) {
            // check job_history fields...

            BOOST_CHECK( rs_ptr->columns()[BGQDB::DBTJob_history::SCHEDULERDATA_COL].isNull() );

            BOOST_CHECK( ! rs_ptr->fetch() );

        } else {

            BOOST_CHECK( false );

        }

    }
}


BOOST_AUTO_TEST_CASE( test_update )
{
    // Can update the status of the job to the different status values, database reflects the change.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    MyFixture::s_job_ops_ptr->update(
            job_id,
            BGQDB::job::status::Starting
        );

    // Query the database to see if it's Starting.

    {
        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( string() + "SELECT status FROM bgqJob WHERE id=" + lexical_cast<string>( job_id ) ));

        rs_ptr->fetch();
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::STATUS_COL].getString(), BGQDB::job::status_code::Starting );
    }

    MyFixture::s_job_ops_ptr->update(
            job_id,
            BGQDB::job::status::Running
        );

    {
        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( string() + "SELECT status FROM bgqJob WHERE id=" + lexical_cast<string>( job_id ) ));

        rs_ptr->fetch();
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::STATUS_COL].getString(), BGQDB::job::status_code::Running );
    }

    MyFixture::s_job_ops_ptr->update(
            job_id,
            BGQDB::job::status::Cleanup
        );

    {
        cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( string() + "SELECT status FROM bgqJob WHERE id=" + lexical_cast<string>( job_id ) ));

        rs_ptr->fetch();
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob::STATUS_COL].getString(), BGQDB::job::status_code::Cleanup );
    }


    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo() );
}


BOOST_AUTO_TEST_CASE( test_remove_exit_0 )
{
    // Job can end with exit status = 0, status is correct in the DB.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo( EXIT_SUCCESS ) );

    // check the database.

    cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob_history WHERE id=" + lexical_cast<string>( job_id ) ));

    if ( rs_ptr->fetch() ) {
        // check job_history fields...

        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::ID_COL].as<BGQDB::job::Id>(), job_id );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::STATUS_COL].getString(), BGQDB::job::status_code::Terminated );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::EXITSTATUS_COL].as<int>(), EXIT_SUCCESS );
        BOOST_CHECK( rs_ptr->columns()[BGQDB::DBTJob_history::ERRTEXT_COL].isNull() );

        BOOST_CHECK( ! rs_ptr->fetch() );

    } else {

        BOOST_CHECK( false );

    }

}


BOOST_AUTO_TEST_CASE( test_remove_exit_nonzero )
{
    // Jobs can end with non-zero exit status, status is correct in the DB.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo( EXIT_FAILURE ) );

    // check the database.

    cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob_history WHERE id=" + lexical_cast<string>( job_id ) ));

    if ( rs_ptr->fetch() ) {
        // check job_history fields...

        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::ID_COL].as<BGQDB::job::Id>(), job_id );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::STATUS_COL].getString(), BGQDB::job::status_code::Terminated );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::EXITSTATUS_COL].as<int>(), EXIT_FAILURE );
        BOOST_CHECK( rs_ptr->columns()[BGQDB::DBTJob_history::ERRTEXT_COL].isNull() );

        BOOST_CHECK( ! rs_ptr->fetch() );

    } else {

        BOOST_CHECK( false );

    }
}


BOOST_AUTO_TEST_CASE( test_remove_fail )
{
    // Jobs can fail, status is correct in the DB.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    MyFixture::s_job_ops_ptr->remove( job_id, BGQDB::job::RemoveInfo( "The job failed for some reason that I'll explain here." ) );

    // check the database.

    cxxdb::ResultSetPtr rs_ptr(MyFixture::s_conn_ptr->query( "SELECT * FROM tBgqJob_history WHERE id=" + lexical_cast<string>( job_id ) ));

    if ( rs_ptr->fetch() ) {
        // check job_history fields...

        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::STATUS_COL].getString(), BGQDB::job::status_code::Error );
        BOOST_CHECK( rs_ptr->columns()[BGQDB::DBTJob_history::EXITSTATUS_COL].isNull() );
        BOOST_CHECK_EQUAL( rs_ptr->columns()[BGQDB::DBTJob_history::ERRTEXT_COL].getString(), "The job failed for some reason that I'll explain here." );

        BOOST_CHECK( ! rs_ptr->fetch() );

    } else {

        BOOST_CHECK( false );

    }
}


BOOST_AUTO_TEST_CASE( test_insert_no_block )
{
    // If try to insert a job and the block doesn't exist will get BGQDB::job::exception::BlockNotFound exception.

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "NOTABLOCK" );

    BGQDB::job::Id job_id;

    BOOST_CHECK_THROW(
            MyFixture::s_job_ops_ptr->insert(
                insert_info,
                &job_id
            ),
            BGQDB::job::exception::BlockNotFound
        );
}


BOOST_AUTO_TEST_CASE( test_insert_block_not_initialized )
{
    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJobNotInitialized" );

    BGQDB::job::Id job_id;

    BOOST_CHECK_THROW(
            MyFixture::s_job_ops_ptr->insert(
                insert_info,
                &job_id
            ),
            BGQDB::job::exception::BlockNotInitialized
        );
}


BOOST_AUTO_TEST_CASE( test_insert_block_initialized_with_action )
{
    MyFixture::s_conn_ptr->execute( "UPDATE tbgqblock set action='D' where blockid='TestJob0'" );
    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );

    BGQDB::job::Id job_id;

    BOOST_CHECK_THROW(
            MyFixture::s_job_ops_ptr->insert(
                insert_info,
                &job_id
            ),
            BGQDB::job::exception::BlockActionNotEmpty
        );
    MyFixture::s_conn_ptr->execute( "UPDATE tbgqblock set action='' where blockid='TestJob0'" );
}

BOOST_AUTO_TEST_CASE( test_update_no_job )
{
    // If try to update and the job doesn't exist then get job::exception::JobNotFound exception.

    BOOST_CHECK_THROW(
            MyFixture::s_job_ops_ptr->update(
                    BGQDB::job::Id(999999999),
                    BGQDB::job::status::Running
                ),
            BGQDB::job::exception::JobNotFound
        );
}


BOOST_AUTO_TEST_CASE( test_update_invalid_status )
{
    // Can't set the status to an invalid value.

    BOOST_CHECK_THROW(
            MyFixture::s_job_ops_ptr->update(
                    BGQDB::job::Id(1),
                    BGQDB::job::status::Terminated
                ),
            BGQDB::job::exception::InvalidStatus
        );

    BOOST_CHECK_THROW(
            MyFixture::s_job_ops_ptr->update(
                    BGQDB::job::Id(1),
                    BGQDB::job::status::Error
                ),
            BGQDB::job::exception::InvalidStatus
        );
}


BOOST_AUTO_TEST_CASE( test_remove_no_job )
{
    // If try to remove job and the job doesn't exist then get job::exception::JobNotFound exception.

    BOOST_CHECK_THROW(
            MyFixture::s_job_ops_ptr->remove( BGQDB::job::Id(999999999), BGQDB::job::RemoveInfo() ),
            BGQDB::job::exception::JobNotFound
        );
}

BOOST_AUTO_TEST_CASE( test_remove_job_truncate_error_text )
{
    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );
    insert_info.setShape( BGQDB::job::Shape::Midplane );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    // error text should be 1 character larger than column allows
    std::string errtext( sizeof(BGQDB::DBTJob_history::ERRTEXT_COL) + 1, 'a' );
    BGQDB::job::RemoveInfo remove_info(  errtext );

    // should not throw due to truncated error tex
    MyFixture::s_job_ops_ptr->remove( job_id, remove_info );
}

BOOST_AUTO_TEST_CASE( test_remove_job_with_exit_status_and_truncate_error_text )
{
    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TestJob0" );
    insert_info.setShape( BGQDB::job::Shape::Midplane );

    BGQDB::job::Id job_id;

    MyFixture::s_job_ops_ptr->insert(
            insert_info,
            &job_id
        );

    // error text should be 1 character larger than column allows
    std::string errtext( sizeof(BGQDB::DBTJob_history::ERRTEXT_COL) + 1, 'a' );
    int exit_status = 1;
    BGQDB::job::RemoveInfo remove_info( exit_status, errtext );

    // should not throw due to truncated error tex
    MyFixture::s_job_ops_ptr->remove( job_id, remove_info );
}
