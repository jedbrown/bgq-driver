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

/*! \page timeIntervalFormat Time Interval Format

The format used by BGWS for a time interval in the query string of a URI follows the
<a href="http://en.wikipedia.org/wiki/ISO_8601#Time_intervals">ISO8601 format</a>.

An interval is made up of:
<ul>
<li> Timestamp: <i>YYYYMMDD</i>T<i>hhmmss</i> </li>
<li> Duration: P<i>n</i>Y<i>n</i>M<i>n</i>DT<i>n</i>H<i>n</i>M<i>n</i>S,
where n is the number of years, months, days, etc., and parts can be left off if they're 0s. </li>
</ul>

An interval is either
<ul>
<li> <i>timestamp</i>/<i>timestamp</i> -- start and end time </li>
<li> <i>duration</i> -- a duration, the end time is now </li>
<li> <i>timestamp</i>/<i>duration</i> -- start time and duration </li>
<li> <i>duration</i>/<i>timestamp</i> -- duration and end time </li>
</ul>


If the parameter is nullable, a value of "null" or "-null" can also be used.
"null" indicates that the target object must not have a value.
"-null" indicates that the target object must have a value.


Here's some examples:
<pre>
20100715T155500/20100716T155500 -- start and end time

P1D -- the previous 1 day.

PT1H -- the previous 1 hour.

20100715T155500/P1D -- 1 day since start time
</pre>

In a URL, the solidus (/) is escaped as \%2F,
so in the query string part of the URL an interval using two timestamps would look like this:

<pre>
20100715T155500\%2F20100715T155500
</pre>

 */


#include "TimeIntervalOption.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <stdexcept>


using BGQDB::filtering::TimeInterval;

using namespace boost::assign;

using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


std::string TimeIntervalOption::calcSql( const BGQDB::filtering::Duration& d, bool plus_minus )
{
    std::vector<string> parts;

    if ( d.getDate().years() != 0 )  parts += lexical_cast<string>( d.getDate().years() ) + " YEAR";
    if ( d.getDate().months() != 0 )  parts += lexical_cast<string>( d.getDate().months() ) + " MONTH";
    if ( d.getDate().days() != 0 )  parts += lexical_cast<string>( d.getDate().days() ) + " DAY";

    if ( d.getTime().hours() != 0 )  parts += lexical_cast<string>( d.getTime().hours() ) + " HOUR";
    if ( d.getTime().minutes() != 0 )  parts += lexical_cast<string>( d.getTime().minutes() ) + " MINUTE";
    if ( d.getTime().seconds() != 0 )  parts += lexical_cast<string>( d.getTime().seconds() ) + " SECOND";

    return boost::algorithm::join( parts, plus_minus ? " + " : " - " );
}


TimeIntervalOption::TimeIntervalOption(
        const std::string& name,
        Nullable nullable
    ) :
        _name(name),
        _nullable(nullable),
        _config(Config::NoValue)
{
    // Nothing to do.
}


TimeIntervalOption::TimeIntervalOption(
        const std::string& name,
        const BGQDB::filtering::TimeInterval& interval,
        Nullable nullable
    ) :
        _name(name),
        _nullable(nullable),
        _config((interval == TimeInterval() || interval == TimeInterval::ALL) ? Config::NoValue : Config::HasInterval),
        _interval(interval)
{
    // Nothing to do.
}


void TimeIntervalOption::setTimeInterval(
        const BGQDB::filtering::TimeInterval& interval
    )
{
    _config = ((interval == TimeInterval() || interval == TimeInterval::ALL) ? Config::NoValue : Config::HasInterval);
    _interval = interval;
}


void TimeIntervalOption::addTo(
        boost::program_options::options_description& desc_in_out
    )
{
    desc_in_out.add_options()
            ( _name.c_str(), boost::program_options::value<string>()->notifier( boost::bind( &TimeIntervalOption::setIntervalStr, this, _1 ) ), "TimeIntervalOption" )
        ;
}


bool TimeIntervalOption::addTo(
        WhereClause& wc_in_out,
        cxxdb::ParameterNames& param_names_out,
        const std::string& column_name
    ) const
{
    if ( _config == Config::NoValue )  return false; // Nothing to do.

    if ( _config == Config::IsNull ) {
        wc_in_out.add( column_name + " IS NULL" );
        return true;
    }

    if ( _config == Config::NotNull ) {
        wc_in_out.add( column_name + " IS NOT NULL" );
        return true;
    }

    if ( _interval.getStart() != TimeInterval::Timestamp() || _interval.getEnd() != TimeInterval::Timestamp() ) {
        wc_in_out.add( column_name + " BETWEEN ? AND ?" );

        param_names_out += string() + "_START_" + _name;
        param_names_out += string() + "_END_" + _name;

        return true;
    }

    wc_in_out.add( column_name + " >= (CURRENT_TIMESTAMP - " + calcSql( _interval.getDuration(), false ) + ")" );

    return true;
}


void TimeIntervalOption::bindParameters(
        cxxdb::Parameters &params_in_out
    ) const
{
    if ( _config != Config::HasInterval )  return; // Nothing to do.

    if ( _interval == TimeInterval() || _interval == TimeInterval::ALL )  return;
    if ( _interval.getStart() == TimeInterval::Timestamp() && _interval.getEnd() == TimeInterval::Timestamp() )  return;

    if ( _interval.getStart() != TimeInterval::Timestamp() && _interval.getEnd() != TimeInterval::Timestamp() ) {
        params_in_out[string() + "_START_" + _name].set( _interval.getStart() );
        params_in_out[string() + "_END_" + _name].set( _interval.getEnd() );
        return;
    }

    if ( _interval.getStart() != TimeInterval::Timestamp() ) {
        params_in_out[string() + "_START_" + _name].set( _interval.getStart() );
        params_in_out[string() + "_END_" + _name].set( _interval.getStart() + _interval.getDuration() );
        return;
    }

    params_in_out[string() + "_START_" + _name].set( _interval.getEnd() - _interval.getDuration() );
    params_in_out[string() + "_END_" + _name].set( _interval.getEnd() );
}


void TimeIntervalOption::setIntervalStr( const std::string& s )
{
    LOG_DEBUG_MSG( "Setting interval " << _name << " to '" << s << "'" );

    if ( _nullable == Nullable::Allow ) {
        if ( boost::algorithm::to_lower_copy( s ) == "null" ) {
            _config = Config::IsNull;
            return;
        }
        if ( boost::algorithm::to_lower_copy( s ) == "-null" ) {
            _config = Config::NotNull;
            return;
        }
    }

    try {
        _interval = lexical_cast<TimeInterval>( s );
        _config = ((_interval == TimeInterval() || _interval == TimeInterval::ALL) ? Config::NoValue : Config::HasInterval);
    } catch ( std::bad_cast& e ) {
        LOG_INFO_MSG( "Invalid interval '" << s << "' for " << _name << "'. Ignoring." );
        // Ignoring bad_cast.
        _config = Config::NoValue;
    }
}


std::istream& operator>>( std::istream& is, TimeIntervalOption& tia_out )
{
    TimeInterval ti;
    is >> ti;

    tia_out.setTimeInterval( ti );

    return is;
}


} // namespace bgws
