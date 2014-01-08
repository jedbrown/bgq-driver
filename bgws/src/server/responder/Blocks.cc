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


/*! \page blocksResource /bg/blocks

This resource is a container for the blocks defined on the system.

- \subpage blocksResourceGet
- \subpage blocksResourcePost

 */


/*! \page blocksResourceGet GET /bg/blocks

Get a summary of the blocks on the system.

\section Authority

The user must be authenticated.
The response will only contain the blocks to which the user has READ authority.


\section blocksResourceGetParameters Query parameters

- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - direction is + for ascending or - for descending, the default is ascending.
  - field is one of:
    - computeNodeCount
    - createDate
    - id / name
    - ioNodeCount
    - user
    - status
    - statusChanged

- type: The type of the block, either "compute" or "io", or "any" for either type of block. The default is "any".

- status: Only get blocks with the matching status.
  - The argument can contain multiple status values. All the values are or-ed together.
  - This option can also be specified multiple times. All the values are or-ed together.
  - Status values are:
    - F (Free)
    - A (Allocated)
    - B (Booting)
    - I (Initialized)
    - T (Terminating)


\section blocksResourceGetResponse JSON response format

<pre>
[
  {
    "id": &quot;<i>blockId</i>&quot;,
    "status": &quot;<i>status</i>&quot;,
    "statusSequenceId": <i>number</i>,
    "statusChanged": &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "user": &quot;<i>string</i>&quot;, // Optional
    "computeNodeCount": <i>number</i>, // Optional -- only on compute blocks.
    "ioNodeCount": <i>number</i>, // Optional -- only on I/O blocks.
    "description": &quot;<i>string</i>&quot;,
    "torus": &quot;<i>torusSpec</i>&quot;, // Optional -- only on compute blocks.
    "URI": &quot;<i>\ref blockResource</i>&quot;
  },
  ...
]
</pre>


\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: The user isn't authenticated.

 */


/*! \page blocksResourcePost POST /bg/blocks

Create a new block.

\section Authority

The user must have create block authority to create a block.


\section blocksResourcePostInput JSON request data format

<pre>
{
  "id": &quot;<i>blockId</i>&quot;,
  "description": &quot;<i>text</i>&quot;, // Optional, defaults to "Generated via web services."

  // These fields are for large blocks when using corner and dimension specs.

  "midplane": &quot;<i>midplaneLocation</i>&quot;,
  "midplanes": [ &quot;<i>midplaneIncludes</i>&quot;, ... ], // Optional, defaults to 1 midplane.
  "torus": &quot;<i>torusSpec</i>&quot; // Optional, is torus in all dimensions by default.

  // These fields are for large blocks when using midplanes.

  "midplanes": [ &quot;<i>midplaneLocation</i>&quot;, ... ],
  "passthrough": [ &quot;<i>midplaneLocation</i>&quot;, ... ], // Optional, no passthrough by default.
  "torus": &quot;<i>torusSpec</i>&quot; // Optional, is torus in all dimensions by default.

  // These fields are for small blocks when using a starting node board and count.

  "midplane": &quot;<i>midplaneLocation</i>&quot;,
  "nodeBoard": &quot;<i>nodeBoardPosition</i>&quot;,
  "nodeBoardCount": <i>number</i> // Optional, defaults to 1.

  // These fields are for small blocks when using node board locations.

  "midplane": &quot;<i>midplaneLocation</i>&quot;,
  "nodeBoards": [ &quot;<i>nodeBoardPosition</i>&quot;, ... ]
}
</pre>

Where
- <i>midplaneLocation</i> is like "R00-M0".
- <b>midplanes</b> has 4 elements of <i>midplaneIncludes</i>.
- <i>midplaneIncludes</i> is a string of 1s and 0s where 1 indicates include the midplane and 0 indicates pass through the midplane.
- <i>torusSpec</i> is a string containing the dimensions that should be a torus, where dimensions are A, B, C, and D. Missing dimensions are a mesh.
- <i>nodeBoardPosition</i> is like "N00".


\section Response

HTTP status: 201 Created
- Location header is the URL of the new block resource, \ref blockResource .


\section Errors

HTTP status: 400 Bad Request

- inputMissing: A required field is missing.
- inputWrongType: A value in the input document is not the correct type (e.g., expecting a string and is a number).
- invalidArgument: A value is not valid for the field (e.g., the block ID contains invalid characters).
- duplicate: A block with the same name already exists.
- invalidLocation: A location (midplane or node card) is not valid.

HTTP status: 403 Forbidden
- authority: User doesn't have authority to create a block.
- notAuthenticated: User isn't authenticated.

 */


#include "Blocks.hpp"

#include "Block.hpp"

#include "../BlockingOperationsThreadPool.hpp"
#include "../dbConnectionPool.hpp"
#include "../Error.hpp"
#include "../SortSpec.hpp"

#include "../utility/utility.hpp"

#include "common/common.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Request.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/genblock.h>
#include <db/include/api/Exception.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/filtering/getBlocks.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>


using BGQDB::DBTBlock;

using boost::bind;
using boost::lexical_cast;
using boost::shared_ptr;

using std::invalid_argument;
using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


namespace statics {

void statusNotifier( BGQDB::filtering::BlockFilter& block_filter_in_out, const std::string& status_str )
{
    BGQDB::filtering::BlockFilter::Statuses statuses;

    BOOST_FOREACH( char status_code, status_str ) {
        if ( status_code == 'F' )  statuses.insert( BGQDB::filtering::BlockFilter::Free );
        if ( status_code == 'A' )  statuses.insert( BGQDB::filtering::BlockFilter::Allocated );
        if ( status_code == 'B' )  statuses.insert( BGQDB::filtering::BlockFilter::Booting );
        if ( status_code == 'I' )  statuses.insert( BGQDB::filtering::BlockFilter::Initialized );
        if ( status_code == 'T' )  statuses.insert( BGQDB::filtering::BlockFilter::Terminating );
    }

    static const unsigned NUMBER_OF_STATUS_CODES(int(BGQDB::BLOCK_STATUS::TERMINATING) + 1);

    if ( statuses.empty() || (statuses.size() == NUMBER_OF_STATUS_CODES) ) {
        // Don't set the statuses if no statuses or all statuses.
        return;
    }

    block_filter_in_out.setStatuses( &statuses );
}


void sortNotifier( BGQDB::filtering::BlockSort& block_sort_in_out, const std::string& sort_str )
{
    SortSpec sort_spec( sort_str );

    if ( sort_spec.isDefault() ) {
        return;
    }

    const string &col_id(sort_spec.getColumnId());

    BGQDB::filtering::BlockSort::Field::Value field(
            col_id == "computeNodeCount" ? BGQDB::filtering::BlockSort::Field::ComputeNodeCount :
            col_id == "createDate" ? BGQDB::filtering::BlockSort::Field::CreateDate :
            col_id == "id" || col_id == "name" ? BGQDB::filtering::BlockSort::Field::Name :
            col_id == "ioNodeCount" ? BGQDB::filtering::BlockSort::Field::IoNodeCount :
            col_id == "status" ? BGQDB::filtering::BlockSort::Field::Status :
            col_id == "statusChanged" ? BGQDB::filtering::BlockSort::Field::StatusLastModified :
            col_id == "user" ? BGQDB::filtering::BlockSort::Field::User :
            BGQDB::filtering::BlockSort::Field::Name
        );

    BGQDB::filtering::SortOrder::Value sort_order(sort_spec.getDirection() == utility::SortDirection::Ascending ? BGQDB::filtering::SortOrder::Ascending : BGQDB::filtering::SortOrder::Descending);

    block_sort_in_out = BGQDB::filtering::BlockSort( field, sort_order );
}

} // namespace statics


const capena::http::uri::Path &Blocks::RESOURCE_PATH(::bgws::common::resource_path::BLOCKS);
const capena::http::uri::Path Blocks::RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");


bool Blocks::matchesUrl(
            const capena::http::uri::Path& requested_resource
        )
{
    // Check is /bg/blocks or /bg/blocks/

    return (requested_resource == RESOURCE_PATH || requested_resource == RESOURCE_PATH_EMPTY_CHILD);
}


capena::http::Methods Blocks::_getAllowedMethods() const
{
    return { capena::http::Method::GET, capena::http::Method::POST };
}


void Blocks::_doGet()
{
    namespace po = boost::program_options;

    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get blocks because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get blocks because the user isn't authenticated.",
                "getBlocks", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );
    }


    BGQDB::filtering::BlockFilter block_filter;
    BGQDB::filtering::BlockSort block_sort( BGQDB::filtering::BlockSort::Field::Name, BGQDB::filtering::SortOrder::Ascending );

    po::options_description desc;

    std::string block_type_str;

    desc.add_options()
            ( ::bgws::common::blocks_query::STATUS_OPTION_NAME.c_str(), po::value<string>()->notifier( bind( &statics::statusNotifier, boost::ref( block_filter ), _1 ) ) )
            ( "sort", po::value<string>()->notifier( bind( &statics::sortNotifier, boost::ref( block_sort ), _1 ) ) )
            ( ::bgws::common::blocks_query::TYPE_OPTION_NAME.c_str(), po::value( &block_type_str ) )
        ;

    po::variables_map vm;
    po::store( po::command_line_parser( _getRequest().getUrl().getQuery().calcArguments() ).options( desc ).allow_unregistered().run(), vm );
    po::notify( vm );


    if ( block_type_str == ::bgws::common::blocks_query::type::COMPUTE ) {
        block_filter.setBlockType( BGQDB::filtering::BlockFilter::BlockType::Compute );
    } else if ( block_type_str == ::bgws::common::blocks_query::type::IO ) {
        block_filter.setBlockType( BGQDB::filtering::BlockFilter::BlockType::Io );
    }


    _blocking_operations_thread_pool.post( bind(
            &Blocks::_doQuery, this,
            capena::server::AbstractResponder::shared_from_this(),
            block_filter,
            block_sort
        ) );

}


void Blocks::_doPost( json::ConstValuePtr val_ptr )
{
    _checkCreateBlockAuthority(); // throws if failed.

    BGQDB::GenBlockParams params(_parseCreateBlock( val_ptr ));

    params.setOwner( getRequestUserName() );

    _createBlock( params );

    _getResponse().setCreated( Block::calcPath( _getDynamicConfiguration().getPathBase(), params.getBlockId() ) );
}


void Blocks::_checkCreateBlockAuthority()
{
    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {

        LOG_WARN_MSG( "Cannot create block because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot create blocks because the user isn't authenticated.",
                "createBlock", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );

    }

    if ( _isUserAdministrator() ) {
        LOG_DEBUG_MSG( "User is administrator so can create block." );
        return;
    }

    if ( _getEnforcer().validate(
             hlcs::security::Object(
                    hlcs::security::Object::Block,
                    string() // no block ID when creating block.
             ),
             hlcs::security::Action::Create,
             _getRequestUserId()
         ) )
    {
        LOG_DEBUG_MSG( _getRequestUserInfo() << " can create blocks." );
        return;
    }

    // The user doesn't have authority.

    LOG_WARN_MSG( _getRequestUserInfo() << " tried to create block but doesn't have authority." );

    Error::Data data;
    data["userName"] = getRequestUserName();

    BOOST_THROW_EXCEPTION( Error(
            "Cannot create block because the user doesn't have authority to create blocks.",
            "createBlock",
            "authority",
            data,
            capena::http::Status::Forbidden
        ) );
}


void Blocks::_doQuery(
        capena::server::ResponderPtr,
        const BGQDB::filtering::BlockFilter& block_filter,
        const BGQDB::filtering::BlockSort& block_sort
    )
{
    try {

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(BGQDB::filtering::getBlocks(
                block_filter,
                block_sort,
                *conn_ptr
            ));

        _getStrand().post( bind(
                &Blocks::_queryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );

    } catch ( std::exception& e ) {

        _inCatchPostCurrentExceptionToHandlerFn();

    }
}


void Blocks::_queryComplete(
        capena::server::ResponderPtr,
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    try {

        json::ArrayValue arr_val;
        json::Array &arr(arr_val.get());

        while ( rs_ptr->fetch() ) {
            const cxxdb::Columns &cols(rs_ptr->columns());

            string block_id(cols[DBTBlock::BLOCKID_COL].getString());

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
                LOG_DEBUG_MSG( "Not sending block '" << block_id << "' because " << _getRequestUserInfo() << " doesn't have authority." );
                continue;
            }

            json::Object &block_obj(arr.addObject());

            block_obj.set( "id", block_id );
            block_obj.set( "status", cols[DBTBlock::STATUS_COL].getString() );
            block_obj.set( "statusSequenceId", cols[DBTBlock::SEQID_COL].as<int64_t>() );
            block_obj.set( "statusChanged", cols[DBTBlock::STATUSLASTMODIFIED_COL].getTimestamp() );

            if ( cols[DBTBlock::USERNAME_COL] ) {
                string user(cols[DBTBlock::USERNAME_COL].getString());

                if ( ! user.empty() ) {
                    if ( ! cols[DBTBlock::USERNAME_COL].isNull() )  block_obj.set( "user", cols[DBTBlock::USERNAME_COL].getString() );
                }
            }

            int64_t numcnodes(cols[DBTBlock::NUMCNODES_COL].as<int64_t>());
            if ( numcnodes != 0 )  block_obj.set( "computeNodeCount", numcnodes );

            int64_t numionodes(cols[DBTBlock::NUMIONODES_COL].as<int64_t>());
            if ( numionodes != 0 )  block_obj.set( "ioNodeCount", numionodes );

            if ( ! cols[DBTBlock::DESCRIPTION_COL].isNull() )  block_obj.set( "description", cols[DBTBlock::DESCRIPTION_COL].getString() );

            if ( ! cols[DBTBlock::ISTORUS_COL].isNull() ) {
                string torus_inds(cols[DBTBlock::ISTORUS_COL].getString());

                string torus_str;
                if ( torus_inds[0] == '1' )  torus_str += 'A';
                if ( torus_inds[1] == '1' )  torus_str += 'B';
                if ( torus_inds[2] == '1' )  torus_str += 'C';
                if ( torus_inds[3] == '1' )  torus_str += 'D';
                if ( torus_inds[4] == '1' )  torus_str += 'E';

                block_obj.set( "torus", torus_str );
            }

            block_obj.set( "URI", Block::calcPath( _getDynamicConfiguration().getPathBase(), block_id ).toString() );
        }


        capena::server::Response &response(_getResponse());

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( arr_val, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }

}


BGQDB::GenBlockParams Blocks::_parseCreateBlock(
        json::ConstValuePtr val_ptr
    )
{
    LOG_DEBUG_MSG( "create block with data: \n" << json::Formatter()( *val_ptr ) );

    Error::Data error_data;

    string block_id;

    try {
        const json::Object &block_obj(val_ptr->getObject());

        block_id = block_obj.getString( "id" );

        error_data["block_id"] = block_id;

        BGQDB::GenBlockParams params;

        params.setBlockId( block_id );

        if ( block_obj.contains( "description" ) ) {
            params.setDescription( block_obj.getString( "description" ) );
        } else {
            params.setDescription( "Generated via web services" );
        }

        if ( block_obj.contains( "midplane" ) && block_obj.contains( "nodeBoard" ) ) {
            // It's a small block with nodeBoard and optional count.

            params.setMidplane( block_obj.getString( "midplane" ) );

            uint8_t node_board_count(1);
            if ( block_obj.contains( "nodeBoardCount" ) ) {
                node_board_count = block_obj.as<uint8_t>( "nodeBoardCount" );
            }

            params.setNodeBoardAndCount( block_obj.getString( "nodeBoard" ), node_board_count );

        } else if ( block_obj.contains( "midplane" ) && block_obj.contains( "nodeBoards" ) ) {
            // It's a small block with node board locations.

            BGQDB::GenBlockParams::NodeBoardLocations node_board_locations;

            const json::Array &node_boards_arr(block_obj.getArray( "nodeBoards" ));

            for ( json::Array::const_iterator i(node_boards_arr.begin()) ; i != node_boards_arr.end() ; ++i ) {
                node_board_locations.push_back( block_obj.getString( "midplane" ) + "-" + (*i)->getString() );
            }

            params.setNodeBoardLocations( node_board_locations );

        } else if ( block_obj.contains( "midplane" ) ) {
            // It's a large block with sizes.

            params.setMidplane( block_obj.getString( "midplane" ) );

            string torus_str("ABCDE");
            if ( block_obj.contains( "torus" ) ) {
                torus_str = block_obj.getString( "torus" );
            }

            BGQDB::DimensionSpecs dim_specs;

            if ( block_obj.contains( "midplanes" ) ) {
                const json::Array &mps(block_obj.getArray( "midplanes" ));

                if ( mps.size() != BGQDB::Dimension::Count ) {
                    BOOST_THROW_EXCEPTION( std::invalid_argument( "invalid midplanes array" ) );
                }

                dim_specs[BGQDB::Dimension::A] = BGQDB::DimensionSpec(
                            BGQDB::DimensionSpec::strToMidplanes( mps.at( BGQDB::Dimension::A )->getString() ),
                            (torus_str.find( 'A' ) != string::npos) ? BGQDB::Connectivity::Torus : BGQDB::Connectivity::Mesh
                        );

                dim_specs[BGQDB::Dimension::B] = BGQDB::DimensionSpec(
                            BGQDB::DimensionSpec::strToMidplanes( mps.at( BGQDB::Dimension::B )->getString() ),
                            (torus_str.find( 'B' ) != string::npos) ? BGQDB::Connectivity::Torus : BGQDB::Connectivity::Mesh
                        );

                dim_specs[BGQDB::Dimension::C] = BGQDB::DimensionSpec(
                            BGQDB::DimensionSpec::strToMidplanes( mps.at( BGQDB::Dimension::C )->getString() ),
                            (torus_str.find( 'C' ) != string::npos) ? BGQDB::Connectivity::Torus : BGQDB::Connectivity::Mesh
                        );

                dim_specs[BGQDB::Dimension::D] = BGQDB::DimensionSpec(
                            BGQDB::DimensionSpec::strToMidplanes( mps.at( BGQDB::Dimension::D )->getString() ),
                            (torus_str.find( 'D' ) != string::npos) ? BGQDB::Connectivity::Torus : BGQDB::Connectivity::Mesh
                        );
            }

            params.setDimensionSpecs( dim_specs );

        } else if ( block_obj.contains( "midplanes" ) ) {
            // It's a large block with midplanes.

            BGQDB::GenBlockParams::MidplaneLocations midplane_locations;

            const json::Array& mps(block_obj.getArray( "midplanes" ));

            for ( json::Array::const_iterator i(mps.begin()) ; i != mps.end() ; ++i ) {
                midplane_locations.push_back( (*i)->getString() );
            }

            BGQDB::GenBlockParams::MidplaneLocations passthrough_locations;

            if ( block_obj.contains( "passthrough" ) ) {
                const json::Array& pts(block_obj.getArray( "passthrough" ));

                for ( json::Array::const_iterator i(pts.begin()) ; i != pts.end() ; ++i ) {
                    passthrough_locations.push_back( (*i)->getString() );
                }
            }

            params.setMidplanes(
                    midplane_locations,
                    passthrough_locations
                );

            BGQDB::DimensionSpecs dim_specs;

            if ( block_obj.contains( "torus" ) ) {
                const string &torus_str(block_obj.getString( "torus" ));
                dim_specs[BGQDB::Dimension::A] = BGQDB::DimensionSpec( torus_str.find( 'A' ) == string::npos ? BGQDB::Connectivity::Mesh : BGQDB::Connectivity::Torus );
                dim_specs[BGQDB::Dimension::B] = BGQDB::DimensionSpec( torus_str.find( 'B' ) == string::npos ? BGQDB::Connectivity::Mesh : BGQDB::Connectivity::Torus );
                dim_specs[BGQDB::Dimension::C] = BGQDB::DimensionSpec( torus_str.find( 'C' ) == string::npos ? BGQDB::Connectivity::Mesh : BGQDB::Connectivity::Torus );
                dim_specs[BGQDB::Dimension::D] = BGQDB::DimensionSpec( torus_str.find( 'D' ) == string::npos ? BGQDB::Connectivity::Mesh : BGQDB::Connectivity::Torus );
            }

            params.setDimensionSpecs( dim_specs );
        } else {
            BOOST_THROW_EXCEPTION( invalid_argument( "required field missing" ) );
        }

        return params;

    } catch ( const json::MemberNotFound& e ) {

        error_data["member"] = e.getMemberName();

        LOG_WARN_MSG( "Missing required member '" + e.getMemberName() + "' on create block request." );
        BOOST_THROW_EXCEPTION( Error(
                string() + "Cannot create block because the '" + e.getMemberName() + "' member is missing.",
                "createBlock", "inputMissing", error_data,
                capena::http::Status::BadRequest
            ) );

    } catch ( const json::WrongType& e ) {

        if ( ! e.getMemberName().empty() ) {
            error_data["member"] = e.getMemberName();
        }

        LOG_WARN_MSG( "Member '" + e.getMemberName() + "' is wrong type on create block request." );
        BOOST_THROW_EXCEPTION( Error(
                string() + "Cannot create block because member '" + e.getMemberName() + "' is of an unexpected type.",
                "createBlock", "inputWrongType", error_data,
                capena::http::Status::BadRequest
            ) );

    } catch ( std::invalid_argument& e ) { // GenBlockParams throws invalid argument when error.

        error_data["errMsg"] = e.what();

        LOG_WARN_MSG( "Invalid argument on create block request for block '" << block_id << "', error = '" << e.what() << "'." );
        BOOST_THROW_EXCEPTION( Error(
                string() + "Could not create '" + block_id + "' because an argument was not valid. The error is '" + e.what() + "'.",
                "createBlock", "invalidArgument", error_data,
                capena::http::Status::BadRequest
            ) );

    }

    // should never get here
    return BGQDB::GenBlockParams();
}


void Blocks::_createBlock(
        const BGQDB::GenBlockParams& params
    )
{
    LOG_INFO_MSG( "Creating block '" << params.getBlockId() << "'" );

    BGQDB::STATUS db_status;
    string err_msg;

    try {

        BGQDB::genBlockEx( params );

        return;

    } catch( BGQDB::Exception& bgdbe ) {

        err_msg = bgdbe.what();
        db_status = bgdbe.getStatus();

    } catch ( std::exception& e ) {

        err_msg = boost::str( boost::format( "Failed to create block '%1%'. genBlock returns %2%" ) % params.getBlockId() % e.what() );
        db_status = BGQDB::FAILED;

    }


    // Process err_msg and db_status.

    LOG_WARN_MSG( err_msg );

    Error::Data data;
    data["blockId"] = params.getBlockId();
    data["userName"] = getRequestUserName();
    if ( err_msg != string() )  data["msg"] = err_msg;

    if ( db_status == BGQDB::DUPLICATE ) {
        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not create block '%1%' because a block with that name already exists." ) % params.getBlockId() ),
                "createBlock", "duplicate", data,
                capena::http::Status::BadRequest
            ) );
    }

    if ( db_status == BGQDB::INVALID_ARG ) {
        BOOST_THROW_EXCEPTION( Error(
                "Error creating block, " + err_msg,
                "createBlock", "invalidArgument", data,
                capena::http::Status::BadRequest
            ) );
    }

    if ( db_status == BGQDB::NOT_FOUND ) {
        BOOST_THROW_EXCEPTION( Error(
                "Error creating block, " + err_msg,
                "createBlock", "invalidLocation", data,
                capena::http::Status::BadRequest
            ) );
    }

    BOOST_THROW_EXCEPTION( Error(
            boost::str( boost::format( "Could not create block '%1%' because the database operation returned '%2%'" ) %
                    params.getBlockId() %
                    lexical_cast<string>( db_status )
                ),
            "createBlock", "databaseError", data,
            capena::http::Status::InternalServerError
        ) );
}


} } // namespace bgws::responder
