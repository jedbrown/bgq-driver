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


#include "TimestampFormat.hpp"

#include <boost/regex.hpp>

#include <iostream>
#include <sstream>
#include <string>


using std::istringstream;
using std::string;


namespace log_merge {

//---------------------------------------------------------------------------
// TimestampFormat

TimestampFormat::TimestampFormat()
{
    // Nothing to do.
}

TimestampFormat::~TimestampFormat()
{
    // Nothing to do
}


//---------------------------------------------------------------------------
// TimestampFormats


TimestampFormats TimestampFormats::instance;


TimestampFormats::TimestampFormats()
{
    // Nothing to do.
}


void TimestampFormats::split(
            const std::string& line,
            TimestampFormat const** timestamp_format_p_out,
            std::string* time_str_out,
            std::string* remaining_out
        ) const
{
    for ( _Formats::const_iterator i(_formats.begin()) ; i != _formats.end() ; ++i ) {
        if ( (*i)->split( line, time_str_out, remaining_out ) ) {
            *timestamp_format_p_out = *i;
            return;
        }
    }
    *timestamp_format_p_out = NULL;
    return;
}


bool TimestampFormats::add( const TimestampFormat& format )
{
    _formats.push_back( &format );
    return true;
}


//---------------------------------------------------------------------------
// TimestampFormat1

const boost::regex TimestampFormat1::_LINE_RE( "(\\d\\d/\\d\\d/\\d\\d-\\d\\d:\\d\\d:\\d\\d)( .*)" );  // 04/08/09-01:22:29  /bgsys/local/etc/startmmcs STARTED
const std::locale TimestampFormat1::_PARSE_LOCALE( std::cin.getloc(), new boost::posix_time::time_input_facet( "%m/%d/%y-%H:%M:%S" ) );

const TimestampFormat1 TimestampFormat1::INSTANCE;

const bool added_timestampFormat1(TimestampFormats::instance.add( TimestampFormat1::INSTANCE ) || false);


TimestampFormat1::TimestampFormat1() :
    TimestampFormat()
{

}

bool TimestampFormat1::split( const std::string& line, std::string* time_str_out, std::string* remaining_out ) const
{
    boost::smatch m;

    if ( ! regex_match( line, m, _LINE_RE ) ) {
        return false;
    }

    *time_str_out = m[1].str();
    *remaining_out = m[2].str();
    return true;
}


boost::posix_time::ptime TimestampFormat1::parse( const std::string& time_str ) const
{
    istringstream iss( time_str );
    iss.imbue( _PARSE_LOCALE );

    boost::posix_time::ptime ret;
    iss >> ret;

#if 0 // This method worked, too.
    boost::regex re(
            "(\\d\\d)/(\\d\\d)/(\\d\\d)-(\\d\\d):(\\d\\d):(\\d\\d)"
        );

    boost::smatch m;

    if ( ! regex_match( time_str, m, re ) ) {
        return boost::posix_time::ptime();
    }

    boost::posix_time::ptime ret(
                boost::gregorian::date(
                        2000 + boost::lexical_cast<unsigned>( m[3].str() ),
                        boost::lexical_cast<unsigned>( m[1].str() ),
                        boost::lexical_cast<unsigned>( m[2].str() )
                    ),
                boost::posix_time::hours( boost::lexical_cast<unsigned>( m[4].str() ) ) +
                boost::posix_time::minutes( boost::lexical_cast<unsigned>( m[5].str() ) ) +
                boost::posix_time::seconds( boost::lexical_cast<unsigned>( m[6].str() ) )
            );
#endif

    return ret;
}


//---------------------------------------------------------------------------
// TimestampFormat3

const boost::regex TimestampFormat3::_LINE_RE( "(\\d{4}-\\d{2}-\\d{2}-\\d{2}:\\d{2}:\\d{2}.\\d{3})( .*)" ); // 2009-04-22-23:20:00.025 mc::bringup
const std::locale TimestampFormat3::_PARSE_LOCALE( std::cin.getloc(), new boost::posix_time::time_input_facet( "%Y-%m-%d-%H:%M:%S%f" ) ); // 2009-04-22-23:20:00.025

const TimestampFormat3 TimestampFormat3::INSTANCE;
const bool added_timestampFormat3(TimestampFormats::instance.add( TimestampFormat3::INSTANCE ) || false);


TimestampFormat3::TimestampFormat3() :
    TimestampFormat()
{

}

bool TimestampFormat3::split( const std::string& line, std::string* time_str_out, std::string* remaining_out ) const
{
    boost::smatch m;

    if ( ! regex_match( line, m, _LINE_RE ) ) {
        return false;
    }

    *time_str_out = m[1].str();
    *remaining_out = m[2].str();
    return true;
}


boost::posix_time::ptime TimestampFormat3::parse( const std::string& time_str ) const
{
    istringstream iss( time_str );

    iss.imbue( _PARSE_LOCALE );

    boost::posix_time::ptime ret;
    iss >> ret;

    return ret;
}


//---------------------------------------------------------------------------
// TimestampFormatQ

const boost::regex TimestampFormatQ::_LINE_RE( "(\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}.\\d{3})( ?.*)" ); // 2009-08-25 10:31:54.660 (INFO ) [queenBee] mcservermain - File limits: 1024

const TimestampFormatQ TimestampFormatQ::INSTANCE;

TimestampFormatQ::TimestampFormatQ() :
    TimestampFormat()
{

}


bool TimestampFormatQ::split( const std::string& line, std::string* time_str_out, std::string* remaining_out ) const
{
    boost::smatch m;

    if ( ! regex_match( line, m, _LINE_RE ) ) {
        return false;
    }

    *time_str_out = m[1].str();
    *remaining_out = m[2].str();
    return true;
}


boost::posix_time::ptime TimestampFormatQ::parse( const std::string& time_str ) const
{
    return boost::posix_time::time_from_string( time_str ); // this is an easy one.
}


const bool added_timestampFormatQ(TimestampFormats::instance.add( TimestampFormatQ::INSTANCE ) || false);

} // namespace log_merge
