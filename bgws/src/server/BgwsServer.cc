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

#include "BgwsServer.hpp"

#include "AbstractResponder.hpp"
#include "BlockingOperationsThreadPool.hpp"
#include "CheckUserAdminExecutor.hpp"
#include "PwauthExecutor.hpp"
#include "ResponderFactory.hpp"
#include "Sessions.hpp"

#include "blue_gene/diagnostics/Runs.hpp"

#include "blue_gene/service_actions/ServiceActions.hpp"

#include "teal/Teal.hpp"

#include <utility/include/Log.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <string>

#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>


using std::string;

LOG_DECLARE_FILE( "bgws" );


namespace bgws {


BgwsServer::RefreshConfigurationError::RefreshConfigurationError( const std::string& what_str )
    : std::runtime_error( what_str )
{
    // Nothing to do.
}


BgwsServer::BgwsServer(
        const bgq::utility::Properties::Ptr& bg_properties_ptr,
        const bgq::utility::ServerPortConfiguration& port_configuration,
        boost::asio::io_service& io_service,
        bgq::utility::SignalHandler<SIGCHLD>& sig_handler
    ) :
        _io_service(io_service),
        _sig_handler(sig_handler),
        _start_time(boost::posix_time::microsec_clock::local_time()),
        _child_processes( _io_service ),
        _server_stats( _io_service )
{
    _dynamic_configuration_ptr = DynamicConfiguration::create( bg_properties_ptr );

    _check_user_admin_executor_ptr.reset( new CheckUserAdminExecutor(
            _child_processes,
            _io_service,
            _dynamic_configuration_ptr
        ) );

    _diagnostics_runs_ptr.reset( new blue_gene::diagnostics::Runs(
            _child_processes,
            _io_service,
            _dynamic_configuration_ptr->getDiagnosticsExecutable()
        ) );

    _pwauth_executor_ptr.reset( new PwauthExecutor(
            _child_processes,
            _dynamic_configuration_ptr
        ) );

    _service_actions_ptr.reset( new blue_gene::service_actions::ServiceActions(
            _io_service,
            _child_processes,
            _dynamic_configuration_ptr->getServiceActionExecutable(),
            _dynamic_configuration_ptr->getPropertiesFilename()
        ) );

    _sessions_ptr.reset( new Sessions(
            _io_service,
            _dynamic_configuration_ptr
        ) );

    _teal_ptr.reset( new teal::Teal(
            _io_service,
            _child_processes,
            _dynamic_configuration_ptr->getTealCloseAlertExecutable(),
            _dynamic_configuration_ptr->getTealRemoveAlertExecutable()
        ) );

    _blocking_operations_thread_pool_ptr.reset( new BlockingOperationsThreadPool() );


    bool calculate_blocking_operations_thread_pool_size(true);

    string blocking_operations_thread_pool_size_str;
    unsigned blocking_operations_thread_pool_size;

    try {

        blocking_operations_thread_pool_size_str = bg_properties_ptr->getValue( "bgws", "blocking_operations_thread_pool_size" );
        LOG_INFO_MSG( "Blocking operations thread pool size in configuration file is '" << blocking_operations_thread_pool_size_str << "'" );

        if ( blocking_operations_thread_pool_size_str == "auto" ) {
            calculate_blocking_operations_thread_pool_size = true;
        } else {
            blocking_operations_thread_pool_size = boost::lexical_cast<unsigned>( blocking_operations_thread_pool_size_str );

            if ( blocking_operations_thread_pool_size == 0 ) {
                calculate_blocking_operations_thread_pool_size = true;
            } else {
                calculate_blocking_operations_thread_pool_size = false;
            }
        }

    } catch ( boost::bad_lexical_cast& e ) {
        LOG_INFO_MSG( "Faled to get blocking_operations_thread_pool_size configuration from Blue Gene configuration file, the value is not valid. The value must be 'auto' or an integer greater than 0. Will use the default." );
        calculate_blocking_operations_thread_pool_size = true;
    } catch ( std::exception& e ) {
        LOG_INFO_MSG( "Faled to get blocking_operations_thread_pool_size configuration from Blue Gene configuration file, will use default. The error is " << e.what() );
        calculate_blocking_operations_thread_pool_size = true;
    }

    if ( calculate_blocking_operations_thread_pool_size ) {
        blocking_operations_thread_pool_size = (boost::thread::hardware_concurrency() / 2) + 1;
        LOG_INFO_MSG( "Calculated size for blocking operations thread pool is " << blocking_operations_thread_pool_size );
    }

    _blocking_operations_thread_pool_ptr->start( blocking_operations_thread_pool_size );

    LOG_DEBUG_MSG( "Creating BlueGene..." );

    _blue_gene_ptr.reset( new BlueGene() );

    LOG_DEBUG_MSG( "Finished creating BlueGene." );

    _responder_factory_ptr.reset( new ResponderFactory(
            *this,
            *_blue_gene_ptr,
            *_check_user_admin_executor_ptr,
            *_diagnostics_runs_ptr,
            _dynamic_configuration_ptr,
            *_pwauth_executor_ptr,
            _server_stats,
            *_service_actions_ptr,
            *_sessions_ptr,
            *_teal_ptr,
            *_blocking_operations_thread_pool_ptr
        ) );

    _server_ptr = capena::server::Server::create(
            _io_service,
            port_configuration,
            boost::bind( &ResponderFactory::createResponder, _responder_factory_ptr, _1, _2 )
        );
}


void BgwsServer::start()
{
    _startWaitForSignal();

    _server_ptr->start();
}


void BgwsServer::refreshConfiguration( const common::RefreshBgwsServerConfiguration& config_data )
{
    try {

        DynamicConfiguration::ConstPtr dynamic_configuration_ptr(_dynamic_configuration_ptr); // keep ref to the old configuration for this fn.

        string properties_filename;

        if ( config_data.getType() == common::RefreshBgwsServerConfiguration::Type::RereadCurrent ) {
            properties_filename = dynamic_configuration_ptr->getPropertiesFilename();
            LOG_INFO_MSG( "Re-reading same configuration file '" << properties_filename << "'" );
        } else if ( config_data.getType() == common::RefreshBgwsServerConfiguration::Type::ReadDefault ) {
            properties_filename = string();
            LOG_INFO_MSG( "Reading default configuration file." );
        } else {
            properties_filename = config_data.getFilename().string();
            LOG_INFO_MSG( "Reading new configuration file '" << properties_filename << "'" );
        }

        bgq::utility::Properties::ConstPtr new_properties_ptr( bgq::utility::Properties::create( properties_filename ) ); // Read properties file, may fail with some Properties error.

        DynamicConfiguration::ConstPtr new_dynamic_configuration_ptr(DynamicConfiguration::create( new_properties_ptr ) ); // Create a new dynamic configuration.


        // Set the new configuration on the different consumers.

        _dynamic_configuration_ptr = new_dynamic_configuration_ptr;

        _check_user_admin_executor_ptr->setNewDynamicConfiguration( _dynamic_configuration_ptr );
        _diagnostics_runs_ptr->setExecutable( _dynamic_configuration_ptr->getDiagnosticsExecutable() );
        _pwauth_executor_ptr->setNewDynamicConfiguration( _dynamic_configuration_ptr );

        _service_actions_ptr->setConfiguration(
                _dynamic_configuration_ptr->getServiceActionExecutable(),
                _dynamic_configuration_ptr->getPropertiesFilename()
            );

        _sessions_ptr->setNewDynamicConfiguration( _dynamic_configuration_ptr );
        _responder_factory_ptr->setNewDynamicConfiguration( _dynamic_configuration_ptr );

        _teal_ptr->setConfiguration(
                _dynamic_configuration_ptr->getTealCloseAlertExecutable(),
                _dynamic_configuration_ptr->getTealRemoveAlertExecutable()
            );

    } catch ( std::exception& e ) {

        BOOST_THROW_EXCEPTION( RefreshConfigurationError( e.what() ) );

    }

}


void BgwsServer::_handleSignal(
        const boost::system::error_code& ec,
        const siginfo_t& sig_info
    )
{
    if ( ec ) {
        LOG_WARN_MSG( "Signal handler got error " << ec );

        // Start up the loop again.
        _startWaitForSignal();
        return;
    }

    if ( sig_info.si_signo == SIGCHLD ) {
        _child_processes.notifySigChld();
    } else {
        LOG_WARN_MSG( "Wasn't expecting signal " << sig_info.si_signo );
    }

    // Start up the loop again.
    _startWaitForSignal();
}


void BgwsServer::_startWaitForSignal()
{
    _sig_handler.async_wait(
            boost::bind( &BgwsServer::_handleSignal, this, _1, _2 )
        );
}


} // namespace bgws
