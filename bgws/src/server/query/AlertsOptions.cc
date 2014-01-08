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


#include "AlertsOptions.hpp"

#include "../teal/Teal.hpp"

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <set>
#include <string>


using boost::lexical_cast;

using std::set;
using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace query {


AlertsOptions::AlertsOptions(
        const RequestRange& req_range,
        const capena::http::uri::Query& query
    ) :
        dups(false),
        interval( "interval" ),
        location( "location", teal::Teal::LOCATION_DB_COLUMN_SIZE, BGQDB::DBObj::ColumnType::Varchar ),
        req_range(req_range)
{
    namespace po = boost::program_options;


    string dup_str;
    string location_types_str;
    string severities_str;
    string states_str;
    string urgencies_str;

    po::options_description desc;
    desc.add_options()
            ( "dup", po::value( &dup_str ) )
            ( "locationType", po::value( &location_types_str ) )
            ( "severity", po::value( &severities_str ) )
            ( "sort", po::value( &sort_spec ) )
            ( "state", po::value( &states_str ) )
            ( "urgency", po::value( &urgencies_str ) )
        ;

    interval.addTo( desc );
    location.addTo( desc );

    po::variables_map vm;
    po::store(
            po::command_line_parser( query.calcArguments() ).options( desc ).allow_unregistered().run(),
            vm
        );
    po::notify( vm );


    if ( dup_str == "T" )  dups = true;


    BOOST_FOREACH( char ch, location_types_str ) {
        if ( ch == 'A' || ch == 'J' || ch == 'C' || ch == 'I' ) {
            location_types.insert( lexical_cast<string>( ch ) );
        }
    }

    if ( location_types.size() == 4 )  location_types.clear();


    BOOST_FOREACH( char ch, states_str ) {
        try {
            int32_t severity_val(lexical_cast<int32_t>( string() + ch ));
            if ( severity_val == 1 || severity_val == 2 ) {
                states.insert( lexical_cast<string>( severity_val ) );
            }
        } catch ( std::bad_cast& e ) {
            // Ignore.
        }
    }

    if ( states.size() == 2 )  states.clear();


    BOOST_FOREACH( char ch, severities_str ) {
        if ( ch == 'F' || ch == 'E' || ch == 'W' || ch == 'I' ) {
            severities.insert( lexical_cast<string>( ch ) );
        }
    }

    if ( severities.size() == 4 )  severities.clear();


    BOOST_FOREACH( char ch, urgencies_str ) {
        if ( ch == 'I' || ch == 'S' || ch == 'N' || ch == 'D' || ch == 'O' ) {
            urgencies.insert( lexical_cast<string>( ch ) );
        }
    }

    if ( urgencies.size() == 5 )  urgencies.clear();
}


} } // namespace bgws::query
