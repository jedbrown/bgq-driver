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


#include "../http/uri/Path.hpp"

#include <sstream>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE UriPath
#include <boost/test/unit_test.hpp>


using namespace capena::http::uri;

using std::ostringstream;


BOOST_AUTO_TEST_CASE( test_default_constructor )
{
    Path url_path;
    BOOST_CHECK( url_path.empty() );
}


BOOST_AUTO_TEST_CASE( test_parse_simple )
{
    // Can have a single part.

    Path url_path( "/bg" );

    BOOST_REQUIRE_EQUAL( url_path.size(), 1U );
    BOOST_CHECK_EQUAL( url_path[0], "bg" );
}


BOOST_AUTO_TEST_CASE( test_parse_multi )
{
    // Can have multiple parts

    Path url_path( "/bg/blocks" );

    BOOST_REQUIRE_EQUAL( url_path.size(), 2U );
    BOOST_CHECK_EQUAL( url_path[0], "bg" );
    BOOST_CHECK_EQUAL( url_path[1], "blocks" );
}


BOOST_AUTO_TEST_CASE( test_parse_empty )
{
    // Can be the empty string, returns empty values.

    BOOST_CHECK( Path( "" ).empty() );
}


BOOST_AUTO_TEST_CASE( test_parse_root )
{
    // path can be just /

    Path url_path( "/" );

    BOOST_REQUIRE_EQUAL( url_path.size(), 1U );
    BOOST_CHECK_EQUAL( url_path[0], "" );
}


BOOST_AUTO_TEST_CASE( test_parse_space_esc )
{
    // Can have multiple parts

    Path url_path( "/b%20g" );

    BOOST_REQUIRE_EQUAL( url_path.size(), 1U );
    BOOST_CHECK_EQUAL( url_path[0], "b g" );
}


BOOST_AUTO_TEST_CASE( test_parse_hex_esc )
{
    // Can have multiple parts

    Path url_path( "/b%2Bg" );

    BOOST_REQUIRE_EQUAL( url_path.size(), 1U );
    BOOST_CHECK_EQUAL( url_path[0], "b+g" );
}


BOOST_AUTO_TEST_CASE( test_gen_simple )
{
    BOOST_CHECK_EQUAL( (Path() / "bg").toString(), "/bg" );
}


BOOST_AUTO_TEST_CASE( test_gen_multiple )
{
    BOOST_CHECK_EQUAL( (Path() / "bg" / "blocks" ).toString(), "/bg/blocks" );
}


BOOST_AUTO_TEST_CASE( test_gen_empty )
{
    BOOST_CHECK_EQUAL( Path().toString(), "" );
}


BOOST_AUTO_TEST_CASE( test_gen_empty_part )
{
    BOOST_CHECK_EQUAL( (Path() / "").toString(), "/" );
}


BOOST_AUTO_TEST_CASE( test_gen_esc_space )
{
    // spaces in paths are turned to +

    BOOST_CHECK_EQUAL( (Path() / "b g" ).toString(), "/b%20g" );
}


BOOST_AUTO_TEST_CASE( test_gen_esc_other_chars )
{
    // spaces in paths are turned to +

    BOOST_CHECK_EQUAL( (Path() / "b+g").toString(), "/b%2bg" );
}


BOOST_AUTO_TEST_CASE( test_ostream_operator )
{
    // operator<< implemented that writes escaped string to ostream.

    ostringstream oss;
    oss << (Path() / "b+g");

    BOOST_CHECK_EQUAL( oss.str(), "/b%2bg" );
}


BOOST_AUTO_TEST_CASE( test_safe_chars )
{
    // - is safe and shouldn't be escaped.
    BOOST_CHECK_EQUAL( (Path() / "b-g" ).toString(), "/b-g" );
}


BOOST_AUTO_TEST_CASE( test_isDescendantOf )
{
    // /bg/jobs is a descendant of /bg.
    // /bg/jobs is a descendant of empty path.
    // /bg/jobs/1 is a descendant of /bg
    // /bg/p1/p2/p3 is a descendant of /bg/p1

    // /bgq/jobs is not a descendant of /bg/jobs, easy since is same length.
    // /bg is not a descendant of /bg/jobs, easy since is shorter.
    // /bg/jobs is not a descendant of /bgq, since parent doesn't match.
    // /bg/jobs is not a descendant of /bg/jobs.

    BOOST_CHECK( Path( "/bg/jobs" ).isDescendantOf( Path( "/bg" ) ) );
    BOOST_CHECK( ! Path( "/bg" ).isDescendantOf( Path( "/bg/jobs" ) ) );

    BOOST_CHECK( Path( "/bg/jobs" ).isDescendantOf( Path() ) );
    BOOST_CHECK( ! Path().isDescendantOf( Path( "/bg/jobs" ) ) );

    BOOST_CHECK( Path( "/bg/jobs/1" ).isDescendantOf( Path( "/bg" ) ) );
    BOOST_CHECK( ! Path( "/bg" ).isDescendantOf( Path( "/bg/jobs/1" ) ) );

    BOOST_CHECK( Path( "/bg/p1/p2/p3" ).isDescendantOf( Path( "/bg/p1" ) ) );
    BOOST_CHECK( ! Path( "/bg/p1" ).isDescendantOf( Path( "/bg/p1/p2/p3" ) ) );

    BOOST_CHECK( ! Path( "/bgq/jobs" ).isDescendantOf( Path( "/bg/jobs" ) ) );
    BOOST_CHECK( ! Path( "/bg" ).isDescendantOf( Path( "/bg/jobs" ) ) );
    BOOST_CHECK( ! Path( "/bg/jobs" ).isDescendantOf( Path( "/bgq" ) ) );
}


BOOST_AUTO_TEST_CASE( test_isAncestorOf )
{
    BOOST_CHECK( Path( "/bg" ).isAncestorOf( Path( "/bg/jobs" ) ) );
    BOOST_CHECK( ! Path( "/bg/jobs" ).isAncestorOf( Path( "/bg" ) ) );

    BOOST_CHECK( Path().isAncestorOf( Path( "/bg/jobs" ) ) );
    BOOST_CHECK( ! Path( "/bg/jobs" ).isAncestorOf( Path() ) );

    BOOST_CHECK( Path( "/bg" ).isAncestorOf( Path( "/bg/jobs/1" ) ) );
    BOOST_CHECK( ! Path( "/bg/jobs/1" ).isAncestorOf( Path( "/bg" ) ) );

    BOOST_CHECK( Path( "/bg/p1" ).isAncestorOf( Path( "/bg/p1/p2/p3" ) ) );
    BOOST_CHECK( ! Path( "/bg/p1/p2/p3" ).isAncestorOf( Path( "/bg/p1" ) ) );

    BOOST_CHECK( ! Path( "/bg/jobs" ).isAncestorOf( Path( "/bgq/jobs" ) ) );
    BOOST_CHECK( ! Path( "/bg/jobs" ).isAncestorOf( Path( "/bg" ) ) );
    BOOST_CHECK( ! Path( "/bgq" ).isAncestorOf( Path( "/bg/jobs" ) ) );
}


BOOST_AUTO_TEST_CASE( test_isChildOf )
{
    BOOST_CHECK( Path( "/bg/jobs" ).isChildOf( Path( "/bg" ) ) );
    BOOST_CHECK( Path( "/bg/jobs/1" ).isChildOf( Path( "/bg/jobs" ) ) );

    BOOST_CHECK( ! Path( "/bg/jobs" ).isChildOf( Path( "/bg/jobs" ) ) );
    BOOST_CHECK( ! Path( "/bg" ).isChildOf( Path( "/bg/jobs" ) ) );
    BOOST_CHECK( ! Path( "/bg/jobs" ).isChildOf( Path( "/bgq" ) ) );

    BOOST_CHECK( ! Path( "/bg/jobs/1/2" ).isChildOf( Path( "/bg/jobs" ) ) );
}


BOOST_AUTO_TEST_CASE( test_isParentOf )
{
    BOOST_CHECK( Path( "/bg" ).isParentOf( Path( "/bg/jobs" ) ) );
    BOOST_CHECK( Path( "/bg/jobs" ).isParentOf( Path( "/bg/jobs/1" ) ) );

    BOOST_CHECK( ! Path( "/bg/jobs" ).isParentOf( Path( "/bg/jobs" ) ) );
    BOOST_CHECK( ! Path( "/bg/jobs" ).isParentOf( Path( "/bg" ) ) );
    BOOST_CHECK( ! Path( "/bgq" ).isParentOf( Path( "/bg/jobs" ) ) );

    BOOST_CHECK( ! Path( "/bg/jobs" ).isParentOf( Path( "/bg/jobs/1/2" ) ) );
}
