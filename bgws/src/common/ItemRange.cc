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

#include "ItemRange.hpp"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <boost/xpressive/xpressive.hpp>

#include <iostream>
#include <stdexcept>
#include <string>


using boost::lexical_cast;
using namespace boost::xpressive;

using std::uint64_t;
using std::string;
using std::runtime_error;


LOG_DECLARE_FILE( "bgws.common" );


namespace bgws {
namespace common {


ItemRange ItemRange::parse(
        const std::string& s,
        unsigned default_size,
        unsigned max_size
    )
{
    static const sregex REGEX = as_xpr( "items" ) >> *_s >> "=" >> *_s >> (s1=+_d) >> "-" >> *_s >> (s2=+_d);

    smatch matches;

    if ( ! regex_search( s, matches, REGEX ) ) {
        LOG_DEBUG_MSG( "Invalid range string in request, using default. range=" << s );
        return ItemRange( 0, default_size-1 );
    }

    try {
        ItemRange ret(
                lexical_cast<uint64_t>( string( matches[1].first, matches[1].second ) ),
                lexical_cast<uint64_t>( string( matches[2].first, matches[2].second ) )
            );

        if ( ret._end < ret._start ) {
            LOG_DEBUG_MSG( "Invalid range string in request, end before start, using default. range=" << s );
            return ItemRange( 0, default_size-1 );
        }

        if ( (ret._end - ret._start + 1) > max_size ) {
            LOG_DEBUG_MSG( "Invalid range string in request, too big, using max size. range=" << s << " max=" << max_size );
            return ItemRange( ret._start, ret._start + max_size - 1 );
        }

        return ret;
    } catch ( std::bad_cast& e ) {
    }

    LOG_DEBUG_MSG( "Invalid range numbers in request, using default. range=" << s );

    return ItemRange( 0, default_size-1 );
}


ItemRange ItemRange::createAbs(
        std::uint64_t start,
        std::uint64_t end
    )
{
    return ItemRange( start, end );
}


ItemRange ItemRange::createCount(
        std::uint64_t start,
        std::uint64_t count
    )
{
    if ( count == 0 ) {
        BOOST_THROW_EXCEPTION( runtime_error( "invalid Item Range, count must not be 0." ) );
    }
    return ItemRange( start, start + count - 1 );
}



std::ostream& operator<<( std::ostream& os, const ItemRange& range )
{
    os << "items=" << range.getStart() << "-" << range.getEnd();
    return os;
}


} } // namespace bgws::common
