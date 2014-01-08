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

#ifndef JSON_FUNCTIONS_HPP_
#define JSON_FUNCTIONS_HPP_


#include "exception.hpp"
#include "fwd.hpp"
#include "Value.hpp"

#include "value_types.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <sstream>


namespace json {


/*! \brief Create a null value. */
ValuePtr createNull();


template <class T>
class ValueCreator
{
public:
    static ValuePtr create( const T& value ) {
        return ValuePtr( new NumberValue( value ) );
    }

    static ValuePtr create( const char* value_p ) {
        if ( ! value_p ) {
            return createNull();
        }
        return ValuePtr( new StringValue( value_p ) );
    }

};


template <>
class ValueCreator<std::string>
{
public:
    static ValuePtr create( const std::string& value ) {
        return ValuePtr( new StringValue( value ) );
    }
};


template <>
class ValueCreator<const char*>
{
public:
    static ValuePtr create( const char* value_p ) {
        if ( ! value_p ) {
            return createNull();
        }
        return ValuePtr( new StringValue( value_p ) );
    }
};


template<>
class ValueCreator<bool>
{
public:
    static ValuePtr create( const bool& value ) {
        return ValuePtr( new BoolValue( value ) );
    }
};


template<>
class ValueCreator<Object>
{
public:
    static ValuePtr create( const Object& value ) {
        return ValuePtr( new ObjectValue( value ) );
    }
};


template<>
class ValueCreator<Array>
{
public:
    static ValuePtr create( const Array& value ) {
        return ValuePtr( new ArrayValue( value ) );
    }
};


template<>
class ValueCreator<boost::posix_time::ptime>
{
public:
    static ValuePtr create( const boost::posix_time::ptime& t ) {
        std::ostringstream oss;
        oss.imbue( std::locale( oss.getloc(), new boost::posix_time::time_facet( "%Y-%m-%dT%H:%M:%S.%f" ) ) );
        oss << t;
        return ValuePtr( new StringValue( oss.str() ) );
    }
};


/*! \brief This is how you create a value. */
template <class T> inline ValuePtr create( const T& value )
{
    return ValueCreator<T>::create( value );
}


/*! \brief This is how you create a value. */
template <class T> inline ValuePtr create( const T* value_p )
{
    return ValueCreator<T*>::create( value_p );
}


template <class T> inline bool is( const Value& v )
{
    const NumberValue *n_p(dynamic_cast<const NumberValue*>( &v ));
    if ( ! n_p )  return false;
    return n_p->is<T>();
}


template <class T> inline T as( const Value& v )
{
    const NumberValue *n_p(dynamic_cast<const NumberValue*>( &v ));
    if ( ! n_p ) {
        BOOST_THROW_EXCEPTION( WrongType( "not a number", "number" ) );
    }
    return n_p->as<T>();
}


} // namespace json

#endif
