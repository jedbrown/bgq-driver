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

#include "../http/MediaType.hpp"

#include <sstream>
#include <stdexcept>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE MediaType
#include <boost/test/unit_test.hpp>


using namespace capena::http;

using std::ostringstream;


BOOST_AUTO_TEST_CASE( test_const_type_subtype )
{
    MediaType media_type( "application", "json" );

    BOOST_CHECK_EQUAL( media_type.getType(), "application" );
    BOOST_CHECK_EQUAL( media_type.getSubtype(), "json" );
    BOOST_CHECK( media_type.getParameters().empty() );
}


BOOST_AUTO_TEST_CASE( test_const_type_subtype_parameters )
{
    MediaType::Parameters parameters;
    parameters["charset"] = "ISO8859-4";

    MediaType media_type( "application", "json", parameters );

    BOOST_CHECK_EQUAL( media_type.getType(), "application" );
    BOOST_CHECK_EQUAL( media_type.getSubtype(), "json" );
    BOOST_REQUIRE_EQUAL( media_type.getParameters().size(), 1 );
    BOOST_CHECK_EQUAL( media_type.getParameters().at("charset"), "ISO8859-4" );
}


BOOST_AUTO_TEST_CASE( test_parse_simple )
{
    MediaType media_type(MediaType::parse( "application/json" ));

    BOOST_CHECK_EQUAL( media_type.getType(), "application" );
    BOOST_CHECK_EQUAL( media_type.getSubtype(), "json" );
    BOOST_CHECK( media_type.getParameters().empty() );
}


BOOST_AUTO_TEST_CASE( test_parse_parameter )
{
    MediaType media_type(MediaType::parse( "application/json; charset=UTF-8" ));

    BOOST_CHECK_EQUAL( media_type.getType(), "application" );
    BOOST_CHECK_EQUAL( media_type.getSubtype(), "json" );
    BOOST_CHECK_EQUAL( media_type.getParameters().at( "charset" ), "UTF-8" );
}


BOOST_AUTO_TEST_CASE( test_parse_exception_invalid_no_slash )
{
    BOOST_CHECK_THROW( MediaType::parse( "application" ), std::invalid_argument );
}


BOOST_AUTO_TEST_CASE( test_json_const )
{
    BOOST_CHECK_EQUAL( MediaType::JSON.getType(), "application" );
    BOOST_CHECK_EQUAL( MediaType::JSON.getSubtype(), "json" );
    BOOST_CHECK( MediaType::JSON.getParameters().empty() );
}


BOOST_AUTO_TEST_CASE( test_equivalent )
{
    MediaType::Parameters parameters;
    parameters["charset"] = "UTF=8";

    MediaType media_type( "application", "json", parameters );

    BOOST_CHECK( media_type.equivalent( MediaType::JSON ) );
}


BOOST_AUTO_TEST_CASE( test_not_equivalent )
{
    MediaType media_type( "application", "x-www-form-urlencoded" );

    BOOST_CHECK( ! media_type.equivalent( MediaType::JSON ) );
}


BOOST_AUTO_TEST_CASE( test_output_operator_no_parameters )
{
    ostringstream oss;
    oss << MediaType::JSON;
    BOOST_CHECK_EQUAL( oss.str(), "application/json" );
}


BOOST_AUTO_TEST_CASE( test_output_operator_parameters )
{
    ostringstream oss;
    oss << MediaType::JSON_UTF8;
    BOOST_CHECK_EQUAL( oss.str(), "application/json; charset=UTF-8" );
}
