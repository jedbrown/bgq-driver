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

#include "LocationsOptions.hpp"

#include "../../SortInfo.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <boost/algorithm/string.hpp>

#include <string>


using boost::bind;

using std::string;


namespace bgws {
namespace query {
namespace diagnostics {


LocationsOptions::LocationsOptions(
        const capena::http::uri::Query::Arguments& query_values,
        const RequestRange& range
    ) :
        _interval( "interval" ),
        _location( "location", BGQDB::DBTDiagresults::LOCATION_SIZE, BGQDB::DBTDiagresults::LOCATION_COL_TYPE ),
        _range(range)
{
    namespace po = boost::program_options;

    po::options_description desc;

    desc.add_options()
            ( "hardwareStatus", po::value<string>()->notifier( bind( &LocationsOptions::_notifyHardwareStatus, this, _1 ) ) )
            ( "replace", po::value<string>()->notifier( bind( &LocationsOptions::_notifyReplace, this, _1 ) ) )
            ( "sort", po::value( &_sort_spec ) )
        ;

    _interval.addTo( desc );
    _location.addTo( desc );

    po::variables_map vm;
    po::store( po::command_line_parser( query_values ).options( desc ).allow_unregistered().run(), vm );
    po::notify( vm );
}


void LocationsOptions::calcWhereClause(
        WhereClause* loc_out,
        WhereClause* main_out,
        cxxdb::ParameterNames* param_names_out
    )
{
    loc_out->add( "run.endTime IS NOT NULL" );

    _location.addTo( *loc_out, *param_names_out, "res.location" );

    if ( ! _hardware_statuses.empty() ) {
        string hardware_status_sql = string() + "grouped.hardwareStatusOrd IN (" + boost::algorithm::join( _hardware_statuses, "," ) + ")";

        main_out->add( hardware_status_sql );
    }

    _interval.addTo( *main_out, *param_names_out, "grouped.lastrun" );

    if ( _replace_ptr ) {
        main_out->add( string() + "grouped.hardwareReplaceOrd = " + (*_replace_ptr ? "1" : "0") );
    }
}


std::string LocationsOptions::calcSortClauseSql()
{
    static const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of
            ( "location", "grouped.location" )
            ( "lastRun", "grouped.lastrun" )
            ( "hardwareStatus", "grouped.hardwareStatusOrd" )
            ( "replace", "grouped.hardwareReplaceOrd" )
        ;

    static const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( "grouped.location" );
    static const string DEFAULT_COLUMN_NAME( "grouped.location" );
    static const bgws::utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Ascending);

    static SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );


    return sort_info.calcSortClause( _sort_spec );
}


void LocationsOptions::bindParameters(
        cxxdb::Parameters& params_in_out
    )
{
    _interval.bindParameters( params_in_out );
    _location.bindParameters( params_in_out );
}


namespace statics {

std::map<char,std::string> createHardwareStatusCodeToOrd()
{
    std::map<char,std::string> ret;
    ret['S'] = "0"; // Success
    ret['M'] = "1"; // Marginal
    ret['K'] = "2"; // Unknown
    ret['F'] = "3"; // Failed
    ret['U'] = "4"; // Uninitialized
    return ret;
}

} // namespace statics


void LocationsOptions::_notifyHardwareStatus( const std::string& s )
{
    static const std::map<char,std::string> HardwareStatusCodeToOrd(statics::createHardwareStatusCodeToOrd());
    static const unsigned HardwareStatusCodeCount(HardwareStatusCodeToOrd.size());


    std::set<char> hardware_codes;

    BOOST_FOREACH( char c, s ) {
        if ( HardwareStatusCodeToOrd.find( c ) != HardwareStatusCodeToOrd.end() ) {
            hardware_codes.insert( c );
        }
        // ignore unexpected codes.
    }

    if ( hardware_codes.empty() || (hardware_codes.size() == HardwareStatusCodeCount) ) {
        // all hardware status values are requested, no filtering, nothing to do.
        return;
    }

    BOOST_FOREACH( char c, hardware_codes ) {
        _hardware_statuses.push_back( HardwareStatusCodeToOrd.at( c ) );
    }
}


void LocationsOptions::_notifyReplace( const std::string& s )
{
    if ( s == "any" )  return;

    if ( s == "true" )  _replace_ptr.reset( new bool(true) );
    else if ( s == "false" )  _replace_ptr.reset( new bool(false) );

    // ignore unexpected value.
}


} } } // namespace bgws::query::diagnostics
