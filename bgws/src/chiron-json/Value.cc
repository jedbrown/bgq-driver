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

#include "Value.hpp"

#include "exception.hpp"
#include "value_types.hpp"

#include <boost/throw_exception.hpp>

#include <stdexcept>


using std::runtime_error;
using std::string;


namespace json {


const ValuePtr Undefined( new UndefinedValue );


//---------------------------------------------------------------------
// class Value


const std::string& Value::getString() const
{
    BOOST_THROW_EXCEPTION( WrongType( _getTypeName(), "String" ) );

    static const string string_to_return;
    return string_to_return;
}


double Value::getDouble() const
{
    BOOST_THROW_EXCEPTION( WrongType( _getTypeName(), "Number" ) );

    return 0.0;
}


const Object& Value::getObject() const
{
    BOOST_THROW_EXCEPTION( WrongType( _getTypeName(), "Object" ) );

    static const Object object_to_return;
    return object_to_return;
}


Object& Value::getObject()
{
    BOOST_THROW_EXCEPTION( WrongType( _getTypeName(), "Object" ) );

    static Object object_to_return;
    return object_to_return;
}


const Array& Value::getArray() const
{
    BOOST_THROW_EXCEPTION( WrongType( _getTypeName(), "Array" ) );

    static const Array array_to_return;
    return array_to_return;
}


Array& Value::getArray()
{
    BOOST_THROW_EXCEPTION( WrongType( _getTypeName(), "Array" ) );

    static Array array_to_return;
    return array_to_return;
}


bool Value::getBool() const
{
    BOOST_THROW_EXCEPTION( WrongType( _getTypeName(), "Bool" ) );
    return false;
}


std::ostream& operator<<( std::ostream& os, const Value& v )
{
    v.write( os );
    return os;
}


} // namespace json
