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

#include "ContentRange.hpp"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <boost/xpressive/xpressive.hpp>

#include <iostream>
#include <string>


using namespace boost::xpressive;

using boost::lexical_cast;

using std::ostringstream;
using std::string;


LOG_DECLARE_FILE( "bgws.common" );


namespace bgws {
namespace common {


ContentRange::ParseError::ParseError( const std::string& cr_str )
    : std::runtime_error( string() + "invalid BGWS content range string '" + cr_str + "'" ),
      _cr_str(cr_str)
{
    // Nothing to do.
}


ContentRange::InvalidRangeError::InvalidRangeError( const std::string& msg )
    : std::runtime_error( msg )
{
    // Nothing to do
}


ContentRange ContentRange::createAbs( std::uint64_t start, std::uint64_t end, std::uint64_t total )
{
    return ContentRange( start, end, total );
}


ContentRange ContentRange::createCount( std::uint64_t start, std::uint64_t count, std::uint64_t total )
{
    if ( count == 0 ) {
        LOG_DEBUG_MSG( "Invalid content range with count = 0. Returning start-start/total" );
        return ContentRange( start, start, total );
    }

    return ContentRange( start, start + count - 1, total );
}


ContentRange ContentRange::parse( const std::string& cr_str )
{
    static const sregex RE = *_s >> "items" >> +_s >> (s1=+_d) >> *_s >> "-" >> *_s >> (s2=+_d) >> *_s >> "/" >> *_s >> (s3=+_d) >> *_s;

    smatch match;

    if ( ! regex_match( cr_str, match, RE ) ) {
        LOG_DEBUG_MSG( "invalid BGWS content range string '" << cr_str << "'" );

        BOOST_THROW_EXCEPTION( ParseError( cr_str ) );
    }


    std::uint64_t
        start(lexical_cast<std::uint64_t>( match[1] )),
        end(lexical_cast<std::uint64_t>( match[2] )),
        total(lexical_cast<std::uint64_t>( match[3] ));

    LOG_DEBUG_MSG( "parse string='" << cr_str << "' start=" << start << " end=" << end << " total=" << total );

    return createAbs( start, end, total );
}


ContentRange::ContentRange( std::uint64_t start, std::uint64_t end, std::uint64_t total )
    : _start(start), _end(end), _total(total)
{
    if ( end < start ) {
        BOOST_THROW_EXCEPTION( InvalidRangeError( "invalid content-range, end is before start" ) );
    }
    if ( total == 0 ) {
        BOOST_THROW_EXCEPTION( InvalidRangeError( "invalid content-range, total is 0" ) );
    }
}


std::ostream& operator<<( std::ostream& os, const ContentRange& cr )
{
    os << "items " << cr.getStart() << "-" << cr.getEnd() << "/" << cr.getTotal();
    return os;
}


} } // namespace bgws::common
