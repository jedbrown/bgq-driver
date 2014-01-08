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

#ifndef BGWS_COMMON_CONTENT_RANGE_HPP_
#define BGWS_COMMON_CONTENT_RANGE_HPP_


#include <iosfwd>
#include <string>

#include <cstdint>
#include <stdexcept>


namespace bgws {
namespace common {


class ContentRange
{
public:

    class ParseError : public std::runtime_error
    {
        public:
            ParseError( const std::string& cr_str );
            const std::string& getContentRangeString() const  { return _cr_str; }
            ~ParseError() throw() {}
        private:
            std::string _cr_str;
    };


    class InvalidRangeError : public std::runtime_error
    {
        public:
            InvalidRangeError( const std::string& msg );
    };


    static ContentRange createAbs( std::uint64_t start, std::uint64_t end, std::uint64_t total );
    static ContentRange createCount( std::uint64_t start, std::uint64_t count, std::uint64_t total );

    static ContentRange parse( const std::string& cr_str );


    std::uint64_t getStart() const  { return _start; }
    std::uint64_t getEnd() const  { return _end; }
    std::uint64_t getTotal() const  { return _total; }


private:

    ContentRange( std::uint64_t start, std::uint64_t end, std::uint64_t total );


    std::uint64_t _start, _end, _total;

};


std::ostream& operator<<( std::ostream& os, const ContentRange& cr );


} } // namespace bgws::common

#endif
