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


#ifndef JSON_VALUE_HPP_
#define JSON_VALUE_HPP_


/*! \file
 *  \brief Value classes.
 */


#include "fwd.hpp"

#include <iosfwd>
#include <string>


namespace json {


/*! \brief Undefined value object. */
extern const ValuePtr Undefined;


/*! \brief Represents a JSON value. */
class Value
{
public:


    /*! \brief Returns true if this value is a string. */
    virtual bool isString() const  { return false; }

    /*! \brief Returns true if this value is a number. */
    virtual bool isNumber() const  { return false; }

    /*! \brief Returns true if this value is an object value. */
    virtual bool isObject() const  { return false; }

    /*! \brief Returns true if this value is an array value. */
    virtual bool isArray() const  { return false; }

    /*! \brief Returns true if this value is a Boolean. */
    virtual bool isBool() const  { return false; }

    /*! \brief Returns true if this value is null. */
    virtual bool isNull() const  { return false; }

    /*! \brief Returns the string value.
     *
     *  \throw WrongType the value isn't a String.
     */
    virtual const std::string& getString() const;

    /*! \brief Returns the number value as a double.
     *
     *  \throw WrongType the value isn't a Double.
     */
    virtual double getDouble() const;

    /*! \brief Returns the object value.
     *
     *  \throw WrongType the value isn't a Object.
     */
    virtual const Object& getObject() const;

    /*! \brief Returns the object value.
     *
     *  \throw WrongType the value isn't a Object.
     */
    virtual Object& getObject();

    /*! \brief Returns the array value.
     *
     *  \throw WrongType the value isn't a Array.
     */
    virtual const Array& getArray() const;

    /*! \brief Returns the array value.
     *
     *  \throw WrongType the value isn't a Array.
     */
    virtual Array& getArray();

    /*! \brief Returns the Boolean value.
     *
     *  \throw WrongType the value isn't a Bool.
     */
    virtual bool getBool() const;

    /*! \brief Formats the value to the stream. */
    virtual void write( std::ostream& os ) const =0;

    /*! \brief Destructor. */
    virtual ~Value()  { /* Nothing to do */ }


protected:

    virtual const std::string& _getTypeName() const =0;
};


/*! \brief Output a value. */
std::ostream& operator<<( std::ostream& os, const Value& v );


} // namespace json

#endif
