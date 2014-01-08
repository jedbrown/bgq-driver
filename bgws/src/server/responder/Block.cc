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

/*! \page blockResource /bg/blocks/<i>blockId</i>

This resource provides operations on an individual block in the system.

- \subpage blockResourceGet
- \subpage blockResourceDelete

 */

/*! \page blockResourceDelete DELETE /bg/blocks/<i>blockId</i>

Remove the block from the system.

\section Authority

The user must have DELETE authority to the block.

\section Response

HTTP status: 204 No Content

\section Errors

HTTP status: 400 Bad Request
- invalidId: The block ID is not valid.
- notFree: The block is not Free so can't be deleted.

HTTP status: 403 Forbidden
- notAuthenticated: User isn't authenticated.
- authority: The use doesn't have DELETE authority to the block.

HTTP status: 404 Not Found
- notFound: The block doesn't exist or the user doesn't have READ authority to the block.

 */

/*! \page blockResourceGet GET /bg/blocks/<i>blockId</i>

Get details for a block.

\section Authority

The user must have READ authority to the block.

\section blockResourceGetOutput JSON response format

<pre>
{
  "id" : &quot;<i>blockId</i>&quot;,
  "description" : &quot;<i>string</i>&quot;
  "createTime" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
  "status" : &quot;<i>status</i>&quot;,
  "statusChangeTime" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
  "owner" : &quot;<i>string</i>&quot;,
  "user" : &quot;<i>string</i>&quot;, // Optional.
  "bootOptions" : &quot;<i>string</i>&quot;, // Optional.
  "microloaderImage" : &quot;<i>string</i>&quot;, // Optional.
  "nodeConfiguration" : &quot;<i>string</i>&quot;,
  "options" : &quot;<i>string</i>&quot;, // Optional.
  "errorMessage": &quot;<i>string</i>&quot;, // Optional.

  // These values are present for compute blocks

  "computeNodeCount" : <i>number</i>,
  "shape" : &quot;<i>shapeSpec</i>&quot;,
  "torus" : &quot;<i>torusSpec</i>&quot;,
  "jobCount" : <i>number</i>,

  // These values are present for large compute blocks

  "midplanes" : [ &quot;<i>midplaneLocation</i>&quot;, ... ],
  "passthrough" : [ &quot;<i>midplaneLocation</i>&quot;, ... ] // Optional, present only if there are any passthrough midplanes.

  // These values are present for small compute blocks

  "midplane" : &quot;<i>midplaneLocation</i>&quot;,
  "nodeBoards" : [ &quot;<i>nodeBoardPosition</i>&quot;, ... ]

  // These values are present for I/O blocks.

  "ioNodeCount" : <i>number</i>,
  "locations" : [ &quot;<i>location</i>&quot;, ... ]

}
</pre>

Where

- <i>status</i> Is the block status, one of F = Free, A = Allocating, B = Booting, I = Initialized, T = Terminating.

- <i>shapeSpec</i> Is <i>A</i>x<i>B</i>x<i>C</i>x<i>D</i>x<i>E</i>, where each element is the size in the dimension.

- <i>torusSpec</i> is a string that contains the dimension id (A, B, C, D, and E) only if the block is a torus in the dimension.
For example, if the block is a torus in all dimensions, then the torusSpec is "ABCDE". If the block is a torus in all dimensions
but D, then the torusSpec is "ABCE".


\section Errors

HTTP status: 400 Bad Request
- invalidId: The block ID is not valid.

HTTP status: 403 Forbidden
- notAuthenticated: User isn't authenticated.

HTTP status: 404 Not Found
- notFound: The block doesn't exist.

 */


#include "Block.hpp"

#include "Blocks.hpp"

#include "../dbConnectionPool.hpp"
#include "../Error.hpp"

#include "../utility/utility.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/BGQDBlib.h>

#include <hlcs/include/security/exception.h>
#include <hlcs/include/security/wrapper.h>

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>


using boost::lexical_cast;

using std::set;
using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


bool Block::matchesUrl(
        const capena::http::uri::Path& requested_resource
    )
{
    // Check is /bg/blocks/<blockId> (parent is Blocks)

    if ( requested_resource.empty() ) {
        return false;
    }

    if ( requested_resource.back().empty() ) {
        return false;
    }

    return Blocks::matchesUrl( requested_resource.calcParent() ); // parent is blocks.
}


capena::http::uri::Path Block::calcPath(
        const capena::http::uri::Path& path_base,
        const std::string& block_id
    )
{
    return (path_base / Blocks::RESOURCE_PATH / block_id);
}


capena::http::Methods Block::_getAllowedMethods() const
{
    return { capena::http::Method::GET, capena::http::Method::DELETE };
}


void Block::_doGet()
{
    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get block because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get block because the user isn't authenticated.",
                "getBlock", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );
    }

    bool is_block_id_valid;
    string block_id(_calcBlockId( &is_block_id_valid ));

    if ( ! is_block_id_valid ) {
        Error::Data error_data;
        error_data["blockId"] = block_id;

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not get details for block '%1%' because the block ID is not valid." ) % block_id ),
                "getBlockDetails", "invalidId", error_data,
                capena::http::Status::BadRequest
            ) );

    }

    _getBlockDetails( block_id );
}


void Block::_doDelete()
{
    bool is_block_id_valid;
    string block_id(_calcBlockId( &is_block_id_valid ));

    if ( ! is_block_id_valid ) {
        Error::Data error_data;
        error_data["blockId"] = block_id;

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not delete '%1%' because the block ID is not valid." ) % block_id ),
                "deleteBlock", "invalidId", error_data,
                capena::http::Status::BadRequest
            ) );

    }

    _deleteBlock( block_id );
}


std::string Block::_calcBlockId( bool* is_valid_out ) const
{
    const string &block_id(_getRequestedResourcePath().back());
    *is_valid_out = BGQDB::isBlockIdValid( block_id, __FUNCTION__ );
    return block_id;
}


void Block::_getBlockDetails(
        const std::string& block_id
    )
{
    json::ValuePtr obj_val_ptr(_queryBlock( block_id ));

    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( *obj_val_ptr, response.out() );
}


json::ValuePtr Block::_queryBlock( const std::string& block_id )
{
    auto conn_ptr(dbConnectionPool::getConnection());

    static const std::string SQL(

"SELECT"
    " numCnodes,"
    " numIoNodes,"
    " owner,"
    " userName,"
    " isTorus,"
    " sizeA,"
    " sizeB,"
    " sizeC,"
    " sizeD,"
    " sizeE,"
    " description,"
    " options,"
    " status,"
    " statusLastModified,"
    " mloaderImg,"
    " nodeConfig,"
    " bootOptions,"
    " createDate,"
    " errtext"
 " FROM bgqBlock WHERE blockId=?"

        );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery(
            SQL,
            cxxdb::ParameterNames{ "blockId" }
        ));

    stmt_ptr->parameters()["blockId"].set( block_id );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    if ( ! rs_ptr->fetch() ) {
        // Failed to find the block.

        Error::Data data;
        data["blockId"] = block_id;

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not get details for block '%1%' because the block does not exist." ) % block_id ),
                "getBlockDetails", "notFound", data,
                capena::http::Status::NotFound
            ) );

    }

    // Found the block...


    // Make sure the user has READ authority to the block.
    if ( ! (_isUserAdministrator() ||
            _getEnforcer().validate(
             hlcs::security::Object(
                    hlcs::security::Object::Block,
                    block_id
             ),
             hlcs::security::Action::Read,
             _getRequestUserId()
         )) )
    {
        // don't send this block because the user doesn't have authority.
        LOG_WARN_MSG( "Not sending block '" << block_id << "' because the user doesn't have authority." );

        // The error looks just like if the block doesn't exist.

        Error::Data data;
        data["blockId"] = block_id;

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not get details for block '%1%' because the block does not exist." ) % block_id ),
                "getBlockDetails", "notFound", data,
                capena::http::Status::NotFound
            ) );
    }


    json::ObjectValuePtr obj_val_ptr(json::Object::create());
    json::Object &block_obj(obj_val_ptr->get());

    block_obj.set( "id", block_id );

    uint64_t compute_node_count(rs_ptr->columns()["numCnodes"].as<uint64_t>());

    if ( compute_node_count != 0 ) {
        block_obj.set( "computeNodeCount", compute_node_count );
    }

    uint64_t io_node_count(rs_ptr->columns()["numIoNodes"].as<uint64_t>());

    if ( io_node_count != 0 ) {
        block_obj.set( "ioNodeCount", io_node_count );
    }

    block_obj.set( "owner", rs_ptr->columns()["owner"].getString() );

    if ( ! rs_ptr->columns()["userName"].isNull() ) {
        const string user_name(rs_ptr->columns()["userName"].getString());

        if ( ! user_name.empty() )  block_obj.set( "user", user_name );
    }

    if ( ! rs_ptr->columns()["isTorus"].isNull() ) {
        string torus_val(rs_ptr->columns()["isTorus"].getString());
        string torus_out;
        if ( torus_val[0] == '1' ) torus_out += 'A';
        if ( torus_val[1] == '1' ) torus_out += 'B';
        if ( torus_val[2] == '1' ) torus_out += 'C';
        if ( torus_val[3] == '1' ) torus_out += 'D';
        if ( torus_val[4] == '1' ) torus_out += 'E';

        block_obj.set( "torus", torus_out );
    }

    uint64_t sizea(rs_ptr->columns()["sizeA"].as<uint64_t>());
    uint64_t sizeb(rs_ptr->columns()["sizeB"].as<uint64_t>());
    uint64_t sizec(rs_ptr->columns()["sizeC"].as<uint64_t>());
    uint64_t sized(rs_ptr->columns()["sizeD"].as<uint64_t>());
    uint64_t sizee(rs_ptr->columns()["sizeE"].as<uint64_t>());

    if ( sizea != 0 || sizeb != 0 || sizec != 0 || sized != 0 || sizee != 0 ) {
        string shape_str(
                lexical_cast<string>(sizea) + "x" +
                lexical_cast<string>(sizeb) + "x" +
                lexical_cast<string>(sizec) + "x" +
                lexical_cast<string>(sized) + "x" +
                lexical_cast<string>(sizee)
            );
        block_obj.set( "shape", shape_str );
    }

    block_obj.set( "description", rs_ptr->columns()["description"].getString() );
    if ( ! rs_ptr->columns()["options"].getString().empty() ) {
        block_obj.set( "options", rs_ptr->columns()["options"].getString() );
    }
    block_obj.set( "status", string() + rs_ptr->columns()["status"].getChar() );
    block_obj.set( "statusChangeTime", rs_ptr->columns()["statusLastModified"].getTimestamp() );
    if ( ! rs_ptr->columns()["mloaderImg"].isNull() ) {
        block_obj.set( "microloaderImage", rs_ptr->columns()["mloaderImg"].getString() );
    }
    block_obj.set( "nodeConfiguration", rs_ptr->columns()["nodeConfig"].getString() );

    string boot_options;
    if ( ! rs_ptr->columns()["bootOptions"].isNull() )  boot_options = rs_ptr->columns()["bootOptions"].getString();
    if ( ! boot_options.empty() ) {
        block_obj.set( "bootOptions", boot_options );
    }

    block_obj.set( "createTime", rs_ptr->columns()["createDate"].getTimestamp() );

    if ( ! rs_ptr->columns()["errText"].isNull() ) {
        const string err_text(rs_ptr->columns()["errText"].getString());

        if ( ! err_text.empty() )  block_obj.set( "errorMessage", err_text );
    }

    if ( compute_node_count != 0 ) {
        if ( compute_node_count < BGQDB::Nodes_Per_Midplane ) {
            // It's a small block.

            _addSmallBlockInfo(
                    block_id,
                    obj_val_ptr,
                    conn_ptr
                );

        } else {
            // It's a large block.

            _addLargeBlockInfo(
                    block_id,
                    obj_val_ptr,
                    conn_ptr
                );
        }

        _addJobCount(
                block_id,
                obj_val_ptr,
                conn_ptr
            );
    }

    if ( io_node_count != 0 ) {

        _addIoBlockInfo(
                block_id,
                obj_val_ptr,
                conn_ptr
            );

    }

    return obj_val_ptr;
}


void Block::_addSmallBlockInfo(
        const std::string& block_id,
        json::ObjectValuePtr obj_val_ptr,
        cxxdb::ConnectionPtr conn_ptr
    )
{
    static const std::string SQL(

"SELECT"
    " posInMachine,"
    " nodeCardPos"
 " FROM bgqSmallBlock WHERE blockId=?"
 " ORDER BY nodeCardPos"

        );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery(
            SQL,
            cxxdb::ParameterNames{ "blockId" }
        ));

    stmt_ptr->parameters()["blockId"].set( block_id );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    json::Object &block_obj(obj_val_ptr->get());

    json::Array &nb_arr(block_obj.createArray( "nodeBoards" ));

    bool set_midplane(true);

    while ( rs_ptr->fetch() ) {

        if ( set_midplane ) {
            block_obj.set( "midplane", rs_ptr->columns()["posInMachine"].getString() );
            set_midplane = false;
        }

        nb_arr.add( rs_ptr->columns()["nodeCardPos"].getString() );
    }
}


void Block::_addLargeBlockInfo(
        const std::string& block_id,
        json::ObjectValuePtr obj_val_ptr,
        cxxdb::ConnectionPtr conn_ptr
    )
{
    static const std::string SQL(

 "WITH sbm AS ("

" SELECT DISTINCT SUBSTR(switchId,3,6) AS mp"
   " FROM bgqSwitchBlockMap"
   " WHERE blockId=?"

" ),"
" bbm AS ("

" SELECT bpid"
  " FROM bgqBpBlockMap"
  " WHERE blockId=?"

" )"
" SELECT sbm.mp,"
       " CASE WHEN bbm.bpid IS NULL THEN 'T' ELSE 'F' END AS pt"
  " FROM sbm LEFT OUTER JOIN bbm ON sbm.mp = bbm.bpid"

        );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery(
            SQL,
            cxxdb::ParameterNames{ "blockId", "blockId" }
        ));

    stmt_ptr->parameters()["blockId"].set( block_id );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    json::Object &block_obj(obj_val_ptr->get());

    json::Array &mp_arr(block_obj.createArray( "midplanes" ));
    json::Array *pt_arr(NULL);

    while ( rs_ptr->fetch() ) {

        string mp(rs_ptr->columns()["mp"].getString());

        if ( rs_ptr->columns()["pt"].getString() == "T" ) {
            // This is passthrough

            if ( ! pt_arr ) {
                pt_arr = &(block_obj.createArray( "passthrough" ));
            }

            pt_arr->add( mp );

            continue;
        }

        mp_arr.add( mp );
    }
}


void Block::_addJobCount(
        const std::string& block_id,
        json::ObjectValuePtr obj_val_ptr,
        cxxdb::ConnectionPtr conn_ptr
    )
{
    static const std::string SQL(

 "SELECT COUNT(*) AS c"
  " FROM bgqJob"
  " WHERE blockId=?"

        );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery(
            SQL,
            cxxdb::ParameterNames{ "blockId" }
        ));

    stmt_ptr->parameters()["blockId"].set( block_id );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    json::Object &block_obj(obj_val_ptr->get());

    if ( rs_ptr->fetch() ) {
        block_obj.set( "jobCount", 0 );
    } else {
        block_obj.set( "jobCount", rs_ptr->columns()["c"].as<uint64_t>() );
    }
}


void Block::_addIoBlockInfo(
        const std::string& block_id,
        json::ObjectValuePtr obj_val_ptr,
        cxxdb::ConnectionPtr conn_ptr
    )
{
    static const std::string SQL(

"SELECT"
    " location"
 " FROM bgqIoBlockMap WHERE blockId=?"

        );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery(
            SQL,
            cxxdb::ParameterNames{ "blockId" }
        ));

    stmt_ptr->parameters()["blockId"].set( block_id );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    json::Object &block_obj(obj_val_ptr->get());

    json::Array &locs_arr(block_obj.createArray( "locations" ));

    while ( rs_ptr->fetch() ) {
        locs_arr.add( rs_ptr->columns()["location"].getString() );
    }
}


void Block::_deleteBlock(
        const std::string& block_id
    )
{
    // The user must be authenticated to delete a block, return 403 Forbidden.
    // If the block exists and the user has DELETE authority then delete the block and return 204 No Content.
    // If the block exists and the user does not have DELETE authority and the user has READ authority then return 403 Forbidden.
    // If the block exists and the user does not have DELETE authority and the user does not have READ authority then return 404 Not Found.
    // If the block doesn't exist then return 404 Not Found.

    LOG_DEBUG_MSG( _getRequestUserInfo() << " attempting to delete block '" << block_id << "'" );

    Error::Data error_data;
    error_data["blockId"] = block_id;

    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot delete block because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                string() + "Cannot delete block because the user isn't authenticated.",
                "deleteBlock", "authority", error_data,
                capena::http::Status::Forbidden
            ) );
    }

    // The user is authenticated.

    error_data["userName"] = getRequestUserName();

    BGQDB::STATUS db_status;

    bool user_has_read(_isUserAdministrator());

    try {

        if ( ! user_has_read ) {
            user_has_read = _getEnforcer().validate(
                    hlcs::security::Object(
                            hlcs::security::Object::Block,
                            block_id
                        ),
                    hlcs::security::Action::Read,
                    _getRequestUserId()
                );
        }

        bool user_has_delete(_isUserAdministrator() || _getEnforcer().validate(
                hlcs::security::Object(
                        hlcs::security::Object::Block,
                        block_id
                    ),
                hlcs::security::Action::Delete,
                _getRequestUserId()
            ));

        if ( ! user_has_delete ) {

            LOG_WARN_MSG( _getRequestUserInfo() << " doesn't have authority to delete block '" << block_id << "'" );

            // If the user has READ authority to the block then say 403 Forbidden.
            // The user doesn't have READ authority to the block then report 404 Not Found.

            if ( user_has_read )
            {
                // The user has READ authority to the block, so send back 403 Forbidden.

                BOOST_THROW_EXCEPTION( Error(
                        boost::str( boost::format( "Cannot delete block '%1%' because the user doesn't have authority." ) % block_id ),
                        "deleteBlock",
                        "authority",
                        error_data,
                        capena::http::Status::Forbidden
                    ) );

            }

            // The user doesn't have READ authority to the block, so respond with 404 Not Found.

            BOOST_THROW_EXCEPTION( Error(
                    boost::str( boost::format( "Could not delete '%1%' because the block does not exist." ) % block_id ),
                    "deleteBlock",
                    "notFound",
                    error_data,
                    capena::http::Status::NotFound
                ) );

        }

        // user has delete authority.

        db_status = BGQDB::deleteBlock( block_id );

        if ( db_status == BGQDB::OK ) {

            LOG_INFO_MSG( _getRequestUserInfo() << " deleted block '" << block_id << "'" );

            capena::server::Response &response(_getResponse());

            response.setStatus( capena::http::Status::NoContent );
            response.headersComplete();

            return;
        }

    } catch ( hlcs::security::exception::ObjectNotFound &/*e*/ ) {

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not delete '%1%' because the block does not exist." ) % block_id ),
                "deleteBlock",
                "notFound",
                error_data,
                capena::http::Status::NotFound
            ) );

    }

    // If got here then failed, db_status indicates error. (The user does have DELETE authority to the block.)

    if ( db_status == BGQDB::FAILED ) {

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not delete '%1%' because the block is not Free." ) % block_id ),
                "deleteBlock", "notFree", error_data,
                capena::http::Status::BadRequest
            ) );

    }

    if ( db_status == BGQDB::INVALID_ID ) {

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not delete '%1%' because the block name is not valid." ) % block_id ),
                "deleteBlock", "invalidId", error_data,
                capena::http::Status::BadRequest
            ) );

    }

    if ( db_status == BGQDB::NOT_FOUND ) {

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not delete '%1%' because the block does not exist." ) % block_id ),
                "deleteBlock", "notFound", error_data,
                capena::http::Status::NotFound
            ) );

    }

    // Otherwise internal error.

    error_data["rc"] = lexical_cast<string>( db_status );

    BOOST_THROW_EXCEPTION( Error(
            boost::str( boost::format( "Failed to delete '%1%' because of an internal error."
                    " BG database error code = %2%" ) %
                    block_id %
                    lexical_cast<string>( db_status )
                ),
            "deleteBlock", "internalError", error_data,
            capena::http::Status::InternalServerError
        ) );
}


} } // namespace bgws::responder
