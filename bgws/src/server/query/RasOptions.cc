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

#include "RasOptions.hpp"

#include "../WhereClause.hpp"

#include <db/include/api/tableapi/gensrc/DBTEventlog.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <boost/algorithm/string.hpp>

#include <stdexcept>
#include <vector>


using namespace boost::assign;

using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace query {


const RasOptions::RasSeverities RasOptions::ALL_RAS_SEVERITIES = boost::assign::list_of
        ( RasEvent::INFO )
        ( RasEvent::WARN )
        ( RasEvent::FATAL )
        ( RasEvent::UNKNOWN )
    ;


const SortInfo RasOptions::_s_sort_info(_createSortInfo());


SortInfo RasOptions::_createSortInfo()
{
    const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of
            ( "id", BGQDB::DBTEventlog::RECID_COL ) ( "recid", BGQDB::DBTEventlog::RECID_COL )
            ( "block", BGQDB::DBTEventlog::BLOCK_COL )
            ( "category", BGQDB::DBTEventlog::CATEGORY_COL )
            ( "component", BGQDB::DBTEventlog::COMPONENT_COL )
            ( "controlAction", BGQDB::DBTEventlog::CTLACTION_COL )
            ( "count", BGQDB::DBTEventlog::COUNT_COL )
            ( "eventTime", BGQDB::DBTEventlog::EVENT_TIME_COL )( "event_time", BGQDB::DBTEventlog::EVENT_TIME_COL )
            ( "jobId", BGQDB::DBTEventlog::JOBID_COL )
            ( "location", BGQDB::DBTEventlog::LOCATION_COL )
            ( "message", BGQDB::DBTEventlog::MESSAGE_COL )
            ( "msgId", BGQDB::DBTEventlog::MSG_ID_COL )
            ( "severity", BGQDB::DBTEventlog::SEVERITY_COL )
            ( "serialNumber", BGQDB::DBTEventlog::SERIALNUMBER_COL )
        ;

    const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( BGQDB::DBTEventlog::RECID_COL );
    const string DEFAULT_COLUMN_NAME( BGQDB::DBTEventlog::RECID_COL );
    const bgws::utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Descending);

    SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );

    return sort_info;
}


RasOptions::RasOptions(
        const capena::http::uri::Query::Arguments& parameters,
        const RequestRange& req_range
    ) :
        _block( "block", BGQDB::DBTEventlog::BLOCK_SIZE, BGQDB::DBTEventlog::BLOCK_COL_TYPE ),
        _category( "category", BGQDB::DBTEventlog::CATEGORY_SIZE, BGQDB::DBTEventlog::CATEGORY_COL_TYPE ),
        _component( "component", BGQDB::DBTEventlog::COMPONENT_SIZE, BGQDB::DBTEventlog::COMPONENT_COL_TYPE ),
        _interval( "interval" ),
        _location( "location", BGQDB::DBTEventlog::LOCATION_SIZE, BGQDB::DBTEventlog::LOCATION_COL_TYPE ),
        _message( "message", BGQDB::DBTEventlog::MESSAGE_SIZE, BGQDB::DBTEventlog::MESSAGE_COL_TYPE ),
        _msg_id( "msgid", BGQDB::DBTEventlog::MSG_ID_SIZE, BGQDB::DBTEventlog::MSG_ID_COL_TYPE ),
        _serialnumber( "serialnumber", BGQDB::DBTEventlog::SERIALNUMBER_SIZE ),
        _range(req_range)
{
    namespace po = boost::program_options;

    string control_actions_str;
    string severity_str;

    po::options_description desc;
    desc.add_options()
            ( "controlAction", po::value( &control_actions_str ) )
            ( "job", po::value( &_jobid ) )
            ( "severity", po::value( &severity_str) )
            ( "sort", po::value( &_sort_spec ) )
        ;

    _block.addTo( desc );
    _category.addTo( desc );
    _component.addTo( desc );
    _location.addTo( desc );
    _message.addTo( desc );
    _msg_id.addTo( desc );
    _interval.addTo( desc );
    _serialnumber.addTo( desc );

    po::variables_map vm;
    po::store(
            po::command_line_parser( parameters ).options( desc ).allow_unregistered().run(),
            vm
        );
    po::notify( vm );

    if ( control_actions_str != string() ) {
        std::vector<std::string> control_actions_vec;
        boost::algorithm::split( control_actions_vec, control_actions_str, boost::is_any_of( "," ) );

        _ControlActions control_actions_tmp( control_actions_vec.begin(), control_actions_vec.end() );

        static const std::string ControlActionsAny("any");
        static const std::string ControlActionsAction("action");

        if ( control_actions_tmp.count( ControlActionsAny ) != 0 ) {
            // not filtering on control action, nothing to do.
        } else if ( control_actions_tmp.count( ControlActionsAction ) != 0 ) {
            _control_actions.insert( "action" );
        } else {
            _control_actions = control_actions_tmp;
        }

    }

    if ( severity_str != string() ) {

        _severities.clear();

        BOOST_FOREACH( char sev_char, severity_str ) {
            if ( sev_char == 'I' )  _severities.insert( RasEvent::INFO );
            else if ( sev_char == 'W' )  _severities.insert( RasEvent::WARN );
            else if ( sev_char == 'F' )  _severities.insert( RasEvent::FATAL );
            else if ( sev_char == 'U' )  _severities.insert( RasEvent::UNKNOWN );
            else {
                LOG_WARN_MSG( "Ignoring invalid severity in request parameters, " << sev_char );
            }
        }

    } else {

        _severities = ALL_RAS_SEVERITIES;

    }
}


void RasOptions::calcWhereClauseSql(
        std::string* sql_out,
        cxxdb::ParameterNames* parameter_names_out
    ) const
{
    WhereClause where_clause;

    _block.addTo( where_clause, *parameter_names_out, BGQDB::DBTEventlog::BLOCK_COL );
    _category.addTo( where_clause, *parameter_names_out, BGQDB::DBTEventlog::CATEGORY_COL );
    _component.addTo( where_clause, *parameter_names_out, BGQDB::DBTEventlog::COMPONENT_COL );
    _interval.addTo( where_clause, *parameter_names_out );
    _location.addTo( where_clause, *parameter_names_out, BGQDB::DBTEventlog::LOCATION_COL );
    _message.addTo( where_clause, *parameter_names_out, BGQDB::DBTEventlog::MESSAGE_COL );
    _msg_id.addTo( where_clause, *parameter_names_out, BGQDB::DBTEventlog::MSG_ID_COL );
    _serialnumber.addTo( where_clause, *parameter_names_out, BGQDB::DBTEventlog::SERIALNUMBER_COL );


    if ( ! _control_actions.empty() ) {

        if ( _control_actions.count( "action" ) ) {

            where_clause.add( "ctlAction IS NOT NULL" );

        } else {

            std::vector<std::string> parts;

            for ( unsigned i(1) ; i <= _control_actions.size() ; ++i ) {
                parts.push_back( "ctlAction LIKE ?" );
                *parameter_names_out += string() + "ctlAction_" + lexical_cast<string>(i);
            }

            where_clause.add( boost::algorithm::join( parts, " OR " ) );

        }
    }

    if ( _jobid.get() ) {
        where_clause.add( BGQDB::DBTEventlog::JOBID_COL + "=?" );
        *parameter_names_out += "jobId";
    }

    if ( (! _severities.empty()) && (_severities.size() != ALL_RAS_SEVERITIES.size()) ) {
        std::vector<std::string> parts;
        if ( _severities.find( RasEvent::INFO ) != _severities.end() )  parts += BGQDB::DBTEventlog::SEVERITY_COL + "='INFO'";
        if ( _severities.find( RasEvent::WARN ) != _severities.end() )  parts += BGQDB::DBTEventlog::SEVERITY_COL + "='WARN'";
        if ( _severities.find( RasEvent::FATAL ) != _severities.end() )  parts += BGQDB::DBTEventlog::SEVERITY_COL + "='FATAL'";
        if ( _severities.find( RasEvent::UNKNOWN ) != _severities.end() )  parts += BGQDB::DBTEventlog::SEVERITY_COL + "='UNKNOWN'";

        where_clause.add( boost::algorithm::join( parts, " OR " ) );
    }

    *sql_out = where_clause.getString();
}


void RasOptions::bindParameters(
        cxxdb::QueryStatementPtr stmt_ptr
    ) const
{
    cxxdb::Parameters &params(stmt_ptr->parameters());

    _block.bindParameters( params );
    _category.bindParameters( params );
    _component.bindParameters( params );
    _interval.bindParameters( params );
    _location.bindParameters( params );
    _message.bindParameters( params );
    _msg_id.bindParameters( params );
    _serialnumber.bindParameters( params );

    if ( _control_actions.count( "action" ) ) {

        // Don't bind any parameters.

    } else {

        unsigned i(1);
        BOOST_FOREACH( const auto& str, _control_actions ) {
            params[string() + "ctlAction_" + lexical_cast<string>(i++)].set( string() + "%" + str + "%" );
        }

    }

    if ( _jobid.get() )  params["jobId"].cast( *(_jobid.get()) );
}


std::string RasOptions::calcSortClauseSql() const
{
    return _s_sort_info.calcSortClause( _sort_spec );
}


} } // namespace bgws::query
