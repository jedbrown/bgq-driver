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

#include "Runs.hpp"

#include "Run.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>


using boost::bind;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace blue_gene {
namespace diagnostics {


Runs::Runs(
        utility::ChildProcesses& child_processes,
        boost::asio::io_service& io_service,
        const boost::filesystem::path& executable
    ) :
        _child_processes(child_processes),
        _io_service(io_service),
        _executable(executable),
        _strand(_io_service)
{
    // Nothing to do.
}


void Runs::setExecutable( const boost::filesystem::path& executable )
{
    _strand.post( bind(
            &Runs::_setExecutableImpl, this, executable
        ) );
}


void Runs::startNewRun(
        const RunOptions& run_options,
        RunIdCallback run_id_cb
    )
{
    _strand.post( bind(
            &Runs::_startNewRunImpl, this, run_options, run_id_cb
        ) );
}


void Runs::getSnapshot(
        SnapshotCallback snapshot_cb
    )
{
    _strand.post( bind(
            &Runs::_getSnapshotImpl, this, snapshot_cb
        ) );
}


void Runs::getRunSnapshot(
        RunId run_id,
        RunSnapshotCallback cb
    )
{
    _strand.post( bind(
            &Runs::_getRunSnapshotImpl, this, run_id, cb
        ) );
}


void Runs::cancelRun(
        RunId run_id,
        CancelResultCallback cb
    )
{
    _strand.post( bind(
            &Runs::_cancelRunImpl, this, run_id, cb
        ) );
}


void Runs::notifyRunComplete(
        boost::shared_ptr<Run> run_ptr
    )
{
    _strand.post( bind(
            &Runs::_notifyRunCompleteImpl, this, run_ptr
        ) );
}


void Runs::_setExecutableImpl(
        const boost::filesystem::path& executable
    )
{
    _executable = executable;
}


void Runs::_startNewRunImpl(
        const RunOptions& run_options,
        RunIdCallback run_id_cb
    )
{
    LOG_INFO_MSG( "Starting new diagnostics run." );

    _RunPtr run_ptr( new Run(
            run_options,
            _executable,
            *this,
            _child_processes
        ) );

    _run_ptrs.insert( std::make_pair( run_ptr, _RunInfo() ) );

    run_ptr->setRunIdCallback(
            _strand.wrap( boost::bind(
                    &Runs::_gotRunId,
                    this,
                    run_ptr,
                    run_id_cb,
                    _1,
                    _2
                ) )
        );

    run_ptr->start();
}


void Runs::_gotRunId(
        _RunPtr run_ptr,
        RunIdCallback orig_run_id_cb,
        RunId run_id,
        const std::string &error_msg
    )
{
    _RunPtrs::iterator run_ptr_i(_run_ptrs.find( run_ptr ));

    if ( run_ptr_i != _run_ptrs.end() ) {
        run_ptr_i->second.run_id = run_id;
    }

    orig_run_id_cb( run_id, error_msg );
}


void Runs::_getSnapshotImpl(
        SnapshotCallback snapshot_cb
    )
{
    SnapshotPtr snapshot_ptr( new Snapshot() );

    for ( _RunPtrs::const_iterator i(_run_ptrs.begin()) ; i != _run_ptrs.end() ; ++i ) {
        if ( i->second.run_id == RunId(-1) )  continue; // No run ID assigned yet.

        snapshot_ptr->insert( Snapshot::value_type(
                i->second.run_id,
                RunInfo( i->first->getUserName(), i->second.canceled )
            ) );
    }

    snapshot_cb( snapshot_ptr );
}


void Runs::_getRunSnapshotImpl(
        RunId run_id,
        RunSnapshotCallback cb
    )
{

    for ( _RunPtrs::const_iterator i(_run_ptrs.begin()) ; i != _run_ptrs.end() ; ++i ) {
        if ( i->second.run_id != run_id )  continue; // This isn't the one.

        // found it.

        cb( RunInfoPtr( new RunInfo( i->first->getUserName(), i->second.canceled ) ) );
        return;
    }

    // Didn't find the run ID.
    cb( RunInfoPtr() );
}


void Runs::_cancelRunImpl(
        RunId run_id,
        CancelResultCallback cb
    )
{
    _RunPtrs::iterator run_ptrs_i;

    for ( run_ptrs_i = _run_ptrs.begin() ; run_ptrs_i != _run_ptrs.end() ; ++run_ptrs_i ) {
        if ( run_ptrs_i->second.run_id == run_id ) {
            break;
        }
    }

    if ( run_ptrs_i == _run_ptrs.end() ) {
        // Didn't find it.
        cb( CancelResult::NotFound );
        return;
    }

    if ( run_ptrs_i->second.canceled ) {
        // Already did cancel on this run.

        LOG_WARN_MSG( "Requested cancel run that's already canceled. Run ID=" << run_id );

        cb( CancelResult::Success );
        return;
    }

    run_ptrs_i->first->cancel();

    run_ptrs_i->second.canceled = true;

    cb( CancelResult::Success );
}


void Runs::_notifyRunCompleteImpl(
        boost::shared_ptr<Run> run_ptr
    )
{
    _run_ptrs.erase( run_ptr );
}


} } } // namespace bgws::blue_gene::diagnostics
