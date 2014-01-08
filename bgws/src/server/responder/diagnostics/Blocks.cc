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

/*! \page diagnosticsBlocksResource /bg/diagnostics/blocks

This resource provides a summary of blocks used for diagnostics.

- \subpage diagnosticsBlocksResourceGet

 */

/*! \page diagnosticsBlocksResourceGet GET /bg/diagnostics/blocks

Get a summary of blocks used for diagnostics.
For each block, the most recent result is returned.

\section Authority

The user must have hardware READ authority.

\section diagnosticsBlocksResourceGetParameters Query parameters

- type: one of midplane, ioDrawer, or user.

\section diagnosticsBlocksResourceGetResponse JSON response format

<pre>
[
  {
    "blockId" : &quot;<i>string</i>&quot;,
    "location" : &quot;<i>string</i>&quot;, // optional
    "runId" : &quot;<i>string</i>&quot;,
    "start" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // optional
    "end" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "hardwareStatus" : &quot;<i>string</i>&quot;,
    "URI" : &quot;<i>\ref diagnosticsBlockResource</i>&quot;
  },
  ...
]
</pre>

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.


 */


#include "Blocks.hpp"

#include "Block.hpp"
#include "common.hpp"

#include "../../blue_gene.hpp"
#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <boost/algorithm/string.hpp>

#include <string>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace diagnostics {


const capena::http::uri::Path Blocks::RESOURCE_PATH(common::getBaseUrlPath() / "blocks");
const capena::http::uri::Path Blocks::RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");


void Blocks::_doGet()
{
    if ( ! _userHasHardwareRead() ) {
        LOG_WARN_MSG( "Could not get diagnostics blocks because " << _getRequestUserInfo() << " doesn't have authority." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get diagnostics blocks because the user doesn't have authority.",
                "getDiagnosticsBlocks", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }

    namespace po = boost::program_options;

    string type;

    po::options_description desc;

    desc.add_options()
            ( "type", po::value( &type ) )
        ;

    po::variables_map vm;
    po::store( po::command_line_parser( _getRequest().getUrl().getQuery().calcArguments() ).options( desc ).allow_unregistered().run(), vm );
    po::notify( vm );

    // Query to get the most recent result for each midplane.

    string block_sel_sql = "";

    if ( type == "midplane" ) {
        block_sel_sql = string() + "blockId LIKE '" + blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX + "R__-M_ %' AND";
    } else if ( type == "ioDrawer" ) {
        block_sel_sql = string() + "blockId LIKE '" + blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX + "___-I_ %' AND";
    } else if ( type == "user" ) {
        block_sel_sql = string() + "blockId NOT LIKE '" + blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX + "%' AND";
    } else {
        block_sel_sql = "";
    }

    LOG_DEBUG_MSG( "type='" << type << "' -> '" << block_sel_sql << "'" );

    string sql = string() +

 "WITH mr AS"
" ("

" SELECT blockId, MAX(endTime) AS endTime"
  " FROM bgqDiagBlocks"
  " WHERE " + block_sel_sql + " endtime IS NOT NULL"
  " GROUP BY blockId"

" )"
" SELECT db.*"
  " FROM bgqDiagBlocks AS db"
       " JOIN"
       " mr"
       " ON db.blockId = mr.blockId AND db.endTime = mr.endTime"
" ORDER BY blockId"

        ;

    auto conn_ptr(dbConnectionPool::getConnection());

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql ));

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    json::ArrayValue arr_val;
    json::Array &arr(arr_val.get());

    while ( rs_ptr->fetch() )
    {
        json::Object &obj(arr.addObject());

        const cxxdb::Columns &cols(rs_ptr->columns());


        string block_id(cols["blockId"].getString());

        obj.set( "blockId", block_id );
        obj.set( "URI", Block::calcUri( _getDynamicConfiguration().getPathBase(), block_id ).toString() );

        if ( boost::algorithm::starts_with( block_id, blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX ) ) {
            obj.set( "location", block_id.substr( blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX.length() ) );
        }

        obj.set( "runId", lexical_cast<string>(cols["runId"].as<int64_t>()) );

        if ( cols["startTime"] )  obj.set( "start", cols["startTime"].getTimestamp() );
        obj.set( "end", cols["endTime"].getTimestamp() );
        obj.set( "hardwareStatus", cols["hardwareStatus"].getString() );
    }


    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( arr_val, response.out() );
}


} } } // namespace bgws::responder::diagnostics
