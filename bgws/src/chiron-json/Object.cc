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

#include "Object.hpp"

#include "Array.hpp"
#include "exception.hpp"
#include "functions.hpp"
#include "Value.hpp"
#include "value_types.hpp"

#include <boost/throw_exception.hpp>


namespace json {


ObjectValuePtr Object::create( const Object& other )
{
    return ObjectValuePtr( new ObjectValue( other ) );
}


const ValuePtr& Object::_get( const std::string& name, bool throw_if_not_found ) const
{
    const_iterator i(find( name ));
    if ( i == end() ) {
        // The member didn't exist.

        if ( throw_if_not_found ) {
            BOOST_THROW_EXCEPTION( MemberNotFound( name ) );
        }
        return Undefined;
    }
    return find( name )->second;
}


void Object::setNull( const std::string& name )
{
    set( name, json::createNull() );
}


Object& Object::createObject( const std::string& name )
{
    ObjectValuePtr obj_ptr(create());

    set( name, ValuePtr(obj_ptr) );

    return obj_ptr->get();
}


Array& Object::createArray( const std::string& name )
{
    ArrayValuePtr arr_ptr(Array::create());

    set( name, ValuePtr(arr_ptr) );

    return arr_ptr->get();
}


bool Object::contains( const std::string& name ) const
{
    return (find( name ) != end());
}


bool Object::isString( const std::string& name ) const
{
    return get( name )->isString();
}


bool Object::isNumber( const std::string& name ) const
{
    return get( name )->isNumber();
}


bool Object::isObject( const std::string& name ) const
{
    return get( name )->isObject();
}


bool Object::isArray( const std::string& name ) const
{
    return get( name )->isArray();
}


bool Object::isBool( const std::string& name ) const
{
    return get( name )->isBool();
}


bool Object::isNull( const std::string& name ) const
{
    return get( name )->isNull();
}


const std::string& Object::getString( const std::string& name ) const
{
    try {
        return _get( name, true )->getString();
    } catch ( WrongType& e ) {
        e.setMemberName( name );
        throw e;
    }
}


double Object::getDouble( const std::string& name ) const
{
    try {
        return _get( name, true )->getDouble();
    } catch ( WrongType& e ) {
        e.setMemberName( name );
        throw e;
    }
}


const Object& Object::getObject( const std::string& name ) const
{
    try {
        return _get( name, true )->getObject();
    } catch ( WrongType& e ) {
        e.setMemberName( name );
        throw e;
    }
}


Object& Object::getObject( const std::string& name )
{
    try {
        return _get( name, true )->getObject();
    } catch ( WrongType& e ) {
        e.setMemberName( name );
        throw e;
    }
}


const Array& Object::getArray( const std::string& name ) const
{
    try {
        return _get( name, true )->getArray();
    } catch ( WrongType& e ) {
        e.setMemberName( name );
        throw e;
    }
}


Array& Object::getArray( const std::string& name )
{
    try {
        return _get( name, true )->getArray();
    } catch ( WrongType& e ) {
        e.setMemberName( name );
        throw e;
    }
}


bool Object::getBool( const std::string& name ) const
{
    try {
        return _get( name, true )->getBool();
    } catch ( WrongType& e ) {
        e.setMemberName( name );
        throw e;
    }
}


void Object::set( const std::string& name, ValuePtr value_ptr )
{
    iterator i(find( name ));
    if ( i == end() ) {
        insert( std::make_pair( name, value_ptr ) );
    } else {
        i->second = value_ptr;
    }
}


ValuePtr Object::get( const std::string& name )
{
    iterator i(find( name ));
    if ( i == end() ) {
        // The member didn't exist.
        return Undefined;
    }
    return i->second;
}


const ValuePtr& Object::get( const std::string& name ) const
{
    return _get( name, false );
}

} // namespace json
