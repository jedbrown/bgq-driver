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


#include "Alerts.hpp"

#include "../BlockingOperationsThreadPool.hpp"
#include "../dbConnectionPool.hpp"
#include "../SortInfo.hpp"

#include "../responder/Alert.hpp"

#include "../teal/Teal.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/bind.hpp>

#include <boost/algorithm/string.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace query {


void Alerts::executeAsync(
        const AlertsOptions& options,
        ResultFn result_fn
    )
{
    _blocking_operations_thread_pool.post( boost::bind(
            &Alerts::_doQuery, this,
            options, result_fn
        ) );
}


void Alerts::_doQuery(
        const AlertsOptions& options,
        ResultFn result_fn
    )
{
    try {

        static const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of
                ( "recId", "\"rec_id\"" )
                ( "alertId", "\"alert_id\"" )
                ( "created", "\"creation_time\"" )
                ( "severity", "\"severity\"" )
                ( "urgency", "\"urgency\"" )
                ( "eventLocationType", "\"event_loc_type\"" )
                ( "eventLocation", "\"event_loc\"" )
                ( "fruLocation", "\"fru_loc\"" )
                ( "recommendation", "\"recommendation\"" )
                ( "reason", "\"reason\"" )
                ( "source", "\"src_name\"" )
                ( "state", "\"state\"" )
            ;

        static const SortInfo::KeyCols KEY_COLS = { "\"rec_id\"" };
        static const string DEFAULT_COLUMN_NAME( "\"rec_id\"" );
        static const utility::SortDirection::Value DEFAULT_DIRECTION(utility::SortDirection::Descending);

        static const SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );


        json::ArrayValuePtr arr_val_ptr(json::Array::create());
        auto &arr(arr_val_ptr->get());

        auto conn_ptr(dbConnectionPool::getConnection());


        WhereClause where_clause;
        cxxdb::ParameterNames param_names;

        if ( ! options.dups ) {
            where_clause.add( "\"rec_id\" NOT IN ( SELECT \"t_alert_recid\" FROM x_tealAlert2Alert WHERE \"assoc_type\"='D' )" );
        }
        if ( ! options.location_types.empty() ) {
            where_clause.add( string() + "\"event_loc_type\" IN ( '" + boost::algorithm::join( options.location_types, "','" ) + "')" );
        }
        if ( ! options.states.empty() ) {
            where_clause.add( string() + "\"state\" IN (" + boost::algorithm::join( options.states, "," ) + ")" );
        }
        if ( ! options.severities.empty() ) {
            where_clause.add( string() + "\"severity\" IN ( '" + boost::algorithm::join( options.severities, "','" ) + "' )" );
        }
        if ( ! options.urgencies.empty() ) {
            where_clause.add( string() + "\"urgency\" IN ( '" + boost::algorithm::join( options.urgencies, "','" ) + "' )" );
        }

        options.interval.addTo( where_clause, param_names, "\"creation_time\"" );
        options.location.addTo( where_clause, param_names, "\"event_loc\"" );


        uint64_t total_count(0);

        {
            string sql = string() +

     "SELECT COUNT(*) AS c"
      " FROM x_tealalertlog"
      + where_clause.getString()

                ;

            LOG_DEBUG_MSG( "Preparing '" << sql << "'" );

            auto stmt_ptr(conn_ptr->prepareQuery( sql, param_names ));

            options.interval.bindParameters( stmt_ptr->parameters() );
            options.location.bindParameters( stmt_ptr->parameters() );

            auto rs_ptr(stmt_ptr->execute());

            if ( rs_ptr->fetch() ) {
                total_count = rs_ptr->columns()["c"].as<uint64_t>();
            }

            LOG_DEBUG_MSG( "Count=" << total_count );
        }

        if ( total_count > 0 ) {

            string sort_clause_sql(sort_info.calcSortClause( options.sort_spec ));

            string sql;

            sql +=
    "WITH"
                ;

            if ( options.dups ) {

                sql +=
    " dups AS ("

    " SELECT \"t_alert_recid\" AS dupRecId, \"alert_recid\" AS duplicateOf"
      " FROM x_tealalert2alert"
      " WHERE \"assoc_type\" = 'D'"

    " ),"
                    ;

            }

            sql +=
    " all_ordered AS ("

    " SELECT \"rec_id\", \"alert_id\", \"creation_time\", \"severity\", \"urgency\", \"event_loc_type\", \"event_loc\", \"fru_loc\", \"recommendation\", \"reason\", \"src_name\", \"state\", \"raw_data\","
           " ROW_NUMBER() OVER ( ORDER BY " + sort_clause_sql + " ) AS row_num"
      " FROM x_tealalertlog"
      + where_clause.getString() +

    " )"

    " SELECT a.\"rec_id\", a.\"alert_id\", a.\"creation_time\", a.\"severity\", a.\"urgency\", a.\"event_loc_type\", a.\"event_loc\", a.\"fru_loc\", a.\"recommendation\", a.\"reason\", a.\"src_name\", a.\"state\", a.\"raw_data\","
                ;

            if ( options.dups ) {
                sql +=
           " dups.duplicateOf"
                    ;
            } else {
                sql +=
           " CAST(NULL AS INTEGER) AS duplicateOf"
                    ;
            }

            sql +=
     " FROM all_ordered AS a"
                ;

            if ( options.dups ) {
                sql +=
          " LEFT OUTER JOIN"
          " dups"
          " ON a.\"rec_id\" = dups.dupRecId"
                    ;

            }

            sql +=
     " WHERE row_num BETWEEN ? AND ?"
     " ORDER BY row_num"
                ;

            param_names.push_back( "row_start" );
            param_names.push_back( "row_end" );

            LOG_DEBUG_MSG( "Preparing '" << sql << "'" );

            auto stmt_ptr(conn_ptr->prepareQuery( sql, param_names ));

            auto &parameters(stmt_ptr->parameters());
            options.interval.bindParameters( parameters );
            options.location.bindParameters( parameters );
            options.req_range.bindParameters( parameters, "row_start", "row_end" );

            auto rs_ptr(stmt_ptr->execute());

            while ( rs_ptr->fetch() ) {
                const auto &cols(rs_ptr->columns());

                auto &obj(arr.addObject());

                teal::Id rec_id(cols["rec_id"].as<teal::Id>());

                obj.set( "recId", rec_id );
                obj.set( "alertId", cols["alert_id"].getString() );
                obj.set( "created", cols["creation_time"].getTimestamp() );
                obj.set( "severity", cols["severity"].getString() );
                obj.set( "urgency", cols["urgency"].getString() );
                obj.set( "eventLocationType", cols["event_loc_type"].getString() );
                obj.set( "eventLocation", cols["event_loc"].getString() );
                if ( cols["fru_loc"] )  obj.set( "fruLocation", cols["fru_loc"].getString() );
                obj.set( "recommendation", cols["recommendation"].getString() );
                obj.set( "reason", cols["reason"].getString() );
                obj.set( "source", cols["src_name"].getString() );
                if ( cols["state"] )  obj.set( "state", cols["state"].as<teal::State>() );
                if ( cols["raw_data"] )  obj.set( "rawData", cols["raw_data"].getString() );
                if ( cols["duplicateOf"] ) obj.set( "duplicateOf", responder::Alert::calcPath( _path_base, cols["duplicateOf"].as<teal::Id>() ).toString() );
                obj.set( "URI", responder::Alert::calcPath( _path_base, rec_id ).toString() );
            }
        }

        result_fn(
                std::exception_ptr(),
                arr_val_ptr,
                total_count
            );

        return;

    } catch ( std::exception& e ) {

        result_fn(
                std::current_exception(),
                json::ArrayValuePtr(),
                0
            );

        return;

    }
}


} } // namespace bgws::query
