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

#ifndef BGWS_BLUE_GENE_DIAGNOSTICS_RUN_HPP_
#define BGWS_BLUE_GENE_DIAGNOSTICS_RUN_HPP_


#include "RunOptions.hpp"
#include "types.hpp"

#include "../../utility/ChildProcesses.hpp"
#include "../../utility/EasyChildProcess.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <string>


namespace bgws {
namespace blue_gene {
namespace diagnostics {


class Run : public boost::enable_shared_from_this<Run>
{
public:

    Run(
            const RunOptions& run_options,
            const boost::filesystem::path& executable,
            Runs& runs,
            utility::ChildProcesses& child_processes
        );

    void setRunIdCallback(
            RunIdCallback cb
        );

    const std::string& getUserName() const  { return _run_options.getUserName(); }

    void start();

    void cancel();

    ~Run();


private:

    RunOptions _run_options;
    boost::filesystem::path _executable;
    Runs &_runs;

    utility::EasyChildProcess::Ptr _process_ptr;

    RunIdCallback _run_id_cb;

    RunId _run_id;


    void _handleProcessEnded(
            const bgq::utility::ExitStatus& exit_status
        );


    void _handleLine(
            utility::EasyChildProcess::OutputType output_type,
            utility::EasyChildProcess::OutputIndicator output_ind,
            const std::string& line
        );
};

} } } // namespace bgws::blue_gene::diagnostics

#endif
