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

/*! \page rasDetailsResource /bg/ras/<i>id</i>

This resource represents a RAS event.

- \subpage rasDetailsResourceGet

 */

/*!
 *  \page rasDetailsResourceGet GET /bg/ras/<i>id</i>

Get details for a RAS event.

\section Authority

The user must have hardware READ authority.

\section rasDetailsResourceGetResponse JSON response format

<pre>
{
  "eventTime": &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
  "msg_id": &quot;<i>string</i>&quot;, // Optional
  "category": &quot;<i>string</i>&quot;, // Optional
  "component": &quot;<i>string</i>&quot;, // Optional
  "severity": &quot;<i>string</i>&quot;, // Optional
  "jobid": <i>number</i>, // Optional
  "block": &quot;<i>string</i>&quot;, // Optional
  "location": &quot;<i>string</i>&quot;, // Optional
  "ecid": &quot;<i>hexadecimal</i>&quot;, // Optional
  "serialnumber": &quot;<i>string</i>&quot;, // Optional
  "cpu": <i>number</i>, // Optional
  "count": <i>number</i>, // Optional
  "message": &quot;<i>string</i>&quot;, // Optional
  "controlAction": &quot;<i>string</i>&quot;, // Optional
  "description": &quot;<i>string</i>&quot;, // Optional
  "svcAction": &quot;<i>string</i>&quot;, // Optional
  "relevantDiags": &quot;<i>string</i>&quot;, // Optional
  "thresholdCount": <i>number</i>, // Optional
  "thresholdPeriod": &quot;<i>string</i>&quot;, // Optional
  "rawData": &quot;<i>string</i>&quot;
}
</pre>


\section Errors

HTTP status: 400 Bad Request
- invalidRecordId: The record ID is not a valid record ID.

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

HTTP status: 404 Not Found
- notFound: The record doesn't exist.

 */


#include "RasDetails.hpp"

#include "Ras.hpp"

#include "../dbConnectionPool.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <string>


using BGQDB::DBTEventlog;
using BGQDB::DBTMsgtypes;

using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


bool RasDetails::matchesUrl( const capena::http::uri::Path& requested_resource )
{
    return ((requested_resource.calcParent() == Ras::RESOURCE_PATH) && (! requested_resource.back().empty()) );
}


capena::http::uri::Path RasDetails::calcPath( const capena::http::uri::Path& path_base, uint64_t recid )
{
    return (path_base / Ras::RESOURCE_PATH / lexical_cast<string>( recid ));
}


void RasDetails::_doGet()
{
    const string &recid_str(_getRequestedResourcePath().back());

    Error::Data error_data;
    error_data["id"] = recid_str;


    uint64_t recid;

    try {

        recid = lexical_cast<uint64_t>( recid_str );

    } catch ( std::exception& e ) {

        BOOST_THROW_EXCEPTION( Error(
                string() + "Invalid record ID '" + recid_str + "'",
                "getRasDetails",
                "invalidRecordId",
                error_data,
                capena::http::Status::BadRequest
            ) );

    }


    _checkAuthority( error_data );


    auto conn_ptr(dbConnectionPool::getConnection());

    const string SQL = string() +

"SELECT e." + DBTEventlog::MSG_ID_COL + ","
      " e." + DBTEventlog::CATEGORY_COL + ","
      " e." + DBTEventlog::COMPONENT_COL + ","
      " e." + DBTEventlog::SEVERITY_COL + ","
      " e." + DBTEventlog::EVENT_TIME_COL + ","
      " e." + DBTEventlog::JOBID_COL + ","
      " e." + DBTEventlog::BLOCK_COL + ","
      " e." + DBTEventlog::LOCATION_COL + ","
      " e." + DBTEventlog::SERIALNUMBER_COL + ","
      " HEX(RTRIM(e." + DBTEventlog::ECID_COL + ")) AS " + DBTEventlog::ECID_COL + ","
      " e." + DBTEventlog::CPU_COL + ","
      " e." + DBTEventlog::COUNT_COL + ","
      " e." + DBTEventlog::CTLACTION_COL + ","
      " e." + DBTEventlog::MESSAGE_COL + ","
      " e." + DBTEventlog::RAWDATA_COL + ","
      " m." + DBTMsgtypes::DESCRIPTION_COL + ","
      " m." + DBTMsgtypes::SVCACTION_COL + ","
      " m." + DBTMsgtypes::THRESHOLDCOUNT_COL + ","
      " m." + DBTMsgtypes::THRESHOLDPERIOD_COL + ","
      " m." + DBTMsgtypes::RELEVANTDIAGS_COL +
 " FROM " + DBTEventlog().getTableName() + " AS e"
      " LEFT OUTER JOIN"
      " " + DBTMsgtypes().getTableName() + " AS m"
      " ON e.msg_id = m.msg_id"
 " WHERE e." + DBTEventlog::RECID_COL + "=?"

        ;

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( SQL, cxxdb::ParameterNames{ "recid" } ));

    stmt_ptr->parameters()["recid"].cast( recid );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    if ( ! rs_ptr->fetch() ) {

        BOOST_THROW_EXCEPTION( Error(
                string() + "RAS record ID " + recid_str + " not found",
                "getRasDetails",
                "notFound",
                error_data,
                capena::http::Status::NotFound
            ) );

        return;
    }

    json::ObjectValue obj_value;
    json::Object &obj(obj_value.get());

    const cxxdb::Columns &cols(rs_ptr->columns());

    if ( cols[DBTEventlog::MSG_ID_COL] )  obj.set( "msg_id", cols[DBTEventlog::MSG_ID_COL].getString() );
    if ( cols[DBTEventlog::CATEGORY_COL] )  obj.set( "category", cols[DBTEventlog::CATEGORY_COL].getString() );
    if ( cols[DBTEventlog::COMPONENT_COL] )  obj.set( "component", cols[DBTEventlog::COMPONENT_COL].getString() );
    if ( cols[DBTEventlog::SEVERITY_COL] )  obj.set( "severity", cols[DBTEventlog::SEVERITY_COL].getString() );
    obj.set( "eventTime", cols[DBTEventlog::EVENT_TIME_COL].getTimestamp() );
    if ( cols[DBTEventlog::JOBID_COL] )  obj.set( "jobid", cols[DBTEventlog::JOBID_COL].as<int64_t>() );
    if ( cols[DBTEventlog::BLOCK_COL] )  obj.set( "block", cols[DBTEventlog::BLOCK_COL].getString() );
    if ( cols[DBTEventlog::LOCATION_COL] )  obj.set( "location", cols[DBTEventlog::LOCATION_COL].getString() );
    if ( cols[DBTEventlog::SERIALNUMBER_COL] )  obj.set( "serialnumber", cols[DBTEventlog::SERIALNUMBER_COL].getString() );
    if ( cols[DBTEventlog::ECID_COL] )  obj.set( "ecid", cols[DBTEventlog::ECID_COL].getString() );
    if ( cols[DBTEventlog::CPU_COL] )  obj.set( "cpu", cols[DBTEventlog::CPU_COL].as<int64_t>() );
    if ( cols[DBTEventlog::COUNT_COL] )  obj.set( "count", cols[DBTEventlog::COUNT_COL].as<int64_t>() );
    if ( cols[DBTEventlog::CTLACTION_COL] )  obj.set( "controlAction", cols[DBTEventlog::CTLACTION_COL].getString() );
    if ( cols[DBTEventlog::MESSAGE_COL] )  obj.set( "message", cols[DBTEventlog::MESSAGE_COL].getString() );
    if ( cols[DBTEventlog::RAWDATA_COL] )  obj.set( "rawData", cols[DBTEventlog::RAWDATA_COL].getString() );
    if ( cols[DBTMsgtypes::DESCRIPTION_COL] )  obj.set( "description", cols[DBTMsgtypes::DESCRIPTION_COL].getString() );
    if ( cols[DBTMsgtypes::SVCACTION_COL] )  obj.set( "svcAction", cols[DBTMsgtypes::SVCACTION_COL].getString() );
    if ( cols[DBTMsgtypes::THRESHOLDCOUNT_COL] )  obj.set( "thresholdCount", cols[DBTMsgtypes::THRESHOLDCOUNT_COL].as<int64_t>() );
    if ( cols[DBTMsgtypes::THRESHOLDPERIOD_COL] )  obj.set( "thresholdPeriod", cols[DBTMsgtypes::THRESHOLDPERIOD_COL].getString() );
    if ( cols[DBTMsgtypes::RELEVANTDIAGS_COL] )  obj.set( "relevantDiags", cols[DBTMsgtypes::RELEVANTDIAGS_COL].getString() );

    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( obj_value, response.out() );
    response.out() << std::endl;

}


void RasDetails::_checkAuthority(
        const Error::Data& error_data
    ) const
{
    if ( _userHasHardwareRead() ) {
        return;
    }

    LOG_WARN_MSG( "Could not get ras details because " << _getRequestUserInfo() << " doesn't have authority." );

    BOOST_THROW_EXCEPTION( Error(
            "Could not get ras details because the user doesn't have authority.",
            "getRasDetails", "authority", error_data,
            capena::http::Status::Forbidden
        ) );
}


} } // namespace bgws::responder
