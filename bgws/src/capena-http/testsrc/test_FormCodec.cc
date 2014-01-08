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


using capena::http::uri::form::encode;
using capena::http::uri::form::decode;


BOOST_AUTO_TEST_CASE( test_encode_empty )
{
    BOOST_CHECK_EQUAL( encode( "" ), "" );
}

BOOST_AUTO_TEST_CASE( test_encode_safe )
{
    // encode safe don't get %-encoded.
    BOOST_CHECK_EQUAL( encode( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$-_.!*'()," ), "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$-_.!*'()," );
}


BOOST_AUTO_TEST_CASE( test_encode )
{
    // Non-safe characters do get %-encoded.
    // Try a bunch of characters, %?&=+
    BOOST_CHECK_EQUAL( encode( "%" ), "%25" );
    BOOST_CHECK_EQUAL( encode( "?" ), "%3f" );
    BOOST_CHECK_EQUAL( encode( "&" ), "%26" );
    BOOST_CHECK_EQUAL( encode( "=" ), "%3d" );
    BOOST_CHECK_EQUAL( encode( "+" ), "%2b" );

    BOOST_CHECK_EQUAL( encode( "%?&=+" ), "%25%3f%26%3d%2b" );
}

BOOST_AUTO_TEST_CASE( test_encode_space )
{
    // Test encode ' ' specially since is kind of an oddball.
    BOOST_CHECK_EQUAL( encode( " " ), "+" );
}


BOOST_AUTO_TEST_CASE( test_decode_empty )
{
    // empty string is easy to decode.
    BOOST_CHECK_EQUAL( decode( "" ), "" );
}


BOOST_AUTO_TEST_CASE( test_decode_no_escape_seqs )
{
    // string with no escape sequences is easy too since no changes.
    BOOST_CHECK_EQUAL( decode( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$-_.!*'()," ), "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$-_.!*'()," );
}


BOOST_AUTO_TEST_CASE( test_decode_esc_seq )
{
    // string with escape characters gets those chars decoded.
    BOOST_CHECK_EQUAL( decode( "%25" ), "%" );
    BOOST_CHECK_EQUAL( decode( "%3f" ), "?" );
    BOOST_CHECK_EQUAL( decode( "%26" ), "&" );
    BOOST_CHECK_EQUAL( decode( "%3d" ), "=" );
    BOOST_CHECK_EQUAL( decode( "%2b" ), "+" );

    BOOST_CHECK_EQUAL( decode( "%25%3f%26%3d%2b" ), "%?&=+" );

    // can also use uppercase.
    BOOST_CHECK_EQUAL( decode( "%3F" ), "?" );

    // Can decode any chars to make ' '
    BOOST_CHECK_EQUAL( decode( "s%20a" ), "s a" );
}


BOOST_AUTO_TEST_CASE( test_decode_plus )
{
    // Test decode + specially since is kind of an oddball.
    BOOST_CHECK_EQUAL( decode( "+" ), " " );
}


BOOST_AUTO_TEST_CASE( test_decode_fail )
{
    // decode with not 2 chars fails with invalid_argument
    BOOST_CHECK_THROW( decode( "%" ), std::invalid_argument );

    // decode with not 2 chars fails with invalid_argument
    BOOST_CHECK_THROW( decode( "%3" ), std::invalid_argument );

    // decode with not hex fails.
    BOOST_CHECK_THROW( decode( "%2g" ), std::invalid_argument );

    // %% is not escape for %.
    BOOST_CHECK_THROW( decode( "%%%%" ), std::invalid_argument );
}
