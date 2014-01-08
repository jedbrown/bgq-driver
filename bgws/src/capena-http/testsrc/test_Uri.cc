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
#define BOOST_TEST_MODULE Uri
#include <boost/test/unit_test.hpp>


using capena::http::uri::Uri;
using capena::http::uri::Path;
using capena::http::uri::Query;


BOOST_AUTO_TEST_CASE( test_relative_no_query )
{
    Uri uri(
            Path() / "bg" / "jobs"
        );

    BOOST_CHECK( ! uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
    BOOST_CHECK_EQUAL( uri.calcString(), "/bg/jobs" );
}


BOOST_AUTO_TEST_CASE( test_relative_query )
{
    Uri uri(
            Path() / "bg" / "jobs",
            Query( Query::Parameters { Query::Parameter( "status", "R" ) } )
        );

    BOOST_CHECK( ! uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "?status=R" );
    BOOST_CHECK_EQUAL( uri.calcString(), "/bg/jobs?status=R" );
}


BOOST_AUTO_TEST_CASE( test_absolute_nonsecure_no_path_no_query )
{
    Uri uri(
            "localhost"
        );

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::NotSecure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
    BOOST_CHECK_EQUAL( uri.calcString(), "http://localhost" );
}


BOOST_AUTO_TEST_CASE( test_absolute_nonsecure_path_no_query )
{
    Uri uri(
            "localhost",
            Path() / "bg" / "jobs"
        );

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::NotSecure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
    BOOST_CHECK_EQUAL( uri.calcString(), "http://localhost/bg/jobs" );
}


BOOST_AUTO_TEST_CASE( test_absolute_nonsecure_path_query )
{
    Uri uri(
            "localhost",
            Path() / "bg" / "jobs",
            Query( Query::Parameters { Query::Parameter( "status", "R" ) } )
        );

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::NotSecure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "?status=R" );
    BOOST_CHECK_EQUAL( uri.calcString(), "http://localhost/bg/jobs?status=R" );
}


BOOST_AUTO_TEST_CASE( test_absolute_secure_no_path_no_query )
{
    Uri uri(
            Uri::Security::Secure,
            "localhost"
        );

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::Secure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
    BOOST_CHECK_EQUAL( uri.calcString(), "https://localhost" );
}


BOOST_AUTO_TEST_CASE( test_absolute_secure_path_no_query )
{
    Uri uri(
            Uri::Security::Secure,
            "localhost",
            Path() / "bg" / "jobs"
        );

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::Secure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "" );
    BOOST_CHECK_EQUAL( uri.calcString(), "https://localhost/bg/jobs" );
}


BOOST_AUTO_TEST_CASE( test_absolute_secure_path_query )
{
    Uri uri(
            Uri::Security::Secure,
            "localhost",
            Path() / "bg" / "jobs",
            Query( Query::Parameters { Query::Parameter( "status", "R" ) } )
        );

    BOOST_CHECK( uri.isAbsolute() );
    BOOST_CHECK_EQUAL( uri.getSecurity(), Uri::Security::Secure );
    BOOST_CHECK_EQUAL( uri.getHostPort(), "localhost" );
    BOOST_CHECK_EQUAL( uri.getPath().toString(), "/bg/jobs" );
    BOOST_CHECK_EQUAL( uri.getQuery().calcString(), "?status=R" );
    BOOST_CHECK_EQUAL( uri.calcString(), "https://localhost/bg/jobs?status=R" );
}
