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

#ifndef BGWS_BLUE_GENE_SERVICE_ACTIONS_END_SERVICE_ACTION_HPP_
#define BGWS_BLUE_GENE_SERVICE_ACTIONS_END_SERVICE_ACTION_HPP_


#include "fwd.hpp"

#include "../../utility/ChildProcesses.hpp"
#include "../../utility/EasyChildProcess.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>

#include <string>


namespace bgws {
namespace blue_gene {
namespace service_actions {


class EndServiceAction : public boost::enable_shared_from_this<EndServiceAction>
{
public:

    EndServiceAction(
            const std::string& location,
            const std::string& username,
            const boost::filesystem::path& service_action_executable_path,
            const std::string& properties_filename,
            utility::ChildProcesses& child_processes
        );

    void start(
            EndCb cb
        );


private:

    std::string _location;

    std::string _username;

    EndCb _cb;

    utility::EasyChildProcess::Ptr _process_ptr;

    std::string _error_text;


    void _handleProcessEnded(
            const bgq::utility::ExitStatus& exit_status
        );


    void _handleLine(
            utility::EasyChildProcess::OutputType output_type,
            utility::EasyChildProcess::OutputIndicator output_ind,
            const std::string& line
        );

};

} } } // namespace bgws::blue_gene::service_actions


#endif
