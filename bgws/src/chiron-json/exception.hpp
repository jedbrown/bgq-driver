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

#ifndef JSON_EXCEPTION_HPP_
#define JSON_EXCEPTION_HPP_

/*! \file
 *  \brief Exceptions.
 */

#include <stdexcept>
#include <string>


namespace json {


/*!
 *  \brief Exception for member doesn't contain a value.
 */
class MemberNotFound : public std::logic_error
{
public:

    MemberNotFound( const std::string& member_name )
        : std::logic_error( std::string() + "JSON error: object doesn't contain '" + member_name + "'" ),
          _member_name(member_name)
    { /* Nothing to do */ }


    const std::string& getMemberName() const  { return _member_name; }

    ~MemberNotFound()  throw()  { /* Nothing to do */ }

private:

    std::string _member_name;
};


/*!
 *  \brief Exception for wrong type when calling a get* function on a Value.
 */
class WrongType : public std::logic_error
{
public:
    WrongType( const std::string& value_type_name, const std::string& type_requested )
        : std::logic_error( std::string() + "JSON error: tried to get value as " + type_requested + " when is " + value_type_name ),
          _value_type_name(value_type_name),
          _type_requested(type_requested)
    { /* Nothing to do */ }

    void setMemberName( const std::string& name )  { _member_name = name; }

    const std::string& getValueTypeName() const  { return _value_type_name; }
    const std::string& getTypeRequested() const  { return _type_requested; }
    const std::string& getMemberName() const  { return _member_name; }

    ~WrongType() throw()  { /* Nothing to do */ }

private:

    std::string _value_type_name;
    std::string _type_requested;
    std::string _member_name;
};


/*!
 *  \brief Exception for JSON parsing error.
 */
class ParseError : public std::runtime_error
{
public:

    /*! \brief Constructor. */
    ParseError( const std::string& json_text, unsigned stop )
        : std::runtime_error( "invalid JSON string" ),
          _json_text(json_text),
          _stop(stop)
    { /* Nothing to do */ }

    /*! \brief Get the document that failed to parse. */
    const std::string getJsonText() const  { return _json_text; }

    /*! \brief Get the location where the parsing stopped. */
    unsigned getStop() const  { return _stop; }


    ~ParseError() throw()  { /* Nothing to do */ }

private:
    std::string _json_text;
    unsigned _stop;
};


} // namespace json

#endif
