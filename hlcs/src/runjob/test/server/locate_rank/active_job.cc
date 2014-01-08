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
#define BOOST_TEST_MODULE active_job
#include <boost/test/unit_test.hpp>

#include "server/handlers/locate_rank/ActiveJob.h"
#include "server/handlers/locate_rank/Job.h"

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/job/Operations.h>

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/GenBlockParams.h>
#include <db/include/api/genblock.h>

#include <utility/include/Log.h>

using namespace runjob::server;

class MyFixture
{
public:
    MyFixture()
    {
        const bgq::utility::Properties::Ptr properties(bgq::utility::Properties::create());
        bgq::utility::initializeLogging( *properties);
        BGQDB::init( properties, 1 );
        
        this->createBlocks();
    }

    ~MyFixture()
    {
        for ( unsigned i = 0; i < 16; ++i ) {
            std::ostringstream os;
            os << "N";
            os << std::setw(2) << std::setfill('0') << i;

            const std::string blockid( "locate_rank-" + os.str() );

            (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
            (void)BGQDB::deleteBlock( blockid );
        }
    }

    void createBlocks()
    {
        for ( unsigned i = 0; i < 16; ++i ) {
            std::ostringstream os;
            os << "N";
            os << std::setw(2) << std::setfill('0') << i;

            const std::string blockid( "locate_rank-" + os.str() );

            BGQDB::GenBlockParams params;
            params.setBlockId( blockid );
            params.setOwner( "nobody" );
            params.setMidplane( "R00-M0" );
            params.setNodeBoardAndCount( os.str(), 1 );

            // in case the block was left over from a previous run
            (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
            (void)BGQDB::deleteBlock( blockid );

            BGQDB::genBlockEx( params );
            assert( BGQDB::setBlockStatus( blockid, BGQDB::ALLOCATED ) == BGQDB::OK );
            assert( BGQDB::setBlockStatus( blockid, BGQDB::BOOTING ) == BGQDB::OK );
            assert( BGQDB::setBlockStatus( blockid, BGQDB::INITIALIZED ) == BGQDB::OK );
        }
    }
};

BOOST_GLOBAL_FIXTURE( MyFixture );

BOOST_AUTO_TEST_CASE( fullblock_job )
{
    BGQDB::job::Operations operations;
    const cxxdb::ConnectionPtr db_connection( BGQDB::DBConnectionPool::instance().getConnection() );
    assert( db_connection );

    for ( unsigned i = 0; i < 16; ++i ) {
        std::ostringstream os;
        os << "N";
        os << std::setw(2) << std::setfill('0') << i;
        const std::string blockid( "locate_rank-" + os.str() );

        BGQDB::job::InsertInfo job;
        job.setBlock( blockid );
        job.setExe( "test" );
        job.setArgs( BGQDB::job::InsertInfo::Args() );
        job.setEnvs( BGQDB::job::InsertInfo::Envs() );
        job.setCwd( "testcwd" );
        job.setUserName( "testuser" );
        job.setProcessesPerNode( 1 );
        job.setNp( 32 );
        job.setShape( BGQDB::job::Shape::NodeBoard );
        job.setMapping( "ABCDET" );
        job.setHostname( "testhost" );
        job.setPid( 1 );

        BGQDB::job::Id id;

        operations.insert( job, &id );

        // job scaffolding is done, here are the actual locate_rank tests
        const runjob::server::handlers::locate_rank::Job::Ptr locate_job(
                runjob::server::handlers::locate_rank::Job::create(
                    db_connection,
                    id
                    )
                );

        BOOST_CHECK(
                boost::dynamic_pointer_cast<runjob::server::handlers::locate_rank::ActiveJob>(locate_job)
                );

        switch( i ) {
            // these locations come from control/src/bgqconfig/BGQTopology.cc
            case 0:  BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N00-J00") ); break;
            case 1:  BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N01-J01") ); break;
            case 2:  BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N02-J12") ); break;
            case 3:  BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N03-J13") ); break;
            case 4:  BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N04-J29") ); break;
            case 5:  BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N05-J28") ); break;
            case 6:  BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N06-J17") ); break;
            case 7:  BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N07-J16") ); break;
            case 8:  BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N08-J04") ); break;
            case 9:  BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N09-J05") ); break;
            case 10: BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N10-J08") ); break;
            case 11: BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N11-J09") ); break;
            case 12: BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N12-J25") ); break;
            case 13: BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N13-J24") ); break;
            case 14: BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N14-J21") ); break;
            case 15: BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N15-J20") ); break;
            default: 
                     // shouldn't get here
                     BOOST_CHECK_EQUAL( 1, 0u );
                     break;
        }

        operations.remove( id, BGQDB::job::RemoveInfo() );
    }
}

BOOST_AUTO_TEST_CASE( subblock_single_node_job )
{
    BGQDB::job::Operations operations;
    const cxxdb::ConnectionPtr db_connection( BGQDB::DBConnectionPool::instance().getConnection() );
    assert( db_connection );

    const std::string blockid( "locate_rank-N12" ); // N12 chosen for no particular reason

    for ( unsigned i = 0; i < 32; ++i ) {
        std::ostringstream os;
        os << "R00-M0-N12-J";
        os << std::setw(2) << std::setfill('0') << i;

        const std::string corner( os.str() );

        BGQDB::job::InsertInfo job;
        job.setBlock( blockid );
        job.setExe( "test" );
        job.setArgs( BGQDB::job::InsertInfo::Args() );
        job.setEnvs( BGQDB::job::InsertInfo::Envs() );
        job.setCwd( "testcwd" );
        job.setUserName( "testuser" );
        job.setProcessesPerNode( 1 );
        job.setNp( 1 );
        job.setShape( BGQDB::job::Shape::SingleNode );
        job.setMapping( "ABCDET" );
        job.setHostname( "testhost" );
        job.setPid( 1 );
        job.setCorner( corner );

        BGQDB::job::Id id;

        operations.insert( job, &id );

        // job scaffolding is done, here are the actual locate_rank tests
        const runjob::server::handlers::locate_rank::Job::Ptr locate_job(
                runjob::server::handlers::locate_rank::Job::create(
                    db_connection,
                    id
                    )
                );

        BOOST_CHECK(
                boost::dynamic_pointer_cast<runjob::server::handlers::locate_rank::ActiveJob>(locate_job)
                );

        BOOST_CHECK_EQUAL(
                locate_job->find(0), corner
                );

        operations.remove( id, BGQDB::job::RemoveInfo() );
    }
}

BOOST_AUTO_TEST_CASE( subblock_multi_node_job )
{
    BGQDB::job::Operations operations;
    const cxxdb::ConnectionPtr db_connection( BGQDB::DBConnectionPool::instance().getConnection() );
    assert( db_connection );

    const std::string blockid( "locate_rank-N08" ); // N08 chosen for no particular reason
    const std::string corner( "R00-M0-N08-J04" ); // this is 0,0,0,0,0 for N08

    BGQDB::job::InsertInfo job;
    job.setBlock( blockid );
    job.setExe( "test" );
    job.setArgs( BGQDB::job::InsertInfo::Args() );
    job.setEnvs( BGQDB::job::InsertInfo::Envs() );
    job.setCwd( "testcwd" );
    job.setUserName( "testuser" );
    job.setProcessesPerNode( 1 );
    job.setNp( 8 ); // 8 nodes in this job
    job.setShape( BGQDB::job::Shape(2,2,1,2,1) ); // again, 8 nodes in use
    job.setMapping( "ABCDET" );
    job.setHostname( "testhost" );
    job.setPid( 8 );
    job.setCorner( corner );

    BGQDB::job::Id id;
    operations.insert( job, &id );

    // job scaffolding is done, here are the actual locate_rank tests
    const runjob::server::handlers::locate_rank::Job::Ptr locate_job(
        runjob::server::handlers::locate_rank::Job::create(
            db_connection,
            id
            )
        );

    BOOST_CHECK(
        boost::dynamic_pointer_cast<runjob::server::handlers::locate_rank::ActiveJob>(locate_job)
        );

    for ( unsigned i = 0; i <= 7; ++i ) {
        switch( i ) {
            // assumes ABCDET mapping
            case 0: BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N08-J04") ); break; // 0,0,0,0,0
            case 1: BOOST_CHECK_EQUAL( locate_job->find(1), std::string("R00-M0-N08-J08") ); break; // 0,0,0,1,0
            case 2: BOOST_CHECK_EQUAL( locate_job->find(2), std::string("R00-M0-N08-J07") ); break; // 0,1,0,0,0
            case 3: BOOST_CHECK_EQUAL( locate_job->find(3), std::string("R00-M0-N08-J11") ); break; // 0,1,0,1,0
            case 4: BOOST_CHECK_EQUAL( locate_job->find(4), std::string("R00-M0-N08-J25") ); break; // 1,0,0,0,0
            case 5: BOOST_CHECK_EQUAL( locate_job->find(5), std::string("R00-M0-N08-J21") ); break; // 1,0,0,1,0
            case 6: BOOST_CHECK_EQUAL( locate_job->find(6), std::string("R00-M0-N08-J26") ); break; // 1,1,0,0,0
            case 7: BOOST_CHECK_EQUAL( locate_job->find(7), std::string("R00-M0-N08-J22") ); break; // 1,1,0,1,0
            default:
                     // shouldn't get here
                     BOOST_CHECK_EQUAL( 1, 0u );
                     break;
        }
    }

    operations.remove( id, BGQDB::job::RemoveInfo() );
}

BOOST_AUTO_TEST_CASE( subblock_single_core_job )
{
    BGQDB::job::Operations operations;
    const cxxdb::ConnectionPtr db_connection( BGQDB::DBConnectionPool::instance().getConnection() );
    assert( db_connection );

    const std::string blockid( "locate_rank-N14" ); // N14 chosen for no particular reason
    const std::string corner( "R00-M0-N14-J00" );

    BGQDB::job::InsertInfo job;
    job.setBlock( blockid );
    job.setExe( "test" );
    job.setArgs( BGQDB::job::InsertInfo::Args() );
    job.setEnvs( BGQDB::job::InsertInfo::Envs() );
    job.setCwd( "testcwd" );
    job.setUserName( "testuser" );
    job.setProcessesPerNode( 1 );
    job.setNp( 1 );
    job.setShape( BGQDB::job::Shape::SingleNode );
    job.setMapping( "ABCDET" );
    job.setHostname( "testhost" );
    job.setPid( 1 );
    job.setCorner( corner + "-C00" ); // sub-node jobs have a core specification

    BGQDB::job::Id id;

    operations.insert( job, &id );

    // job scaffolding is done, here are the actual locate_rank tests
    const runjob::server::handlers::locate_rank::Job::Ptr locate_job(
            runjob::server::handlers::locate_rank::Job::create(
                db_connection,
                id
                )
            );

    BOOST_CHECK(
            boost::dynamic_pointer_cast<runjob::server::handlers::locate_rank::ActiveJob>(locate_job)
            );

    BOOST_CHECK_EQUAL(
            locate_job->find(0), corner
            );

    operations.remove( id, BGQDB::job::RemoveInfo() );
}
