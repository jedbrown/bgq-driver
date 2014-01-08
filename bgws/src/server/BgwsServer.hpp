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

#ifndef BGWS_BGWS_SERVER_HPP_
#define BGWS_BGWS_SERVER_HPP_


#include "BlueGene.hpp"
#include "DynamicConfiguration.hpp"
#include "ServerStats.hpp"
#include "types.hpp"

#include "blue_gene/diagnostics/types.hpp"

#include "blue_gene/service_actions/fwd.hpp"

#include "teal/fwd.hpp"

#include "utility/ChildProcesses.hpp"

#include "common/RefreshBgwsServerConfiguration.hpp"

#include "capena-http/server/Server.hpp"

#include <utility/include/Properties.h>
#include <utility/include/SignalHandler.h>

#include <utility/include/portConfiguration/ServerPortConfiguration.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <stdexcept>
#include <string>

#include <signal.h>

#include <sys/types.h>


namespace bgws {


/*! \brief A BGWS Server. */
class BgwsServer : boost::noncopyable
{
public:


    class RefreshConfigurationError : public std::runtime_error {
        public:
        RefreshConfigurationError( const std::string& what_str );
    };


    typedef boost::optional<boost::filesystem::path> OptionalPath;


    /*! \brief Constructor */
    BgwsServer(
            const bgq::utility::Properties::Ptr& bg_properties_ptr, //!< [in]
            const bgq::utility::ServerPortConfiguration& port_configuration, //!< [ref]
            boost::asio::io_service& io_service, //!< [ref]
            bgq::utility::SignalHandler<SIGCHLD>& sig_handler //!< [ref]
        );


    void start();

    /*! \throws RefreshConfigurationError if an error occurred. */
    void refreshConfiguration( const common::RefreshBgwsServerConfiguration& config_data );

    const boost::posix_time::ptime& getStartTime() const  { return _start_time; }


private:

    boost::asio::io_service &_io_service;
    bgq::utility::SignalHandler<SIGCHLD> &_sig_handler;

    DynamicConfiguration::ConstPtr _dynamic_configuration_ptr;

    boost::posix_time::ptime _start_time;

    utility::ChildProcesses _child_processes;
    ServerStats _server_stats;

    boost::shared_ptr<BlueGene> _blue_gene_ptr;
    boost::shared_ptr<CheckUserAdminExecutor> _check_user_admin_executor_ptr;
    boost::shared_ptr<blue_gene::diagnostics::Runs> _diagnostics_runs_ptr;
    boost::shared_ptr<PwauthExecutor> _pwauth_executor_ptr;
    boost::shared_ptr<capena::server::Server> _server_ptr;
    boost::shared_ptr<blue_gene::service_actions::ServiceActions> _service_actions_ptr;
    boost::shared_ptr<Sessions> _sessions_ptr;
    boost::shared_ptr<teal::Teal> _teal_ptr;
    boost::shared_ptr<BlockingOperationsThreadPool> _blocking_operations_thread_pool_ptr;

    boost::shared_ptr<ResponderFactory> _responder_factory_ptr;


    void _handleSignal(
            const boost::system::error_code& ec,
            const siginfo_t& sig_info
        );

    void _startWaitForSignal();
};


} // namespace bgws

#endif
