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
#include "utility.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/job/types.h>

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <stdint.h>


using bgws::command::BgwsClient;
using bgws::command::JobsSummaryFilter;

using boost::lexical_cast;

using std::cout;
using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws.command" );


struct SummaryFormat {
    enum Value {
        SHORT, LONG
    };
};


static boost::format NQ_LINE_FMT( "%1%: %2%\n" );
static boost::format Q_LINE_FMT( "%1%: '%2%'\n" );


static string statusCodeToString( const std::string &status_code )
{
    return (status_code == "D" ? "Debug" :
            status_code == "E" ? "Error" :
            status_code == "L" ? "Loading" :
            status_code == "N" ? "Cleanup" :
            status_code == "P" ? "Setup" :
            status_code == "R" ? "Running" :
            status_code == "S" ? "Starting" :
            status_code == "T" ? "Terminated" :
            status_code);
}


typedef std::vector<json::ArrayValuePtr> ArrayValuePtrs;


static void printJobsSummary(
        const ArrayValuePtrs& value_ptrs,
        unsigned total_jobs,
        unsigned jobs_returned_count,
        SummaryFormat::Value summary_format
    )
{
    if ( value_ptrs.empty() || value_ptrs.front()->getArray().empty() ) {
        cout << "No jobs returned.\n";
        return;
    }

    if ( summary_format == SummaryFormat::SHORT ) {

        if ( total_jobs == 0 ) {
            json::ValuePtr jobs_summary_val_ptr(value_ptrs.front());
            const json::Array &jobs(jobs_summary_val_ptr->getArray());

            std::cout << jobs.size() << " job" << (jobs.size() == 1 ? "" : "s") << std::endl;
        } else if ( jobs_returned_count == total_jobs ) {
            std::cout << total_jobs << " job" << (total_jobs == 1 ? "" : "s") << std::endl;
        } else {
            std::cout << jobs_returned_count << " of " << total_jobs << " job" << (total_jobs == 1 ? "" : "s") << std::endl;
        }

        static const string id_column_name( "ID" );
        static const string status_column_name( "Status" );
        static const string executable_column_name( "Executable" );
        static const string block_column_name( "Block" );
        static const string user_column_name( "User" );


        size_t id_column_size = id_column_name.size();
        size_t status_column_size = 10; // length of longest of any status text and column name
        size_t executable_column_size = executable_column_name.size();
        size_t block_column_size = block_column_name.size();
        size_t user_column_size = user_column_name.size();


        // calculate maximum size of each value when converted to a string
        BOOST_FOREACH( json::ValuePtr jobs_summary_val_ptr, value_ptrs ) {

            const json::Array &jobs(jobs_summary_val_ptr->getArray());

            BOOST_FOREACH( const json::Array::value_type &job_info_ptr, jobs ) {
                const json::Object &job_obj(job_info_ptr->getObject());

                const std::string idString = boost::lexical_cast<std::string>( job_obj.as<uint64_t>( "id" ) );
                id_column_size = std::max( lexical_cast<string>( job_obj.as<int64_t>( "id" ) ).size(), id_column_size );
                executable_column_size = std::max( job_obj.getString( "executable" ).size(), executable_column_size );
                block_column_size = std::max( job_obj.getString( "block" ).size(), block_column_size );
                user_column_size = std::max( job_obj.getString( "username" ).size(), user_column_size );
            }

        }

        // create format string using sizes previously calculated
        std::ostringstream formatting;
        formatting << "%" << (id_column_size + 1) << "s ";
        formatting << "%-" << (status_column_size + 1) << "s ";
        formatting << "%-" << (executable_column_size + 1) << "s ";
        formatting << "%-" << (block_column_size + 1) << "s ";
        formatting << "%-" << (user_column_size + 1) << "s\n";

        std::cout << boost::format( formatting.str() ) % id_column_name % status_column_name % executable_column_name % block_column_name % user_column_name;

        BOOST_FOREACH( json::ValuePtr jobs_summary_val_ptr, value_ptrs ) {

            const json::Array &jobs(jobs_summary_val_ptr->getArray());

            // Output each of the result rows.
            BOOST_FOREACH( const json::Array::value_type &job_info_ptr, jobs ) {
                const json::Object &job_obj(job_info_ptr->getObject());

                std::cout << boost::format( formatting.str() )
                        % job_obj.as<uint64_t>( "id" )
                        % statusCodeToString( job_obj.getString( "status" ) )
                        % job_obj.getString( "executable" )
                        % job_obj.getString( "block" )
                        % job_obj.getString( "username" )
                    ;
            }

        }

    } else {
        // Long format.

        BOOST_FOREACH( json::ValuePtr jobs_summary_val_ptr, value_ptrs ) {

            const json::Array &jobs(jobs_summary_val_ptr->getArray());

            BOOST_FOREACH( const json::Array::value_type &job_info_ptr, jobs ) {

                const json::Object &job_obj(job_info_ptr->getObject());

                cout << str( NQ_LINE_FMT % "ID" % job_obj.as<int64_t>( "id" ) )
                     << str( Q_LINE_FMT % "User" % job_obj.getString( "username" ) )
                     << str( Q_LINE_FMT % "Executable" % job_obj.getString( "executable" ) )
                     << str( NQ_LINE_FMT % "Status" % statusCodeToString( job_obj.getString( "status" ) ) )
                     << str( NQ_LINE_FMT % "Block" % job_obj.getString( "block" ) )
                     << str( NQ_LINE_FMT % "Nodes" % job_obj.as<int64_t>( "nodesUsed" ) )
                     << str( NQ_LINE_FMT % "Processes per node" % job_obj.as<int64_t>( "processesPerNode" ) )
                     << str( NQ_LINE_FMT % "Start time" % job_obj.getString( "startTime" ) );

                if ( job_obj.contains( "endTime" ) ) {
                    cout << str( NQ_LINE_FMT % "End time" % job_obj.getString( "endTime" ) );
                }
                if ( job_obj.contains( "exitStatus" ) ) {
                    cout << str( NQ_LINE_FMT % "Exit status" % job_obj.as<int>( "exitStatus" ) );
                }
                if ( job_obj.contains( "errorText" ) ) {
                    cout << str( Q_LINE_FMT % "Error text" % job_obj.getString( "errorText" ) );
                }

                cout << "\n";
            }

        }
    }
}


static void printJobDetails(
        json::ObjectValuePtr job_details_val_ptr
    )
{
    const json::Object &job_obj(job_details_val_ptr->get());

    cout << str( NQ_LINE_FMT % "ID" % job_obj.as<uint64_t>( "id" ) )
         << str( Q_LINE_FMT % "User" % job_obj.getString( "username" ) )
         << str( Q_LINE_FMT % "Executable" % job_obj.getString( "executable" ) )
         << str( NQ_LINE_FMT % "Status" % statusCodeToString( job_obj.getString( "status" ) ) )
         << str( Q_LINE_FMT % "Working directory" % job_obj.getString( "cwd" ) )
         << str( Q_LINE_FMT % "Arguments" % job_obj.getString( "arguments" ) )
         << str( Q_LINE_FMT % "Environment" % job_obj.getString( "environment" ) );

    if ( job_obj.contains( "statusChangeTime" ) ) {
        cout << NQ_LINE_FMT % "Status changed" % job_obj.getString( "statusChangeTime" );
    }

    cout << str( NQ_LINE_FMT % "Block" % job_obj.getString( "block" ) )
         << str( NQ_LINE_FMT % "Nodes" % job_obj.as<int64_t>( "nodesUsed" ) )
         << str( NQ_LINE_FMT % "Processes per node" % job_obj.as<int64_t>( "processesPerNode" ) )
         << str( NQ_LINE_FMT % "Shape" % job_obj.getString( "shape" ) );

    if ( job_obj.contains( "corner" ) ) {
        cout << NQ_LINE_FMT % "Corner" % job_obj.getString( "corner" );
    }

    cout << str( NQ_LINE_FMT % "Start time" % bgws::command::utility::formatTimestamp( job_obj.getString( "startTime" ) ) );

    if ( job_obj.contains( "endTime" ) ) {
        cout << str( NQ_LINE_FMT % "End time" % bgws::command::utility::formatTimestamp( job_obj.getString( "endTime" ) ) );
    }

    if ( job_obj.contains( "mapping" ) ) {
        cout << str( Q_LINE_FMT % "Mapping" % job_obj.getString( "mapping" ) );
    }

    if ( job_obj.contains( "schedulerData" ) ) {
       cout << str( Q_LINE_FMT % "Scheduler data" % job_obj.getString( "schedulerData" ) );
    }

    cout << str( NQ_LINE_FMT % "Client" % job_obj.getString( "client" ) );

    if ( job_obj.contains( "exitStatus" ) ) {
       cout << str( NQ_LINE_FMT % "Exit status" % job_obj.as<int>( "exitStatus" ) );
    }

    if ( job_obj.contains( "errorText" ) ) {
        cout << str( Q_LINE_FMT % "Error text" % job_obj.getString( "errorText" ) );
    }

    cout << "\n";
}


static void listJobs(
        BGQDB::job::Id jobid,
        const JobsSummaryFilter& jobs_summary_filter,
        SummaryFormat::Value summary_format,
        bool request_all,
        BgwsClient& bgws_client
    )
{
    LOG_DEBUG_MSG( "jobid= " << jobid );

    if ( jobid == BGQDB::job::Id(-1) ) {

        ArrayValuePtrs value_ptrs;

        uint64_t all_job_count(0), all_jobs_returned_count(0);

        uint64_t range_start(0);

        while ( true ) {

            json::ArrayValuePtr jobs_summary_val_ptr;
            uint64_t request_total_jobs(0), content_range_end_out(0);

            boost::optional<bgws::common::ItemRange> range_opt;

            if ( range_start != 0 ) {
                static const uint64_t fetch_size(50);
                range_opt = bgws::common::ItemRange::createCount( range_start, fetch_size );
            }

            bgws_client.getJobsSummary(
                    jobs_summary_filter,
                    range_opt,
                    &jobs_summary_val_ptr,
                    &request_total_jobs,
                    &content_range_end_out
                );

            value_ptrs.push_back( jobs_summary_val_ptr );

            all_job_count = request_total_jobs;
            all_jobs_returned_count += jobs_summary_val_ptr->get().size();

            // If just want first batch, done.
            if ( ! request_all )  break;

            if ( request_total_jobs == 0 ) {
                // wasn't a partial response, got everything, so can go on to printing results.
                break;
            }

            // if got all jobs, then can go on to printing results.
            if ( content_range_end_out + 1 >= request_total_jobs ) {
                break;
            }

            // Otherwise update range_start to request the next batch.
            range_start = content_range_end_out + 1;
        }

        // in case some inconsistency, set all jobs to jobs returned.
        if ( range_start != 0 ) {
            all_job_count = all_jobs_returned_count;
        }

        printJobsSummary(
                value_ptrs,
                all_job_count,
                all_jobs_returned_count,
                summary_format
            );

    } else {

        json::ObjectValuePtr job_details_val_ptr(bgws_client.getJobDetails( jobid ));

        printJobDetails( job_details_val_ptr );

    }
}


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {

        bool help(false);

        BgwsClient bgws_client;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );

        JobsSummaryFilter jobs_summary_filter;
        BGQDB::job::Id jobid(-1);
        bool use_long_format(false);
        bool request_all(false);


        po::options_description visible_desc( "Options" );

        visible_desc.add_options()
                ( "long,l", po::bool_switch( &use_long_format ), "Print summary using long format" )
                ( "all,a", po::bool_switch( &request_all ), "Fetch all jobs rather than limit" )
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;

        po::options_description filter_options_desc( "Job Summary Filter and Sort Options" );

        jobs_summary_filter.addOptionsTo( filter_options_desc );

        visible_desc.add( filter_options_desc );

        logging_program_options.addTo( visible_desc );

        visible_desc.add( bgws_client.getDesc() );

        po::options_description all_opts( "all" );

        all_opts.add_options()
                ( "id", po::value( &jobid ), "job id" )
            ;

        all_opts.add( visible_desc );

        po::positional_options_description p;
        p.add( "id", 1 );

        po::variables_map vm;

        try {

            po::store( po::command_line_parser( argc, argv ).options( all_opts ).positional( p ).run(), vm );
            po::notify( vm );

            if ( help ) {

                cout <<

"Usage: " << argv[0] << " [OPTIONS] [SUMMARY_OPTIONS | JOBID]\n"
"\n"
"Prints a summary of all jobs or details for a job.\n"
"\n"
"Requires Read authority on the job.\n"
"\n"
<< visible_desc << std::endl;

                return EXIT_SUCCESS;

            }

        } catch ( std::exception& e ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "invalid arguments, " + e.what() + ". Use -h for help." ) );
        }


        bgws_client.notifyOptionsSet();

        bgq::utility::initializeLogging(
                *bgws_client.getBgProperties(),
                logging_program_options
            );

        bgws_client.notifyLoggingIsInitialized();


        listJobs(
                jobid,
                jobs_summary_filter,
                use_long_format ? SummaryFormat::LONG : SummaryFormat::SHORT,
                request_all,
                bgws_client
            );

    } catch( std::exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
