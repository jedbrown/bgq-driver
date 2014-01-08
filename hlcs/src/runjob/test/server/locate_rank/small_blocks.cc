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
#define BOOST_TEST_MODULE small_blocks
#include <boost/test/unit_test.hpp>

#include "server/handlers/locate_rank/HistoryJob.h"
#include "server/handlers/locate_rank/Job.h"

#include "server/CommandConnection.h"

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
        std::cout << "properties: " << properties->getFilename() << std::endl;
        bgq::utility::initializeLogging( *properties);
        BGQDB::init( properties, 1 );
    }
};

BOOST_GLOBAL_FIXTURE( MyFixture );

BOOST_AUTO_TEST_CASE( nodeboard )
{
    // create a Q32 block
    const std::string blockid( "locate_rank_small_block_test" );
    BGQDB::GenBlockParams params;
    params.setBlockId( blockid );
    params.setOwner( "nobody" );
    params.setMidplane( "R00-M0" );
    params.setNodeBoardAndCount( "N04", 1 ); // N04 chosen for no particular reason

    // in case the block was left over from a previous run
    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );

    BGQDB::genBlockEx( params );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::ALLOCATED ) == BGQDB::OK );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::BOOTING ) == BGQDB::OK );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::INITIALIZED ) == BGQDB::OK );

    BGQDB::job::Operations operations;
    const cxxdb::ConnectionPtr db_connection( BGQDB::DBConnectionPool::instance().getConnection() );
    assert( db_connection );

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
    // insert and then immediately remove the job
    operations.insert( job, &id );
    operations.remove( id, BGQDB::job::RemoveInfo() );
   
    const runjob::server::handlers::locate_rank::Job::Ptr locate_job(
            runjob::server::handlers::locate_rank::Job::create(
                db_connection,
                id
                )
            );

    BOOST_CHECK(
            boost::dynamic_pointer_cast<runjob::server::handlers::locate_rank::HistoryJob>(locate_job)
            );

    // spot check some ranks
    BOOST_CHECK_EQUAL( locate_job->find(0),  std::string("R00-M0-N04-J29") );   // 0,0,0,0,0
    BOOST_CHECK_EQUAL( locate_job->find(7),  std::string("R00-M0-N04-J23") );   // 2,0,1,1,1
    BOOST_CHECK_EQUAL( locate_job->find(24), std::string("R00-M0-N04-J03") );   // 1,1,0,0,0

    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );
}

BOOST_AUTO_TEST_CASE( two_nodeboards )
{
    // create a Q32 block
    const std::string blockid( "locate_rank_small_block_test" );
    BGQDB::GenBlockParams params;
    params.setBlockId( blockid );
    params.setOwner( "nobody" );
    params.setMidplane( "R00-M0" );
    params.setNodeBoardAndCount( "N02", 2 ); // N02 chosen for no particular reason

    // in case the block was left over from a previous run
    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );

    BGQDB::genBlockEx( params );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::ALLOCATED ) == BGQDB::OK );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::BOOTING ) == BGQDB::OK );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::INITIALIZED ) == BGQDB::OK );

    BGQDB::job::Operations operations;
    const cxxdb::ConnectionPtr db_connection( BGQDB::DBConnectionPool::instance().getConnection() );
    assert( db_connection );

    BGQDB::job::InsertInfo job;
    job.setBlock( blockid );
    job.setExe( "test" );
    job.setArgs( BGQDB::job::InsertInfo::Args() );
    job.setEnvs( BGQDB::job::InsertInfo::Envs() );
    job.setCwd( "testcwd" );
    job.setUserName( "testuser" );
    job.setProcessesPerNode( 1 );
    job.setNp( 64 );
    job.setShape( BGQDB::job::Shape::Block );
    job.setMapping( "ABCDET" );
    job.setHostname( "testhost" );
    job.setPid( 1 );

    BGQDB::job::Id id;
    // insert and then immediately remove the job
    operations.insert( job, &id );
    operations.remove( id, BGQDB::job::RemoveInfo() );
   
    const runjob::server::handlers::locate_rank::Job::Ptr locate_job(
            runjob::server::handlers::locate_rank::Job::create(
                db_connection,
                id
                )
            );

    BOOST_CHECK(
            boost::dynamic_pointer_cast<runjob::server::handlers::locate_rank::HistoryJob>(locate_job)
            );

    // spot check some ranks
    BOOST_CHECK_EQUAL( locate_job->find(0),  std::string("R00-M0-N02-J12") );   // 0,0,0,0,0
    BOOST_CHECK_EQUAL( locate_job->find(7),  std::string("R00-M0-N02-J06") );   // 0,0,1,1,1
    BOOST_CHECK_EQUAL( locate_job->find(24), std::string("R00-M0-N03-J14") );   // 0,1,2,0,0
    BOOST_CHECK_EQUAL( locate_job->find(36), std::string("R00-M0-N02-J16") );   // 1,0,1,0,0
    BOOST_CHECK_EQUAL( locate_job->find(63), std::string("R00-M0-N03-J25") );   // 1,1,3,1,1

    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );
}

BOOST_AUTO_TEST_CASE( four_nodeboards )
{
    // create a Q32 block
    const std::string blockid( "locate_rank_small_block_test" );
    BGQDB::GenBlockParams params;
    params.setBlockId( blockid );
    params.setOwner( "nobody" );
    params.setMidplane( "R00-M0" );
    params.setNodeBoardAndCount( "N08", 4 ); // N08 chosen for no particular reason

    // in case the block was left over from a previous run
    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );

    BGQDB::genBlockEx( params );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::ALLOCATED ) == BGQDB::OK );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::BOOTING ) == BGQDB::OK );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::INITIALIZED ) == BGQDB::OK );

    BGQDB::job::Operations operations;
    const cxxdb::ConnectionPtr db_connection( BGQDB::DBConnectionPool::instance().getConnection() );
    assert( db_connection );

    BGQDB::job::InsertInfo job;
    job.setBlock( blockid );
    job.setExe( "test" );
    job.setArgs( BGQDB::job::InsertInfo::Args() );
    job.setEnvs( BGQDB::job::InsertInfo::Envs() );
    job.setCwd( "testcwd" );
    job.setUserName( "testuser" );
    job.setProcessesPerNode( 1 );
    job.setNp( 128 );
    job.setShape( BGQDB::job::Shape::Block );
    job.setMapping( "ABCDET" );
    job.setHostname( "testhost" );
    job.setPid( 1 );

    BGQDB::job::Id id;
    // insert and then immediately remove the job
    operations.insert( job, &id );
    operations.remove( id, BGQDB::job::RemoveInfo() );
   
    const runjob::server::handlers::locate_rank::Job::Ptr locate_job(
            runjob::server::handlers::locate_rank::Job::create(
                db_connection,
                id
                )
            );

    BOOST_CHECK(
            boost::dynamic_pointer_cast<runjob::server::handlers::locate_rank::HistoryJob>(locate_job)
            );

    // spot check some ranks
    BOOST_CHECK_EQUAL( locate_job->find(0),   std::string("R00-M0-N08-J04") );   // 0,0,0,0,0
    BOOST_CHECK_EQUAL( locate_job->find(7),   std::string("R00-M0-N10-J03") );   // 0,0,0,3,1
    BOOST_CHECK_EQUAL( locate_job->find(24),  std::string("R00-M0-N09-J04") );   // 0,0,3,0,0
    BOOST_CHECK_EQUAL( locate_job->find(57),  std::string("R00-M0-N09-J00") );   // 0,1,3,0,1
    BOOST_CHECK_EQUAL( locate_job->find(112), std::string("R00-M0-N09-J27") );   // 1,1,2,0,0

    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );
}

BOOST_AUTO_TEST_CASE( eight_nodeboards )
{
    // create a Q32 block
    const std::string blockid( "locate_rank_small_block_test" );
    BGQDB::GenBlockParams params;
    params.setBlockId( blockid );
    params.setOwner( "nobody" );
    params.setMidplane( "R00-M0" );
    params.setNodeBoardAndCount( "N00", 8 ); // N00 chosen for no particular reason

    // in case the block was left over from a previous run
    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );

    BGQDB::genBlockEx( params );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::ALLOCATED ) == BGQDB::OK );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::BOOTING ) == BGQDB::OK );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::INITIALIZED ) == BGQDB::OK );

    BGQDB::job::Operations operations;
    const cxxdb::ConnectionPtr db_connection( BGQDB::DBConnectionPool::instance().getConnection() );
    assert( db_connection );

    BGQDB::job::InsertInfo job;
    job.setBlock( blockid );
    job.setExe( "test" );
    job.setArgs( BGQDB::job::InsertInfo::Args() );
    job.setEnvs( BGQDB::job::InsertInfo::Envs() );
    job.setCwd( "testcwd" );
    job.setUserName( "testuser" );
    job.setProcessesPerNode( 1 );
    job.setNp( 256 );
    job.setShape( BGQDB::job::Shape::Block );
    job.setMapping( "ABCDET" );
    job.setHostname( "testhost" );
    job.setPid( 1 );

    BGQDB::job::Id id;
    // insert and then immediately remove the job
    operations.insert( job, &id );
    operations.remove( id, BGQDB::job::RemoveInfo() );
   
    const runjob::server::handlers::locate_rank::Job::Ptr locate_job(
            runjob::server::handlers::locate_rank::Job::create(
                db_connection,
                id
                )
            );

    BOOST_CHECK(
            boost::dynamic_pointer_cast<runjob::server::handlers::locate_rank::HistoryJob>(locate_job)
            );

    // spot check some ranks
    BOOST_CHECK_EQUAL( locate_job->find(0),   std::string("R00-M0-N00-J00") );   // 0,0,0,0,0
    BOOST_CHECK_EQUAL( locate_job->find(13),  std::string("R00-M0-N02-J10") );   // 0,0,1,2,1
    BOOST_CHECK_EQUAL( locate_job->find(96),  std::string("R00-M0-N00-J30") );   // 1,1,0,0,0
    BOOST_CHECK_EQUAL( locate_job->find(178), std::string("R00-M0-N05-J19") );   // 2,1,2,1,0
    BOOST_CHECK_EQUAL( locate_job->find(212), std::string("R00-M0-N07-J13") );   // 3,0,2,2,0

    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );
}
