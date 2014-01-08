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

/*! \page jobResource /bg/jobs/<i>jobId</i>

Represents a job on the system.

- \subpage jobResourceGet

 */

/*! \page jobResourceGet GET /bg/jobs/<i>jobId</i>

Get details for a job.

\section Authority

The user must have READ authority to the job.

\section jobResourceResponse JSON response format

<pre>
[
  { "id": <i>ID</i>,
    "executable" : &quot;<i>path</i>&quot;,
    "cwd" : &quot;<i>path</i>&quot;,
    "username" : &quot;<i>string</i>&quot;,
    "block" : &quot;<i>blockID</i>&quot;
    "workingDirectory" : &quot;<i>directory</i>&quot;,
    "arguments" : &quot;<i>args</i>&quot;,
    "environment" : &quot;<i>envs</i>&quot;,
    "startTime" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "endTime" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // Optional
    "status" : &quot;<i>jobStatus</i>&quot;,
    "nodesUsed" : <i>number</i>,
    "shape" : &quot;<i>shape</i>&quot;, // like AxBxCxDxE
    "corner" : &quot;<i>location</i>&quot;, // Optional
    "processesPerNode" : <i>number</i>,
    "np" : <i>number</i>,
    "mapping" : &quot;<i>ABCDET or filename</i>&quot;, // Optional
    "schedulerData" : &quot;<i>string</i>&quot;, // Optional
    "client" : &quot;<i>host:pid</i>&quot;,
    "exitStatus" : <i>number</i>, // Optional
    "errorText" : &quot;<i>string</i>&quot; // Optional
  },
  ...
]
</pre>

Where
- <i>jobStatus</i> is one of P=Setup, L=Loading, S=Starting, R=Running, D=Debug, N=Cleanup, T=Terminated, E=Error

\section Errors

HTTP status: 400 Bad Request
- invalidId: Job ID isn't valid.

HTTP status: 403 Forbidden
- notAuthenticated: User isn't authenticated.

HTTP status: 404 Not Found
- notFound: The Job doesn't exist, or user doesn't have authority to get the job details.

 */


#include "Job.hpp"

#include "../dbConnectionPool.hpp"
#include "../Error.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/gensrc/DBTJob.h>
#include <db/include/api/tableapi/gensrc/DBTJob_history.h>

#include <hlcs/include/security/wrapper.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <string>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


capena::http::uri::Path Job::calcUri(
        const capena::http::uri::Path& path_base,
        uint64_t job_id
    )
{
    return (path_base / Jobs::RESOURCE_PATH / lexical_cast<string>( job_id ));
}


void Job::_doGet()
{
    const capena::server::Request &request(_getRequest());

    const string &jobid_str(request.getUrl().getPath().back());

    Error::Data error_data;
    error_data["id"] = jobid_str;

    uint64_t jobid;

    try {
        jobid = lexical_cast<uint64_t>( jobid_str );
    } catch ( boost::bad_lexical_cast& e ) {

        BOOST_THROW_EXCEPTION( Error(
                string() + "Could not get details for job '" + jobid_str + "' because the job does not exist.",
                "getJobDetails", "invalidId", error_data,
                capena::http::Status::BadRequest
            ) );
    }

    if ( jobid == 0 ) {
        BOOST_THROW_EXCEPTION( Error(
                string() + "Could not get details for job " + lexical_cast<string>( jobid ) + " because the job does not exist.",
                "getJobDetails", "notFound", error_data,
                capena::http::Status::NotFound
            ) );
    }

    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get job because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                string() + "Cannot get job because the user isn't authenticated.",
                "getJobDetails", "notAuthenticated", error_data,
                capena::http::Status::Forbidden
            ) );
    }


    bgq::utility::UserId::ConstPtr user_id_ptr;

    if ( ! _isUserAdministrator() ) {
        user_id_ptr = _getRequestUserInfo().getUserIdPtr();
    }


    auto conn_ptr(dbConnectionPool::getConnection());
    uint64_t all_count(0);
    cxxdb::ResultSetPtr rs_ptr;

    BGQDB::filtering::JobFilter job_filter;
    job_filter.setJobId( jobid );
    job_filter.setJobType( BGQDB::filtering::JobFilter::JobType::All );

    LOG_DEBUG_MSG( "Getting details for job " << jobid << "..." );

    hlcs::security::getJobs(
            job_filter,
            BGQDB::filtering::JobSort(),
            conn_ptr,
            _properties_ptr,
            0 /* range_start */,
            0 /* range_end */,
            &all_count,
            &rs_ptr,
            user_id_ptr
        );


    LOG_DEBUG_MSG( "Got details for job " << jobid << ", count=" << all_count );

    if ( all_count == 0 ) {
        // Didn't find the job, or didn't have authority.

        BOOST_THROW_EXCEPTION( Error(
                string() + "Could not get details for job " + lexical_cast<string>( jobid ) + " because the job does not exist.",
                "getJobDetails", "notFound", error_data,
                capena::http::Status::NotFound
            ) );
    }

    rs_ptr->fetch();

    json::ObjectValue obj_val;
    json::Object &obj(obj_val.get());

    const auto &cols(rs_ptr->columns());

    string shape_str(
            lexical_cast<string>( cols[BGQDB::DBTJob_history::SHAPEA_COL].as<uint64_t>() ) + "x" +
            lexical_cast<string>( cols[BGQDB::DBTJob_history::SHAPEB_COL].as<uint64_t>() ) + "x" +
            lexical_cast<string>( cols[BGQDB::DBTJob_history::SHAPEC_COL].as<uint64_t>() ) + "x" +
            lexical_cast<string>( cols[BGQDB::DBTJob_history::SHAPED_COL].as<uint64_t>() ) + "x" +
            lexical_cast<string>( cols[BGQDB::DBTJob_history::SHAPEE_COL].as<uint64_t>() )
        );

    string hostname(cols[BGQDB::DBTJob_history::HOSTNAME_COL].getString());
    bool hostname_brackets(hostname.find( ':' ) != string::npos);

    string client_str( string() +
            (hostname_brackets ? "[" : "") + hostname + (hostname_brackets ? "]" : "") + ":" +
            lexical_cast<string>( cols[BGQDB::DBTJob_history::PID_COL].as<uint64_t>() )
        );


    obj.set( "id", cols[BGQDB::DBTJob_history::ID_COL].as<uint64_t>() );
    obj.set( "executable", cols[BGQDB::DBTJob_history::EXECUTABLE_COL].getString() );
    obj.set( "cwd", cols[BGQDB::DBTJob_history::WORKINGDIR_COL].getString() );
    obj.set( "username", cols[BGQDB::DBTJob_history::USERNAME_COL].getString() );
    obj.set( "block", cols[BGQDB::DBTJob_history::BLOCKID_COL].getString() );
    obj.set( "workingDirectory", cols[BGQDB::DBTJob_history::WORKINGDIR_COL].getString() );
    obj.set( "arguments", cols[BGQDB::DBTJob_history::ARGS_COL].getString() );
    obj.set( "environment", cols[BGQDB::DBTJob_history::ENVS_COL].getString() );
    obj.set( "startTime", cols[BGQDB::DBTJob_history::STARTTIME_COL].getTimestamp() );
    if ( ! cols[BGQDB::DBTJob_history::ENTRYDATE_COL].isNull() )  obj.set( "endTime", cols[BGQDB::DBTJob_history::ENTRYDATE_COL].getTimestamp() );
    obj.set( "status", cols[BGQDB::DBTJob_history::STATUS_COL].getString() );
    obj.set( "nodesUsed", cols[BGQDB::DBTJob_history::NODESUSED_COL].as<uint64_t>() );
    obj.set( "processesPerNode", cols[BGQDB::DBTJob_history::PROCESSESPERNODE_COL].as<uint64_t>() );
    obj.set( "np", cols[BGQDB::DBTJob_history::NP_COL].as<uint64_t>() );
    if ( ! cols[BGQDB::DBTJob_history::CORNER_COL].isNull() ) {
        obj.set( "corner", cols[BGQDB::DBTJob_history::CORNER_COL].getString() );
    }
    obj.set( "shape", shape_str );
    if ( ! cols[BGQDB::DBTJob_history::MAPPING_COL].getString().empty() ) {
        obj.set( "mapping", cols[BGQDB::DBTJob_history::MAPPING_COL].getString() );
    }
    if ( ! cols[BGQDB::DBTJob_history::SCHEDULERDATA_COL].isNull() ) {
        obj.set( "schedulerData", cols[BGQDB::DBTJob_history::SCHEDULERDATA_COL].getString() );
    }
    obj.set( "client", client_str );
    if ( ! cols[BGQDB::DBTJob_history::EXITSTATUS_COL].isNull() ) {
        obj.set( "exitStatus", cols[BGQDB::DBTJob_history::EXITSTATUS_COL].as<int>() );
    }
    if ( ! cols[BGQDB::DBTJob_history::ERRTEXT_COL].isNull() ) {
        obj.set( "errorText", cols[BGQDB::DBTJob_history::ERRTEXT_COL].getString() );
    }

    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();
    json::Formatter()( obj_val, response.out() );
}


}} // namespace bgws::responder
