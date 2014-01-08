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


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE ContentRange
#include <boost/test/unit_test.hpp>


#include "../ContentRange.hpp"

#include <boost/lexical_cast.hpp>

#include <string>


using bgws::common::ContentRange;

using boost::lexical_cast;

using std::string;
using std::uint64_t;


BOOST_AUTO_TEST_CASE( testCreateStartEndTotal )
{
    // Can create with start, end, and total, can get the start, end, and total back

    static const uint64_t start(0), end(9), total(100);

    ContentRange cr(ContentRange::createAbs( start, end, total ));

    BOOST_CHECK_EQUAL( cr.getStart(), start );
    BOOST_CHECK_EQUAL( cr.getEnd(), end );
    BOOST_CHECK_EQUAL( cr.getTotal(), total );
}


BOOST_AUTO_TEST_CASE( testCreateStartCountTotal )
{
    // Can create with start, count, and total, can get the start, end, and total.

    static const uint64_t start(0), count(10), total(100);

    ContentRange cr(ContentRange::createCount( start, count, total ));

    BOOST_CHECK_EQUAL( cr.getStart(), start );
    BOOST_CHECK_EQUAL( cr.getEnd(), (start + count - 1) );
    BOOST_CHECK_EQUAL( cr.getTotal(), total );
}


BOOST_AUTO_TEST_CASE( testToString )
{
    // Can convert to string, format is like "items start-end/total"

    static const uint64_t start(10), end(19), total(100);

    BOOST_CHECK_EQUAL(
            lexical_cast<string>( ContentRange::createAbs( start, end, total ) ),
            "items 10-19/100"
        );
}


BOOST_AUTO_TEST_CASE( testParse )
{
    // Can parse a string like items 0-10/100 and get back ContentRange.

    BOOST_CHECK_EQUAL(
            lexical_cast<string>( ContentRange::parse( "items 0-10/100" ) ),
            lexical_cast<string>( ContentRange::createAbs( 0, 10, 100 ) )
        );
}


BOOST_AUTO_TEST_CASE( testParseSpaces )
{
    // Can parse a content range string that has more spaces or spaces in different places just to be more accepting.

    BOOST_CHECK_EQUAL(
            lexical_cast<string>( ContentRange::parse( " items  1 - 11 / 100 " ) ),
            lexical_cast<string>( ContentRange::createAbs( 1, 11, 100 ) )
        );
}


BOOST_AUTO_TEST_CASE( testParseInvalidEx )
{
    BOOST_CHECK_THROW( ContentRange::parse( "bytes 0-1000/10000" ), ContentRange::ParseError )
}


BOOST_AUTO_TEST_CASE( testCreateEndBeforeStartError )
{
    BOOST_CHECK_THROW( ContentRange::createAbs( 1, 0, 100 ), ContentRange::InvalidRangeError );
}


BOOST_AUTO_TEST_CASE( testCreateAbsZeroTotal )
{
    BOOST_CHECK_THROW( ContentRange::createAbs( 0, 9, 0 ), ContentRange::InvalidRangeError );
}


BOOST_AUTO_TEST_CASE( testCreateCountZeroCount )
{
    // If create using count and count is 0 then returns like items start-start/count.

    static const uint64_t start(600), count(0), total(500);

    ContentRange cr(ContentRange::createCount( start, count, total ));

    BOOST_CHECK_EQUAL( cr.getStart(), start );
    BOOST_CHECK_EQUAL( cr.getEnd(), start );
    BOOST_CHECK_EQUAL( cr.getTotal(), total );
}


BOOST_AUTO_TEST_CASE( testCreateCountZeroTotal )
{
    BOOST_CHECK_THROW( ContentRange::createCount( 10, 10, 0 ), ContentRange::InvalidRangeError );
}
