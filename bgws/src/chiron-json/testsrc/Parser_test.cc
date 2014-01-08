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

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE jsonParser
#include <boost/test/unit_test.hpp>


using json::Array;
using json::Object;
using json::ValuePtr;

using std::string;


static json::Parser p;


BOOST_AUTO_TEST_CASE( test_no_value_ex )
{
    BOOST_CHECK_THROW( p(""), json::ParseError );
}


BOOST_AUTO_TEST_CASE( test_empty_string )
{
    ValuePtr value_ptr(p("\"\""));
    BOOST_CHECK( value_ptr->isString() );
    BOOST_CHECK_EQUAL( value_ptr->getString(), "" );
}


BOOST_AUTO_TEST_CASE( test_simple_string )
{
    ValuePtr value_ptr(p("\"abcdef\""));
    BOOST_CHECK( value_ptr->isString() );
    BOOST_CHECK_EQUAL( value_ptr->getString(), "abcdef" );
}


BOOST_AUTO_TEST_CASE( test_string_escape )
{
    BOOST_CHECK_EQUAL( p( "\"\\\"\"" )->getString(), "\"" );
    BOOST_CHECK_EQUAL( p( "\"\\\"a\"" )->getString(), "\"a" );
    BOOST_CHECK_EQUAL( p( "\"a\\\"\"" )->getString(), "a\"" );
    BOOST_CHECK_EQUAL( p( "\"a\\\"a\"" )->getString(), "a\"a" );

    BOOST_CHECK_EQUAL( p( "\"\\\\\"" )->getString(), "\\" );
    BOOST_CHECK_EQUAL( p( "\"\\/\"" )->getString(), "/" );
    BOOST_CHECK_EQUAL( p( "\"\\b\"" )->getString(), "\b" );
    BOOST_CHECK_EQUAL( p( "\"\\f\"" )->getString(), "\f" );
    BOOST_CHECK_EQUAL( p( "\"\\n\"" )->getString(), "\n" );
    BOOST_CHECK_EQUAL( p( "\"\\r\"" )->getString(), "\r" );
    BOOST_CHECK_EQUAL( p( "\"\\t\"" )->getString(), "\t" );
    BOOST_CHECK_EQUAL( p( "\"\\u1234\"" )->getString(), "\\u1234" );
}


BOOST_AUTO_TEST_CASE( test_numbers )
{
    // ints

    BOOST_CHECK_EQUAL( json::as<int>( *p( "1" ) ), 1 );
    BOOST_CHECK_EQUAL( json::as<int>( *p( "9" ) ), 9 );
    BOOST_CHECK_EQUAL( json::as<int>( *p( "10" ) ), 10 );

    // fractions
    BOOST_CHECK_EQUAL( p( "0.0" )->getDouble(), double( 0 ) );
    BOOST_CHECK_EQUAL( p( "0.1" )->getDouble(), double( 0.1 ) );
    BOOST_CHECK_EQUAL( p( "0.25" )->getDouble(), double( 0.25 ) );

    // negative ints
    BOOST_CHECK_EQUAL( json::as<int>( *p( "-1" ) ), -1 );

    // negative fraction
    BOOST_CHECK_EQUAL( p( "-0.5" )->getDouble(), double( -0.5 ) );

    // int exp
    BOOST_CHECK_EQUAL( p( "1e1" )->getDouble(), double( 10 ) );
    BOOST_CHECK_EQUAL( p( "1e+2" )->getDouble(), double( 100 ) );
    BOOST_CHECK_EQUAL( p( "5e-1" )->getDouble(), double( .5 ) );
    BOOST_CHECK_EQUAL( p( "25e-2" )->getDouble(), double( .25 ) );
    BOOST_CHECK_EQUAL( p( "10E3" )->getDouble(), double( 10000 ) );
    BOOST_CHECK_EQUAL( p( "2000E+2" )->getDouble(), double( 200000 ) );
    BOOST_CHECK_EQUAL( p( "10E-3" )->getDouble(), double( .01 ) );

    // int frac exp
    BOOST_CHECK_EQUAL( p( "1.5e1" )->getDouble(), double( 15 ) );
    BOOST_CHECK_EQUAL( p( "-123456789.456E-10" )->getDouble(), double( -0.0123456789456 ) );
}


BOOST_AUTO_TEST_CASE( test_boolean )
{
    {
        ValuePtr value_ptr(p("true"));
        BOOST_CHECK( value_ptr->isBool() );
        BOOST_CHECK_EQUAL( value_ptr->getBool(), true );
    }

    {
        ValuePtr value_ptr(p("false"));
        BOOST_CHECK( value_ptr->isBool() );
        BOOST_CHECK_EQUAL( value_ptr->getBool(), false );
    }
}


BOOST_AUTO_TEST_CASE( test_null )
{
    ValuePtr value_ptr(p("null"));
    BOOST_CHECK( value_ptr->isNull() );
}


BOOST_AUTO_TEST_CASE( test_empty_object )
{
    BOOST_CHECK( p( "{}" )->getObject().empty() );
}


BOOST_AUTO_TEST_CASE( test_empty_array )
{
    ValuePtr value_ptr(p("[]"));

    BOOST_CHECK( value_ptr->isArray() );
    BOOST_CHECK( value_ptr->getArray().empty() );
}


BOOST_AUTO_TEST_CASE( test_array_one )
{
    ValuePtr value_ptr(p( "[\"strelem0\"]" ));

    BOOST_CHECK_EQUAL( value_ptr->getArray().size(), unsigned(1) );
    BOOST_CHECK_EQUAL( value_ptr->getArray()[0]->getString(), "strelem0" );
}


BOOST_AUTO_TEST_CASE( test_array_multi )
{
    Array arr(p( "[\"strelem0\",1]" )->getArray());

    BOOST_CHECK_EQUAL( arr.size(), unsigned(2) );
    BOOST_CHECK_EQUAL( arr[0]->getString(), "strelem0" );
    BOOST_CHECK_EQUAL( json::as<int>( *arr[1] ), 1 );
}


BOOST_AUTO_TEST_CASE( test_no_end_ex )
{
    try {
        p( "{" );
    } catch ( const json::ParseError& pe ) {
        BOOST_CHECK_EQUAL( string(pe.what()), "invalid JSON string" );
        BOOST_CHECK_EQUAL( pe.getJsonText(), "{" );
        BOOST_CHECK_EQUAL( pe.getStop(), unsigned(0) );
        return;
    }
    BOOST_CHECK( false );
}


BOOST_AUTO_TEST_CASE( test_no_begin_ex )
{
    try {
        p( "}" );
    } catch ( const json::ParseError& pe ) {
        BOOST_CHECK_EQUAL( pe.getStop(), unsigned(0) );
        return;
    }
    BOOST_CHECK( false );
}


BOOST_AUTO_TEST_CASE( test_obj_string )
{
    Object obj(p( "{\"strname\":\"strvalue\"}" )->getObject());

    BOOST_CHECK( obj.contains( "strname" ) );
    BOOST_CHECK( obj.isString( "strname" ) );
    BOOST_CHECK_EQUAL( obj.getString( "strname" ), "strvalue" );
}


BOOST_AUTO_TEST_CASE( test_simple_number )
{
    Object obj(p( "{\"intname\":0}" )->getObject());

    BOOST_CHECK( obj.contains( "intname" ) );
    BOOST_CHECK( obj.is<int>( "intname" ) );
    BOOST_CHECK_EQUAL( obj.as<int>( "intname" ), 0 );
}


BOOST_AUTO_TEST_CASE( test_simple_array_empty )
{
    Object obj(p( "{\"a\":[]}" )->getObject());

    BOOST_CHECK( obj.contains( "a" ) );
    BOOST_CHECK( obj.isArray( "a" ) );
    BOOST_CHECK( obj.getArray( "a" ).empty() );
}


BOOST_AUTO_TEST_CASE( test_simple_array_one )
{
    Array a_arr(p( "{\"a\":[\"strelem0\"]}" )->getObject().getArray( "a" ));

    BOOST_CHECK_EQUAL( a_arr.size(), unsigned(1) );
    BOOST_CHECK_EQUAL( a_arr[0]->getString(), "strelem0" );
}


BOOST_AUTO_TEST_CASE( test_simple_array_multi )
{
    Array a_arr(p( "{\"a\":[\"strelem0\",1]}" )->getObject().getArray( "a" ));

    BOOST_CHECK_EQUAL( a_arr.size(), unsigned(2) );
    BOOST_CHECK_EQUAL( a_arr[0]->getString(), "strelem0" );
    BOOST_CHECK_EQUAL( json::as<int>( *a_arr[1] ), 1 );
}


BOOST_AUTO_TEST_CASE( test_simple_boolean_true )
{
    Object obj(p( "{\"a\":true}" )->getObject());

    BOOST_CHECK( obj.contains( "a" ) );
    BOOST_CHECK( obj.isBool( "a" ) );
    BOOST_CHECK( obj.getBool( "a" ) );
}


BOOST_AUTO_TEST_CASE( test_simple_null )
{
    Object obj(p( "{\"c\":null}" )->getObject());

    BOOST_CHECK( obj.contains( "c" ) );
    BOOST_CHECK( obj.isNull( "c" ) );
}


BOOST_AUTO_TEST_CASE( test_multi_member )
{
    Object obj(p( "{\"strname0\":\"strvalue0\",\"strname1\":\"strvalue1\"}" )->getObject());

    BOOST_CHECK( obj.contains( "strname0" ) );
    BOOST_CHECK_EQUAL( obj.getString( "strname0" ), "strvalue0" );

    BOOST_CHECK( obj.contains( "strname1" ) );
    BOOST_CHECK_EQUAL( obj.getString( "strname1" ), "strvalue1" );
}


BOOST_AUTO_TEST_CASE( test_object_with_object )
{
    Object obj(p( "{\"a\":{\"b\":\"c\"}}" )->getObject());

    BOOST_CHECK( obj.contains( "a" ) );
    BOOST_CHECK( obj.isObject( "a" ) );

    const Object &a_obj(obj.getObject( "a" ));

    BOOST_CHECK( a_obj.contains( "b" ) );
    BOOST_CHECK_EQUAL( a_obj.getString( "b" ), "c" );
}


BOOST_AUTO_TEST_CASE( test_object_with_empty_object )
{
    BOOST_CHECK( p( "{\"a\":{}}" )->getObject().getObject( "a" ).empty() );
}


BOOST_AUTO_TEST_CASE( test_array_empty_objects )
{
    Array arr(p( "[{},{}]" )->getArray());

    BOOST_REQUIRE_EQUAL( arr.size(), unsigned(2) );
    BOOST_CHECK( arr[0]->getObject().empty() );
    BOOST_CHECK( arr[1]->getObject().empty() );
}


BOOST_AUTO_TEST_CASE( test_array_objects )
{
    const Object obj(p( "{\"a\":[{\"b\":\"bval\",\"c\":[]},{\"d\":4}]}" )->getObject());

    const Array &a_arr(obj.getArray( "a" ));

    BOOST_REQUIRE_EQUAL( a_arr.size(), unsigned(2) );

    BOOST_CHECK( a_arr[0]->isObject() );

    const Object &a0_obj(a_arr[0]->getObject());

    BOOST_CHECK( a0_obj.contains( "b" ) );

    BOOST_CHECK_EQUAL( a0_obj.getString( "b" ), "bval" );

    BOOST_CHECK( a0_obj.getArray( "c" ).empty() );

    BOOST_CHECK( a_arr[1]->isObject() );

    const Object &a1_obj(a_arr[1]->getObject());

    BOOST_CHECK_EQUAL( a1_obj.as<int>( "d" ), 4 );
}


BOOST_AUTO_TEST_CASE( test_whitespace )
{
    BOOST_CHECK( p( " { } " )->getObject().empty() );
    BOOST_CHECK_EQUAL( p( " \" \" " )->getString(), " " );
    BOOST_CHECK_EQUAL( json::as<int>( *p( " 123 " ) ), 123 );
    BOOST_CHECK( p( " [ ] " )->getArray().empty() );
    BOOST_CHECK( p( " true " )->getBool() );
    BOOST_CHECK( ! p( " false " )->getBool() );
    BOOST_CHECK( p( " null " )->isNull() );

    BOOST_CHECK_EQUAL( p( "{ \"strname\" : \"strvalue\" }" )->getObject().size(), unsigned(1) );
    BOOST_CHECK_EQUAL( p( "{ \"strname\" : [ ] }" )->getObject().size(), unsigned(1) );

    BOOST_CHECK_EQUAL( p( "{ \"a\" : \"aval\", \"b\" : \"bval\" }" )->getObject().size(), unsigned(2) );

    BOOST_CHECK_EQUAL( p( "{ \"a\" : \n\t [ \"first\", 2, { \"3\": true }, [ 4 ], false, null ] }")->getObject().size(), unsigned(1) );
}
