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

#ifndef BGWS_BLUE_GENE_DIAGNOSTICS_RUNS_HPP_
#define BGWS_BLUE_GENE_DIAGNOSTICS_RUNS_HPP_


#include "RunOptions.hpp"
#include "types.hpp"

#include "../../utility/ChildProcesses.hpp"

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <map>
#include <set>
#include <string>

#include <stdint.h>


namespace bgws {
namespace blue_gene {
namespace diagnostics {


class Runs : boost::noncopyable
{
public:

    struct RunInfo {
        std::string user;
        bool canceled;

        explicit RunInfo( const std::string& user = std::string(), bool canceled = false ) : user(user), canceled(canceled) {}
    };

    typedef std::map<RunId,RunInfo> Snapshot;

    typedef boost::shared_ptr<Snapshot> SnapshotPtr;

    typedef boost::function<void ( SnapshotPtr snapshot_ptr )> SnapshotCallback;

    typedef boost::shared_ptr<RunInfo> RunInfoPtr;

    typedef boost::function<void ( RunInfoPtr run_info_ptr )> RunSnapshotCallback;

    struct CancelResult {
        enum Value {
            Success,
            NotFound
        };
    };

    typedef boost::function<void ( CancelResult::Value )> CancelResultCallback;


    Runs(
            utility::ChildProcesses& child_processes,
            boost::asio::io_service& io_service,
            const boost::filesystem::path& executable
        );

    void setExecutable( const boost::filesystem::path& executable );

    void startNewRun(
            const RunOptions& run_options,
            RunIdCallback run_id_cb
        );

    void getSnapshot(
            SnapshotCallback snapshot_cb
        );

    void getRunSnapshot(
            RunId run_id,
            RunSnapshotCallback cb
        );

    void cancelRun(
            RunId run_id,
            CancelResultCallback cb
        );

    void notifyRunComplete(
            boost::shared_ptr<Run> run_ptr
        );


private:

    struct _RunInfo
    {
        RunId run_id;
        bool canceled;

        _RunInfo() : run_id(-1), canceled(false) {}
    };


    typedef boost::shared_ptr<Run> _RunPtr;

    typedef std::map<_RunPtr, _RunInfo> _RunPtrs;


    utility::ChildProcesses &_child_processes;
    boost::asio::io_service &_io_service;
    boost::filesystem::path _executable;

    boost::asio::strand _strand;

    _RunPtrs _run_ptrs;


    void _setExecutableImpl(
            const boost::filesystem::path& executable
        );

    void _startNewRunImpl(
            const RunOptions& run_options,
            RunIdCallback run_id_cb
        );

    void _gotRunId(
            _RunPtr run_ptr,
            RunIdCallback orig_run_id_cb,
            RunId run_id,
            const std::string &error_msg
        );

    void _getSnapshotImpl(
            SnapshotCallback snapshot_cb
        );

    void _getRunSnapshotImpl(
            RunId run_id,
            RunSnapshotCallback cb
        );

    void _cancelRunImpl(
            RunId run_id,
            CancelResultCallback cb
        );

    void _notifyRunCompleteImpl(
            boost::shared_ptr<Run> run_ptr
        );

};

} } } // namespace bgws::blue_gene::diagnostics

#endif
