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

// some of these tests require a Sequoia sized configuration of 4x3x4x4

#include "api/job/Operations.h"
#include "api/BGQDBlib.h"
#include "api/Exception.h"
#include "api/genblock.h"
#include "api/GenBlockParams.h"

#include "api/tableapi/gensrc/bgqtableapi.h"

#include <ras/include/RasEventHandlerChain.h>
#include <ras/include/RasEventImpl.h>

#include <utility/include/Log.h>

#include <boost/scoped_ptr.hpp>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE putras
#include <boost/test/unit_test.hpp>

using namespace std;

struct BGQDBInfo
{
    BGQDB::DimensionSizes machine_size;
    bool is_2422;
    bool is_4344;
    boost::scoped_ptr<BGQDB::job::Operations> job_operations;

    BGQDBInfo() {
        bgq::utility::Properties::Ptr properties_ptr(bgq::utility::Properties::create());
        bgq::utility::initializeLogging( *properties_ptr );
        BGQDB::init( properties_ptr, 1 );

        job_operations.reset( new BGQDB::job::Operations );

        BGQDB::STATUS db_status(BGQDB::getMachineBPSize( machine_size ));

        if ( db_status != BGQDB::OK ) {
            throw runtime_error( string("getMachineBPSize failed with ") + boost::lexical_cast<string>(db_status) );
        }

        std::cout << "Machine size is " << machine_size << "\n";

        is_2422 = (machine_size[BGQDB::Dimension::A] == 2) &&
                  (machine_size[BGQDB::Dimension::B] == 4) &&
                  (machine_size[BGQDB::Dimension::C] == 2) &&
                  (machine_size[BGQDB::Dimension::D] == 2);

        is_4344 = (machine_size[BGQDB::Dimension::A] == 4) &&
                  (machine_size[BGQDB::Dimension::B] == 3) &&
                  (machine_size[BGQDB::Dimension::C] == 4) &&
                  (machine_size[BGQDB::Dimension::D] == 4);

    }
};


static BGQDBInfo bgqdb_info;


BOOST_AUTO_TEST_CASE( small_block )
{
    const std::string block( "TESTPRSB" );
    BGQDB::GenBlockParams params;
    BGQDB::GenBlockParams::NodeBoardLocations node_board_locations;
    node_board_locations.push_back( "R00-M0-N00" );

    params.setBlockId( block );
    params.setOwner( "root" );
    params.setNodeBoardLocations( node_board_locations );

    BOOST_CHECK_NO_THROW( BGQDB::genBlockEx( params ) );

    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::BOOTING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::INITIALIZED ), BGQDB::OK );

    RasEventImpl event(0x00061012);
    event.setDetail( RasEvent::LOCATION, "R00-M0-N00" );
    RasEventHandlerChain::handle(event);

    BOOST_CHECK_EQUAL(
            BGQDB::putRAS(event),
            BGQDB::OK
            );

    BGQDB::BLOCK_ACTION action;
    std::string b;
    BOOST_CHECK_EQUAL(
            getBlockAction( b, action),
            BGQDB::OK
            );

    BOOST_CHECK_EQUAL( b, block );
    BOOST_CHECK_EQUAL( action, BGQDB::DEALLOCATE_BLOCK );
            
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::TERMINATING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::FREE ), BGQDB::OK );

    BOOST_CHECK_EQUAL( BGQDB::deleteBlock( block ), BGQDB::OK );
}

BOOST_AUTO_TEST_CASE( large_block )
{
    const std::string block( "TESTPRLB" );
    BGQDB::GenBlockParams params;
    params.setMidplane( "R00-M0" );
    params.setBlockId( block );
    params.setOwner( "root" );
    
    BGQDB::DimensionSpecs dim_specs = { {
        BGQDB::DimensionSpec( 1 ),
        BGQDB::DimensionSpec( 1 ),
        BGQDB::DimensionSpec( 1 ),
        BGQDB::DimensionSpec( "M:11")
        } };
    params.setDimensionSpecs( dim_specs );

    try {
        BGQDB::genBlockEx( params );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        return;
    }

    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::BOOTING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::INITIALIZED ), BGQDB::OK );

    RasEventImpl event(0x00061012);
    event.setDetail( RasEvent::LOCATION, "R00-M0-N00" );
    RasEventHandlerChain::handle(event);

    BOOST_CHECK_EQUAL(
            BGQDB::putRAS(event),
            BGQDB::OK
            );

    BGQDB::BLOCK_ACTION action;
    std::string b;
    BOOST_CHECK_EQUAL(
            getBlockAction( b, action ),
            BGQDB::OK
            );

    BOOST_CHECK_EQUAL( b, block );
    BOOST_CHECK_EQUAL( action, BGQDB::DEALLOCATE_BLOCK );
            
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::TERMINATING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::FREE ), BGQDB::OK );

    BOOST_CHECK_EQUAL( BGQDB::deleteBlock( block ), BGQDB::OK );
}

BOOST_AUTO_TEST_CASE( large_block_passthrough )
{
    if ( ! bgqdb_info.is_4344 )  { cout << "skipping test because db not populate with 4344.\n"; return; }

    const std::string block( "TESTPRPT" );
    BGQDB::GenBlockParams params;
    params.setMidplane( "R00-M0" );
    params.setBlockId( block );
    params.setOwner( "root" );
    
    BGQDB::DimensionSpecs dim_specs = { {
            BGQDB::DimensionSpec( 1, "1" ),
            BGQDB::DimensionSpec( 1, "1" ),
            BGQDB::DimensionSpec( 1, "1" ),
            BGQDB::DimensionSpec( 2, "1100") // pass through R01-M0 and R01-M1
        } };
    params.setDimensionSpecs( dim_specs );

    try {
        BGQDB::genBlockEx( params );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        return;
    }

    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::BOOTING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::INITIALIZED ), BGQDB::OK );

    RasEventImpl event(0x00061012);
    event.setDetail( RasEvent::LOCATION, "R01-M0-N00" ); // we pass through this midplane
    RasEventHandlerChain::handle(event);

    BOOST_CHECK_EQUAL(
            BGQDB::putRAS(event),
            BGQDB::OK
            );

    BGQDB::BLOCK_ACTION action;
    std::string b;
    BOOST_CHECK_EQUAL(
            getBlockAction( b, action ),
            BGQDB::OK
            );

    BOOST_CHECK_EQUAL( b, block );
    BOOST_CHECK_EQUAL( action, BGQDB::DEALLOCATE_BLOCK );
            
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::TERMINATING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::FREE ), BGQDB::OK );

    BOOST_CHECK_EQUAL( BGQDB::deleteBlock( block ), BGQDB::OK );
}

BOOST_AUTO_TEST_CASE( large_block_should_not_free )
{
    const std::string block( "TESTPRNF" );
    BGQDB::GenBlockParams params;
    params.setMidplane( "R00-M0" );
    params.setBlockId( block );
    params.setOwner( "root" );
    
    BGQDB::DimensionSpecs dim_specs = { {
        BGQDB::DimensionSpec( 1 ),
        BGQDB::DimensionSpec( 1 ),
        BGQDB::DimensionSpec( 1 ),
        BGQDB::DimensionSpec( "M:11")
        } };
    params.setDimensionSpecs( dim_specs );

    try {
        BGQDB::genBlockEx( params );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        return;
    }

    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::BOOTING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::INITIALIZED ), BGQDB::OK );

    RasEventImpl event(0x00061012);
    event.setDetail( RasEvent::LOCATION, "R01-M0-N00" ); // we aren't using this midplane
    RasEventHandlerChain::handle(event);

    BOOST_CHECK_EQUAL(
            BGQDB::putRAS(event),
            BGQDB::OK
            );

    BGQDB::BLOCK_ACTION action;
    std::string b;
    BOOST_CHECK_EQUAL(
            getBlockAction( b, action ),
            BGQDB::OK
            );

    BOOST_CHECK( b.empty() );
    BOOST_CHECK_EQUAL( action, BGQDB::NO_BLOCK_ACTION );
           
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::TERMINATING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::FREE ), BGQDB::OK );

    BOOST_CHECK_EQUAL( BGQDB::deleteBlock( block ), BGQDB::OK );
}

BOOST_AUTO_TEST_CASE( large_block_passthrough_and_smallblock )
{
    if ( ! bgqdb_info.is_4344 )  { cout << "skipping test because db not populate with 4344.\n"; return; }

    const std::string large_block( "TESTPRBOTHLARGE" );
    const std::string small_block( "TESTPRBOTHSMALL" );
    {
        BGQDB::GenBlockParams params;
        BGQDB::GenBlockParams::NodeBoardLocations node_board_locations;
        node_board_locations.push_back( "R01-M0-N00" );

        params.setBlockId( small_block );
        params.setOwner( "root" );
        params.setNodeBoardLocations( node_board_locations );

        BOOST_CHECK_NO_THROW( BGQDB::genBlockEx( params ) );

        BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( small_block, BGQDB::ALLOCATED ), BGQDB::OK );
        BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( small_block, BGQDB::BOOTING ), BGQDB::OK );
        BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( small_block, BGQDB::INITIALIZED ), BGQDB::OK );
    }

    { 
        BGQDB::GenBlockParams params;
        params.setMidplane( "R00-M0" );
        params.setBlockId( large_block );
        params.setOwner( "root" );

        BGQDB::DimensionSpecs dim_specs = { {
            BGQDB::DimensionSpec( 1, "1" ),
                BGQDB::DimensionSpec( 1, "1" ),
                BGQDB::DimensionSpec( 1, "1" ),
                BGQDB::DimensionSpec( 2, "1100") // pass through R01-M0 and R01-M1
        } };
        params.setDimensionSpecs( dim_specs );

        try {
            BGQDB::genBlockEx( params );
        } catch ( const std::exception& e ) {
            std::cerr << e.what() << std::endl;
            return;
        }

        BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( large_block, BGQDB::ALLOCATED ), BGQDB::OK );
        BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( large_block, BGQDB::BOOTING ), BGQDB::OK );
        BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( large_block, BGQDB::INITIALIZED ), BGQDB::OK );
    }

    RasEventImpl event(0x00061012);
    event.setDetail( RasEvent::LOCATION, "R01-M0-N00" ); // we pass through this midplane
    RasEventHandlerChain::handle(event);

    BOOST_CHECK_EQUAL(
            BGQDB::putRAS(event),
            BGQDB::OK
            );

    BGQDB::BLOCK_ACTION action;
    std::string b;
    BOOST_CHECK_EQUAL(
            getBlockAction( b, action, "('" + small_block + "')" ),
            BGQDB::OK
            );

    BOOST_CHECK_EQUAL( b, large_block );
    BOOST_CHECK_EQUAL( action, BGQDB::DEALLOCATE_BLOCK );
            
    BOOST_CHECK_EQUAL(
            getBlockAction( b, action, "('" + large_block + "')" ),
            BGQDB::OK
            );

    BOOST_CHECK_EQUAL( b, small_block );
    BOOST_CHECK_EQUAL( action, BGQDB::DEALLOCATE_BLOCK );
            
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( large_block, BGQDB::TERMINATING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( large_block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( large_block, BGQDB::FREE ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::deleteBlock( large_block ), BGQDB::OK );
    
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( small_block, BGQDB::TERMINATING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( small_block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( small_block, BGQDB::FREE ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::deleteBlock( small_block ), BGQDB::OK );
}

BOOST_AUTO_TEST_CASE( job )
{
    const std::string block( "TESTPRJOB" );
    BGQDB::GenBlockParams params;
    params.setMidplane( "R00-M0" );
    params.setBlockId( block );
    params.setOwner( "root" );
    
    BGQDB::DimensionSpecs dim_specs = { {
        BGQDB::DimensionSpec( 1 ),
        BGQDB::DimensionSpec( 1 ),
        BGQDB::DimensionSpec( 1 ),
        BGQDB::DimensionSpec( "M:11")
        } };
    params.setDimensionSpecs( dim_specs );

    try {
        BGQDB::genBlockEx( params );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        return;
    }

    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::BOOTING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::INITIALIZED ), BGQDB::OK );

    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( block );
    BGQDB::job::Id job_id;
    bgqdb_info.job_operations->insert(
            insert_info,
            &job_id
        );

    BOOST_CHECK( job_id != 0 );

    RasEventImpl event(0x00061012);
    event.setDetail( RasEvent::LOCATION, "R00-M0-N00" );
    RasEventHandlerChain::handle(event);

    const BGQDB::job::Id j( 0 );
    const uint32_t qualifier( 0 );
    std::vector<BGQDB::job::Id> jobs_to_kill;
    BOOST_CHECK_EQUAL(
            BGQDB::putRAS(event, block, j, qualifier, &jobs_to_kill ),
            BGQDB::OK
            );

    BOOST_CHECK_EQUAL( jobs_to_kill.size(), 1u );
    BOOST_CHECK_EQUAL( jobs_to_kill.at(0), job_id );

    BGQDB::BLOCK_ACTION action;
    std::string b;
    BOOST_CHECK_EQUAL(
            getBlockAction( b, action ),
            BGQDB::OK
            );

    BOOST_CHECK_EQUAL( b, block );
    BOOST_CHECK_EQUAL( action, BGQDB::DEALLOCATE_BLOCK );
            
    bgqdb_info.job_operations->remove( job_id, BGQDB::job::RemoveInfo() );

    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::TERMINATING ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::ALLOCATED ), BGQDB::OK );
    BOOST_CHECK_EQUAL( BGQDB::setBlockStatus( block, BGQDB::FREE ), BGQDB::OK );

    BOOST_CHECK_EQUAL( BGQDB::deleteBlock( block ), BGQDB::OK );
}
