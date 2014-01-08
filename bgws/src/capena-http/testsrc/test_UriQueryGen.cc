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

#include "../http/uri/Query.hpp"

#include <string>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE uri::Query::gen
#include <boost/test/unit_test.hpp>


using namespace capena::http::uri;

using std::string;


BOOST_AUTO_TEST_CASE( test_empty )
{
    // empty parameters -> empty string.

    BOOST_CHECK_EQUAL( Query().calcString(), "" );
}


BOOST_AUTO_TEST_CASE( test_one )
{
    // { ("name", "value") } -> "?name=value"

    Query::Parameters params;
    params.push_back( Query::Parameter( "name", "value" ) );

    string qstr(Query( params ).calcString());
    BOOST_CHECK_EQUAL( qstr, "?name=value" );
}


BOOST_AUTO_TEST_CASE( test_two )
{
    // { ("name1", "value1"), ("name2","value2" } -> "?name1=value1&name2=value2

    Query::Parameters params;
    params.push_back( Query::Parameter( "name1", "value1" ) );
    params.push_back( Query::Parameter( "name2", "value2" ) );

    string qstr(Query( params ).calcString());
    BOOST_CHECK_EQUAL( qstr, "?name1=value1&name2=value2" );
}


BOOST_AUTO_TEST_CASE( test_esc_space )
{
    // { ("name", "val val") } -> "?name=val+val"

    Query::Parameters params;
    params.push_back( Query::Parameter( "name", "val val" ) );

    string qstr(Query( params ).calcString());
    BOOST_CHECK_EQUAL( qstr, "?name=val+val" );
}


BOOST_AUTO_TEST_CASE( test_esc_space_name )
{
    // { ("nam nam", "value") } -> "?nam+nam=value"

    Query::Parameters params;
    params.push_back( Query::Parameter( "nam nam", "value" ) );

    string qstr(Query( params ).calcString());
    BOOST_CHECK_EQUAL( qstr, "?nam+nam=value" );
}


BOOST_AUTO_TEST_CASE( test_escape_chars )
{
    Query::Parameters params;
    params.push_back( Query::Parameter( "n1=", "+value" ) );

    string qstr(Query( params ).calcString());
    BOOST_CHECK_EQUAL( qstr, "?n1%3d=%2bvalue" );
}


BOOST_AUTO_TEST_CASE( test_empty_name_value_one )
{
    Query::Parameters params;
    params.push_back( Query::Parameter( "", "" ) );

    string qstr(Query( params ).calcString());
    BOOST_CHECK_EQUAL( qstr, "" );
}


BOOST_AUTO_TEST_CASE( test_empty_name_value_last )
{
    Query::Parameters params;
    params.push_back( Query::Parameter( "name", "value" ) );
    params.push_back( Query::Parameter( "", "" ) );

    string qstr(Query( params ).calcString());
    BOOST_CHECK_EQUAL( qstr, "?name=value" );
}


BOOST_AUTO_TEST_CASE( test_empty_name_value_middle )
{
    Query::Parameters params;
    params.push_back( Query::Parameter( "name1", "value1" ) );
    params.push_back( Query::Parameter( "", "" ) );
    params.push_back( Query::Parameter( "name2", "value2" ) );

    string qstr(Query( params ).calcString());
    BOOST_CHECK_EQUAL( qstr, "?name1=value1&name2=value2" );
}


BOOST_AUTO_TEST_CASE( test_empty_value )
{
    Query::Parameters params;
    params.push_back( Query::Parameter( "name", "" ) );

    string qstr(Query( params ).calcString());
    BOOST_CHECK_EQUAL( qstr, "?name=" );
}


BOOST_AUTO_TEST_CASE( test_empty_name )
{
    Query::Parameters params;
    params.push_back( Query::Parameter( "", "test" ) );

    string qstr(Query( params ).calcString());
    BOOST_CHECK_EQUAL( qstr, "?=test" );
}
