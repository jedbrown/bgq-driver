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

#include "Run.hpp"

#include "Runs.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/xpressive/xpressive.hpp>


using namespace boost::xpressive;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace blue_gene {
namespace diagnostics {


Run::Run(
        const RunOptions& run_options,
        const boost::filesystem::path& executable,
        Runs& runs,
        utility::ChildProcesses& child_processes
    ) :
        _run_options(run_options),
        _executable(executable),
        _runs(runs),
        _process_ptr(child_processes.createEasy( "diags", executable, _run_options.getArgs() )),
        _run_id( -1 )
{
    // Nothing to do.
}


void Run::setRunIdCallback(
        RunIdCallback cb
    )
{
    _run_id_cb = cb;
}


void Run::start()
{
    _process_ptr->start(
            boost::bind( &Run::_handleProcessEnded, shared_from_this(), _1 ),
            boost::bind( &Run::_handleLine, shared_from_this(), _1, _2, _3 )
        );
}


void Run::cancel()
{
    LOG_DEBUG_MSG( "Sending SIGINT to " << _process_ptr << " to cancel diagnostics run " << _run_id );
    _process_ptr->kill( SIGINT );
}


Run::~Run()
{
    // If haven't called the run ID callback, call it with no run ID.
    if ( _run_id_cb )  _run_id_cb( RunId(-1), "No run ID found in diagnostics output." );
}


void Run::_handleProcessEnded(
        const bgq::utility::ExitStatus& /*exit_status*/
    )
{
    LOG_INFO_MSG( _process_ptr << " Diagnostics run " << _run_id << " completed." );

    _runs.notifyRunComplete( shared_from_this() );
}


void Run::_handleLine(
        utility::EasyChildProcess::OutputType output_type,
        utility::EasyChildProcess::OutputIndicator output_ind,
        const std::string& line
    )
{
    // Only care about stdout.
    if ( output_type != utility::EasyChildProcess::OutputType::Out ) {
        return;
    }

    // Only care about normal output.
    if ( output_ind != utility::EasyChildProcess::OutputIndicator::Normal ) {

        // End of output or error, either way not going to get a run ID if haven't gotten one yet.
        if ( _run_id_cb ) {
            _run_id_cb( RunId(-1), "End of output for diagnostics run with no run ID." );
            _run_id_cb = RunIdCallback();
        }

        return;
    }

    // Already got the run ID, so ignore this.
    if ( ! _run_id_cb )  return;

    static const sregex RUN_ID_REGEX = as_xpr( "Run" ) >> +_s >> "ID:" >> *_s >> (s1=+~_s);
    static const sregex FAIL_START_REGEX = as_xpr( "Unable to start diagnostics:" ) >> +_s >> (s1=*_);

    smatch what;

    if ( regex_search( line, what, RUN_ID_REGEX ) ) {
        // Found the run ID.

        string run_id_str(what[1]);

        LOG_INFO_MSG( _process_ptr << " run ID is '" << run_id_str << "'" );
        _run_id = boost::lexical_cast<RunId>( run_id_str );

        _run_id_cb( _run_id, "" );
        _run_id_cb = RunIdCallback();

        return;
    }

    if ( regex_search( line, what, FAIL_START_REGEX ) ) {

        string err_msg(what[1]);

        LOG_WARN_MSG( _process_ptr << " Error starting diagnostics, " << err_msg );

        _run_id_cb( RunId(-1), err_msg );
        _run_id_cb = RunIdCallback();

        return;
    }
}


} } } // namespace bgws::blue_gene::diagnostics
