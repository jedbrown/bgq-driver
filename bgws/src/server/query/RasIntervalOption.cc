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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#include "RasIntervalOption.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/gensrc/DBTEventlog.h>

#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/xpressive/xpressive.hpp>


using BGQDB::filtering::TimeInterval;

using namespace boost::assign;
using namespace boost::xpressive;

using boost::lexical_cast;

using std::string;


namespace bgws {
namespace query {


RasIntervalOption::RasIntervalOption(
        const std::string& name
    ) :
        _name(name),
        _config(Config::NotUsed),
        _time_interval_option(_name)
{
    // Nothing to do.
}


void RasIntervalOption::setRange( std::uint64_t start, std::uint64_t end )
{
    _config = Config::IdRange;
    _range_start = start;
    _range_end = end;
}


void RasIntervalOption::setTimeIntervalOption( const TimeIntervalOption& tio )
{
    if ( tio.getConfig() == TimeIntervalOption::Config::NoValue ) {
        _config = Config::NotUsed;
        return;
    }

    _config = Config::TimeInterval;
    _time_interval_option = tio;
}


void RasIntervalOption::addTo(
        boost::program_options::options_description& desc_in_out
    )
{
    desc_in_out.add_options()
            ( _name.c_str(), boost::program_options::value<string>()->notifier( boost::bind( &RasIntervalOption::_set, this, _1 ) ), "RasIntervalOption" )
        ;
}


bool RasIntervalOption::addTo(
        WhereClause& wc_in_out,
        cxxdb::ParameterNames& param_names_out
    ) const
{
    if ( _config == Config::NotUsed )  return false; // Nothing to do.

    if ( _config == Config::IdRange ) {
        wc_in_out.add( "recId BETWEEN ? AND ?" );

        param_names_out += string() + "_START_" + _name;
        param_names_out += string() + "_END_" + _name;

        return true;
    }

    _time_interval_option.addTo( wc_in_out, param_names_out, BGQDB::DBTEventlog::EVENT_TIME_COL /*column_name*/ );
    return true;
}


void RasIntervalOption::bindParameters(
        cxxdb::Parameters& params_in_out
    ) const
{
    if ( _config == Config::NotUsed )  return; // Nothing to do.

    if ( _config == Config::IdRange ) {
        params_in_out[string() + "_START_" + _name].cast( _range_start );
        params_in_out[string() + "_END_" + _name].cast( _range_end );
        return;
    }

    _time_interval_option.bindParameters( params_in_out );
}


void RasIntervalOption::_set( const std::string& s )
{
    static const sregex RE = (s1=+_d) >> "-" >> (s2=+_d);

    smatch match;

    if ( regex_match( s, match, RE ) ) {
        std::uint64_t start(lexical_cast<std::uint64_t>( match[1] )),
                      end(lexical_cast<std::uint64_t>( match[2] ));

        setRange( start, end );
        return;
    }


    _time_interval_option.setIntervalStr( s );

    if ( _time_interval_option.getConfig() == TimeIntervalOption::Config::HasInterval ) {
        _config = Config::TimeInterval;
        return;
    }

    _config = Config::NotUsed;
}


} } // namespace bgws::query
