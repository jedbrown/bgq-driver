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

#ifndef JSON_FORMATTER_HPP_
#define JSON_FORMATTER_HPP_


/*! \file
 *  \brief class Formatter.
 */


#include "Value.hpp"

#include <iosfwd>
#include <string>


namespace json {


/*! \brief Formats a JSON object. */
class Formatter
{
public:

    /*! \brief Format the Object to the stream. */
    void operator()( const Value& v, std::ostream& os );

    /*! \brief Format the object and return the JSON string. */
    std::string operator()( const Value& v );


private:

    void _format( const Value& v, std::ostream& os, unsigned indent_level );

    void _format( const Array& a, std::ostream& os, unsigned indent_level );

    void _format( const Object& o, std::ostream& os, unsigned indent_level );

    void _format( const std::string& member_name, const Value& value, std::ostream& os, unsigned indent_level  );
};


} // namespace json


#endif
