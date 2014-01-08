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

#include "api/GenBlockParams.h"

#include "common.h"

#include <control/include/mcServer/defaults.h>
#include <boost/assign.hpp>

#include <stdexcept>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE GenBlockParams
#include <boost/test/unit_test.hpp>

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

using namespace boost::assign; // bring 'operator+=()' into scope

using BGQDB::Connectivity;
using BGQDB::DimensionSpec;
using BGQDB::GenBlockParams;

using std::logic_error;
using std::invalid_argument;
using std::string;


BOOST_AUTO_TEST_CASE( test_setBlockId_lengths )
{
    GenBlockParams params;

    params.setBlockId( "a" ); // min length
    BOOST_CHECK_EQUAL( params.getBlockId(), "a" );

    params.setBlockId( "12345678910234567892023456789302" ); // max length
    BOOST_CHECK_EQUAL( params.getBlockId(), "12345678910234567892023456789302" );
}


BOOST_AUTO_TEST_CASE( test_setBlockId_invalid_id )
{
    GenBlockParams params;
    BOOST_CHECK_THROW( params.setBlockId( "" ), invalid_argument ); // Cannot be empty
    BOOST_CHECK_THROW( params.setBlockId( " " ), invalid_argument ); // Cannot contain space.
    BOOST_CHECK_THROW( params.setBlockId( "BLKTEST`~!@#$%^&*()=+" ), invalid_argument ); // Cannot have these characters.
    BOOST_CHECK_THROW( params.setBlockId( "BLKTEST[{]}\\|" ), invalid_argument ); // Cannot have these characters.
    BOOST_CHECK_THROW( params.setBlockId( "BLKTEST'" ), invalid_argument ); // Cannot have '
    BOOST_CHECK_THROW( params.setBlockId( "BLKTEST;:'\",<.>/?" ), invalid_argument ); // Cannot have these characters.
    BOOST_CHECK_THROW( params.setBlockId( mc_server::DefaultListener ), invalid_argument ); // Cannot be default event listener
    // BOOST_CHECK_THROW( params.setBlockId( "ALL" ), invalid_argument ); // Cannot be all?

    BOOST_CHECK_THROW( params.setBlockId( "123456789102345678920234567893023"), invalid_argument ); // Too long!
}


BOOST_AUTO_TEST_CASE( test_setMidplane_max_length )
{
    GenBlockParams params;

    params.setMidplane( "R00-M07891023456" );
    BOOST_CHECK_EQUAL( params.getMidplane(), "R00-M07891023456" );
}


BOOST_AUTO_TEST_CASE( test_setMidplane_invalid )
{
    BOOST_CHECK_THROW( GenBlockParams().setMidplane( "R00-M078910234567" ), invalid_argument );
}


BOOST_AUTO_TEST_CASE( test_setMidplane_empty_invalid )
{
    BOOST_CHECK_THROW( GenBlockParams().setMidplane( "" ), invalid_argument );
}


BOOST_AUTO_TEST_CASE( test_DS_default )
{
    DimensionSpec ds;

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( ds.getIncludedMidplanes().size(), unsigned(1) );
    BOOST_CHECK( ds.getIncludedMidplanes()[0] );
    BOOST_CHECK_EQUAL( ds.size(), 1 );
    BOOST_CHECK_EQUAL( ds.toString(), "T:1" );
}


BOOST_AUTO_TEST_CASE( test_DS_size_1 )
{
    DimensionSpec ds( 1 );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( ds.getIncludedMidplanes().size(), unsigned(1) );
    BOOST_CHECK( ds.getIncludedMidplanes()[0] );
    BOOST_CHECK_EQUAL( ds.size(), 1 );
    BOOST_CHECK_EQUAL( ds.toString(), "T:1" );
}


BOOST_AUTO_TEST_CASE( test_DS_size_2 )
{
    DimensionSpec ds( 2 );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( ds.getIncludedMidplanes().size(), unsigned(2) );
    BOOST_CHECK( ds.getIncludedMidplanes()[0] );
    BOOST_CHECK( ds.getIncludedMidplanes()[1] );
    BOOST_CHECK_EQUAL( ds.size(), 2 );
    BOOST_CHECK_EQUAL( ds.toString(), "T:11" );
}


BOOST_AUTO_TEST_CASE( test_DS_im_empty )
{
    DimensionSpec::IncludedMidplanes included_mps;
    DimensionSpec ds( included_mps );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Torus );
    BOOST_CHECK( ds.getIncludedMidplanes().empty() );
}


BOOST_AUTO_TEST_CASE( test_DS_im_one )
{
    DimensionSpec::IncludedMidplanes included_mps;
    included_mps.push_back( true );

    DimensionSpec ds( included_mps );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( ds.getIncludedMidplanes().size(), unsigned(1) );
    BOOST_CHECK( ds.getIncludedMidplanes()[0] );
}


BOOST_AUTO_TEST_CASE( test_DS_im_pt )
{
    DimensionSpec::IncludedMidplanes included_mps;
    included_mps.push_back( true );
    included_mps.push_back( false );
    included_mps.push_back( true );

    DimensionSpec ds( included_mps );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( ds.getIncludedMidplanes().size(), unsigned(3) );
    BOOST_CHECK( ds.getIncludedMidplanes()[0] );
    BOOST_CHECK( ! ds.getIncludedMidplanes()[1] );
    BOOST_CHECK( ds.getIncludedMidplanes()[2] );
    BOOST_CHECK_EQUAL( ds.size(), 2 );
    BOOST_CHECK_EQUAL( ds.toString(), "T:101" );
}


BOOST_AUTO_TEST_CASE( test_DS_im_pt_first )
{
    DimensionSpec::IncludedMidplanes included_mps;
    included_mps.push_back( false );
    included_mps.push_back( true );
    included_mps.push_back( true );

    DimensionSpec ds( included_mps );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( ds.getIncludedMidplanes().size(), unsigned(3) );
    BOOST_CHECK( ! ds.getIncludedMidplanes()[0] );
    BOOST_CHECK( ds.getIncludedMidplanes()[1] );
    BOOST_CHECK( ds.getIncludedMidplanes()[2] );
    BOOST_CHECK_EQUAL( ds.size(), 2 );
    BOOST_CHECK_EQUAL( ds.toString(), "T:011" );
}


BOOST_AUTO_TEST_CASE( test_DS_im_pt_last )
{
    DimensionSpec::IncludedMidplanes included_mps;
    included_mps.push_back( true );
    included_mps.push_back( true );
    included_mps.push_back( false );

    DimensionSpec ds( included_mps );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( ds.getIncludedMidplanes().size(), unsigned(3) );
    BOOST_CHECK( ds.getIncludedMidplanes()[0] );
    BOOST_CHECK( ds.getIncludedMidplanes()[1] );
    BOOST_CHECK( ! ds.getIncludedMidplanes()[2] );
    BOOST_CHECK_EQUAL( ds.size(), 2 );
    BOOST_CHECK_EQUAL( ds.toString(), "T:110" );
}


BOOST_AUTO_TEST_CASE( test_DS_im_pt_two )
{
    DimensionSpec::IncludedMidplanes included_mps;
    included_mps.push_back( true );
    included_mps.push_back( false );
    included_mps.push_back( true );
    included_mps.push_back( false );

    DimensionSpec ds( included_mps );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( ds.getIncludedMidplanes().size(), unsigned(4) );
    BOOST_CHECK( ds.getIncludedMidplanes()[0] );
    BOOST_CHECK( ! ds.getIncludedMidplanes()[1] );
    BOOST_CHECK( ds.getIncludedMidplanes()[2] );
    BOOST_CHECK( ! ds.getIncludedMidplanes()[3] );
    BOOST_CHECK_EQUAL( ds.size(), 2 );
    BOOST_CHECK_EQUAL( ds.toString(), "T:1010" );
}


BOOST_AUTO_TEST_CASE( test_DS_connectivity_torus )
{
    DimensionSpec ds( Connectivity::Torus );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Torus );
    BOOST_CHECK( ds.getIncludedMidplanes().empty() );
    BOOST_CHECK_EQUAL( ds.size(), 0 );
    BOOST_CHECK_EQUAL( ds.toString(), "T" );
}


BOOST_AUTO_TEST_CASE( test_DS_connectivity_mesh )
{
    DimensionSpec ds( Connectivity::Mesh );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Mesh );
    BOOST_CHECK( ds.getIncludedMidplanes().empty() );
    BOOST_CHECK_EQUAL( ds.size(), 0 );
    BOOST_CHECK_EQUAL( ds.toString(), "M" );
}


BOOST_AUTO_TEST_CASE( test_DS_cx_inc_mesh )
{
    DimensionSpec::IncludedMidplanes included_mps;
    included_mps.push_back( true );
    included_mps.push_back( true );

    DimensionSpec ds( included_mps, Connectivity::Mesh );

    BOOST_CHECK_EQUAL( ds.getConnectivity(), Connectivity::Mesh );
    BOOST_CHECK_EQUAL( ds.getIncludedMidplanes().size(), unsigned(2) );
    BOOST_CHECK( ds.getIncludedMidplanes()[0] );
    BOOST_CHECK( ds.getIncludedMidplanes()[1] );
    BOOST_CHECK_EQUAL( ds.size(), 2 );
    BOOST_CHECK_EQUAL( ds.toString(), "M:11" );
}


BOOST_AUTO_TEST_CASE( test_DS_im_ex )
{
    // If no midplanes then should fail.
    {
        DimensionSpec::IncludedMidplanes included_mps;
        included_mps.push_back( false );

        BOOST_CHECK_THROW( DimensionSpec ds( included_mps ), invalid_argument );
    }

    // If size 1 then no passthrough allowed.
    {
        DimensionSpec::IncludedMidplanes included_mps;
        included_mps.push_back( true );
        included_mps.push_back( false );

        BOOST_CHECK_THROW( DimensionSpec ds( included_mps, Connectivity::Torus ), invalid_argument );
    }

    // If size 1 then must be a torus.
    {
        DimensionSpec::IncludedMidplanes included_mps;
        included_mps.push_back( true );

        BOOST_CHECK_THROW( DimensionSpec ds( included_mps, Connectivity::Mesh ), invalid_argument );
    }

    // If mesh then must start and end with a midplane -- end
    {
        DimensionSpec::IncludedMidplanes included_mps;
        included_mps.push_back( true );
        included_mps.push_back( false );
        included_mps.push_back( true );
        included_mps.push_back( false );

        BOOST_CHECK_THROW( DimensionSpec ds( included_mps, Connectivity::Mesh ), invalid_argument );
    }

    // If mesh then must start and end with a midplane -- start
    {
        DimensionSpec::IncludedMidplanes included_mps;
        included_mps.push_back( false );
        included_mps.push_back( true );
        included_mps.push_back( false );
        included_mps.push_back( true );

        BOOST_CHECK_THROW( DimensionSpec ds( included_mps, Connectivity::Mesh ), invalid_argument );
    }
}


BOOST_AUTO_TEST_CASE( test_DS_str )
{
    BOOST_CHECK_EQUAL( DimensionSpec( "" ).toString(), "T" );
    BOOST_CHECK_EQUAL( DimensionSpec( "1" ).toString(), "T:1" );
    BOOST_CHECK_EQUAL( DimensionSpec( "2" ).toString(), "T:11" );
    BOOST_CHECK_EQUAL( DimensionSpec( "2T" ).toString(), "T:11" );
    BOOST_CHECK_EQUAL( DimensionSpec( "2M" ).toString(), "M:11" );
    BOOST_CHECK_EQUAL( DimensionSpec( ":1" ).toString(), "T:1" );
    BOOST_CHECK_EQUAL( DimensionSpec( ":11" ).toString(), "T:11" );
    BOOST_CHECK_EQUAL( DimensionSpec( "M:11" ).toString(), "M:11" );
    BOOST_CHECK_EQUAL( DimensionSpec( ":1100" ).toString(), "T:1100" );
    BOOST_CHECK_EQUAL( DimensionSpec( "M:1001" ).toString(), "M:1001" );
    BOOST_CHECK_EQUAL( DimensionSpec( "1:1" ).toString(), "T:1" );
    BOOST_CHECK_EQUAL( DimensionSpec( "2M:1001" ).toString(), "M:1001" );
}


BOOST_AUTO_TEST_CASE( test_DS_str_ex )
{
    BOOST_CHECK_THROW( DimensionSpec ds( "2S:1" ), invalid_argument );
    BOOST_CHECK_THROW( DimensionSpec ds( "2:1" ), invalid_argument );
}


BOOST_AUTO_TEST_CASE( test_getblockId_not_set )
{
    BOOST_CHECK_THROW( GenBlockParams().getBlockId(), logic_error );
}


BOOST_AUTO_TEST_CASE( test_getMidplane_not_set )
{
    BOOST_CHECK_THROW( GenBlockParams().getMidplane(), logic_error );
}


BOOST_AUTO_TEST_CASE( test_NodeBoardAndCount_min )
{
    GenBlockParams params;

    params.setNodeBoardAndCount( "N00", 1 );

    BOOST_CHECK_EQUAL( params.getNodeBoard(), "N00" );
    BOOST_CHECK_EQUAL( params.getNodeBoardCount(), 1 );

    BOOST_REQUIRE_EQUAL( params.getNodeBoardPositions().size(), 1U );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[0], "N00" );

    // These are all valid and should not throw.
    params.setNodeBoardAndCount( "N01", 1 );
    params.setNodeBoardAndCount( "N02", 1 );
    params.setNodeBoardAndCount( "N03", 1 );
    params.setNodeBoardAndCount( "N04", 1 );
    params.setNodeBoardAndCount( "N05", 1 );
    params.setNodeBoardAndCount( "N06", 1 );
    params.setNodeBoardAndCount( "N07", 1 );
    params.setNodeBoardAndCount( "N08", 1 );
    params.setNodeBoardAndCount( "N09", 1 );
    params.setNodeBoardAndCount( "N10", 1 );
    params.setNodeBoardAndCount( "N11", 1 );
    params.setNodeBoardAndCount( "N12", 1 );
    params.setNodeBoardAndCount( "N13", 1 );
    params.setNodeBoardAndCount( "N14", 1 );
    params.setNodeBoardAndCount( "N15", 1 );

    params.setNodeBoardAndCount( "N00", 2 );
    params.setNodeBoardAndCount( "N02", 2 );
    params.setNodeBoardAndCount( "N04", 2 );
    params.setNodeBoardAndCount( "N06", 2 );
    params.setNodeBoardAndCount( "N08", 2 );

    BOOST_REQUIRE_EQUAL( params.getNodeBoardPositions().size(), 2U );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[0], "N08" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[1], "N09" );

    params.setNodeBoardAndCount( "N10", 2 );
    params.setNodeBoardAndCount( "N12", 2 );
    params.setNodeBoardAndCount( "N14", 2 );

    params.setNodeBoardAndCount( "N00", 4 );
    params.setNodeBoardAndCount( "N04", 4 );

    BOOST_REQUIRE_EQUAL( params.getNodeBoardPositions().size(), 4U );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[0], "N04" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[1], "N05" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[2], "N06" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[3], "N07" );

    params.setNodeBoardAndCount( "N08", 4 );
    params.setNodeBoardAndCount( "N12", 4 );

    params.setNodeBoardAndCount( "N00", 8 );

    BOOST_REQUIRE_EQUAL( params.getNodeBoardPositions().size(), 8U );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[0], "N00" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[1], "N01" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[2], "N02" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[3], "N03" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[4], "N04" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[5], "N05" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[6], "N06" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[7], "N07" );

    params.setNodeBoardAndCount( "N08", 8 );

}


BOOST_AUTO_TEST_CASE( test_NodeBoardAndCount_invalid_ex )
{
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardAndCount( "N00", 3 ), invalid_argument );
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardAndCount( "N01", 2 ), invalid_argument );
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardAndCount( "", 1 ), invalid_argument );
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardAndCount( "N16", 1 ), invalid_argument ); // Node Board position too big.
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardAndCount( "N00", 0 ), invalid_argument );
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardAndCount( "N00", 16 ), invalid_argument );
}


BOOST_AUTO_TEST_CASE( test_getNodeBoard_not_set_ex )
{
    BOOST_CHECK_THROW( GenBlockParams().getNodeBoard(), logic_error );
}


BOOST_AUTO_TEST_CASE( test_getNodeBoardCount_not_set_ex )
{
    BOOST_CHECK_THROW( GenBlockParams().getNodeBoardCount(), logic_error );
}


BOOST_AUTO_TEST_CASE( test_setNodeBoards )
{
    GenBlockParams params;

    params.setNodeBoardLocations( list_of( "R00-M0-N00" ) );

    BOOST_CHECK_EQUAL( params.getMidplane(), "R00-M0" );
    BOOST_REQUIRE_EQUAL( params.getNodeBoardPositions().size(), 1U );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[0], "N00" );

    BOOST_CHECK_EQUAL( params.getNodeBoard(), "N00" );
    BOOST_CHECK_EQUAL( params.getNodeBoardCount(), 1U );

    params.setNodeBoardLocations( list_of( "R00-M0-N01" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N02" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N03" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N04" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N05" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N06" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N07" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N08" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N09" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N10" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N11" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N12" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N13" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N14" ) );
    params.setNodeBoardLocations( list_of( "R00-M0-N15" ) );

    params.setNodeBoardLocations( list_of( "R00-M1-N00" )( "R00-M1-N01" ) );
    params.setNodeBoardLocations( list_of( "R00-M1-N03" )( "R00-M1-N02" ) );

    BOOST_CHECK_EQUAL( params.getMidplane(), "R00-M1" );
    BOOST_REQUIRE_EQUAL( params.getNodeBoardPositions().size(), 2U );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[0], "N02" );
    BOOST_CHECK_EQUAL( params.getNodeBoardPositions()[1], "N03" );

    BOOST_CHECK_EQUAL( params.getNodeBoard(), "N02" );
    BOOST_CHECK_EQUAL( params.getNodeBoardCount(), 2U );

    params.setNodeBoardLocations( list_of( "R00-M1-N04" )( "R00-M1-N05" ) );
    params.setNodeBoardLocations( list_of( "R00-M1-N06" )( "R00-M1-N07" ) );
    params.setNodeBoardLocations( list_of( "R00-M1-N08" )( "R00-M1-N09" ) );
    params.setNodeBoardLocations( list_of( "R00-M1-N10" )( "R00-M1-N11" ) );
    params.setNodeBoardLocations( list_of( "R00-M1-N12" )( "R00-M1-N13" ) );
    params.setNodeBoardLocations( list_of( "R00-M1-N14" )( "R00-M1-N15" ) );


    params.setNodeBoardLocations( list_of( "R00-M1-N00" )( "R00-M1-N01" )( "R00-M1-N02" )( "R00-M1-N03" ) );
    params.setNodeBoardLocations( list_of( "R00-M1-N04" )( "R00-M1-N05" )( "R00-M1-N06" )( "R00-M1-N07" ) );
    params.setNodeBoardLocations( list_of( "R00-M1-N08" )( "R00-M1-N09" )( "R00-M1-N10" )( "R00-M1-N11" ) );
    params.setNodeBoardLocations( list_of( "R00-M1-N12" )( "R00-M1-N13" )( "R00-M1-N14" )( "R00-M1-N15" ) );

    params.setNodeBoardLocations( list_of( "R00-M1-N00" )( "R00-M1-N01" )( "R00-M1-N02" )( "R00-M1-N03" )
                                         ( "R00-M1-N04" )( "R00-M1-N05" )( "R00-M1-N06" )( "R00-M1-N07" ) );
    params.setNodeBoardLocations( list_of( "R00-M1-N08" )( "R00-M1-N09" )( "R00-M1-N10" )( "R00-M1-N11" )
                                         ( "R00-M1-N12" )( "R00-M1-N13" )( "R00-M1-N14" )( "R00-M1-N15" ) );
}


BOOST_AUTO_TEST_CASE( test_setNodeBoards_ex )
{
    // If use empty node board locations then fails.
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardLocations( GenBlockParams::NodeBoardLocations() ), invalid_argument );

    // If try to setNodeBoardLocations and use different midplanes, then fails.
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardLocations( list_of( "R00-M0-N00" )( "R00-M1-N01" ) ), invalid_argument );

    // not a node board location, too short.
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardLocations( list_of( "R00-M0-N00" )( "R00-M0" ) ), invalid_argument );

    // not a node board location, too long.
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardLocations( list_of( "R00-M0-N00" )( "R00-M0-N00-J01" ) ), invalid_argument );

    // invalid number of node cards, 3.
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardLocations( list_of( "R00-M0-N00" )( "R00-M0-N01" )( "R00-M0-N02" ) ), invalid_argument );

    // correct size but invalid start location.
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardLocations( list_of( "R00-M0-N01" )( "R00-M0-N02" ) ), invalid_argument );

    // correct size but skip a location.
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardLocations( list_of( "R00-M0-N00" )( "R00-M0-N03" ) ), invalid_argument );

    // not a node board location, out of range.
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardLocations( list_of( "R00-M0-N17" ) ), invalid_argument );

    // Fails if supply the same node card twice.
    BOOST_CHECK_THROW( GenBlockParams().setNodeBoardLocations( list_of( "R00-M0-N00" )( "R00-M0-N00" ) ), invalid_argument );
}


BOOST_AUTO_TEST_CASE( test_getNodeBoardPositions_not_set_ex )
{
    BOOST_CHECK_THROW( GenBlockParams().getNodeBoardPositions(), logic_error );
}


BOOST_AUTO_TEST_CASE( test_isLarge_large )
{
    GenBlockParams params;
    BOOST_CHECK( params.isLarge() );
    BOOST_CHECK( ! params.isLargeWithMidplanes() );
}


BOOST_AUTO_TEST_CASE( test_isLarge_small )
{
    GenBlockParams params;

    params.setNodeBoardAndCount( "N08", 8 );

    BOOST_CHECK( ! params.isLarge() );
    BOOST_CHECK( ! params.isLargeWithMidplanes() );
}


BOOST_AUTO_TEST_CASE( test_getConnectivity_small_1 )
{
    GenBlockParams params;

    params.setNodeBoardAndCount( "N00", 1 );

    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::A].getConnectivity(), Connectivity::Mesh );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::B].getConnectivity(), Connectivity::Mesh );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::C].getConnectivity(), Connectivity::Mesh );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::D].getConnectivity(), Connectivity::Mesh );
}

BOOST_AUTO_TEST_CASE( test_getConnectivity_small_2 )
{
    GenBlockParams params;

    params.setNodeBoardLocations( list_of( "R00-M0-N00" )( "R00-M0-N01" ) );

    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::A].getConnectivity(), Connectivity::Mesh );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::B].getConnectivity(), Connectivity::Mesh );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::C].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::D].getConnectivity(), Connectivity::Mesh );
}

BOOST_AUTO_TEST_CASE( test_getConnectivity_small_4 )
{
    GenBlockParams params;

    params.setNodeBoardAndCount( "N00", 4 );

    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::A].getConnectivity(), Connectivity::Mesh );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::B].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::C].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::D].getConnectivity(), Connectivity::Mesh );
}

BOOST_AUTO_TEST_CASE( test_getConnectivity_small_8 )
{
    GenBlockParams params;

    params.setNodeBoardAndCount( "N00", 8 );

    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::A].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::B].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::C].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::D].getConnectivity(), Connectivity::Mesh );
}

BOOST_AUTO_TEST_CASE( string_conversion )
{
    using namespace BGQDB;
    Dimension::Value i = Dimension::A;
    BOOST_CHECK_EQUAL( Dimension::toString(i), "A" );
    i = Dimension::B;
    BOOST_CHECK_EQUAL( Dimension::toString(i), "B" );
    i = Dimension::C;
    BOOST_CHECK_EQUAL( Dimension::toString(i), "C" );
    i = Dimension::D;
    BOOST_CHECK_EQUAL( Dimension::toString(i), "D" );
}


BOOST_AUTO_TEST_CASE( test_setMidplanes )
{
    // Can set midplanes. If set midplanes, then isLarge && isLargeWithMidplanes returns true. Can get the midplanes back.
    // Connectivity is torus in all dimensions.

    GenBlockParams params;
    params.setMidplanes(
            list_of( "R00-M0" )
        );

    BOOST_CHECK( params.isLarge() );
    BOOST_CHECK( params.isLargeWithMidplanes() );
    BOOST_REQUIRE_EQUAL( params.getMidplanes().size(), 1U );
    BOOST_CHECK_EQUAL( params.getMidplanes()[0], "R00-M0" );
    BOOST_CHECK( params.getPassthroughLocations().empty() );

    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::A].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::B].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::C].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::D].getConnectivity(), Connectivity::Torus );
}


BOOST_AUTO_TEST_CASE( test_setMidplanes_ex )
{
    // If call setMidplanes with empty list then fails.

    BOOST_CHECK_THROW( GenBlockParams().setMidplanes( GenBlockParams::MidplaneLocations() ), invalid_argument );
}


BOOST_AUTO_TEST_CASE( test_getMidplanes_ex )
{
    // If getMidplanes but haven't setMidplanes then fails.
    BOOST_CHECK_THROW( GenBlockParams().getMidplanes(), logic_error );

    // If getPassthroughSwitchLocations but haven't setMidplanes then fails.
    BOOST_CHECK_THROW( GenBlockParams().getPassthroughLocations(), logic_error );
}


BOOST_AUTO_TEST_CASE( test_setMidplanesWithPassthrough )
{
    // Can set midplanes. If set midplanes, then isLarge && isLargeWithMidplanes returns true. Can get the midplanes back.


    GenBlockParams::MidplaneLocations midplanes = list_of( "R00-M0" )( "R02-M0" );
    GenBlockParams::MidplaneLocations passthroughs = list_of( "R03-M0" )( "R01-M0" );

    GenBlockParams params;
    params.setMidplanes(
            midplanes,
            passthroughs
        );

    BOOST_REQUIRE_EQUAL( params.getMidplanes().size(), midplanes.size() );
    BOOST_CHECK( std::equal( midplanes.begin(), midplanes.end(), params.getMidplanes().begin() ) );
    BOOST_REQUIRE_EQUAL( params.getPassthroughLocations().size(), passthroughs.size() );
    BOOST_CHECK( std::equal( passthroughs.begin(), passthroughs.end(), params.getPassthroughLocations().begin() ) );
}


BOOST_AUTO_TEST_CASE( test_setMidplanesMesh )
{
    // Can set the connectivity to mesh in a dimension.

    BGQDB::DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    GenBlockParams params;
    params.setMidplanes( list_of( "R00-M0" )( "R02-M0" ) );
    params.setDimensionSpecs( dim_specs );

    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::A].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::B].getConnectivity(), Connectivity::Mesh );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::C].getConnectivity(), Connectivity::Torus );
    BOOST_CHECK_EQUAL( params.getDimensionSpecs()[BGQDB::Dimension::D].getConnectivity(), Connectivity::Torus );
}
