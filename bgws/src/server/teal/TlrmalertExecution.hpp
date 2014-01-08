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

#ifndef BGWS_TEAL_TLRMALERT_EXECUTION_HPP_
#define BGWS_TEAL_TLRMALERT_EXECUTION_HPP_


#include "fwd.hpp"

#include "../utility/EasyChildProcess.hpp"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <string>


namespace bgws {
namespace teal {


class TlrmalertExecution : public boost::enable_shared_from_this<TlrmalertExecution>
{
public:

    typedef boost::shared_ptr<TlrmalertExecution> Ptr;


    static Ptr create(
            Id id,
            CloseRemoveCallbackFn cb_fn,
            const boost::filesystem::path& executable,
            utility::ChildProcesses& child_processes,
            boost::asio::io_service& io_service
        );


    void start();


    // Use create().
    TlrmalertExecution(
            Id id,
            CloseRemoveCallbackFn cb_fn,
            const boost::filesystem::path& executable,
            utility::ChildProcesses& child_processes,
            boost::asio::io_service& io_service
        );


private:

    enum class _Result {
        Ok,
        OtherError,
        NotFound,
        InvalidState
    };


    Id _id;
    CloseRemoveCallbackFn _cb_fn;

    boost::asio::strand _strand;
    utility::EasyChildProcess::Ptr _process_ptr;

    bool _exited;
    bgq::utility::ExitStatus _exit_status;

    bool _stdout_complete;
    bool _stderr_complete;
    std::string _output;

    _Result _result;


    void _handleProcessEnded(
            const bgq::utility::ExitStatus& exit_status
        );

    void _handleLine(
            utility::EasyChildProcess::OutputType output_type,
            utility::EasyChildProcess::OutputIndicator output_ind,
            const std::string& line
        );

    void _checkComplete();
};

} } // namespace bgws::teal

#endif
