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

#include <boost/assign.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE uri::Query::parse
#include <boost/test/unit_test.hpp>


using namespace boost::assign;

using namespace capena::http::uri;

using std::invalid_argument;


BOOST_AUTO_TEST_CASE( test_parse_empty_str )
{
    Query::Arguments values(Query::parse( "" ).calcArguments());

    BOOST_CHECK( values.empty() );
}


BOOST_AUTO_TEST_CASE( test_simple_value )
{
    Query::Arguments values(Query::parse( "name=value" ).calcArguments());

    Query::Arguments exp = list_of( "--name" )( "value" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}


BOOST_AUTO_TEST_CASE( test_multi_value_ampersand )
{
    Query::Arguments values(Query::parse( "name1=value1&name2=value2" ).calcArguments());

    Query::Arguments exp = list_of( "--name1" )( "value1" )( "--name2" )( "value2" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}


BOOST_AUTO_TEST_CASE( test_esc_space_value )
{
    Query::Arguments values(Query::parse( "name=val+val" ).calcArguments());

    Query::Arguments exp = list_of( "--name" )( "val val" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}


BOOST_AUTO_TEST_CASE( test_esc_space_name )
{
    Query::Arguments values(Query::parse( "name+name=val+val" ).calcArguments());

    Query::Arguments exp = list_of( "--name name" )( "val val" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}


BOOST_AUTO_TEST_CASE( test_esc_chars_name )
{
    Query::Arguments values(Query::parse( "name%2Bname=val+val" ).calcArguments());

    Query::Arguments exp = list_of( "--name+name" )( "val val" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}


BOOST_AUTO_TEST_CASE( test_esc_chars_value )
{
    Query::Arguments values(Query::parse( "name+name=val%20val" ).calcArguments());

    Query::Arguments exp = list_of( "--name name" )( "val val" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}


BOOST_AUTO_TEST_CASE( test_invalid_esc_not_enough_chars )
{
    BOOST_CHECK_THROW( Query::parse( "name=val%" ).calcArguments(), invalid_argument );
    BOOST_CHECK_THROW( Query::parse( "name=val%2" ).calcArguments(), invalid_argument );
    BOOST_CHECK_THROW( Query::parse( "name=val;%2=val2" ).calcArguments(), invalid_argument );
}


BOOST_AUTO_TEST_CASE( test_empty_value )
{
    Query::Arguments values(Query::parse( "name=" ).calcArguments());

    Query::Arguments exp = list_of( "--name" )( "" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}


BOOST_AUTO_TEST_CASE( test_no_value )
{
    Query::Arguments values(Query::parse( "name" ).calcArguments());

    Query::Arguments exp = list_of( "--name" )( "" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}


BOOST_AUTO_TEST_CASE( test_no_value_multi )
{
    Query::Arguments values(Query::parse( "name1&name2" ).calcArguments());

    Query::Arguments exp = list_of( "--name1" )( "" )( "--name2" )( "" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}


BOOST_AUTO_TEST_CASE( test_ends_sep )
{
    Query::Arguments values(Query::parse( "name=value&" ).calcArguments());

    Query::Arguments exp = list_of( "--name" )( "value" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}


BOOST_AUTO_TEST_CASE( test_empty_pair )
{
    // if empty pair ( && ), skip it.
    Query::Arguments values(Query::parse( "name=value&&name2=value2" ).calcArguments());

    Query::Arguments exp = list_of( "--name" )( "value" )( "--name2" )( "value2" );

    BOOST_CHECK_EQUAL_COLLECTIONS( values.begin(), values.end(), exp.begin(), exp.end() );
}
