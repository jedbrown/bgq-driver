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

#include "BgwsClient.hpp"

#include "CurlHandle.hpp"
#include "RequestData.hpp"
#include "Response.hpp"
#include "utility.hpp"

#include "common/common.hpp"
#include "common/ContentRange.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/http/uri/Query.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scope_exit.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <curl/curl.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>


using namespace boost::assign;

using BGQDB::Connectivity;
using BGQDB::Dimension;
using BGQDB::DimensionSpec;
using BGQDB::DimensionSpecs;
using BGQDB::GenBlockParams;

using boost::lexical_cast;

using std::cerr;
using std::ostringstream;
using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws.command" );


namespace bgws {
namespace command {


namespace statics {


static void checkResponse(
        const Response& response,
        capena::http::Status expected_status = capena::http::Status::OK,
        bool* is_partial_response_out = NULL
    )
{
    // If the operation was successful then expect a 200 OK response.
    capena::http::StatusClass status_class(capena::http::codeToClass( response.getStatusCode() ));

    if ( status_class == capena::http::StatusClass::Successful ) {
        if ( ! ((response.getStatusCode() == expected_status) ||
                (is_partial_response_out && response.getStatusCode() == capena::http::Status::PartialContent)) ) {
            cerr << "Note: successful response with status '" << response.getStatusText() << "'.\n";
        }

        if ( ((expected_status != capena::http::Status::NoContent) &&
              (expected_status != capena::http::Status::Created)) &&
             (response.getContentType() != capena::http::media_type::JSON) ) {
            // successful but content-type wasn't JSON ?

            BOOST_THROW_EXCEPTION( runtime_error( string() +
                    "unexpected content type in response. The content type is '" + response.getContentType() + "'"
                ) );
        }

        if ( is_partial_response_out ) {
            if ( response.getStatusCode() == capena::http::Status::PartialContent ) {
                *is_partial_response_out = true;
            } else {
                *is_partial_response_out = false;
            }
        }

        return;
    }

    // This is some type of error.

    if ( status_class == capena::http::StatusClass::Informational ) {
        // Informational, but don't know how to handle.
        BOOST_THROW_EXCEPTION( runtime_error( string() +
                "don't know how to handle HTTP response '" + response.getStatusText() + "'"
            ) );
    }

    // Check for typical BGWS error response.

    if ( response.getContentType() == capena::http::media_type::JSON ) {
        json::Object err_obj(json::Parser()( response.getData() )->getObject());

        const string &err_text(err_obj.getString( "text" ));

        BOOST_THROW_EXCEPTION( runtime_error( string() +
                "Blue Gene web services response:\n" +
                err_text
            ) );
    }

    // Otherwise, process HTTP error.

    ostringstream oss;

    oss << "failed with HTTP response '" + response.getStatusText() + "'.";

    if ( ! response.getData().empty() ) {
        oss << " Response data:\n" << response.getData();
    }

    BOOST_THROW_EXCEPTION( runtime_error( oss.str() ) );

}


} // namespace statics


BgwsClient::BgwsClient()
{
    CURLcode crc;

    if ( (crc = curl_global_init( CURL_GLOBAL_ALL )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( std::string() + "failed to initialize the curl library, " + curl_easy_strerror( crc ) ) );
    }
}


capena::http::uri::Uri BgwsClient::_calcReqUri(
        const capena::http::uri::Path& path,
        const capena::http::uri::Query& query
    )
{
    const capena::http::uri::Uri &base_uri(_options.getBase());

    capena::http::uri::Uri req_uri(
            base_uri.getSecurity(),
            base_uri.getHostPort(),
            base_uri.getPath() / path,
            query
        );

    return req_uri;
}


void BgwsClient::notifyOptionsSet()
{
    _bg_properties_ptr = bgq::utility::Properties::create( _options.getBgPropertiesFileName() );
}


void BgwsClient::notifyLoggingIsInitialized()
{
    _options.setBgProperties( _bg_properties_ptr );
}


void BgwsClient::createBlock(
        const BGQDB::GenBlockParams& gen_block_params,
        const std::string& description,
        GenBlockParamsType::Value gen_block_params_type
    )
{
    json::ValuePtr val_ptr(_genBlockParamsToJson(
            gen_block_params,
            description,
            gen_block_params_type
        ));

    CurlHandle curl_handle( CurlHandle::CtorArgs(
            _options,
            _calcReqUri( ::bgws::common::resource_path::BLOCKS ),
            val_ptr
        ) );

    curl_handle.perform();

    statics::checkResponse(
            curl_handle.getResponse(),
            capena::http::Status::Created
        );
}


json::ArrayValuePtr BgwsClient::getBlocksSummary(
        const std::string& status,
        common::blocks_query::Type block_type
    )
{
    capena::http::uri::Query::Parameters params;

    if ( ! status.empty() ) {
        params += capena::http::uri::Query::Parameters::value_type( ::bgws::common::blocks_query::STATUS_OPTION_NAME, status );
    }

    string block_type_str(block_type == common::blocks_query::Type::Compute ? common::blocks_query::type::COMPUTE : common::blocks_query::type::IO);

    params += capena::http::uri::Query::Parameters::value_type( ::bgws::common::blocks_query::TYPE_OPTION_NAME, block_type_str );


    capena::http::uri::Uri req_uri(_calcReqUri(
            ::bgws::common::resource_path::BLOCKS,
            capena::http::uri::Query( params )
        ));

    CurlHandle curl_handle( CurlHandle::CtorArgs(
            _options,
            req_uri
        ) );

    curl_handle.perform();

    const Response &response(curl_handle.getResponse());

    statics::checkResponse( response );

    return boost::dynamic_pointer_cast<json::ArrayValue>( response.getJsonResponse() );
}

json::ObjectValuePtr BgwsClient::getBlockDetails( const std::string& block_id )
{
    CurlHandle curl_handle( CurlHandle::CtorArgs(
            _options,
            _calcReqUri( ::bgws::common::resource_path::BLOCKS / block_id )
        ) );

    curl_handle.perform();

    const Response &response(curl_handle.getResponse());

    statics::checkResponse( response );

    return boost::dynamic_pointer_cast<json::ObjectValue>( response.getJsonResponse() );
}


void BgwsClient::deleteBlock( const std::string& block_id )
{
    CurlHandle curl_handle( CurlHandle::CtorArgs(
            _options,
            _calcReqUri( ::bgws::common::resource_path::BLOCKS / block_id ),
            capena::http::Method::DELETE
        ) );

    curl_handle.perform();

    statics::checkResponse( curl_handle.getResponse(), capena::http::Status::NoContent );
}


void BgwsClient::getJobsSummary(
        const JobsSummaryFilter& filter,
        const boost::optional<bgws::common::ItemRange>& range_opt,
        json::ArrayValuePtr* value_ptr_out,
        uint64_t* total_jobs_out,
        uint64_t* content_range_end_out
    )
{
    try {
        capena::http::uri::Uri req_uri(_calcReqUri(
                ::bgws::common::resource_path::JOBS,
                filter.calcQuery()
            ));

        string range_str(
                range_opt ? lexical_cast<string>( *range_opt ) : string()
            );

        CurlHandle curl_handle(CurlHandle::CtorArgs(
                _options,
                req_uri,
                range_str
            ));

        curl_handle.perform();

        const Response &response(curl_handle.getResponse());

        static const capena::http::Status ExpectedStatus(capena::http::Status::OK);
        bool is_partial_response(false);

        statics::checkResponse(
                response,
                ExpectedStatus,
                &is_partial_response
            );

        *value_ptr_out = boost::shared_dynamic_cast<json::ArrayValue>( response.getJsonResponse() );

        if ( is_partial_response ) {

            bgws::common::ContentRange content_range(bgws::common::ContentRange::parse(response.getContentRange()));

            *total_jobs_out = content_range.getTotal();
            *content_range_end_out = content_range.getEnd();
                // or maybe return the ContentRange!

        } else {
            *total_jobs_out = 0;
            *content_range_end_out = 0;
        }

    } catch ( bgws::common::ContentRange::ParseError& e ) {

        BOOST_THROW_EXCEPTION( std::runtime_error( string() +
                "Invalid content range received from server. The content-range is '" + e.getContentRangeString() + "'"
            ) );

    }
}


json::ObjectValuePtr BgwsClient::getJobDetails( BGQDB::job::Id job_id )
{
    CurlHandle curl_handle( CurlHandle::CtorArgs(
            _options,
            _calcReqUri( ::bgws::common::resource_path::JOBS / lexical_cast<std::string>( job_id ) )
        ) );

    curl_handle.perform();

    const Response &response(curl_handle.getResponse());

    statics::checkResponse( response );

    return boost::dynamic_pointer_cast<json::ObjectValue>( response.getJsonResponse() );
}


json::ObjectValuePtr BgwsClient::getBgwsServerStatus()
{
    CurlHandle curl_handle( CurlHandle::CtorArgs(
            _options,
            _calcReqUri( ::bgws::common::resource_path::BGWS_SERVER )
        ) );

    curl_handle.perform();

    const Response &response(curl_handle.getResponse());

    statics::checkResponse( response );

    return boost::dynamic_pointer_cast<json::ObjectValue>( response.getJsonResponse() );
}

json::ObjectValuePtr BgwsClient::getLogging()
{
    CurlHandle curl_handle( CurlHandle::CtorArgs(
            _options,
            _calcReqUri( ::bgws::common::resource_path::LOGGING )
        ) );

    curl_handle.perform();

    const Response &response(curl_handle.getResponse());

    statics::checkResponse( response );

    return boost::dynamic_pointer_cast<json::ObjectValue>( response.getJsonResponse() );
}


void BgwsClient::setLogging( const LoggingSettings& logging_settings )
{
    // Convert the logging settings to the JSON document that bgws is expecting.
    json::ObjectValuePtr object_value_ptr(json::Object::create());
    json::Object &obj(object_value_ptr->get());

    for ( LoggingSettings::const_iterator i(logging_settings.begin()) ; i != logging_settings.end() ; ++i ) {
        obj.set( i->first, i->second );
    }

    CurlHandle curl_handle( CurlHandle::CtorArgs(
            _options,
            _calcReqUri( ::bgws::common::resource_path::LOGGING ),
            object_value_ptr,
            capena::http::Method::PUT
        ) );

    curl_handle.perform();

    statics::checkResponse( curl_handle.getResponse(), capena::http::Status::NoContent );
}


void BgwsClient::refreshBgwsServerConfiguration(
        const ::bgws::common::RefreshBgwsServerConfiguration& refresh_bgws_server_configuration
    )
{
    capena::http::uri::Uri req_uri(_calcReqUri(
            ::bgws::common::resource_path::BGWS_SERVER
        ));

    json::ValuePtr json_data_ptr(refresh_bgws_server_configuration.toJson());

    CurlHandle curl_handle( CurlHandle::CtorArgs(
            _options,
            req_uri,
            json_data_ptr,
            capena::http::Method::POST
        ) );

    curl_handle.perform();

    statics::checkResponse( curl_handle.getResponse(), capena::http::Status::NoContent );
}


BgwsClient::~BgwsClient()
{
    curl_global_cleanup();
}


json::ValuePtr BgwsClient::_genBlockParamsToJson(
        const BGQDB::GenBlockParams& gen_block_params,
        const std::string& description,
        GenBlockParamsType::Value gen_block_params_type
    )
{
    json::ObjectValuePtr object_value_ptr(json::Object::create());
    json::Object &obj(object_value_ptr->get());
    obj.set( "id", gen_block_params.getBlockId() );

    if ( ! description.empty() ) {
        obj.set( "description", description );
    }

    if ( gen_block_params.isLarge() ) {

        const DimensionSpecs &dim_specs(gen_block_params.getDimensionSpecs());

        if ( gen_block_params.isLargeWithMidplanes() ) {
            json::Array &mps_arr(obj.createArray( "midplanes" ) );

            const GenBlockParams::MidplaneLocations &mp_locs(gen_block_params.getMidplanes());

            BOOST_FOREACH ( const std::string& mp_loc , mp_locs ) {
                mps_arr.add( mp_loc );
            }

            const GenBlockParams::MidplaneLocations &pt_locs(gen_block_params.getPassthroughLocations());

            if ( ! pt_locs.empty() ) {
                json::Array &pts_arr(obj.createArray( "passthrough" ) );

                BOOST_FOREACH ( const std::string& pt_loc , pt_locs ) {
                    pts_arr.add( pt_loc );
                }
            }
        } else {
            obj.set( "midplane", gen_block_params.getMidplane() );

            json::Array &mps_arr(obj.createArray( "midplanes" ));
            mps_arr.add( DimensionSpec::midplanesToStr( dim_specs[Dimension::A].getIncludedMidplanes() ) );
            mps_arr.add( DimensionSpec::midplanesToStr( dim_specs[Dimension::B].getIncludedMidplanes() ) );
            mps_arr.add( DimensionSpec::midplanesToStr( dim_specs[Dimension::C].getIncludedMidplanes() ) );
            mps_arr.add( DimensionSpec::midplanesToStr( dim_specs[Dimension::D].getIncludedMidplanes() ) );
        }

        std::string torus_str;

        if ( dim_specs[Dimension::A].getConnectivity() == Connectivity::Torus )  torus_str += 'A';
        if ( dim_specs[Dimension::B].getConnectivity() == Connectivity::Torus )  torus_str += 'B';
        if ( dim_specs[Dimension::C].getConnectivity() == Connectivity::Torus )  torus_str += 'C';
        if ( dim_specs[Dimension::D].getConnectivity() == Connectivity::Torus )  torus_str += 'D';
        torus_str += 'E';

        obj.set( "torus", torus_str );

    } else { // it's a small block.
        obj.set( "midplane", gen_block_params.getMidplane() );

        if ( gen_block_params_type == GenBlockParamsType::SMALL ) {
            obj.set( "nodeBoard", gen_block_params.getNodeBoard() );
            obj.set( "nodeBoardCount", gen_block_params.getNodeBoardCount() );
        } else if ( gen_block_params_type == GenBlockParamsType::NODE_BOARDS ) {
            json::Array &nbs_arr(obj.createArray( "nodeBoards" ));

            BOOST_FOREACH ( const std::string& nb_pos , gen_block_params.getNodeBoardPositions() ) {
                nbs_arr.add( nb_pos );
            }
        }
    }

    return object_value_ptr;
}


json::ArrayValuePtr BgwsClient::getRas(
        const std::string& block,
        BGQDB::job::Id job_id
    )
{
    capena::http::uri::Query::Parameters params;

    if ( ! block.empty() ) {
        params += capena::http::uri::Query::Parameters::value_type( "block", block );
    }
    if ( job_id != BGQDB::job::Id(-1) ) {
        params += capena::http::uri::Query::Parameters::value_type( "job", lexical_cast<string>( job_id ) );
    }


    capena::http::uri::Uri req_uri(_calcReqUri(
            ::bgws::common::resource_path::RAS,
            capena::http::uri::Query( params )
        ));


    CurlHandle curl_handle( CurlHandle::CtorArgs(
            _options,
            req_uri
        ) );

    curl_handle.perform();

    const Response &response(curl_handle.getResponse());

    statics::checkResponse( response );

    return boost::shared_dynamic_cast<json::ArrayValue>( response.getJsonResponse() );
}

} } // namespace bgws::command
