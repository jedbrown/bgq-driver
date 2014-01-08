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


/*! \page hardwareReplacementsResource /bg/hardwareReplacements

This resource is a container for hardware replacement records.

- \subpage hardwareReplacementsResourceGet

 */

/*! \page hardwareReplacementsResourceGet GET /bg/hardwareReplacements

Query hardware replacement records.

\note This resource supports \ref partialResponse "partial responses".


\section Authority

The user must have hardware READ authority.


\section hardwareReplacementsResourceGetParameters Query parameters

- ecid: The ECID values to return, either newEcid or oldEcid.
- interval: a \ref timeIntervalFormat "time interval"
- location: \ref multiWildcardFormat "multi-wildcard string"
- serialnumber: The serial number values to return, either newSerialnumber or oldSerialnumber.
- types: The hardware types to return.
  - By default, all location types are returned.
  - A string where each character is a code for a hardware type.
    - M : Midplane
    - B : Node Board
    - A : Node Board DCA
    - N : Node
    - O : I/O Node
    - R : I/O Rack
    - D : I/O Drawer
    - S : Service Card
    - C : Clock Card
    - L : Link Chip
    - H : I/O Link Chip
    - P : Bulk Power
- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - direction is + for ascending or - for descending, the default is ascending.
  - field is one of:
    - time
    - type
    - location
    - oldSerialNumber
    - newSerialNumber
    - oldEcid
    - newEcid


\section hardwareReplacementsResourceGetResponse JSON response format

<pre>
[
  { "time" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "type" : &quot;<i>string</i>&quot;, // optional
    "location" : &quot;<i>string</i>&quot;, // optional
    "oldSerialNumber" : &quot;<i>string</i>&quot;,
    "newSerialNumber" : &quot;<i>string</i>&quot;,
    "oldEcid" : &quot;<i>string</i>&quot;, // optional
    "newEcid" : &quot;<i>string</i>&quot;, // optional
  },
  ...
]
</pre>


\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

 */



#include "HardwareReplacements.hpp"

#include "../dbConnectionPool.hpp"
#include "../Error.hpp"
#include "../MultiWildcardOption.hpp"
#include "../RequestRange.hpp"
#include "../SortInfo.hpp"
#include "../SortSpec.hpp"
#include "../StringDbColumnOption.hpp"
#include "../TimeIntervalOption.hpp"
#include "../WhereClause.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <set>
#include <string>

#include <stdint.h>


using std::set;
using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {

namespace statics {
    namespace type_codes {
        static const char Midplane('M');
        static const char NodeCard('B');
        static const char NodeBoardDCA('A');
        static const char Node('N');
        static const char IoNode('O');
        static const char IoRack('R');
        static const char IoDrawer('D');
        static const char ServiceCard('S');
        static const char ClockCard('C');
        static const char LinkChip('L');
        static const char IoLinkChip('H');
        static const char BulkPower('P');
    }

    static const unsigned NumberOfTypes(12);
}


void HardwareReplacements::_doGet()
{
    namespace po = boost::program_options;


    static const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of
            ( "time", BGQDB::DBTReplacement_history::ENTRYDATE_COL )
            ( "type", BGQDB::DBTReplacement_history::TYPE_COL )
            ( "location", BGQDB::DBTReplacement_history::LOCATION_COL )
            ( "oldSerialNumber", BGQDB::DBTReplacement_history::OLDSERIALNUMBER_COL )
            ( "newSerialNumber", BGQDB::DBTReplacement_history::NEWSERIALNUMBER_COL )
            ( "oldEcid", BGQDB::DBTReplacement_history::OLDECID_COL )
            ( "newEcid", BGQDB::DBTReplacement_history::NEWECID_COL )
        ;

    static const SortInfo::KeyCols KEY_COLS = { BGQDB::DBTReplacement_history::ENTRYDATE_COL };
    static const string DEFAULT_COLUMN_NAME( BGQDB::DBTReplacement_history::ENTRYDATE_COL );
    static const utility::SortDirection::Value DEFAULT_DIRECTION(utility::SortDirection::Descending);

    static const SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );


    _checkAuthority();


    const auto &request(_getRequest());

    static const unsigned DEFAULT_ITEM_COUNT(100), MAX_ITEM_COUNT(200);

    RequestRange req_range( request, DEFAULT_ITEM_COUNT, MAX_ITEM_COUNT );


    StringDbColumnOption ecid( "ecid", BGQDB::DBTReplacement_history::OLDECID_SIZE );
    TimeIntervalOption interval( "interval" );
    MultiWildcardOption location( "location", BGQDB::DBTReplacement_history::LOCATION_SIZE, BGQDB::DBTReplacement_history::LOCATION_COL_TYPE );
    StringDbColumnOption serialnumber( "serialnumber", BGQDB::DBTReplacement_history::OLDSERIALNUMBER_SIZE );
    SortSpec sort_spec;
    string types_str;

    po::options_description desc;
    desc.add_options()
            ( "sort", po::value( &sort_spec ) )
            ( "types", po::value( &types_str ) )
        ;

    ecid.addTo( desc );
    interval.addTo( desc );
    location.addTo( desc );
    serialnumber.addTo( desc );

    po::variables_map vm;
    po::store(
            po::command_line_parser( request.getUrl().getQuery().calcArguments() ).options( desc ).allow_unregistered().run(),
            vm
        );
    po::notify( vm );


    set<string> types;

    BOOST_FOREACH( char ch, types_str ) {
        if ( ch == statics::type_codes::Midplane )  types.insert( BGQDB::replacement_history_types::Midplane );
        else if ( ch == statics::type_codes::NodeCard )  types.insert( BGQDB::replacement_history_types::NodeCard );
        else if ( ch == statics::type_codes::NodeBoardDCA )  types.insert( BGQDB::replacement_history_types::NodeBoardDCA );
        else if ( ch == statics::type_codes::Node )  types.insert( BGQDB::replacement_history_types::Node );
        else if ( ch == statics::type_codes::IoNode )  types.insert( BGQDB::replacement_history_types::IoNode );
        else if ( ch == statics::type_codes::IoRack )  types.insert( BGQDB::replacement_history_types::IoRack );
        else if ( ch == statics::type_codes::IoDrawer )  types.insert( BGQDB::replacement_history_types::IoDrawer );
        else if ( ch == statics::type_codes::ServiceCard )  types.insert( BGQDB::replacement_history_types::ServiceCard );
        else if ( ch == statics::type_codes::ClockCard )  types.insert( BGQDB::replacement_history_types::ClockCard );
        else if ( ch == statics::type_codes::LinkChip )  types.insert( BGQDB::replacement_history_types::LinkChip );
        else if ( ch == statics::type_codes::IoLinkChip )  types.insert( BGQDB::replacement_history_types::IoLinkChip );
        else if ( ch == statics::type_codes::BulkPower )  types.insert( BGQDB::replacement_history_types::BulkPower );
    }

    if ( types.size() == statics::NumberOfTypes )  types.clear();


    WhereClause where_clause;
    cxxdb::ParameterNames param_names;

    if ( ecid.hasValue() ) {
        where_clause.add( "HEX(TRIM(oldEcid))=? OR HEX(TRIM(newEcid))=?" );
        param_names.push_back( "ecid" );
        param_names.push_back( "ecid" );
    }

    interval.addTo( where_clause, param_names, BGQDB::DBTReplacement_history::ENTRYDATE_COL );
    location.addTo( where_clause, param_names, "location" );

    if ( serialnumber.hasValue() ) {
        where_clause.add( "oldSerialNumber=? OR newSerialNumber=?" );
        param_names.push_back( "serialnumber" );
        param_names.push_back( "serialnumber" );
    }

    if ( ! types.empty() ) {
        where_clause.add( string() + "type IN ( '" + boost::algorithm::join( types, "','" ) + "')" );
    }


    auto conn_ptr(dbConnectionPool::getConnection());

    uint64_t total_count(0);

    {
        string sql = string() +

 "SELECT COUNT(*) AS c"
 " FROM bgqReplacement_history"
 + where_clause.getString()

            ;


        LOG_DEBUG_MSG( "preparing " << sql );

        auto stmt_ptr(conn_ptr->prepareQuery( sql, param_names ));

        auto &parameters(stmt_ptr->parameters());
        if ( ecid.hasValue() )  parameters["ecid"].set( ecid.getValue() );
        interval.bindParameters( parameters );
        location.bindParameters( parameters );
        if ( serialnumber.hasValue() ) parameters["serialnumber"].set( serialnumber.getValue() );

        auto rs_ptr(stmt_ptr->execute());

        if ( rs_ptr->fetch() ) {
            total_count = rs_ptr->columns()["c"].as<uint64_t>();
        }
    }


    json::ArrayValue arr_val;
    auto &arr(arr_val.get());

    if ( total_count > 0 ) {

        string sort_clause_sql(sort_info.calcSortClause( sort_spec ));

        string sql = string() +

 "WITH"
" all_ordered AS ("

" SELECT entryDate, type, location, oldSerialNumber, newSerialNumber, oldEcid, newEcid,"
       " ROW_NUMBER() OVER ( ORDER BY " + sort_clause_sql + " ) AS row_num"
  " FROM bgqReplacement_history"
  + where_clause.getString() +

" )"

" SELECT entryDate, type, location, oldSerialNumber, newSerialNumber, HEX(TRIM(oldEcid)) AS oldEcid, HEX(TRIM(newEcid)) AS newEcid"
  " FROM all_ordered"
  " WHERE row_num BETWEEN ? AND ?"
  " ORDER BY row_num";


        LOG_DEBUG_MSG( "preparing " << sql );

        param_names.push_back( "row_start" );
        param_names.push_back( "row_end" );

        auto stmt_ptr(conn_ptr->prepareQuery( sql, param_names ));

        auto &parameters(stmt_ptr->parameters());
        if ( ecid.hasValue() )  parameters["ecid"].set( ecid.getValue() );
        interval.bindParameters( parameters );
        location.bindParameters( parameters );
        if ( serialnumber.hasValue() ) parameters["serialnumber"].set( serialnumber.getValue() );
        req_range.bindParameters( parameters, "row_start", "row_end" );


        auto rs_ptr(stmt_ptr->execute());

        while ( rs_ptr->fetch() ) {
            const auto &cols(rs_ptr->columns());

            auto &obj(arr.addObject());

            obj.set( "time", cols[BGQDB::DBTReplacement_history::ENTRYDATE_COL].getTimestamp() );
            if ( cols["type"] )  obj.set( "type", cols["type"].getString() );
            if ( cols["location"] )  obj.set( "location", cols["location"].getString() );
            obj.set( "oldSerialNumber", cols["oldSerialNumber"].getString() );
            obj.set( "newSerialNumber", cols["newSerialNumber"].getString() );
            if ( cols["oldEcid"] )  obj.set( "oldEcid", cols["oldEcid"].getString() );
            if ( cols["newEcid"] )  obj.set( "newEcid", cols["newEcid"].getString() );
        }

    }


    auto &response(_getResponse());

    req_range.updateResponse( response, arr.size(), total_count );

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( arr_val, response.out() );
}


void HardwareReplacements::_checkAuthority()
{
    if ( _userHasHardwareRead() ) {
        return;
    }

    LOG_WARN_MSG( "Could not get replacement history because " << _getRequestUserInfo() << " doesn't have authority." );

    BOOST_THROW_EXCEPTION( Error(
            "Could not get replacement history because the user doesn't have authority.",
            "getReplacementHistory", "authority", Error::Data(),
            capena::http::Status::Forbidden
        ) );
}


}} // namespace bgws::responder
