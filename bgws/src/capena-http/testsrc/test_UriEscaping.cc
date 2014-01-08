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


#include "../http/uri/utility.hpp"


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE UriEscaping
#include <boost/test/unit_test.hpp>


using capena::http::uri::escape;
using capena::http::uri::unescape;


BOOST_AUTO_TEST_CASE( test_escape_empty )
{
    BOOST_CHECK_EQUAL( escape( "" ), "" );
}


BOOST_AUTO_TEST_CASE( test_escape_safe )
{
    // escape safe don't get %-encoded.
    BOOST_CHECK_EQUAL( escape( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$-_.!*'()," ), "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$-_.!*'()," );
}


BOOST_AUTO_TEST_CASE( test_escape )
{
    // Non-safe characters do get %-encoded.
    // Try a bunch of characters, %?&=+
    BOOST_CHECK_EQUAL( escape( "%" ), "%25" );
    BOOST_CHECK_EQUAL( escape( "?" ), "%3f" );
    BOOST_CHECK_EQUAL( escape( "&" ), "%26" );
    BOOST_CHECK_EQUAL( escape( "=" ), "%3d" );
    BOOST_CHECK_EQUAL( escape( "+" ), "%2b" );
    BOOST_CHECK_EQUAL( escape( " " ), "%20" );

    BOOST_CHECK_EQUAL( escape( "%?&=+" ), "%25%3f%26%3d%2b" );
}


BOOST_AUTO_TEST_CASE( test_unescape_empty )
{
    // empty string is easy to unescape.
    BOOST_CHECK_EQUAL( unescape( "" ), "" );
}


BOOST_AUTO_TEST_CASE( test_unescape_no_escape_seqs )
{
    // string with no escape sequences is easy too since no changes.
    BOOST_CHECK_EQUAL( unescape( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$-_.!*'()," ), "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$-_.!*'()," );
}


BOOST_AUTO_TEST_CASE( test_unescape_esc_seq )
{
    // string with escape characters gets those chars unescaped.
    BOOST_CHECK_EQUAL( unescape( "%25" ), "%" );
    BOOST_CHECK_EQUAL( unescape( "%3f" ), "?" );
    BOOST_CHECK_EQUAL( unescape( "%26" ), "&" );
    BOOST_CHECK_EQUAL( unescape( "%3d" ), "=" );
    BOOST_CHECK_EQUAL( unescape( "%2b" ), "+" );

    BOOST_CHECK_EQUAL( unescape( "%25%3f%26%3d%2b" ), "%?&=+" );

    // can also use uppercase.
    BOOST_CHECK_EQUAL( unescape( "%3F" ), "?" );

    // Can unescape any chars to make ' '
    BOOST_CHECK_EQUAL( unescape( "s%20a" ), "s a" );
}


BOOST_AUTO_TEST_CASE( test_unescape_plus )
{
    // Test unescape + specially since is kind of an oddball.
    BOOST_CHECK_EQUAL( unescape( "+" ), "+" );
}


BOOST_AUTO_TEST_CASE( test_unescape_fail )
{
    // Unescape with not 2 chars fails with invalid_argument
    BOOST_CHECK_THROW( unescape( "%" ), std::invalid_argument );

    // Unescape with not 2 chars fails with invalid_argument
    BOOST_CHECK_THROW( unescape( "%3" ), std::invalid_argument );

    // unescape with not hex fails.
    BOOST_CHECK_THROW( unescape( "%2g" ), std::invalid_argument );

    // %% is not escape for %.
    BOOST_CHECK_THROW( unescape( "%%%%" ), std::invalid_argument );
}
