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

#include "SortSpec.hpp"

#include <utility/include/Log.h>

#include <iostream>
#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


std::istream& operator>>( std::istream& is, SortSpec& ss_out )
{
    string s;
    is >> std::skipws >> s;

    // Ignore any problem reading the string & just mark is eof. This tells boost::program_options that the argument was completely parsed.
    is.clear( std::ios::eofbit );

    ss_out = SortSpec( s );

    return is;
}


SortSpec::SortSpec( const std::string& str )
{
    string col_id;

    if ( (! str.empty()) && (str[0] == '-') ) {
        _direction = utility::SortDirection::Descending;
        col_id = str.substr( 1 );
    } else if ( (! str.empty()) && (str[0] == '+' || str[0] == ' ') ) {
        _direction = utility::SortDirection::Ascending;
        col_id = str.substr( 1 );
    } else {
        _direction = utility::SortDirection::Ascending;
        col_id = str;
    }

    LOG_DEBUG_MSG( "Given str '" << str << "' -> d:" << _direction << " c:" << col_id );

    _column_id = col_id;
}


} // namespace bgws
