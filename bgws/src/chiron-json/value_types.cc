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

#include "value_types.hpp"

#include "constants.hpp"

#include <iostream>


using std::string;


namespace json {


//---------------------------------------------------------------------
// class NumberValue


const std::string NumberValue::_TYPE_NAME( "Number" );


void NumberValue::write( std::ostream& os ) const
{
    if ( _which == 0 ) { os << _val_dbl; }
    else if ( _which == 1 ) { os << _val_i; }
    else { os << _val_u; }
}


//---------------------------------------------------------------------
// class ObjectValue

const std::string ObjectValue::_TYPE_NAME( "Object" );


void ObjectValue::write( std::ostream& os ) const
{
    os << beginObject;

    for ( Object::const_iterator i(_value.begin()) ; i != _value.end() ; ++i ) {
        if ( i != _value.begin() ) {
            os << valueSeparator;
        }
        os << quotationMark << i->first << quotationMark << nameSeparator;
        if ( i->second ) {
            os << (*i->second);
        } else {
            os << nullString;
        }
    }

    os << endObject;
}


//---------------------------------------------------------------------
// class StringValue

const std::string StringValue::_TYPE_NAME("String");


void StringValue::write( std::ostream& os ) const
{
    os << quotationMark;
    for ( string::const_iterator i(_value.begin()) ; i != _value.end() ; ++i ) {
        switch ( *i ) {
        case quotationMark:
        case reverseSolidus:
        case '/':
            os << reverseSolidus << *i;
            break;
        case '\b':
            os << reverseSolidus << 'b';
            break;
        case '\f':
            os << reverseSolidus << 'f';
            break;
        case '\n':
            os << reverseSolidus << 'n';
            break;
        case '\r':
            os << reverseSolidus << 'r';
            break;
        case '\t':
            os << reverseSolidus << 't';
            break;
        default:
            os << *i;
        }
    }
    os << quotationMark;
}


//---------------------------------------------------------------------
// class ArrayValue

const std::string ArrayValue::_TYPE_NAME( "Array" );


void ArrayValue::write( std::ostream& os ) const
{
    os << beginArray;

    for ( Array::const_iterator i(_value.begin()) ; i != _value.end() ; ++i ) {
        if ( i != _value.begin() ) {
            os << valueSeparator;
        }
        if ( *i ) {
            os << **i;
        } else {
            os << nullString;
        }
    }

    os << endArray;
}


//---------------------------------------------------------------------
// class BoolValue

const std::string BoolValue::_TYPE_NAME( "Boolean" );

const ValuePtr BoolValue::True(new BoolValue(true));
const ValuePtr BoolValue::False(new BoolValue(false));


void BoolValue::write( std::ostream& os ) const
{
    os << (_value ? trueString : falseString);
}


//---------------------------------------------------------------------
// class NullValue

const std::string NullValue::_TYPE_NAME( "NullValue" );


const ValuePtr NullValue::Null( new NullValue );


void NullValue::write( std::ostream& os ) const
{
    os << nullString;
}


//---------------------------------------------------------------------
// class UndefinedValue

const std::string UndefinedValue::_TYPE_NAME( "Undefined" );


} // namespace json
