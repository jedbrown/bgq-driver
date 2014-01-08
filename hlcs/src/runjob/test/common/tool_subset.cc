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
#define BOOST_TEST_MODULE tool_subset

#include <boost/test/unit_test.hpp>

#include "common/Exception.h"
#include "common/tool/Subset.h"

#include "test/common.h"

#include <boost/assign/list_of.hpp>

#include <boost/foreach.hpp>

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

using namespace runjob;

BOOST_AUTO_TEST_CASE( single_rank )
{
    const tool::Subset subset( "1" );

    // rank 1 should exist
    BOOST_CHECK_EQUAL(
            subset.match( 1 ),
            true
            );

    // some other rank should not
    BOOST_CHECK_EQUAL(
            subset.match( 2 ),
            false
            );
}

BOOST_AUTO_TEST_CASE( negative_single_rank )
{
    BOOST_CHECK_THROW(
            tool::Subset( "-1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( garbage_single_rank )
{
    // something that is not an integer
    BOOST_CHECK_THROW(
            tool::Subset( "asdf" ),
            std::invalid_argument
            );
    BOOST_CHECK_THROW(
            tool::Subset( "1234asdf" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( list_of_ranks )
{
    const std::vector<unsigned> ranks = boost::assign::list_of(1)(2)(4)(6)(7)(100);
    std::ostringstream arg;
    for ( std::vector<unsigned>::const_iterator i = ranks.begin(); i != ranks.end(); ++i ) {
        if ( i != ranks.begin() ) {
            arg << " ";
        }
        arg << *i;
    }
    const tool::Subset subset( arg.str() );

    BOOST_FOREACH( const unsigned i, ranks ) {
        BOOST_CHECK_EQUAL(
                subset.match( i ),
                true
            );
    }
}

BOOST_AUTO_TEST_CASE( range_with_default_stride )
{
    const tool::Subset subset( "1-10" );
    for ( unsigned i = 1; i <=10; ++i ) {
        BOOST_CHECK_EQUAL(
                subset.match( i ),
                true
                );
    }

    // some rank outside of the range
    BOOST_CHECK_EQUAL(
            subset.match( 11 ),
            false
            );
}

BOOST_AUTO_TEST_CASE( range_with_an_explicit_stride )
{
    const tool::Subset subset( "1-10:1" );
    for ( unsigned i = 1; i <=10; ++i ) {
        BOOST_CHECK_EQUAL(
                subset.match( i ),
                true
                );
    }

    // some rank outside of the range
    BOOST_CHECK_EQUAL(
            subset.match( 11 ),
            false
            );
}

BOOST_AUTO_TEST_CASE( range_with_an_explicit_stride_of_2 )
{
    const tool::Subset subset( "1-10:2" );
    for ( unsigned i = 1; i <=10; i += 2 ) {
        BOOST_CHECK_EQUAL(
                subset.match( i ),
                true
                );
    }

    // some rank outside of the range
    BOOST_CHECK_EQUAL(
            subset.match( 11 ),
            false
            );
}

BOOST_AUTO_TEST_CASE( range_with_dollar_max )
{
    const tool::Subset subset( "10-$max" );

    for ( unsigned i = 0; i < 10; ++i ) {
        BOOST_CHECK_EQUAL(
                subset.match( i ),
                false
                );
    }

    // all ranks above 10 should be in this range
    BOOST_CHECK_EQUAL(
            subset.match( 10 ),
            true
            );
    BOOST_CHECK_EQUAL(
            subset.match( 1010 ),
            true
            );

    BOOST_CHECK_EQUAL(
            subset.match( 99999 ),
            true
            );
}

BOOST_AUTO_TEST_CASE( range_with__max )
{
    const tool::Subset subset( "20-max" );

    for ( unsigned i = 0; i < 20; ++i ) {
        BOOST_CHECK_EQUAL(
                subset.match( i ),
                false
                );
    }

    // all ranks above 20 should be in this range
    BOOST_CHECK_EQUAL(
            subset.match( 20 ),
            true
            );
    BOOST_CHECK_EQUAL(
            subset.match( 1010 ),
            true
            );

    BOOST_CHECK_EQUAL(
            subset.match( 99999 ),
            true
            );
}

BOOST_AUTO_TEST_CASE( list_and_range )
{
    const tool::Subset subset( "1 2 4 8 10-20 25" );

    BOOST_CHECK_EQUAL( subset.match(1), true );
    BOOST_CHECK_EQUAL( subset.match(2), true );
    BOOST_CHECK_EQUAL( subset.match(3), false );
    BOOST_CHECK_EQUAL( subset.match(4), true );
    BOOST_CHECK_EQUAL( subset.match(5), false );
    BOOST_CHECK_EQUAL( subset.match(7), false );
    BOOST_CHECK_EQUAL( subset.match(8), true );
    BOOST_CHECK_EQUAL( subset.match(9), false );
    for ( unsigned i = 10; i <= 20; ++i ) {
        BOOST_CHECK_EQUAL(
                subset.match( i ),
                true
                );
    }
    BOOST_CHECK_EQUAL( subset.match(25), true );
}

BOOST_AUTO_TEST_CASE( missing_second_rank )
{
    BOOST_CHECK_THROW(
            tool::Subset( "11-" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( missing_stride )
{
    BOOST_CHECK_THROW(
            tool::Subset( "11-20:" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( negative_stride )
{
    BOOST_CHECK_THROW(
            tool::Subset( "21-33:-1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( zero_stride )
{
    BOOST_CHECK_THROW(
            tool::Subset( "31-44:0" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( garbage_stride )
{
    BOOST_CHECK_THROW(
            tool::Subset( "41-55:asdf" ),
            std::invalid_argument
            );
    BOOST_CHECK_THROW(
            tool::Subset( "41-55:123asdf" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( max_as_first_rank )
{
    BOOST_CHECK_THROW(
            tool::Subset( "$max-10" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( reverse_range )
{
    BOOST_CHECK_THROW(
            tool::Subset( "10-1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( no_range )
{
    BOOST_CHECK_THROW(
            tool::Subset( "100-100" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( decreasing_range )
{
    // subset specifications are required to be in increasing order
    BOOST_REQUIRE_THROW(
            tool::Subset( "0 1 2 4 4-10" ),
            std::logic_error
            );
    BOOST_REQUIRE_THROW(
            tool::Subset( "0-5 1 2 4-10" ),
            std::logic_error
            );
    BOOST_REQUIRE_THROW(
            tool::Subset( "0 1 2 4 3-10" ),
            std::logic_error
            );
    BOOST_REQUIRE_THROW(
            tool::Subset( "0 1-5 5" ),
            std::logic_error
            );
    BOOST_REQUIRE_THROW(
            tool::Subset( "0 1-5 4" ),
            std::logic_error
            );
    BOOST_REQUIRE_THROW(
            tool::Subset( "0 1-$max 400 500 10000" ),
            std::logic_error
            );
}

BOOST_AUTO_TEST_CASE( subset_too_long )
{
    std::ostringstream os;
    os << "0 ";
    for ( unsigned i = 0; i < tool::Subset::MaximumLength; ++i ) {
        os << i << " ";
    }

    BOOST_CHECK(
            os.str().size() > tool::Subset::MaximumLength 
            );

    BOOST_CHECK_THROW(
            tool::Subset( os.str() ),
            std::runtime_error
            );
}

