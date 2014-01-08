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

#include "Formatter.hpp"

#include "constants.hpp"
#include "json.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>


using std::ostringstream;
using std::string;


namespace json {


void Formatter::operator()( const Value& v, std::ostream& os )
{
    _format( v, os, 0 );
    os << std::endl;
}


std::string Formatter::operator()( const Value& v )
{
    ostringstream oss;

    (*this)( v, oss );

    return oss.str();
}


void Formatter::_format( const Value& v, std::ostream& os, unsigned indent_level )
{
    if ( v.isObject() ) {
        _format( v.getObject(), os, indent_level );
        return;
    }

    if ( v.isArray() ) {
        _format( v.getArray(), os, indent_level );
        return;
    }

    // Format a non-object
    os << v;
}


void Formatter::_format( const Array& a, std::ostream& os, unsigned /*indent_level*/ )
{
    if ( a.empty() ) {
        os << beginArray << endArray;
        return;
    }

    os << beginArray << ' ';

    for ( Array::const_iterator i(a.begin()) ; i != a.end() ; ++i ) {
        if ( i != a.begin() ) {
            os << valueSeparator << ' ';
        }
        if ( *i ) {
            os << **i;
        } else {
            os << nullString;
        }
    }

    os << ' ' << endArray;
}


void Formatter::_format( const Object& o, std::ostream& os, unsigned indent_level )
{
    if ( o.empty() ) {
        os << beginObject << endObject;
        return;
    }

    os << beginObject;

    os << '\n';
    for ( Object::const_iterator i(o.begin()) ; i != o.end() ; ++i ) {
        if ( i != o.begin() ) {
            os << valueSeparator << "\n";
        }
        _format( i->first, *(i->second), os, indent_level + 2 );
    }
    os << "\n"
       << std::setw( indent_level ) << ""
       << endObject;
}


void Formatter::_format( const std::string& member_name, const Value& value, std::ostream& os, unsigned indent_level  )
{
    os << std::setw( indent_level ) << ""
       << StringValue( member_name ) << " " << nameSeparator << " ";

    _format( value, os, indent_level + 2 );
}


} // namespace json
