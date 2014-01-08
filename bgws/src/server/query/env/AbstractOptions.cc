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

#include "AbstractOptions.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>


using namespace boost::assign;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace query {
namespace env {


const std::string AbstractOptions::_QUAL( "env." );


AbstractOptions::AbstractOptions(
        unsigned location_column_size
    ) :
        _interval( "interval" ),
        _location( "location", location_column_size, BGQDB::DBObj::ColumnType::Char )
{
    // Nothing to do.
}


void AbstractOptions::setArgs(
        const std::vector<std::string>& args,
        const RequestRange& range
    )
{
    _sort_info_ptr = _createSortInfo();

    _range_ptr.reset( new RequestRange( range ) );

    namespace po = boost::program_options;

    po::options_description desc;

    desc.add_options()
            ( "sort", po::value( &_sort_spec ) )
        ;

    _interval.addTo( desc );
    _location.addTo( desc );

    _addOptions( desc );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( desc ).allow_unregistered().run(), vm );
    po::notify( vm );
}


void AbstractOptions::calcWhereClauseSql(
        bool is_count,
        std::string* where_clause_sql_out,
        cxxdb::ParameterNames* parameter_names_out
    ) const
{
    WhereClause where_clause;

    _interval.addTo( where_clause, *parameter_names_out, _QUAL + BGQDB::DBTServicecardenvironment::TIME_COL ); // Used service card as sample environmental table.
    _location.addTo( where_clause, *parameter_names_out, _QUAL + BGQDB::DBTServicecardenvironment::LOCATION_COL );

    _addOptions( where_clause, *parameter_names_out );

    if ( ! is_count ) {
        *parameter_names_out += "rangeStart";
        *parameter_names_out += "rangeEnd";
    }

    *where_clause_sql_out = where_clause.getString();
}


std::string AbstractOptions::calcSortClauseSql() const
{
    string ret(_sort_info_ptr->calcSortClause( _sort_spec ));

    LOG_TRACE_MSG( "sort clause=" << ret );

    return ret;
}


void AbstractOptions::bindParameters(
        bool is_count,
        cxxdb::QueryStatementPtr stmt_ptr
    )
{
    _interval.bindParameters( stmt_ptr->parameters() );
    _location.bindParameters( stmt_ptr->parameters() );

    _bindParameters( stmt_ptr->parameters() );

    if ( ! is_count ) {
        _range_ptr->bindParameters( stmt_ptr->parameters(), "rangeStart", "rangeEnd" );
    }
}


boost::shared_ptr<SortInfo> AbstractOptions::_createSortInfo()
{
    const string QUALIFIED_LOCATION_COL_NAME(_QUAL + BGQDB::DBTServicecardenvironment::LOCATION_COL);
    const string QUALIFIED_TIME_COL_NAME(_QUAL + BGQDB::DBTServicecardenvironment::TIME_COL);

    SortInfo::IdToCol id_to_col = boost::assign::map_list_of
            ( "location", QUALIFIED_LOCATION_COL_NAME )
            ( "time", QUALIFIED_TIME_COL_NAME )
        ;

    _addExtraMappings( id_to_col );

    SortInfo::KeyCols key_cols;
    key_cols.push_back( QUALIFIED_LOCATION_COL_NAME );

    if ( getExtraKeyColumnName() != string() ) {
        key_cols.push_back( _QUAL + getExtraKeyColumnName() );
    }

    key_cols.push_back( QUALIFIED_TIME_COL_NAME );

    const string DEFAULT_COLUMN_NAME( QUALIFIED_LOCATION_COL_NAME );
    const bgws::utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Ascending);

    boost::shared_ptr<SortInfo> ret( new SortInfo( id_to_col, key_cols, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION ) );

    return ret;
}


} } } // namespace bgws::query::env
