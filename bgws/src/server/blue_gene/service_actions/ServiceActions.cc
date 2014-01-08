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

#include "ServiceActions.hpp"

#include "CloseServiceAction.hpp"
#include "EndServiceAction.hpp"
#include "PrepareServiceAction.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace blue_gene {
namespace service_actions {


ServiceActions::ServiceActions(
        boost::asio::io_service& io_service,
        utility::ChildProcesses& child_processes,
        const boost::filesystem::path& executable_path,
        const std::string& properties_filename
    ) :
        _child_processes(child_processes),
        _strand(io_service),
        _executable_path(executable_path),
        _properties_filename(properties_filename)
{
    // Nothing to do
}


void ServiceActions::start(
        const std::string& location,
        const std::string& username,
        StartCb cb
    )
{
    _strand.post( boost::bind( &ServiceActions::_startImpl, this, location, username, cb ) );
}


void ServiceActions::end(
        const std::string& location,
        const std::string& username,
        EndCb cb
    )
{
    _strand.post( boost::bind( &ServiceActions::_endImpl, this, location, username, cb ) );
}


void ServiceActions::close(
        const std::string& location,
        const std::string& username,
        CloseCb cb
    )
{
    _strand.post( boost::bind( &ServiceActions::_closeImpl, this, location, username, cb ) );
}



void ServiceActions::setConfiguration(
        const boost::filesystem::path& executable_path,
        const std::string& properties_filename
    )
{
    _strand.post( boost::bind( &ServiceActions::_setConfigurationImpl, this, executable_path, properties_filename ) );
}


void ServiceActions::_startImpl(
        const std::string& location,
        const std::string& username,
        StartCb cb
    )
{
    try {

        boost::shared_ptr<PrepareServiceAction> service_action_ptr( new PrepareServiceAction(
                location,
                username,
                _executable_path,
                _properties_filename,
                _child_processes
            ) );

        service_action_ptr->start(
                cb
            );

    } catch ( std::exception& e ) {

        LOG_ERROR_MSG( "Failed to start service action, an exception occurred. The message is '" << e.what() << "'" );

        cb( std::current_exception(), "" /*id (none)*/ );

    }
}


void ServiceActions::_endImpl(
        const std::string& location,
        const std::string& username,
        EndCb cb
    )
{
    try {

        boost::shared_ptr<EndServiceAction> end_service_action_ptr( new EndServiceAction(
                location,
                username,
                _executable_path,
                _properties_filename,
                _child_processes
            ) );

        end_service_action_ptr->start(
                cb
            );

    } catch ( std::exception& e ) {

        cb( string() + "Error starting end service action, " + e.what() );

    }
}


void ServiceActions::_closeImpl(
        const std::string& location,
        const std::string& username,
        CloseCb cb
    )
{
    try {

        boost::shared_ptr<CloseServiceAction> close_service_action_ptr( new CloseServiceAction(
                location,
                username,
                _executable_path,
                _properties_filename,
                _child_processes
            ) );

        close_service_action_ptr->start(
                cb
            );

    } catch ( std::exception& e ) {

        cb( std::current_exception() );

    }
}


void ServiceActions::_setConfigurationImpl(
        const boost::filesystem::path& executable_path,
        const std::string& properties_filename
    )
{
    _executable_path = executable_path;
    _properties_filename = properties_filename;
}


} } } // namespace bgws::blue_gene::service_actions
