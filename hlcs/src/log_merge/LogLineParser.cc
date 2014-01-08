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


#include "LogLineParser.hpp"

#include "TimestampFormat.hpp"

using std::string;


namespace log_merge {

void LogLineParser::parse(
        const std::string& line,
        bool* has_time_out,
        boost::posix_time::ptime *time_out,
        std::string* remaining_out
    ) const
{
    const TimestampFormat *timestamp_format_p;
    string time_str;

    TimestampFormats::getInstance().split( line, &timestamp_format_p, &time_str, remaining_out );

    if ( ! timestamp_format_p ) {
        *has_time_out = false;
        return;
    }

    *has_time_out = true;
    *time_out = timestamp_format_p->parse( time_str );
}


const LogLineParser LogLineParser::_INSTANCE((LogLineParser()));

} // namespace log_merge
