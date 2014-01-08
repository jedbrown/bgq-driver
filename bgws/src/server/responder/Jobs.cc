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

/*! \page jobsResource /bg/jobs

This resource is a container for the jobs defined on the system.

- \subpage jobsResourceGet

 */

/*! \page jobsResourceGet GET /bg/jobs

Get a summary of jobs.

\note This resource supports \ref partialResponse "partial responses".


\section Authority

The user must be authenticated. The user will only get jobs to which they have READ authority.

If the query parameters contains serviceLocation, the user must be an administrator.


\section jobsResourceGetParameters Query parameters

- status: Job status filter.
  - A string of characters, where each character is a status code.
  - Only jobs with status in the string are returned.
  - Status codes and the status value that each code maps to are:
    - D: Debug
    - E: Error
    - L: Loading
    - N: Cleanup
    - P: Setup
    - R: Running
    - S: Starting
    - T: Terminated

- id: Job ID. If set, only the job with the given job ID is returned.
- user: User name filter. If set, only jobs with a matching username are returned.
- block: block ID filter. If set, only jobs with a matching block are returned.
- executable: executable filter. The format allows wildcard characters, * for any string of characters and ? for any single character. If set, only jobs with a matching executable are returned. Wildcard characters are allowed in V1R1M1.
- startTime: Start time filter, a \ref timeIntervalFormat "time interval". If set, only jobs with a start time in the time interval are returned.
- endTime: End time filter, a \ref timeIntervalFormat "time interval". If set, only jobs with an end time in the time interval are returned.
- exitStatus: exit status filter, a number. If set, only jobs with a matching exit status are returned.

- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - direction is + for ascending or - for descending, the default is ascending.
  - field is one of:
    - block
    - endTime
    - executable
    - id
    - nodesUsed
    - processesPerNode
    - startTime
    - status
    - username

- serviceLocation: This is a special parameter, the response will include a list of current jobs that are affected by a service action on that location.
  - The user must be an administrator.
  - Other query parameters are ignored.
  - Partial responses are not supported.


\section jobsResourceGetResponse JSON response format

<pre>
[
  { "id": <i>ID</i>,
    "username" : &quot;<i>string</i>&quot;,
    "block" : <i>blockID</i>,
    "executable" : &quot;<i>path</i>&quot;, // Optional
    "cwd" : &quot;<i>path</i>&quot;, // Optional
    "startTime" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "endTime" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // Optional
    "status" : &quot;<i>jobStatus</i>&quot;,
    "nodesUsed" : <i>number</i>,
    "processesPerNode" : <i>number</i>,
    "np" : <i>number</i>,
    "exitStatus": <i>number</i>, // Optional
    "errorText" : &quot;<i>string</i>&quot;, // Optional
    "URI" : &quot;<i>\ref jobResource</i>&quot;
  },
  ...
]
</pre>

Where
- <i>jobStatus</i> is one of P=Setup, L=Loading, S=Starting, R=Running, D=Debug, N=Cleanup, T=Terminated, E=Error


\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: User isn't authenticated.
- notAdministrator: User isn't administrator.

HTTP status: 500 Internal Server Error
- listMidplaneJobsDatabaseError: Database error.

 */


#include "Jobs.hpp"

#include "Job.hpp"

#include "../BlockingOperationsThreadPool.hpp"
#include "../dbConnectionPool.hpp"
#include "../Error.hpp"
#include "../ras.hpp"
#include "../RequestRange.hpp"
#include "../SortSpec.hpp"
#include "../StringDbColumnOption.hpp"
#include "../TimeIntervalOption.hpp"

#include "common/common.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <hlcs/include/security/wrapper.h>

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <sstream>
#include <string>


using BGQDB::DBTJob;
using BGQDB::DBTJob_history;

using BGQDB::filtering::JobFilter;
using BGQDB::filtering::JobSort;
using BGQDB::filtering::SortOrder;

using boost::lexical_cast;

using std::istringstream;
using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


void jobIdNotifier( const std::string& job_id_str, JobFilter::Id& id_out )
{
    istringstream iss( job_id_str );

    iss >> id_out;

    if ( ! iss ) {
        id_out = JobFilter::Id(-1);
        LOG_DEBUG_MSG( "Got invalid job ID in query. Job ID string is '" << job_id_str << "'" );
    }
}


void clientNotifier( const std::string& client_str, std::string& hostname_out, int& pid_out )
{
    try {
        string::size_type colon_pos(client_str.find( ':' ));
        if ( colon_pos == string::npos ) {

            hostname_out = boost::asio::ip::host_name();
            pid_out = lexical_cast<int>( client_str );

        } else {

            hostname_out = client_str.substr( 0, colon_pos );
            pid_out = lexical_cast<int>( client_str.substr( colon_pos + 1 ) );

        }

        if ( hostname_out.size() > DBTJob::HOSTNAME_SIZE ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( "invalid hostname is too long" ) );
        }

    } catch ( std::exception& e ) {

        LOG_WARN_MSG( "Invalid client option, client string is '" << client_str << "'" );
        hostname_out = "";
        pid_out = -1;

    }
}


const capena::http::uri::Path &Jobs::RESOURCE_PATH(::bgws::common::resource_path::JOBS);
const capena::http::uri::Path Jobs::RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");


void Jobs::_doGet()
{
    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get jobs because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                string() + "Cannot get jobs because the user isn't authenticated.",
                "getJobs", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );
    }


    const capena::server::Request &request(_getRequest());

    static const unsigned DefaultRangeSize(50), MaxRangeSize(100);
    RequestRange req_range( request, DefaultRangeSize, MaxRangeSize );


    bool no_jobs(false);
    JobFilter job_filter;
    JobSort job_sort;
    string service_location;

    _calcJobFilterAndSort(
            &no_jobs,
            job_filter,
            job_sort,
            service_location
        );

    if ( no_jobs ) {
        capena::server::Response &response(_getResponse());

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( json::ArrayValue(), response.out() );

        return;
    }

    if ( service_location != string() ) {

        _getServiceJobs( service_location );

        return;
    }


    _blocking_operations_thread_pool.post( boost::bind(
            &Jobs::_doQuery, this,
            capena::server::AbstractResponder::shared_from_this(),
            job_filter,
            job_sort,
            req_range
        ) );

}


void Jobs::_calcJobFilterAndSort(
        bool* no_jobs_out,
        BGQDB::filtering::JobFilter& job_filter_out,
        BGQDB::filtering::JobSort& job_sort_out,
        std::string& service_location_out
    ) const
{
    namespace po = boost::program_options;

    *no_jobs_out = false;

    po::options_description desc;

    StringDbColumnOption block_id( "block", BGQDB::DBTJob::BLOCKID_SIZE );
    StringDbColumnOption executable( "executable", BGQDB::DBTJob::EXECUTABLE_SIZE );
    int exit_status;
    JobFilter::Id id(-1);
    string status_str;
    StringDbColumnOption username( "user", BGQDB::DBTJob::USERNAME_SIZE );

    string hostname;
    int pid(-1);

    TimeIntervalOption end_time_interval( "endTime" );
    TimeIntervalOption start_time_interval( "startTime" );

    SortSpec sort_spec;

    StringDbColumnOption service_location( "serviceLocation", 64 ); // using const for max size of a location in the DB.


    desc.add_options()
            ( "exitStatus", po::value( &exit_status ) )
    		( "id", po::value<string>()->notifier( bind( &jobIdNotifier, _1, boost::ref( id ) ) ) )
            ( "status", po::value( &status_str ) )
            ( "sort", po::value( &sort_spec ) )
            ( "client", po::value<string>()->notifier( bind( &clientNotifier, _1, boost::ref( hostname ), boost::ref( pid ) ) ) )
        ;

    block_id.addTo( desc );
    end_time_interval.addTo( desc );
    executable.addTo( desc );
    start_time_interval.addTo( desc );
    username.addTo( desc );
    service_location.addTo( desc );

    po::variables_map vm;
    po::store( po::command_line_parser( _getRequest().getUrl().getQuery().calcArguments() ).options( desc ).allow_unregistered().run(), vm );
    po::notify( vm );


    if ( service_location.hasValue() ) {
        service_location_out = service_location.getValue();
        return;
    }


    bool active_only(false);
    bool history_only(false);

    JobFilter job_filter;


    if ( block_id.hasValue() )  job_filter.setComputeBlockName( block_id.getValue() );
    if ( executable.hasValue() )  job_filter.setExecutable( executable.getValue() );
    if ( vm.count( "exitStatus" ) ) {
        history_only = true;
        job_filter.setExitStatus( exit_status );
    }
    if ( id != JobFilter::Id(-1) ) {
    	job_filter.setJobId( id );
    }
    if ( username.hasValue() )  job_filter.setUser( username.getValue() );
    if ( ! hostname.empty() )  job_filter.setHostname( hostname );
    if ( pid != -1 )  job_filter.setPid( pid );

    if ( ! (start_time_interval.getInterval() == BGQDB::filtering::TimeInterval() || start_time_interval.getInterval() == BGQDB::filtering::TimeInterval::ALL) ) {
        job_filter.setStartTimeInterval( start_time_interval.getInterval() );
    }

    if ( status_str.empty() ) {
        // Set the statuses to all jobs.

        job_filter.setJobType( JobFilter::JobType::All );

    } else {
        JobFilter::Statuses statuses;

        bool any_active(false);
        bool any_history(false);

        BOOST_FOREACH( char status_code, status_str ) {

            if ( status_code == 'P' )  { statuses.insert( BGQDB::job::status::Setup ); any_active = true; }
            if ( status_code == 'L' )  { statuses.insert( BGQDB::job::status::Loading ); any_active = true; }
            if ( status_code == 'S' )  { statuses.insert( BGQDB::job::status::Starting ); any_active = true; }
            if ( status_code == 'R' )  { statuses.insert( BGQDB::job::status::Running ); any_active = true; }
            if ( status_code == 'D' )  { statuses.insert( BGQDB::job::status::Debug ); any_active = true; }
            if ( status_code == 'N' )  { statuses.insert( BGQDB::job::status::Cleanup ); any_active = true; }
            if ( status_code == 'T' )  { statuses.insert( BGQDB::job::status::Terminated ); any_history = true; }
            if ( status_code == 'E' )  { statuses.insert( BGQDB::job::status::Error ); any_history = true; }

        }

        if ( any_active || any_history ) {

            job_filter.setStatuses( &statuses );

            if ( ! any_active )  history_only = true;
            if ( ! any_history )  active_only = true;

        } else {

            LOG_DEBUG_MSG( "No valid statuses, ignoring statuses" );

        }
    }


    if ( ! (end_time_interval.getInterval() == BGQDB::filtering::TimeInterval() || end_time_interval.getInterval() == BGQDB::filtering::TimeInterval::ALL) ) {
        job_filter.setEndTimeInterval( end_time_interval.getInterval() );
        history_only = true;
    }


    if ( history_only && active_only ) {
        // Not going to return anything.

        *no_jobs_out = true;

        return;

    } else if ( history_only ) {

        job_filter.setJobType( JobFilter::JobType::Completed );

    } else if ( active_only ) {

        job_filter.setJobType( JobFilter::JobType::Active );

    } else { // both history and jobs.

        job_filter.setJobType( JobFilter::JobType::All );

    }


    JobSort job_sort;

    bool set_default_sort(false);

    if ( sort_spec.isDefault() ) {
        set_default_sort = true;
    } else {

        JobSort::Field::Value field;
        SortOrder::Value order(sort_spec.getDirection() == utility::SortDirection::Ascending ? SortOrder::Ascending : SortOrder::Descending);

        const auto &col_id(sort_spec.getColumnId());

        if ( col_id == "id" )  field = JobSort::Field::Id;
        else if ( col_id == "username" )  field = JobSort::Field::User;
        else if ( col_id == "block" )  field = JobSort::Field::Block;
        else if ( col_id == "executable" )  field = JobSort::Field::Executable;
        else if ( col_id == "startTime" )  field = JobSort::Field::StartTime;
        else if ( col_id == "status" )  field = JobSort::Field::Status;
        else if ( col_id == "nodesUsed" )  field = JobSort::Field::ComputeNodesUsed;
        else if ( col_id == "processesPerNode" )  field = JobSort::Field::RanksPerNode;
        else if ( col_id == "endTime" )  field = JobSort::Field::EndTime;
        else if ( col_id == "exitStatus" )  field = JobSort::Field::ExitStatus;
        else {
            LOG_INFO_MSG( "Unexpected sort field '" << col_id << "', ignoring sort parameter." );
            set_default_sort = true;
        }

        if ( ! set_default_sort ) {

            LOG_TRACE_MSG( "Setting sort field=" << field << " order=" << order );

            job_sort.setSort( field, order );
        }
    }

    if ( set_default_sort ) {
        if ( history_only ) {
            job_sort.setSort( JobSort::Field::EndTime, SortOrder::Descending );
        } else {
            job_sort.setSort( JobSort::Field::Id, SortOrder::Ascending );
        }
    }

    job_filter_out = job_filter;
    job_sort_out = job_sort;

    LOG_DEBUG_MSG( "job_sort sorted=" << job_sort.isSorted() << " job_sort_out sorted=" << job_sort_out.isSorted() );
}


void Jobs::_getServiceJobs( const std::string& location )
{
    Error::Data error_data;
    error_data["location"] = location;

    // The user must be authenticated.
    if ( ! _isUserAdministrator() ) {
        LOG_WARN_MSG( "Cannot get jobs affected by service action because the user isn't an administrator." );

        ras::postAdminAuthorityFailure( _getRequestUserInfo(), "get jobs affected by service action" );

        BOOST_THROW_EXCEPTION( Error(
                string() + "Cannot get jobs affected by service action because the user isn't an administrator.",
                "getJobs", "notAdministrator", error_data,
                capena::http::Status::Forbidden
            ) );
    }

    _blocking_operations_thread_pool.post( boost::bind(
            &Jobs::_doServiceQuery, this,
            capena::server::AbstractResponder::shared_from_this(),
            location
        ) );

}


void Jobs::_doQuery(
        capena::server::ResponderPtr,
        const BGQDB::filtering::JobFilter& job_filter,
        const BGQDB::filtering::JobSort& job_sort,
        const RequestRange& req_range
    )
{
    try {

        auto conn_ptr(dbConnectionPool::getConnection());

        uint64_t all_count(0);
        cxxdb::ResultSetPtr rs_ptr;

        bgq::utility::UserId::ConstPtr user_id_ptr;

        if ( ! _isUserAdministrator() ) {
            user_id_ptr = _getRequestUserInfo().getUserIdPtr();
        }

        hlcs::security::getJobs(
                job_filter,
                job_sort,
                conn_ptr,
                _properties_ptr,
                req_range.getRange().getStart() + 1,
                req_range.getRange().getEnd() + 1,
                &all_count,
                &rs_ptr,
                user_id_ptr
            );


        _getStrand().post( boost::bind(
                &Jobs::_queryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                conn_ptr,
                all_count,
                rs_ptr,
                req_range
            ) );

    } catch ( std::exception& e ) {

        _inCatchPostCurrentExceptionToHandlerFn();

    }

}


void Jobs::_queryComplete(
        capena::server::ResponderPtr,
        cxxdb::ConnectionPtr,
        uint64_t all_count,
        cxxdb::ResultSetPtr rs_ptr,
        const RequestRange& req_range
    )
{

    try {

        // Format the result as json.

        json::ArrayValue arr_val;
        json::Array &arr(arr_val.get());


        capena::server::Response &response(_getResponse());

        if ( all_count == 0 ) {

            // No rows match the filter options.

            response.setContentTypeJson();
            response.headersComplete();

            json::Formatter()( arr_val, response.out() );

            return;
        }

        while ( rs_ptr->fetch() ) {
            const cxxdb::Columns &cols(rs_ptr->columns());

            json::Object &obj(arr.addObject());

            BGQDB::job::Id id(cols[DBTJob::ID_COL].as<BGQDB::job::Id>());

            obj.set( "id", id );
            obj.set( "username", cols[DBTJob::USERNAME_COL].getString() );
            obj.set( "block", cols[DBTJob::BLOCKID_COL].getString() );
            obj.set( "executable", cols[DBTJob::EXECUTABLE_COL].getString() );
            obj.set( "cwd", cols[DBTJob::WORKINGDIR_COL].getString() );
            obj.set( "startTime", cols[DBTJob::STARTTIME_COL].getTimestamp() );
            if ( cols[DBTJob_history::ENTRYDATE_COL] ) {
                obj.set( "endTime", cols[DBTJob_history::ENTRYDATE_COL].getTimestamp() );
            }
            obj.set( "status", cols[DBTJob::STATUS_COL].getString() );
            obj.set( "nodesUsed", cols[DBTJob::NODESUSED_COL].as<uint64_t>() );
            obj.set( "processesPerNode", cols[DBTJob::PROCESSESPERNODE_COL].as<uint64_t>() );
            obj.set( "np", cols[DBTJob::NP_COL].as<uint64_t>() );
            if ( cols[DBTJob_history::EXITSTATUS_COL] ) {
                obj.set( "exitStatus", cols[DBTJob_history::EXITSTATUS_COL].as<int>() );
            }
            if ( cols[DBTJob_history::ERRTEXT_COL] ) {
                obj.set( "errorText", cols[DBTJob_history::ERRTEXT_COL].getString() );
            }
            obj.set( "URI", Job::calcUri( _getDynamicConfiguration().getPathBase(), id ).toString() );
        }


        // Send the response...

        req_range.updateResponse( response, arr.size(), all_count );

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( arr_val, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


void Jobs::_doServiceQuery(
        capena::server::ResponderPtr,
        const std::string& location
    )
{
    try {

        Error::Data error_data;
        error_data["location"] = location;


        std::vector<BGQDB::job::Id> job_ids;

        BGQDB::STATUS status(BGQDB::killMidplaneJobs( location, &job_ids, true /* list only */ ));

        if ( status == BGQDB::INVALID_ID ) {
            LOG_DEBUG_MSG( "killMidplaneJobs returned invalid id for " << location << " will return an empty result." );
            status = BGQDB::OK;
        } else if ( status != BGQDB::OK ) {
            error_data["dbStatus"] = lexical_cast<string>( status );

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Cannot get jobs affected by service action because the database operation failed. The database return code is " + lexical_cast<string>( status ),
                    "getJobs", "listMidplaneJobsDatabaseError", error_data,
                    capena::http::Status::InternalServerError
                ) );
        }

        json::ArrayValue arr_val;

        if ( job_ids.empty() ) {

            _getStrand().post( boost::bind(
                    &Jobs::_serviceQueryComplete, this,
                    capena::server::AbstractResponder::shared_from_this(),
                    cxxdb::ConnectionPtr(),
                    cxxdb::ResultSetPtr()
                ) );

            return;
        }

        LOG_INFO_MSG( "killMidplaneJobs for " << location << " returned " << job_ids.size() << " jobs." );

        string job_ids_sql;
        BOOST_FOREACH( BGQDB::job::Id job_id, job_ids ) {
            if ( ! job_ids_sql.empty() )  job_ids_sql += ",";
            job_ids_sql += lexical_cast<string>( job_id );
        }

        // Query to get the job details for each job ID.
        const string sql =
 "SELECT * FROM bgqJob WHERE id IN (" + job_ids_sql + ") ORDER BY id ASC"
            ;

        auto conn_ptr(dbConnectionPool::getConnection());

        LOG_DEBUG_MSG( "executing\n" << sql );

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( sql ));

        _getStrand().post( boost::bind(
                &Jobs::_serviceQueryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );

    } catch ( std::exception& e ) {

        _inCatchPostCurrentExceptionToHandlerFn();

    }
}


void Jobs::_serviceQueryComplete(
        capena::server::ResponderPtr,
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    try {

        json::ArrayValue arr_val;

        auto &response(_getResponse());

        if ( ! rs_ptr ) {
            response.setContentTypeJson();
            response.headersComplete();

            json::Formatter()( arr_val, response.out() );
            return;
        }


        json::Array &arr(arr_val.get());

        while ( rs_ptr->fetch() ) {
            const cxxdb::Columns &cols(rs_ptr->columns());

            json::Object &obj(arr.addObject());

            BGQDB::job::Id id(cols[DBTJob::ID_COL].as<BGQDB::job::Id>());

            obj.set( "id", id );
            obj.set( "username", cols[DBTJob::USERNAME_COL].getString() );
            obj.set( "block", cols[DBTJob::BLOCKID_COL].getString() );
            obj.set( "executable", cols[DBTJob::EXECUTABLE_COL].getString() );
            obj.set( "cwd", cols[DBTJob::WORKINGDIR_COL].getString() );
            obj.set( "startTime", cols[DBTJob::STARTTIME_COL].getTimestamp() );
            obj.set( "status", cols[DBTJob::STATUS_COL].getString() );
            obj.set( "nodesUsed", cols[DBTJob::NODESUSED_COL].as<uint64_t>() );
            obj.set( "processesPerNode", cols[DBTJob::PROCESSESPERNODE_COL].as<uint64_t>() );
            obj.set( "np", cols[DBTJob::NP_COL].as<uint64_t>() );
            obj.set( "URI", Job::calcUri( _getDynamicConfiguration().getPathBase(), id ).toString() );
        }

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( arr_val, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


}} // namespace bgws::responder
