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

#ifndef BGWS_BLUE_GENE_SERVICE_ACTIONS_SERVICE_ACTIONS_HPP_
#define BGWS_BLUE_GENE_SERVICE_ACTIONS_SERVICE_ACTIONS_HPP_


#include "fwd.hpp"

#include "../../utility/ChildProcesses.hpp"

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>

#include <string>


namespace bgws {
namespace blue_gene {
namespace service_actions {


class ServiceActions
{
public:


    ServiceActions(
            boost::asio::io_service& io_service,
            utility::ChildProcesses& child_processes,
            const boost::filesystem::path& executable_path,
            const std::string& properties_filename
        );

    ServiceActions( const ServiceActions& ) = delete;
    ServiceActions& operator =( const ServiceActions& ) = delete;

    void start(
            const std::string& location,
            const std::string& username,
            StartCb cb
        );

    void end(
            const std::string& location,
            const std::string& username,
            EndCb cb
        );

    void close(
            const std::string& location,
            const std::string& username,
            CloseCb cb
        );


    typedef boost::function<void ( AttentionMessagesMap )> GetAttentionMessagesSnapshotCb;

    void getAttentionMessagesSnapshot(
            GetAttentionMessagesSnapshotCb cb
        );


    typedef boost::function<void ( const std::string& )> GetAttentionMessagesCb;

    void getAttentionMessages(
            const std::string& service_action_id,
            GetAttentionMessagesCb cb
        );


    void setConfiguration(
            const boost::filesystem::path& executable_path,
            const std::string& properties_filename
        );


private:

    utility::ChildProcesses &_child_processes;

    boost::asio::strand _strand;

    boost::filesystem::path _executable_path;
    std::string _properties_filename;

    AttentionMessagesMap _attention_messages;


    void _startImpl(
            const std::string& location,
            const std::string& username,
            StartCb cb
        );

    void _endImpl(
            const std::string& location,
            const std::string& username,
            EndCb cb
        );

    void _closeImpl(
            const std::string& location,
            const std::string& username,
            CloseCb cb
        );

    void _setConfigurationImpl(
            const boost::filesystem::path& executable_path,
            const std::string& properties_filename
        );


    void _addAttentionMessages(
            const std::string& service_action_id,
            const std::string& attention_messages
        );

    void _getAttentionMessagesSnapshotImpl(
            GetAttentionMessagesSnapshotCb cb
        );

    void _getAttentionMessagesImpl(
            const std::string& service_action_id,
            GetAttentionMessagesCb cb
        );

};

} } } // namespace bgws::blue_gene::service_actions

#endif
