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


#ifndef LOGLINEPARSER_HPP_
#define LOGLINEPARSER_HPP_


#include "TimestampFormat.hpp"

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>


namespace log_merge {

class LogLineParser
{
public:

    void parse(
            const std::string& line,
            bool* has_time_out,
            boost::posix_time::ptime *time_out,
            std::string* remaining_out
        ) const;


    static const LogLineParser& getInstance()  { return _INSTANCE; }


private:

    static const LogLineParser _INSTANCE;
};

} // namespace log_merge

#endif
