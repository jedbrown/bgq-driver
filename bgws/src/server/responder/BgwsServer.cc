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

/*! \page bgwsServerResource /bg/bgwsServer

This resource provides operations on the BGWS server.

- \subpage bgwsServerResourceGet
- \subpage bgwsServerResourcePost

 */

/*! \page bgwsServerResourceGet GET /bg/bgwsServer

Get the BGWS server status.

\section Authorization

Only a Blue Gene Administrator can get the BGWS server status.


\section bgwsServerResourceGetResponse JSON response format

<pre>
{
  "status" : "OK",
  "startTime" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
  "requestsHandled" : <i>number</i>,
  "requestsInProgress" : <i>number</i>,
  "maxRequestTime" : <i>number</i>,
  "avgRequestTime" : <i>number</i>,
  "databaseConnectionPool" : { // New for V1R2M0.
    "available": <i>number</i>,
    "used": <i>number</i>,
    "max": <i>number</i>,
    "size": <i>number</i>
  },
  "requests" : [ // optional -- New for V1R2M0.
      {
        "method": &quot;<i>string</i>&quot;,
        "url": &quot;<i>URL</i>&quot;,
        "startTime": &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
        "user": &quot;<i>string</i>&quot;
      }, ...
    ]
}
</pre>

\section Errors

HTTP status: 403 Forbidden

- authority: The user doesn't have authority to perform the operation.

 */


/*! \page bgwsServerResourcePost POST /bg/bgwsServer

Perform an operation on the BGWS server.

The only operation supported is "refreshConfig", which causes the server to re-read the bg.properties file.


\section Authorization

Only a Blue Gene Administrator can perform an operation on the BGWS server.


\section bgwsServerResourcePostData JSON request data format

<pre>
{
  "operation" : &quot;refreshConfig&quot;,

  "propertiesFile" : &quot;filename&quot; // Optional, new properties file name
}
</pre>

- The only operation supported is refreshConfig.
- propertiesFile is the new properties file name. By default, the current properties file will be re-read. If the string is empty then the default properties file will be used. Otherwise it must be the fully-qualified path to the new properties file.


\section Response

HTTP status: 204 No Content

\section Errors

HTTP status: 400 Bad Request

- failed: Failed to refresh the server's configuration, the current configuration will remain in effect.
- invalidPath: The path is not valid. It must be fully-qualified.
- noOperation: No operation provided on the request.
- operationNotString: The operation is not a string.
- unexpectedOperation: The provided operation is not valid, it must be refreshConfig.

HTTP status: 403 Forbidden

- authority: The user doesn't have authority to perform the operation.

 */


#include "BgwsServer.hpp"

#include "../BgwsServer.hpp"
#include "../Error.hpp"
#include "../ras.hpp"

#include "common/common.hpp"
#include "common/RefreshBgwsServerConfiguration.hpp"

#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>


using boost::lexical_cast;

using std::string;
using std::vector;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


static bool compareRequestPtrs( const ServerStats::RequestData::Ptr& lhs, const ServerStats::RequestData::Ptr& rhs )
{
    return (lhs->start_time < rhs->start_time);
}


typedef vector<ServerStats::RequestData::Ptr> RequestDataPtrs;


static RequestDataPtrs calcSortedRequests( const ServerStats::Requests& requests )
{
    RequestDataPtrs req_data_ptrs;

    BOOST_FOREACH( const auto& req_map_pair, requests ) {
        req_data_ptrs.push_back( req_map_pair.second );
    }

    std::sort( req_data_ptrs.begin(), req_data_ptrs.end(), &compareRequestPtrs );

    return req_data_ptrs;
}


const capena::http::uri::Path &BgwsServer::RESOURCE_PATH(::bgws::common::resource_path::BGWS_SERVER);


void BgwsServer::_doGet()
{

    if ( ! _isUserAdministrator() ) {
        LOG_WARN_MSG( "Could not get BGWS server status because " << _getRequestUserInfo() << " doesn't have authority (must be administrator)." );

        ras::postAdminAuthorityFailure( _getRequestUserInfo(), "get server status" );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get BGWS server status because the user doesn't have authority.",
                "getBgwsServerStatus", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }

    _server_stats.getSnapshot(
            _getStrand().wrap( boost::bind(
                    &BgwsServer::_gotStatistics,
                    this,
                    capena::server::AbstractResponder::shared_from_this(),
                    _1
                ) )
        );
}


void BgwsServer::_gotStatistics(
        capena::server::ResponderPtr /*shared_ptr*/,
        const ServerStats::Snapshot& snapshot
    )
{
    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::ObjectValue obj_val;
    json::Object &obj(obj_val.get());

    obj.set( "status", "OK" );
    obj.set( "startTime", _bgws_server.getStartTime() );
    obj.set( "requestsHandled", snapshot.requests_complete );
    obj.set( "requestsInProgress", (snapshot.total_requests - snapshot.requests_complete) );
    obj.set( "maxRequestTime", snapshot.max_time_to_process_request_microseconds / 1000000.0 );
    obj.set( "avgRequestTime",
             (snapshot.requests_complete == 0 ? 0.0 : (snapshot.total_time_to_process_requests_microseconds / snapshot.requests_complete / 1000000.0))
           );


    json::Object &db_conn_pool_obj(obj.createObject( "databaseConnectionPool" ));

    db_conn_pool_obj.set( "available", BGQDB::DBConnectionPool::instance().availableCount() );
    db_conn_pool_obj.set( "used", BGQDB::DBConnectionPool::instance().usedCount() );
    db_conn_pool_obj.set( "max", BGQDB::DBConnectionPool::instance().maxCount() );
    db_conn_pool_obj.set( "size", BGQDB::DBConnectionPool::instance().size() );


    if ( ! snapshot.requests.empty() ) {

        RequestDataPtrs req_data_ptrs(calcSortedRequests( snapshot.requests ));


        json::Array &reqs(obj.createArray( "requests" ));

        BOOST_FOREACH( const auto &req_data_ptr, req_data_ptrs ) {

            const ServerStats::RequestData &req_data(*req_data_ptr);

            json::Object &req_obj(reqs.addObject());

            req_obj.set( "method", lexical_cast<string>( req_data.method ) );
            req_obj.set( "url", req_data.url_str );
            req_obj.set( "startTime", req_data.start_time );
            req_obj.set( "user", lexical_cast<string>( req_data.user_info ) );
        }

    }

    json::Formatter()( obj_val, response.out() );
}


void BgwsServer::_doPost( json::ConstValuePtr val_ptr )
{
    Error::Data error_data;

    try {

        if ( ! _isUserAdministrator() ) {
            LOG_WARN_MSG( "Could not perform operation on BGWS server because " << _getRequestUserInfo() << " doesn't have authority (must be administrator)." );

            ras::postAdminAuthorityFailure( _getRequestUserInfo(), "refresh server configuration" );

            BOOST_THROW_EXCEPTION( Error(
                    "Could not perform operation on BGWS server because the user doesn't have authority.",
                    "bgwsServerOperation", "authority", Error::Data(),
                    capena::http::Status::Forbidden
                ) );

            return;
        }


        ::bgws::common::RefreshBgwsServerConfiguration refresh_config_data( *val_ptr );

        LOG_INFO_MSG( _getRequestUserInfo() << " requested refresh configuration." );


        error_data["operation"] = ::bgws::common::RefreshBgwsServerConfiguration::OPERATION_NAME;

        if ( refresh_config_data.getType() == ::bgws::common::RefreshBgwsServerConfiguration::Type::ReadDefault ) {
            error_data["filename"] = "";
        } else if ( refresh_config_data.getType() == ::bgws::common::RefreshBgwsServerConfiguration::Type::ReadNew ) {
            error_data["filename"] = refresh_config_data.getFilename().string();
        }


        _bgws_server.refreshConfiguration( refresh_config_data );


        capena::server::Response &response(_getResponse());

        response.setStatus( capena::http::Status::NoContent );
        response.headersComplete();

    } catch ( ::bgws::common::RefreshBgwsServerConfiguration::PathNotComplete& pnce ) {

        error_data["filename"] = pnce.getPath().string();
        error_data["operation"] = ::bgws::common::RefreshBgwsServerConfiguration::OPERATION_NAME;

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not refresh configuration because the path is not valid, it must be a fully-qualified path. The path is '%1%'." ) % pnce.getPath() ),
                "refreshConfig", "invalidPath", error_data,
                capena::http::Status::BadRequest
            ) );

    } catch ( ::bgws::common::RefreshBgwsServerConfiguration::NotObjectError& noe ) {

        BOOST_THROW_EXCEPTION( Error(
                "Invalid request, data is not a JSON object.",
                "bgwsServerOperation",
                "dataFormat",
                error_data,
                capena::http::Status::BadRequest
            ) );

    } catch ( ::bgws::common::RefreshBgwsServerConfiguration::NoOperationError& noe ) {

        BOOST_THROW_EXCEPTION( Error(
                "Invalid request, no operation provided.",
                "bgwsServerOperation",
                "noOperation",
                error_data,
                capena::http::Status::BadRequest
            ) );

    } catch ( ::bgws::common::RefreshBgwsServerConfiguration::OperationNotStringError& onse ) {

        BOOST_THROW_EXCEPTION( Error(
                "Invalid request, operation is not a string.",
                "bgwsServerOperation",
                "operationNotString",
                error_data,
                capena::http::Status::BadRequest
            ) );

    } catch ( ::bgws::common::RefreshBgwsServerConfiguration::UnexpectedOperationError& uoe ) {

        error_data["operation"] = uoe.getOperation();

        BOOST_THROW_EXCEPTION( Error(
                string() + "Operation '" + uoe.getOperation() + "' not valid.",
                "bgwsServerOperation",
                "unexpectedOperation",
                error_data,
                capena::http::Status::BadRequest
            ) );

    } catch ( bgws::BgwsServer::RefreshConfigurationError& e ) {

        string err_text(string() + "Failed to refresh the server's configuration because the new configuration is not valid. The server will use the previous configuration. The error is '" + e.what() + "'");

        LOG_WARN_MSG( err_text );

        error_data["errorMessage"] = e.what();

        BOOST_THROW_EXCEPTION( Error(
                err_text,
                "refreshConfig", "failed", error_data,
                capena::http::Status::BadRequest
            ) );

    }

}


} } // namespace bgws::responder
