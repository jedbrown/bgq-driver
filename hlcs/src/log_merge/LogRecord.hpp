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


#ifndef LOGRECORD_HPP_
#define LOGRECORD_HPP_


#include "Interval.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>


namespace log_merge {

class LogRecord
{
public:

    LogRecord()  {}

    LogRecord(
            boost::posix_time::ptime timestamp,
            const std::string& text
        ) :
            _ts(timestamp), _text(text)
    { /* nothing to do */ }

    const boost::posix_time::ptime& getTimestamp() const  { return _ts; }
    const std::string& getText() const  { return _text; }


    bool before( const Interval& i ) const  { return i.before( _ts ); }
    bool in( const Interval& i ) const  { return i.includes( _ts ); }
    bool after( const Interval& i ) const  { return i.after( _ts ); }

private:

    boost::posix_time::ptime _ts;
    std::string _text;
};

} // namespace log_merge

#endif
