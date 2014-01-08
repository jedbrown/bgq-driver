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

#include "json.hpp"

#include <iostream>
#include <sstream>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE jsonFormatter
#include <boost/test/unit_test.hpp>


using json::Array;
using json::Object;
using json::Value;
using json::ValuePtr;

using std::ostringstream;


static json::Formatter f;


BOOST_AUTO_TEST_CASE( test_string )
{
    BOOST_CHECK_EQUAL( f( *json::create( "str1" ) ), "\"str1\"\n" );
}


BOOST_AUTO_TEST_CASE( test_number )
{
    BOOST_CHECK_EQUAL( f( *json::create( 1 ) ), "1\n" );
    BOOST_CHECK_EQUAL( f( *json::create( 1.0 ) ), "1\n" );
    BOOST_CHECK_EQUAL( f( *json::create( 1.3 ) ), "1.3\n" );
    BOOST_CHECK_EQUAL( f( *json::create( -1.0 ) ), "-1\n" );
    BOOST_CHECK_EQUAL( f( *json::create( -1.4 ) ), "-1.4\n" );
    BOOST_CHECK_EQUAL( f( *json::create( 1234567 ) ), "1234567\n" ); // Try large number.
}


BOOST_AUTO_TEST_CASE( test_Boolean )
{
    BOOST_CHECK_EQUAL( f( *json::create( true ) ), "true\n" );
    BOOST_CHECK_EQUAL( f( *json::create( false ) ), "false\n" );
}


BOOST_AUTO_TEST_CASE( test_null )
{
    BOOST_CHECK_EQUAL( f( *json::createNull() ), "null\n" );
}


BOOST_AUTO_TEST_CASE( test_empty_array )
{
    BOOST_CHECK_EQUAL( f( *json::Array::create() ), "[]\n" );
}


BOOST_AUTO_TEST_CASE( test_array_one_elem )
{
    Array a;
    a.add( json::create( "a0" ) );

    BOOST_CHECK_EQUAL( f( *json::create( a ) ), "[ \"a0\" ]\n" );
}


BOOST_AUTO_TEST_CASE( test_array_two_elem )
{
    Array a;

    a.add( "a0" );
    a.add( 1 );

    BOOST_CHECK_EQUAL( f( *json::create( a ) ), "[ \"a0\", 1 ]\n" );
}


BOOST_AUTO_TEST_CASE( test_empty_object )
{
    ostringstream oss;
    f( *json::Object::create(), oss );

    BOOST_CHECK_EQUAL( oss.str(), "{}\n" );
}


BOOST_AUTO_TEST_CASE( test_object_string_one_member )
{
    Object o;
    o.set( "a", "aval" );

    ostringstream oss;
    f( *json::create( o ), oss );

    BOOST_CHECK_EQUAL( oss.str(),
"{\n"
"  \"a\" : \"aval\"\n"
"}\n"
        );
}


BOOST_AUTO_TEST_CASE( test_object_name_special_char )
{
    // If the name of a field in an object has a special char like " then it will be escaped.
    Object o;
    o.set( "a\"b", "abval" );

    ostringstream oss;
    f( *json::create( o ), oss );

    BOOST_CHECK_EQUAL( oss.str(),
"{\n"
"  \"a\\\"b\" : \"abval\"\n"
"}\n"
        );
}


BOOST_AUTO_TEST_CASE( test_object_string_one_int )
{
    Object o;
    o.set( "a", double(1) );

    ostringstream oss;
    f( *json::create( o ), oss );

    BOOST_CHECK_EQUAL( oss.str(),
"{\n"
"  \"a\" : 1\n"
"}\n"
        );
}


BOOST_AUTO_TEST_CASE( test_object_multi )
{
    Object o;
    o.set( "a", "aval" );
    o.set( "b", double(1) );

    ostringstream oss;
    f( *json::create( o ), oss );

    BOOST_CHECK_EQUAL( oss.str(),
"{\n"
"  \"a\" : \"aval\",\n"
"  \"b\" : 1\n"
"}\n"
        );
}


BOOST_AUTO_TEST_CASE( test_obj_obj_member )
{
    /* Without escapes:
{
  "a" : {
      "b" : "bval"
    }
}
     */


    Object obj;
    Object &a_obj(obj.createObject( "a" ));
    a_obj.set( "b", "bval" );

    BOOST_CHECK_EQUAL( f( *json::create( obj ) ),
"{\n"
"  \"a\" : {\n"
"      \"b\" : \"bval\"\n"
"    }\n"
"}\n"
        );
}


BOOST_AUTO_TEST_CASE( test_obj_obj_members )
{
    /* Without escapes:
{
  "a" : {
      "b" : "bval",
      "c" : "cval"
    }
}
     */


    Object json;
    Object &a_obj(json.createObject( "a" ));

    a_obj.set( "b", "bval" );
    a_obj.set( "c", "cval" );

    BOOST_CHECK_EQUAL( f( *json::create( json ) ),
"{\n"
"  \"a\" : {\n"
"      \"b\" : \"bval\",\n"
"      \"c\" : \"cval\"\n"
"    }\n"
"}\n"
        );

}
