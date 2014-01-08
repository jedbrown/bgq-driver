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


#include "RunsQueryOptions.hpp"

#include "../../blue_gene.hpp"
#include "../../SortInfo.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <boost/algorithm/string.hpp>

#include <set>
#include <string>
#include <vector>


using boost::lexical_cast;

using std::set;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace diagnostics {


RunsQueryOptions::RunsQueryOptions(
        const capena::http::uri::Query::Arguments& args
    ) :
        _end( "end", TimeIntervalOption::Nullable::Allow ),
        _start( "start" )
{
    namespace po = boost::program_options;

    string blocks_str;
    string statuses_str;

    po::options_description desc;

    desc.add_options()
            ( "blocks", po::value( &blocks_str ) )
            ( "sort", po::value( &_sort_spec ) )
            ( "status", po::value( &statuses_str ) )
        ;

    _end.addTo( desc );
    _start.addTo( desc );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( desc ).allow_unregistered().run(), vm );
    po::notify( vm );


    vector<string> blocks_vec;

    if ( ! blocks_str.empty() ) {
        boost::algorithm::split( blocks_vec, blocks_str, boost::algorithm::is_any_of(",") );
    }

    BOOST_FOREACH( const string& block, blocks_vec ) {
        if ( block.size() <= BGQDB::DBTDiagblocks::BLOCKID_SIZE ) {
            _blocks.insert( block );
        } else {
            LOG_WARN_MSG( "Ignoring diagnostics block '" << block << "' because the name is too long." );
        }
    }


    static const char STATUS_RUNNING_CODE('R'), STATUS_COMPLETED_CODE('C'), STATUS_FAILED_CODE('F'), STATUS_CANCELED_CODE('L');

    BOOST_FOREACH( char status_code, statuses_str ) {
        if ( status_code == STATUS_RUNNING_CODE )  { _statuses.insert( blue_gene::diagnostics::run_status_db::RUNNING ); }
        else if ( status_code == STATUS_COMPLETED_CODE )  { _statuses.insert( blue_gene::diagnostics::run_status_db::COMPLETED ); }
        else if ( status_code == STATUS_FAILED_CODE )  { _statuses.insert( blue_gene::diagnostics::run_status_db::FAILED ); }
        else if ( status_code == STATUS_CANCELED_CODE )  { _statuses.insert( blue_gene::diagnostics::run_status_db::CANCELED ); }
    }

    if ( _statuses.size() == 4 )  _statuses.clear();
}


void RunsQueryOptions::addToWhereClause(
        WhereClause* where_clause_out,
        cxxdb::ParameterNames* param_names_out,
        std::string* sort_clause_sql_out
    ) const
{
    static const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of
            ( "runId", BGQDB::DBTDiagruns::RUNID_COL )
            ( "end", BGQDB::DBTDiagruns::ENDTIME_COL )
            ( "start", BGQDB::DBTDiagruns::STARTTIME_COL )
            ( "status", BGQDB::DBTDiagruns::DIAGSTATUS_COL )
        ;

    static const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( BGQDB::DBTDiagruns::RUNID_COL );

    static const string DEFAULT_COLUMN_NAME( "runId" );
    static const utility::SortDirection::Value DEFAULT_DIRECTION(utility::SortDirection::Descending);

    static const SortInfo SORT_INFO( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );


    if ( ! _blocks.empty() ) {
        string blocks_sql = BGQDB::DBTDiagruns::RUNID_COL + " IN ( SELECT DISTINCT " + BGQDB::DBTDiagblocks::RUNID_COL + " FROM " + BGQDB::DBTDiagblocks().getTableName() + " WHERE ";
        for ( unsigned i = 0 ; i < _blocks.size() ; ++i ) {
            if ( i != 0 )  blocks_sql += " OR ";

            blocks_sql += BGQDB::DBTDiagblocks::BLOCKID_COL + " = ?";
            param_names_out->push_back( string() + "block_" + lexical_cast<string>(i) );
        }

        blocks_sql += " )";

        where_clause_out->add( blocks_sql );
    }

    _end.addTo( *where_clause_out, *param_names_out, BGQDB::DBTDiagruns::ENDTIME_COL );
    _start.addTo( *where_clause_out, *param_names_out, BGQDB::DBTDiagruns::STARTTIME_COL );

    if ( ! _statuses.empty() ) {
        string statuses_sql;
        bool first(true);

        BOOST_FOREACH( const string& status, _statuses ) {
            if ( first )  first = false;
            else  statuses_sql += " OR ";

            statuses_sql += BGQDB::DBTDiagruns::DIAGSTATUS_COL + " = '" + status + "'";
        }

        where_clause_out->add( statuses_sql );
    }


    *sort_clause_sql_out = SORT_INFO.calcSortClause( _sort_spec );
}


void RunsQueryOptions::bindParameters(
        cxxdb::QueryStatement& stmt
    ) const
{
    cxxdb::Parameters &params(stmt.parameters());

    {
        unsigned i=0;
        BOOST_FOREACH( const string& block, _blocks ) {
            params[string() + "block_" + lexical_cast<string>(i)].set( block );
            ++i;
        }
    }

    _end.bindParameters( params );
    _start.bindParameters( params );
}


bool RunsQueryOptions::includesRunning() const
{
    // If ?end=NULL, or no end then inclues running diagnostics.
    return (_end.getConfig() == TimeIntervalOption::Config::IsNull || _end.getConfig() == TimeIntervalOption::Config::NoValue);
}


} } } // namespace bgws::responder::diagnostics
