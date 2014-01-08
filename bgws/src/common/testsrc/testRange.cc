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

#include "../ItemRange.hpp"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <sstream>


using bgws::common::ItemRange;


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE ItemRange
#include <boost/test/unit_test.hpp>


static int initialize_()
{
    bgq::utility::initializeLogging( *(bgq::utility::Properties::create( "" )) );
    return 0;
}

static int s_initialized_(initialize_());


BOOST_AUTO_TEST_CASE( test_equal )
{
    BOOST_CHECK_EQUAL( ItemRange::createAbs( 0, 1 ), ItemRange::createAbs( 0, 1 ) );
    BOOST_CHECK_EQUAL( ItemRange::createAbs( 100, 200 ), ItemRange::createAbs( 100, 200 ) );
}

BOOST_AUTO_TEST_CASE( test_not_equal )
{
    BOOST_CHECK( ! (ItemRange::createAbs( 0, 1 ) == ItemRange::createAbs( 1, 1 )) );
    BOOST_CHECK( ! (ItemRange::createAbs( 0, 1 ) == ItemRange::createAbs( 0, 2 )) );
}


BOOST_AUTO_TEST_CASE( test_createCount )
{
    // Can create with start and count rather than start and end.
    BOOST_CHECK_EQUAL( ItemRange::createCount( 0, 2 ), ItemRange::createAbs( 0, 1 ) );
}


BOOST_AUTO_TEST_CASE( test_createCountZero_ex )
{
    // If create with count == 0 fails
    BOOST_CHECK_THROW( ItemRange::createCount( 100, 0 ), std::runtime_error );
}


BOOST_AUTO_TEST_CASE( test_output )
{
    std::ostringstream oss;
    oss << ItemRange::createAbs( 0, 49 );
    BOOST_CHECK_EQUAL( oss.str(), "items=0-49" );
}

BOOST_AUTO_TEST_CASE( test_parse_valid )
{
    BOOST_CHECK_EQUAL( ItemRange::parse( "items=0-1" ), ItemRange::createAbs( 0, 1 ) );
    BOOST_CHECK_EQUAL( ItemRange::parse( "items=1-2" ), ItemRange::createAbs( 1, 2 ) );
    BOOST_CHECK_EQUAL( ItemRange::parse( "items=0-0" ), ItemRange::createAbs( 0, 0 ) );
}

BOOST_AUTO_TEST_CASE( test_parse_invalid_string )
{
    // parse with invalid string returns the default.
    BOOST_CHECK_EQUAL( ItemRange::parse( "items=0-b", 10 ), ItemRange::createAbs( 0, 9 ) );
    BOOST_CHECK_EQUAL( ItemRange::parse( "titems=0-b", 15 ), ItemRange::createAbs( 0, 14 ) );
}


BOOST_AUTO_TEST_CASE( test_parse_invalid_too_big )
{
    // parse with valid string but > max size returns max allowed.
    BOOST_CHECK_EQUAL( ItemRange::parse( "items=0-100", 50, 100 ), ItemRange::createAbs( 0, 99 ) );
    BOOST_CHECK_EQUAL( ItemRange::parse( "items=0-100" ), ItemRange::createAbs( 0, 99 ) ); // there's defaults for max and default size.
    BOOST_CHECK_EQUAL( ItemRange::parse( "items=0-101", 50, 100 ), ItemRange::createAbs( 0, 99 ) );
    BOOST_CHECK_EQUAL( ItemRange::parse( "items=1-101", 50, 100 ), ItemRange::createAbs( 1, 100 ) );
}

BOOST_AUTO_TEST_CASE( test_parse_invalid_begin_after_end )
{
    // parse with valid string but > max size returns max allowed.
    BOOST_CHECK_EQUAL( ItemRange::parse( "items=1-0" ), ItemRange::createAbs( 0, 49 ) );
}
