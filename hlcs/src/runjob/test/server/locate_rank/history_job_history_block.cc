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
#define BOOST_TEST_MODULE history_job_history_block
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
        bgq::utility::initializeLogging( *properties);
        BGQDB::init( properties, 1 );
    }

    ~MyFixture()
    {
    
    }
};

BOOST_GLOBAL_FIXTURE( MyFixture );

BOOST_AUTO_TEST_CASE( history_midplane_job )
{
    BGQDB::job::Operations operations;
    const cxxdb::ConnectionPtr db_connection( BGQDB::DBConnectionPool::instance().getConnection() );
    assert( db_connection );

    const std::string blockid( "locate_rank_history_block" );
    // create a midplane block
    BGQDB::GenBlockParams params;
    params.setBlockId( blockid );
    params.setOwner( "nobody" );
    params.setMidplane( "R00-M0" );

    // in case the block was left over from a previous run
    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );

    BGQDB::genBlockEx( params );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::ALLOCATED ) == BGQDB::OK );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::BOOTING ) == BGQDB::OK );
    assert( BGQDB::setBlockStatus( blockid, BGQDB::INITIALIZED ) == BGQDB::OK );

    // get creation id from the block so we can verify locate_rank uses the correct
    // block after it has been deleted
    const cxxdb::ResultSetPtr results(
            db_connection->query( "SELECT CREATIONID from BGQBLOCK where blockid='" + blockid + "'" )
            );
    BOOST_CHECK( results->fetch() );
    const int creationId = results->columns()[ "CREATIONID" ].getInt32();

    BGQDB::job::InsertInfo job;
    job.setBlock( blockid );
    job.setExe( "test" );
    job.setArgs( BGQDB::job::InsertInfo::Args() );
    job.setEnvs( BGQDB::job::InsertInfo::Envs() );
    job.setCwd( "testcwd" );
    job.setUserName( "testuser" );
    job.setProcessesPerNode( 1 );
    job.setNp( 512 );
    job.setShape( BGQDB::job::Shape::Midplane );
    job.setMapping( "ABCDET" );
    job.setHostname( "testhost" );
    job.setPid( 1 );

    BGQDB::job::Id id;

    // insert and then immediately remove the job
    operations.insert( job, &id );
    operations.remove( id, BGQDB::job::RemoveInfo() );
   
    // done with the block so remove it
    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );

    const runjob::server::handlers::locate_rank::Job::Ptr locate_job(
            runjob::server::handlers::locate_rank::Job::create(
                db_connection,
                id
                )
            );

    BOOST_CHECK(
            boost::dynamic_pointer_cast<runjob::server::handlers::locate_rank::HistoryJob>(locate_job)
            );

    // ensure we are looking at the correct block
    BOOST_CHECK_EQUAL(
            locate_job->blockCreationId(),
            creationId
            );

    // spot check some ranks
    BOOST_CHECK_EQUAL( locate_job->find(0), std::string("R00-M0-N00-J00") );   // 0,0,0,0,0
    BOOST_CHECK_EQUAL( locate_job->find(211), std::string("R00-M0-N09-J19") ); // 1,2,2,1,1
    BOOST_CHECK_EQUAL( locate_job->find(511), std::string("R00-M0-N15-J00") ); // 3,3,3,3,1
}
