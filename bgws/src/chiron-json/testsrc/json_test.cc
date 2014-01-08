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

#include <boost/lexical_cast.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE json
#include <boost/test/unit_test.hpp>


using boost::lexical_cast;

using json::ValuePtr;

using std::cout;
using std::ostringstream;
using std::runtime_error;
using std::string;


BOOST_AUTO_TEST_CASE( test_string_value )
{
    ValuePtr v_ptr(json::create( string("strval") ));

    BOOST_CHECK( v_ptr->isString() );
    BOOST_CHECK( ! v_ptr->isNumber() );
    BOOST_CHECK( ! v_ptr->isObject() );
    BOOST_CHECK( ! v_ptr->isArray() );
    BOOST_CHECK( ! v_ptr->isBool() );
    BOOST_CHECK( ! v_ptr->isNull() );
    BOOST_CHECK( ! json::is<int>( *v_ptr ) );

    BOOST_CHECK_EQUAL( v_ptr->getString(), "strval" );

    BOOST_CHECK_THROW( v_ptr->getDouble(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getObject(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getArray(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getBool(), json::WrongType );
    BOOST_CHECK_THROW( json::as<int>( *v_ptr ), json::WrongType );

    ostringstream oss;
    v_ptr->write( oss );
    BOOST_CHECK_EQUAL( oss.str(), "\"strval\"" );
}


BOOST_AUTO_TEST_CASE( test_number_value_double )
{
    ValuePtr v_ptr(json::create( 1.5 ));

    BOOST_CHECK( ! v_ptr->isString() );
    BOOST_CHECK( v_ptr->isNumber() );
    BOOST_CHECK( ! v_ptr->isObject() );
    BOOST_CHECK( ! v_ptr->isArray() );
    BOOST_CHECK( ! v_ptr->isBool() );
    BOOST_CHECK( ! v_ptr->isNull() );

    BOOST_CHECK( ! json::is<int>( *v_ptr ) );
    BOOST_CHECK( json::is<double>( *v_ptr ) );

    BOOST_CHECK_EQUAL( v_ptr->getDouble(), 1.5 );

    BOOST_CHECK_THROW( v_ptr->getString(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getObject(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getArray(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getBool(), json::WrongType );
    BOOST_CHECK_THROW( json::as<int>( *v_ptr ), json::WrongType );

    ostringstream oss;
    v_ptr->write( oss );
    BOOST_CHECK_EQUAL( oss.str(), "1.5" );
}


BOOST_AUTO_TEST_CASE( test_number_value_int )
{
    ValuePtr v_ptr(json::create( 1 ));

    BOOST_CHECK( v_ptr->isNumber() );
    BOOST_CHECK( json::is<int>( *v_ptr ) );

    BOOST_CHECK_EQUAL( v_ptr->getDouble(), double( 1.0 ) );

    BOOST_CHECK_THROW( v_ptr->getString(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getObject(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getArray(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getBool(), json::WrongType );

    ostringstream oss;
    v_ptr->write( oss );
    BOOST_CHECK_EQUAL( oss.str(), "1" );
}


BOOST_AUTO_TEST_CASE( test_number_value_UL )
{
    ValuePtr v_ptr(json::create( 1234567UL ));

    BOOST_CHECK( v_ptr->isNumber() );
    BOOST_CHECK( json::is<uint64_t>( *v_ptr ) );

    BOOST_CHECK_EQUAL( v_ptr->getDouble(), double( 1234567.0 ) );

    BOOST_CHECK_THROW( v_ptr->getString(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getObject(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getArray(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getBool(), json::WrongType );

    ostringstream oss;
    v_ptr->write( oss );
    BOOST_CHECK_EQUAL( oss.str(), "1234567" );
}


BOOST_AUTO_TEST_CASE( test_number_value_U )
{
    ValuePtr v_ptr(json::create( unsigned(1234567) ));

    BOOST_CHECK( v_ptr->isNumber() );
    BOOST_CHECK( json::is<uint64_t>( *v_ptr ) );

    BOOST_CHECK_EQUAL( v_ptr->getDouble(), double( 1234567.0 ) );

    BOOST_CHECK_THROW( v_ptr->getString(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getObject(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getArray(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getBool(), json::WrongType );

    ostringstream oss;
    v_ptr->write( oss );
    BOOST_CHECK_EQUAL( oss.str(), "1234567" );
}


BOOST_AUTO_TEST_CASE( test_convert )
{
    // if create value with 1.0, then can get as int.
    BOOST_CHECK( json::is<int>( *json::create( double( 1.0 ) ) ) );
    BOOST_CHECK_EQUAL( json::as<int>( *json::create( double( 1.0 ) ) ), 1 );
    BOOST_CHECK( json::is<unsigned>( *json::create( double( 1.0 ) ) ) );
    BOOST_CHECK_EQUAL( json::as<unsigned>( *json::create( double( 1.0 ) ) ), 1U );
    BOOST_CHECK( json::is<uint32_t>( *json::create( double( 1.0 ) ) ) );
    BOOST_CHECK_EQUAL( json::as<uint32_t>( *json::create( double( 1.0 ) ) ), uint32_t(1) );
    BOOST_CHECK( json::is<double>( *json::create( double( 1.0 ) ) ) );
    BOOST_CHECK_EQUAL( json::as<double>( *json::create( double( 1.0 ) ) ), 1.0 );

    // What if it's negative?
    BOOST_CHECK( json::is<int>( *json::create( double( -1.0 ) ) ) );
    BOOST_CHECK_EQUAL( json::as<int>( *json::create( double( -1.0 ) ) ), -1 );
    BOOST_CHECK( ! json::is<unsigned>( *json::create( double( -1.0 ) ) ) );
    BOOST_CHECK_THROW( json::as<unsigned>( *json::create( double( -1.0 ) ) ), json::WrongType );

    // What if it's too big?
    BOOST_CHECK( json::is<int16_t>( *json::create( double( 32767 ) ) ) );
    BOOST_CHECK_EQUAL( json::as<int16_t>( *json::create( double( 32767 ) ) ), 32767 );
    BOOST_CHECK( ! json::is<int16_t>( *json::create( double( 32768 ) ) ) );
    BOOST_CHECK_THROW( json::as<int16_t>( *json::create( double( 32768 ) ) ), json::WrongType );
}


BOOST_AUTO_TEST_CASE( test_object_value )
{
    ValuePtr v_ptr(json::Object::create());

    BOOST_CHECK( ! v_ptr->isString() );
    BOOST_CHECK( ! v_ptr->isNumber() );
    BOOST_CHECK( v_ptr->isObject() );
    BOOST_CHECK( ! v_ptr->isArray() );
    BOOST_CHECK( ! v_ptr->isBool() );
    BOOST_CHECK( ! v_ptr->isNull() );
    BOOST_CHECK( ! json::is<int>( *v_ptr ) );

    BOOST_CHECK( v_ptr->getObject().empty() );

    BOOST_CHECK_THROW( v_ptr->getString(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getDouble(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getArray(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getBool(), json::WrongType );
    BOOST_CHECK_THROW( json::as<int>( *v_ptr ), json::WrongType );

    ostringstream oss;
    v_ptr->write( oss );
    BOOST_CHECK_EQUAL( oss.str(), "{}" );
}


BOOST_AUTO_TEST_CASE( test_object_value_object_param )
{
    json::Object obj;
    obj.set( "a", "aval" );

    ValuePtr v_ptr(json::create( obj ));

    BOOST_CHECK( v_ptr->isObject() );
}


BOOST_AUTO_TEST_CASE( test_array_value )
{
    ValuePtr v_ptr(json::create( json::Array() ));

    BOOST_CHECK( ! v_ptr->isString() );
    BOOST_CHECK( ! v_ptr->isNumber() );
    BOOST_CHECK( ! v_ptr->isObject() );
    BOOST_CHECK( v_ptr->isArray() );
    BOOST_CHECK( ! v_ptr->isBool() );
    BOOST_CHECK( ! v_ptr->isNull() );
    BOOST_CHECK( ! json::is<int>( *v_ptr ) );

    BOOST_CHECK( v_ptr->getArray().empty() );

    BOOST_CHECK_THROW( v_ptr->getString(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getDouble(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getObject(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getBool(), json::WrongType );
    BOOST_CHECK_THROW( json::as<int>( *v_ptr ), json::WrongType );

    ostringstream oss;
    v_ptr->write( oss );
    BOOST_CHECK_EQUAL( oss.str(), "[]" );
}


BOOST_AUTO_TEST_CASE( test_bool_value )
{
    ValuePtr v_ptr(json::create( true ));

    BOOST_CHECK( ! v_ptr->isString() );
    BOOST_CHECK( ! v_ptr->isNumber() );
    BOOST_CHECK( ! v_ptr->isObject() );
    BOOST_CHECK( ! v_ptr->isArray() );
    BOOST_CHECK( v_ptr->isBool() );
    BOOST_CHECK( ! v_ptr->isNull() );
    BOOST_CHECK( ! json::is<int>( *v_ptr ) );

    BOOST_CHECK( v_ptr->getBool() );

    BOOST_CHECK_THROW( v_ptr->getString(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getDouble(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getObject(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getArray(), json::WrongType );
    BOOST_CHECK_THROW( json::as<int>( *v_ptr ), json::WrongType );

    ostringstream oss;
    v_ptr->write( oss );
    BOOST_CHECK_EQUAL( oss.str(), "true" );
}


BOOST_AUTO_TEST_CASE( test_bool_value_false )
{
    ValuePtr v_ptr(json::create( false ));

    BOOST_CHECK( v_ptr->isBool() );

    BOOST_CHECK( ! v_ptr->getBool() );

    ostringstream oss;
    v_ptr->write( oss );
    BOOST_CHECK_EQUAL( oss.str(), "false" );
}


BOOST_AUTO_TEST_CASE( test_null_value )
{
    ValuePtr v_ptr(json::createNull());

    BOOST_CHECK( ! v_ptr->isString() );
    BOOST_CHECK( ! v_ptr->isNumber() );
    BOOST_CHECK( ! v_ptr->isObject() );
    BOOST_CHECK( ! v_ptr->isArray() );
    BOOST_CHECK( ! v_ptr->isBool() );
    BOOST_CHECK( v_ptr->isNull() );
    BOOST_CHECK( ! json::is<int>( *v_ptr ) );

    BOOST_CHECK_THROW( v_ptr->getString(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getDouble(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getObject(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getArray(), json::WrongType );
    BOOST_CHECK_THROW( v_ptr->getBool(), json::WrongType );
    BOOST_CHECK_THROW( json::as<int>( *v_ptr ), json::WrongType );

    ostringstream oss;
    v_ptr->write( oss );
    BOOST_CHECK_EQUAL( oss.str(), "null" );
}


BOOST_AUTO_TEST_CASE( test_string_escapes )
{
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( (const char*)("ab\"c") ) ),  "\"ab\\\"c\"" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( "ab\\c" ) ),  "\"ab\\\\c\"" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( "ab/c" ) ),  "\"ab\\/c\"" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( "ab\bc" ) ),  "\"ab\\bc\"" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( "ab\fc" ) ),  "\"ab\\fc\"" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( "ab\nc" ) ),  "\"ab\\nc\"" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( "ab\rc" ) ),  "\"ab\\rc\"" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( "ab\tc" ) ),  "\"ab\\tc\"" );
}


BOOST_AUTO_TEST_CASE( test_build_object )
{
    json::Object obj;

    obj.set( "a", "aval" );
    obj.set( "d", 1 );
    obj.set( "e", true );
    obj.setNull( "f" );

    json::Object &b_obj(obj.createObject( "b" ));
    b_obj.set( "b1", "b1val" );

    json::Array &c_arr(obj.createArray( "c" ));
    c_arr.add( "c0" );

    BOOST_CHECK( obj.contains( "a" ) );
    BOOST_CHECK_EQUAL( obj.getString( "a" ), "aval" );

    BOOST_CHECK( obj.contains( "d" ) );
    BOOST_CHECK_EQUAL( obj.getDouble( "d" ), double(1.0) );

    BOOST_CHECK( obj.contains( "e" ) );
    BOOST_CHECK_EQUAL( obj.getBool( "e" ), true );

    BOOST_CHECK( obj.contains( "f" ) );
    BOOST_CHECK( obj.isNull( "f" ) );

    BOOST_CHECK( obj.contains( "b" ) );
    BOOST_CHECK( obj.isObject( "b" ) );
    BOOST_CHECK_EQUAL( obj.getObject( "b" ).getString( "b1" ), "b1val" );

    BOOST_CHECK( obj.contains( "c" ) );
    BOOST_CHECK( obj.isArray( "c" ) );
    BOOST_CHECK_EQUAL( obj.getArray( "c" )[0]->getString(), "c0" );
}


BOOST_AUTO_TEST_CASE( test_get_member_no_member )
{
    json::Object obj;

    BOOST_CHECK_EQUAL( obj.get( "a" ), json::Undefined );
    BOOST_CHECK_THROW( obj.getString( "a" ), json::MemberNotFound );
}


BOOST_AUTO_TEST_CASE( test_get_member_wrong_type )
{
    json::Object obj;
    obj.set( "a", "aval" );

    BOOST_CHECK_THROW( obj.as<int>( "a" ), json::WrongType );
}


BOOST_AUTO_TEST_CASE( test_set_again )
{
    json::Object obj;

    obj.set( "a", "aval1" );
    obj.set( "a", "aval2" );

    BOOST_CHECK_EQUAL( obj.getString( "a" ), "aval2" );
}


BOOST_AUTO_TEST_CASE( test_write_simple )
{
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( "strval" ) ), "\"strval\"" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( double( 1 ) ) ), "1" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( true ) ), "true" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( false ) ), "false" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::createNull() ), "null" );
}


BOOST_AUTO_TEST_CASE( test_write_array )
{
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::Array::create() ), "[]" );

    {
        json::Array arr;
        arr.add( 1 );

        BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( arr ) ), "[1]" );
    }

    {
        json::Array arr;
        arr.add( 1 );
        arr.add( "string" );
        arr.add( true );

        BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( arr ) ), "[1,\"string\",true]" );
    }
}


BOOST_AUTO_TEST_CASE( test_write_object )
{
    BOOST_CHECK_EQUAL( lexical_cast<string>( *json::Object::create() ), "{}" );

    {
        json::Object obj;
        obj.set( "a", "aval" );

        BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( obj ) ), "{\"a\":\"aval\"}" );
    }

    {
        json::Object obj;
        obj.set( "a", "aval" );

        json::Object &b_obj(obj.createObject( "b" ));
        json::Array &c_arr(b_obj.createArray( "c" ));

        c_arr.add( json::Object() );

        json::Object &c1_obj(c_arr.addObject());
        c1_obj.set( "d", true );

        BOOST_CHECK_EQUAL( lexical_cast<string>( *json::create( obj ) ), "{\"a\":\"aval\",\"b\":{\"c\":[{},{\"d\":true}]}}" );
    }
}


BOOST_AUTO_TEST_CASE( test_create_using_timestamp )
{
    // Create with a ptime generates a string with ISO string timestamp.

    ValuePtr val_ptr(json::create( boost::posix_time::ptime( boost::gregorian::date( 2010, 6, 8 ), boost::posix_time::time_duration( 9, 10, 11 ) ) ));

    BOOST_CHECK( val_ptr->isString() );
    BOOST_CHECK_EQUAL( val_ptr->getString(), "2010-06-08T09:10:11.000000" );
}
