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
#define BOOST_TEST_MODULE error_path
#include <boost/test/unit_test.hpp>

#include "server/handlers/locate_rank/Exception.h"
#include "server/handlers/locate_rank/Job.h"

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/job/Operations.h>

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/GenBlockParams.h>
#include <db/include/api/genblock.h>

#include <utility/include/Log.h>

using namespace runjob::server::handlers::locate_rank;

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

bool is_job_not_found( const Exception& e )
{
    return e.error() == runjob::commands::error::job_not_found;
}

BOOST_AUTO_TEST_CASE( job_not_found )
{
    const cxxdb::ConnectionPtr db_connection( BGQDB::DBConnectionPool::instance().getConnection() );
    assert( db_connection );

    // job 0 should never exist
    const BGQDB::job::Id job = 0;

    BOOST_CHECK_EXCEPTION(
                Job::create( db_connection, job ),
                Exception,
                is_job_not_found
            );
}

bool is_invalid_rank( const Exception& e )
{
    return e.error() == runjob::commands::error::rank_invalid;
}

BOOST_AUTO_TEST_CASE( rank_not_found )
{
    // create a Q32 block
    const std::string blockid( "locate_rank_test_rank_not_found" );
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

    const Job::Ptr locate_job(
            Job::create( db_connection, id )
            );

    // any rank 32 or above should fail
    BOOST_CHECK_EXCEPTION(
            locate_job->find( 32 ),
            Exception,
            is_invalid_rank
            );

    (void)BGQDB::setBlockStatus( blockid, BGQDB::FREE );
    (void)BGQDB::deleteBlock( blockid );
}

