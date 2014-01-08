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

#ifndef BGWS_BLUE_GENE_SERVICE_ACTIONS_PREPARE_SERVICE_ACTION_HPP_
#define BGWS_BLUE_GENE_SERVICE_ACTIONS_PREPARE_SERVICE_ACTION_HPP_


#include "fwd.hpp"

#include "../../utility/ChildProcesses.hpp"
#include "../../utility/EasyChildProcess.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include <stdexcept>
#include <string>


namespace bgws {
namespace blue_gene {
namespace service_actions {


class PrepareServiceAction : public boost::enable_shared_from_this<PrepareServiceAction>
{
public:

    typedef boost::function<void ( std::string service_action_id, std::string attention_messages )> NotifyAttentionMessagesCb;

    /*!
     *  \throws InvalidLocationError If the location is not a valid location for a service action.
     *  \throws HardwareDoesntExistError If the location isn't valid because the hardware doesn't exist.
     */
    PrepareServiceAction(
            const std::string& location,
            const std::string& username,
            const boost::filesystem::path& service_action_executable_path,
            const std::string& properties_filename,
            utility::ChildProcesses& child_processes
        );


    void start(
            StartCb start_cb,
            NotifyAttentionMessagesCb notify_attention_messages_cb
        );


private:

    std::string _location;

    std::string _username;

    StartCb _start_cb;
    NotifyAttentionMessagesCb _notify_attention_messages_cb;

    utility::EasyChildProcess::Ptr _process_ptr;

    std::string _id;

    std::string _error_text;

    std::string _attention_text;


    void _handleProcessEnded(
            const bgq::utility::ExitStatus& exit_status
        );


    void _handleLine(
            utility::EasyChildProcess::OutputType output_type,
            utility::EasyChildProcess::OutputIndicator output_ind,
            const std::string& line
        );


    void _checkLineStartup(
            const std::string& line
        );

    void _checkLineAttention(
            const std::string& line
        );

};

} } } // namespace bgws::blue_gene::service_actions

#endif
