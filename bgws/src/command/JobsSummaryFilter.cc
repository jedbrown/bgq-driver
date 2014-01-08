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

#include "JobsSummaryFilter.hpp"

#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <set>
#include <stdexcept>


using namespace boost::assign;

using boost::bind;
using boost::lexical_cast;

using capena::http::uri::Query;

using std::runtime_error;
using std::string;
using std::vector;


namespace bgws {
namespace command {


JobsSummaryFilter::JobsSummaryFilter()
    : _statuses( "DLNPRS" ),
      _exit_status_set(false)
{
    // Nothing to do.
}


void JobsSummaryFilter::addOptionsTo(
        boost::program_options::options_description& desc
    )
{
    namespace po = boost::program_options;

    desc.add_options()
            ( "status", po::value<vector<string > >()->notifier( bind( &JobsSummaryFilter::_notifyStatusValues, this, _1 ) ), "Job status codes"  )
            ( "block", po::value( &_block_id ), "Block ID" )
            ( "executable", po::value( &_executable), "Executable" )
            ( "user", po::value( &_user ), "User name" )
            ( "start-time", po::value( &_start_time ), "Start time interval" )
            ( "end-time", po::value( &_end_time ), "End time interval" )
            ( "exit-status", po::value<int>()->notifier( bind( &JobsSummaryFilter::_notifyExitStatus, this, _1 ) ), "Job exit status" )
            ( "client", po::value( &_client ), "runjob hostname:pid" )
            ( "sort", po::value( &_sort ), "Sort field and direction" )
        ;

}


capena::http::uri::Query JobsSummaryFilter::calcQuery() const
{
    Query::Parameters params;

    if ( ! _block_id.empty() )  params += Query::Parameter( "block", _block_id );
    if ( ! _end_time.empty() )  params += Query::Parameter( "endTime", _end_time );
    if ( ! _executable.empty() )  params += Query::Parameter( "executable", string() + "*" + _executable );
    if ( _exit_status_set )  params += Query::Parameter( "exitStatus", lexical_cast<string>( _exit_status ) );
    if ( ! _start_time.empty() )  params += Query::Parameter( "startTime", _start_time );
    if ( ! _statuses.empty() )  params += Query::Parameter( "status", _statuses );
    if ( ! _user.empty() )  params += Query::Parameter( "user", _user );
    if ( ! _sort.empty() )  params += Query::Parameter( "sort", _sort );
    if ( ! _client.empty() )  params += Query::Parameter( "client", _client );

    return Query( params );
}


void JobsSummaryFilter::_notifyStatusValues( const std::vector<std::string>& status_args )
{
    std::set<std::string> status_vals;

    BOOST_FOREACH( string status, status_args ) {

        boost::to_upper( status );

        if ( status == "D" || status == "DEBUG" )  { status = "D"; }
        else if ( status == "E" || status == "ERROR" )  { status = "E"; }
        else if ( status == "L" || status == "LOADING" )  { status = "L"; }
        else if ( status == "N" || status == "CLEANUP" )  { status = "N"; }
        else if ( status == "P" || status == "SETUP" )  { status = "P"; }
        else if ( status == "R" || status == "RUNNING" )  { status = "R"; }
        else if ( status == "S" || status == "STARTING" )  { status = "S"; }
        else if ( status == "T" || status == "TERMINATED" )  { status = "T"; }
        else if ( status == "ALL" )  { status_vals.clear(); break; }
        else {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "invalid status value " + status ) );
        }

        status_vals.insert( status );
    }

    _statuses.clear();
    BOOST_FOREACH( const string& status, status_vals ) {
        _statuses += status;
    }
}


void JobsSummaryFilter::_notifyExitStatus( int exit_status )
{
    _exit_status_set = true;
    _exit_status = exit_status;
}


} } // namespace bgws::command
