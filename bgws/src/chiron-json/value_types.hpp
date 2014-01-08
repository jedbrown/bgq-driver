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

#ifndef JSON_VALUE_TYPES_HPP_
#define JSON_VALUE_TYPES_HPP_


#include "Array.hpp"
#include "exception.hpp"
#include "Object.hpp"
#include "Value.hpp"

#include <boost/throw_exception.hpp>

#include <boost/numeric/conversion/converter.hpp>

#include <stdint.h>


namespace json {


/*! \brief Represents a JSON string. */
class StringValue : public Value
{
public:
    StringValue( const std::string value ) : _value( value )  { /* Nothing to do */ }

    const std::string& get() const  { return _value; }

    bool isString() const  { return true; }

    const std::string& getString() const  { return _value; }

    void write( std::ostream& os ) const;

protected:
    static const std::string _TYPE_NAME;

    const std::string& _getTypeName() const  { return _TYPE_NAME; }

private:
    std::string _value;
};


// numeric_cast<int>( 1.5 ) should throw an exception rather than rounding,
// (numeric_cast<int>(1) should work)
// So I'll implement my own numeric cast with a different rounder policy,
// it's just like boost::numeric_cast but it throws rather than rounding.

template<class S>
struct Float2IntRounderPolicy
{
    typedef S               source_type ;
    typedef S const&        argument_type ;

    static source_type nearbyint ( argument_type s ) {
        if ( std::floor(s) != s ) {
            throw std::bad_cast();
        }
        return std::floor(s);
    }

    typedef boost::mpl::integral_c<std::float_round_style,std::round_toward_infinity> round_style;
} ;


template<typename Target, typename Source> inline
typename boost::numeric::converter<
        Target,
        Source,
        boost::numeric::conversion_traits<Target,Source>,
        boost::numeric::def_overflow_handler,
        Float2IntRounderPolicy< Source >
    >::result_type
numeric_cast ( Source arg )
{
    return boost::numeric::converter<
            Target,
            Source,
            boost::numeric::conversion_traits<Target,Source>,
            boost::numeric::def_overflow_handler,
            Float2IntRounderPolicy< Source >
        >::convert(arg);
}


/*! \brief Represents a JSON number. */
class NumberValue : public Value
{
public:
    NumberValue( double value ) : _which(0), _val_dbl(value)  { /* Nothing to do */ }
    NumberValue( int value ) : _which(1), _val_dbl( value ), _val_i( value )  { /* Nothing to do */ }
    NumberValue( int64_t value ) : _which(1), _val_dbl( value ), _val_i( value )  { /* Nothing to do */ }
    NumberValue( unsigned value ) : _which(2), _val_dbl( value ), _val_u( value )  { /* Nothing to do */ }
    NumberValue( uint64_t value ) : _which(2), _val_dbl( value ), _val_u( value )  { /* Nothing to do */ }

    double get() const  { return _val_dbl; }

    template <class T> bool is() const {
        try {
            numeric_cast<T>( _val_dbl );
            return true;
        } catch ( std::bad_cast& e ) {
            // ignore this error.
        }
        return false;
    }

    template <class T> T as() const {
        try {
            return numeric_cast<T>( _val_dbl );
        } catch ( std::bad_cast& e ) {
            BOOST_THROW_EXCEPTION( WrongType( _getTypeName(), "supplied type" ) );
            return T();
        }
    }

    bool isNumber() const  { return true; }

    double getDouble() const  { return _val_dbl; }

    void write( std::ostream& os ) const;

protected:
    static const std::string _TYPE_NAME;

    const std::string& _getTypeName() const  { return _TYPE_NAME; }

private:

    int _which;

    double _val_dbl;
    int64_t _val_i;
    uint64_t _val_u;
};


/*! \brief Represents a JSON object value. */
class ObjectValue : public Value
{
public:
    ObjectValue() { /* Nothing to do */ }

    ObjectValue( const Object& o ) : _value( o )  {}

    const Object& get() const  { return _value; }

    Object& get()  { return _value; }

    bool isObject() const  { return true; }

    const Object& getObject() const  { return _value; }

    Object& getObject()  { return _value; }

    void write( std::ostream& os ) const;

protected:
    static const std::string _TYPE_NAME;

    const std::string& _getTypeName() const  { return _TYPE_NAME; }

private:
    Object _value;
};


/*! \brief Represents a JSON array value. */
class ArrayValue : public Value
{
public:

    ArrayValue()  { /* Nothing to do */ }

    ArrayValue( const Array& arr ) : _value( arr )  { /* Nothing to do */ }

    const Array& get() const  { return _value; }

    Array& get()  { return _value; }

    bool isArray() const  { return true; }

    const Array& getArray() const  { return _value; }

    Array& getArray()  { return _value; }

    void write( std::ostream& os ) const;

protected:
    static const std::string _TYPE_NAME;

    const std::string& _getTypeName() const  { return _TYPE_NAME; }

private:
    Array _value;
};


/*! \brief Represents a JSON Boolean. */
class BoolValue : public Value
{
public:

    static const ValuePtr True;
    static const ValuePtr False;


    BoolValue( bool value ) : _value( value )  { /* Nothing to do */ }

    bool get() const  { return _value; }

    bool isBool() const  { return true; }

    bool getBool() const  { return _value; }

    void write( std::ostream& os ) const;

protected:
    static const std::string _TYPE_NAME;

    const std::string& _getTypeName() const  { return _TYPE_NAME; }

private:
    bool _value;
};


/*! \brief Represents a JSON null value. */
class NullValue : public Value
{
public:

    static const ValuePtr Null;


    bool isNull() const  { return true; }

    void write( std::ostream& os ) const;

protected:
    static const std::string _TYPE_NAME;

    const std::string& _getTypeName() const  { return _TYPE_NAME; }

};


/*! \brief Represents an undefined value. */
class UndefinedValue : public Value
{
public:

    void write( std::ostream& /* os */ ) const  { BOOST_THROW_EXCEPTION( WrongType( _getTypeName(), "write" ) ); }

protected:
    static const std::string _TYPE_NAME;

    const std::string& _getTypeName() const  { return _TYPE_NAME; }

private:
    Object _obj;
    Array _arr;
};


} // namespace json


#endif
