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

#define BOOST_SPIRIT_THREADSAFE


#include "Parser.hpp"

#include "constants.hpp"
#include "json.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <boost/spirit/include/classic.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>


using boost::spirit::classic::ch_p;
using boost::spirit::classic::digit_p;
using boost::spirit::classic::hex_p;
using boost::spirit::classic::parse_info;
using boost::spirit::classic::range_p;
using boost::spirit::classic::space_p;
using boost::spirit::classic::str_p;

using boost::bind;

using std::cout;
using std::string;


namespace json {


Parser::Parser()
{
    _jstring_r =
        ch_p( quotationMark )[bind( &Parser::_strbegin, this, _1 )] >>
        *(
          (ch_p( reverseSolidus ) >>
            (ch_p( quotationMark )[bind( &Parser::_strchr, this, _1 )] |
             ch_p( reverseSolidus )[bind( &Parser::_strchr, this, _1)] |
             ch_p( '/' )[bind( &Parser::_strchr, this, _1)] |
             ch_p( 'b' )[bind( &Parser::_strbackspace, this, _1)] |
             ch_p( 'f' )[bind( &Parser::_strformfeed, this, _1)] |
             ch_p( 'n' )[bind( &Parser::_strnewline, this, _1)] |
             ch_p( 'r' )[bind( &Parser::_strcarriagereturn, this, _1)] |
             ch_p( 't' )[bind( &Parser::_strtab, this, _1)] |
             (ch_p( 'u' ) >> (hex_p >> hex_p >> hex_p >> hex_p)[bind( &Parser::_strunicode, this, _1, _2)])
            )
          ) |
          (~ch_p( quotationMark ))[bind( &Parser::_strchr, this, _1 )]
        ) >>
        ch_p( quotationMark )
        ;

    _digits_r =
        ch_p( '0' ) |
        (range_p( '1', '9' ) >> *digit_p)
        ;

    _dj_int_r =
        _digits_r |
        (ch_p( '-' ) >> _digits_r)
        ;

    _frac_r =
        ch_p( '.' ) >> digit_p >> *digit_p
        ;

    _exp_r =
        (str_p( "e+" ) |
         str_p( "e-" ) |
         ch_p( 'e' ) |
         str_p( "E+" ) |
         str_p( "E-" ) |
         ch_p( 'E' )
        ) >>
        _digits_r
        ;

    _number_r =
        (_dj_int_r >> _frac_r >> _exp_r) |
        (_dj_int_r >> _frac_r) |
        (_dj_int_r >> _exp_r) |
        _dj_int_r
        ;

    _empty_array_r =
        ch_p( beginArray ) >> *space_p >> ch_p( endArray )
        ;

    _array_r =
        _empty_array_r[bind( &Parser::_emptyArray, this, _1, _2 )] |
        (ch_p( beginArray )[bind( &Parser::_arrayStart, this, _1 )] >>
         _value_r >> *(ch_p(valueSeparator) >> _value_r) >>
         ch_p( endArray )[bind( &Parser::_containerEnd, this, _1 )]
        )
        ;

    _value_r =
        *space_p >>
        (_jstring_r[bind( &Parser::_string, this, _1, _2 )] |
         _number_r[bind( &Parser::_number, this, _1, _2 )] |
         _object_r |
         _array_r |
         str_p( trueString.c_str() )[bind( &Parser::_booleanTrue, this, _1, _2 )] |
         str_p( falseString.c_str() )[bind( &Parser::_booleanFalse, this, _1, _2 )] |
         str_p( nullString.c_str() )[bind( &Parser::_null, this, _1, _2 )]) >>
        *space_p
        ;

    _pair_r =
        *space_p >>
        _jstring_r[bind( &Parser::_memberName, this, _1, _2 )] >>
        *space_p >> ch_p( nameSeparator ) >>
        _value_r
        ;

    _empty_object_r =
        ch_p( beginObject ) >> *space_p >> ch_p( endObject )
        ;

    _object_r =
        _empty_object_r[bind( &Parser::_emptyObject, this, _1, _2 )] |
        (ch_p( beginObject )[bind( &Parser::_objectStart, this, _1 )] >>
         _pair_r >> *(ch_p( valueSeparator ) >> _pair_r) >>
         ch_p( endObject )[bind( &Parser::_containerEnd, this, _1 )]
        )
        ;
}


ValuePtr Parser::operator()( const std::string& str )
{
    _cur_value_ptr = Undefined;
    _containers = _ContainerStack();

    parse_info<const char*> p_info(boost::spirit::classic::parse( str.c_str(), *space_p >> _value_r >> *space_p ));

    if ( ! p_info.full ) {
        // Failed to parse the full document.

        BOOST_THROW_EXCEPTION( ParseError( str, p_info.stop - str.c_str() ) );
    }

    return _cur_value_ptr;
}


void Parser::_strunicode( const char *str, const char *end )
{
    _cur_str += string( str, end ).c_str();
}


void Parser::_memberName( const char */*str*/, const char */*end*/ )
{
    _member_name = string(_cur_str.begin(), _cur_str.end());
}


void Parser::_string( const char */*str*/, const char */*end*/ )
{
    // cout << "String value=\"" << _member_name << "\": \"" << sval << "\"\n";

    _putValue( create( string( _cur_str.begin(), _cur_str.end() ) ) );
}


void Parser::_number( const char *str, const char *end )
{
    string num_str( str, end );

    // cout << "Number=\"" << _member_name << "\": " << num_str << "\n";

    _putValue( create( boost::lexical_cast<double>( num_str ) ) );
}


void Parser::_emptyObject( const char */*str*/, const char */*end*/ )
{
    _putValue( Object::create() );
}


void Parser::_objectStart( char /*c*/ )
{
    _containers.push( _Pair( _member_name, Object::create() ) );
}


void Parser::_emptyArray( const char */*str*/, const char */*end*/ )
{
    _putValue( Array::create() );
}


void Parser::_arrayStart( char /*c*/ )
{
    // Push an empty array onto the _containers.
    _containers.push( _Pair( _member_name, Array::create() ) );
}


void Parser::_containerEnd( char /*c*/ )
{
    _Pair pair(_containers.top());
    _containers.pop();

    _member_name = pair.first;

    _putValue( pair.second );
}


void Parser::_booleanTrue( const char */*str*/, const char */*end*/ )
{
    _putValue( create( true ) );
}


void Parser::_booleanFalse( const char */*str*/, const char */*end*/ )
{
    _putValue( create( false ) );
}


void Parser::_null( const char */*str*/, const char */*end*/ )
{
    _putValue( createNull() );
}


void Parser::_putValue( ValuePtr ptr )
{
    // If containers is empty then there is no container for the value.
    if ( _containers.empty() ) {
        _cur_value_ptr = ptr;
        return;
    }

    // Container stack isn't empty, put the new member to the container at the top of the containers stack.

    ValuePtr &cur_container_ptr(_containers.top().second);

    // The container is either an array or an object.

    if ( cur_container_ptr->isArray() ) {
        // It's an array.
        ArrayValue &cur_arr(dynamic_cast<ArrayValue&>(*cur_container_ptr));
        cur_arr.get().push_back( ptr );
        return;
    }

    // It's an object

    ObjectValue &cur_obj(dynamic_cast<ObjectValue&>(*cur_container_ptr));
    cur_obj.get().set( _member_name, ptr );
}


} // namespace json
