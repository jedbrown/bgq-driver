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

#ifndef BGWS_COMMON_ITEM_RANGE_HPP_
#define BGWS_COMMON_ITEM_RANGE_HPP_


#include <cstdint>
#include <iosfwd>


namespace bgws {
namespace common {


class ItemRange
{
public:


    /*! Parses a Range request header string. BGWS Range strings are like items=x-y where x & y are unsigned integers.
     *  If s is a valid range string and the size is <= max_size then the ItemRange for that string is returned.
     *  Otherwise, an ItemRange of the default size starting at the first element is returned. */
    static ItemRange parse(
            const std::string& s,
            unsigned default_size = 50,
            unsigned max_size = 100
        );

    static ItemRange createAbs(
            std::uint64_t start,
            std::uint64_t end
        );

    static ItemRange createCount(
            std::uint64_t start,
            std::uint64_t count
        );


    ItemRange() : _start(0), _end(0)  { /* Nothing to do */ }

    bool operator==( const ItemRange& other ) const  { return (_start == other._start && _end == other._end); }


    std::uint64_t getStart() const  { return _start; }
    std::uint64_t getEnd() const  { return _end; }


private:

    std::uint64_t _start, _end;


    ItemRange(
            std::uint64_t start,
            std::uint64_t end
        ) : _start(start), _end(end)
    { /* Nothing to do */ }

};


std::ostream& operator<<( std::ostream& os, const ItemRange& range );

} } // namespace bgws::common


#endif
