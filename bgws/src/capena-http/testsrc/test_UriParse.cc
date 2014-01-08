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


#include "../http/uri/Uri.hpp"


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE UriParse
#include <boost/test/unit_test.hpp>


using capena::http::uri::Uri;


BOOST_AUTO_TEST_CASE( test_relative_no_query )
{
    Uri uri(Uri::parse( "/bg/jobs" ));

    BOOST_CHECK( ! uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
}


BOOST_AUTO_TEST_CASE( test_relative_query )
{
    Uri uri(Uri::parse( "/bg/jobs?status=R" ));

    BOOST_CHECK( ! uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "?status=R" );
}


BOOST_AUTO_TEST_CASE( test_absolute_nonsecure_no_host_no_path_no_query )
{
    Uri uri(Uri::parse( "http://" ));

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::NotSecure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
}


BOOST_AUTO_TEST_CASE( test_absolute_nonsecure_no_path_no_query )
{
    Uri uri(Uri::parse( "http://localhost" ));

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::NotSecure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
}


BOOST_AUTO_TEST_CASE( test_absolute_nonsecure_no_path_no_query2 )
{
    Uri uri(Uri::parse( "http://localhost:32068" ));

    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost:32068" );
}


BOOST_AUTO_TEST_CASE( test_absolute_nonsecure_path_no_query )
{
    Uri uri(Uri::parse( "http://localhost/bg/jobs" ));

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::NotSecure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
}


BOOST_AUTO_TEST_CASE( test_absolute_nonsecure_path_query )
{
    Uri uri(Uri::parse( "http://localhost/bg/jobs?status=R" ));

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::NotSecure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "?status=R" );
}


BOOST_AUTO_TEST_CASE( test_absolute_secure_no_host_no_path_no_query )
{
    Uri uri(Uri::parse( "https://" ));

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::Secure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
}


BOOST_AUTO_TEST_CASE( test_absolute_secure_no_path_no_query )
{
    Uri uri(Uri::parse( "https://localhost" ));

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::Secure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
}


BOOST_AUTO_TEST_CASE( test_absolute_secure_path_no_query )
{
    Uri uri(Uri::parse( "https://localhost/bg/jobs" ));

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::Secure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
}


BOOST_AUTO_TEST_CASE( test_absolute_secure_path_query )
{
    Uri uri(Uri::parse( "https://localhost/bg/jobs?status=R" ));

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::Secure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "?status=R" );
}


BOOST_AUTO_TEST_CASE( test_empty_string_ex )
{
    BOOST_CHECK_THROW( Uri::parse( "" ), std::invalid_argument );
}

BOOST_AUTO_TEST_CASE( test_colon_ex )
{
    BOOST_CHECK_THROW( Uri::parse( ":" ), std::invalid_argument );
}

BOOST_AUTO_TEST_CASE( schema_colon_ex )
{
    BOOST_CHECK_THROW( Uri::parse( "http:" ), std::invalid_argument );
}


BOOST_AUTO_TEST_CASE( test_scheme_single_slash_ex )
{
    BOOST_CHECK_THROW( Uri::parse( "http:/" ), std::invalid_argument );
}

BOOST_AUTO_TEST_CASE( test_scheme_not_slashes )
{
    BOOST_CHECK_THROW( Uri::parse( "http:ablocalhost" ), std::invalid_argument );
}
